/**********************************************************************************************
 *
 *  OKIM 6295 ADPCM chip:
 *
 *  Command bytes are sent:
 *
 *      1xxx xxxx = start of 2-byte command sequence, xxxxxxx is the sample number to trigger
 *      abcd vvvv = second half of command; one of the abcd bits is set to indicate which voice
 *                  the v bits seem to be volumed
 *
 *      0abc d000 = stop playing; one or more of the abcd bits is set to indicate which voice(s)
 *
 *  Status is read:
 *
 *      ???? abcd = one bit per voice, set to 0 if nothing is playing, or 1 if it is active
 *
***********************************************************************************************/

#include <math.h>
#include "emumain.h"

#define OKIM6295_VOICES		4

#define FRAC_BIT	12
#define FRAC_SIZE	(1 << FRAC_BIT)

typedef struct
{
	uint32_t offset;
	int32_t sample;
	uint32_t count;
	int32_t signal;
	int32_t step;
	int data;
	int volume;

} ADPCMVoice;

typedef struct
{
	ADPCMVoice voice[OKIM6295_VOICES];
	int clock;
	int rate;
	int source_step;
	int stream_pos;
	int32_t prev_sample;
	int32_t curr_sample;
	int32_t command;
	uint32_t status;
	uint32_t divisor;

	uint8_t *rom_base;
	uint8_t *sample_rom[OKIM6295_VOICES];

} okim6295_t;

static uint32_t ALIGN_DATA volume_tables[16];
static int32_t ALIGN_DATA diff_lookup[49 * 16];
static const int32_t ALIGN_DATA index_shift[8] = {-1, -1, -1, -1, 2, 4, 6, 8};

static okim6295_t ALIGN_DATA OKIM6295;
static okim6295_t *okim6295 = &OKIM6295;


/**********************************************************************************************

     init_tables -- initialize the difference tables

***********************************************************************************************/

static void OKIM6295_init_tables(void)
{
	/* nibble to bit map */
	int nbl2bit[16][4] =
	{
		{ 1, 0, 0, 0}, { 1, 0, 0, 1}, { 1, 0, 1, 0}, { 1, 0, 1, 1},
		{ 1, 1, 0, 0}, { 1, 1, 0, 1}, { 1, 1, 1, 0}, { 1, 1, 1, 1},
		{-1, 0, 0, 0}, {-1, 0, 0, 1}, {-1, 0, 1, 0}, {-1, 0, 1, 1},
		{-1, 1, 0, 0}, {-1, 1, 0, 1}, {-1, 1, 1, 0}, {-1, 1, 1, 1}
	};
	/* step size index shift table */
	int step, nib;

	/* loop over all possible steps */
	for (step = 0; step <= 48; step++)
	{
		/* compute the step value */
		int stepval = floor(16.0 * pow(11.0 / 10.0, (double)step));

		/* loop over all nibbles and compute the difference */
		for (nib = 0; nib < 16; nib++)
		{
			diff_lookup[step*16 + nib] = nbl2bit[nib][0] *
				(stepval   * nbl2bit[nib][1] +
				 stepval/2 * nbl2bit[nib][2] +
				 stepval/4 * nbl2bit[nib][3] +
				 stepval/8);
		}
	}

	/* generate the OKI6295 volume table */
	for (step = 0; step < 16; step++)
	{
		double out = 256.0;
		int vol = step;

		/* 3dB per step */
		while (vol-- > 0)
			out /= 1.412537545;	/* = 10 ^ (3/20) = 3dB */
		volume_tables[step] = (uint32_t)(out * 0.475);
	}
}


/**********************************************************************************************

     OKIM6295Init -- initialize emulation of an OKIM6295-compatible chip

***********************************************************************************************/

void OKIM6295Init(int clock, int pin7)
{
	int i;
	int divisor = pin7 ? 132 : 165;

	memset(okim6295, 0, sizeof(okim6295_t));

	okim6295->clock = clock;
	okim6295->rate  = sound->frequency >> (2 - option_samplerate);
	okim6295->source_step = ((okim6295->clock / divisor) << FRAC_BIT) / okim6295->rate;
	okim6295->stream_pos = 0;
	okim6295->status = 0;
	okim6295->divisor = divisor;

	if (memory_region_sound1)
	{
		okim6295->rom_base = memory_region_sound1;

		for (i = 0; i < OKIM6295_VOICES; i++)
			okim6295->sample_rom[i] = okim6295->rom_base + (i << 16);
	}

	OKIM6295_init_tables();

	OKIM6295Reset();
}


/**********************************************************************************************

     OKIM6295Reset -- reset emulation of an OKIM6295-compatible chip

***********************************************************************************************/

void OKIM6295Reset(void)
{
	okim6295->status = 0;
	okim6295->command = -1;
	okim6295->stream_pos = 0;
}


/**********************************************************************************************

     OKIM6295_set_samplerate

***********************************************************************************************/

void OKIM6295_set_samplerate(void)
{
	okim6295->rate = sound->frequency >> (2 - option_samplerate);
	okim6295->source_step = ((okim6295->clock / okim6295->divisor) << FRAC_BIT) / okim6295->rate;
}


/**********************************************************************************************

     OKIM6295Update -- update the sound chip so that it is in sync with CPU execution

***********************************************************************************************/

static void OKIM6295Update(int32_t *buffer, int length)
{
	int prev_sample = okim6295->prev_sample;
	int curr_sample = okim6295->curr_sample;
	int stream_pos  = okim6295->stream_pos;
	int i, nibble, signal;
	int32_t *buf = buffer;

	while (length--)
	{
		if (stream_pos >= FRAC_SIZE)
		{
			prev_sample = curr_sample;

			do
			{
				curr_sample = 0;

				for (i = 0; i < OKIM6295_VOICES; i++)
				{
					if (okim6295->status & (1 << i))
					{
						ADPCMVoice *voice = &okim6295->voice[i];

						/* check for end of sample */
						if (voice->count-- == 0)
						{
							okim6295->status &= ~(1 << i);
							continue;
						}

						/* compute the new amplitude and update the current step */
						if (voice->offset & 1)
						{
							nibble = voice->data & 0x0f;
						}
						else
						{
							voice->data = okim6295->sample_rom[voice->offset >> 17][(voice->offset >> 1) & 0xffff];
							nibble = voice->data >> 4;
						}

						/* clamp to the maximum */
						signal = voice->signal + diff_lookup[(voice->step << 4) + nibble];
						if (signal > 2047)
						{
							signal = 2047;
						}
						else if (signal < -2048)
						{
							signal = -2048;
						}
						voice->signal = signal;
						voice->sample = signal * voice->volume;

						/* adjust the step size and clamp */
						voice->step = voice->step + index_shift[nibble & 7];
						if (voice->step > 48)
						{
							voice->step = 48;
						}
						else if (voice->step < 0)
						{
							voice->step = 0;
						}

						curr_sample += voice->sample >> 4;

						/* advance sample offset */
						voice->offset++;
					}
				}

				stream_pos -= FRAC_SIZE;

			} while (stream_pos >= FRAC_SIZE);
		}

		/* interpolate sample */
		signal = prev_sample + (((curr_sample - prev_sample) * stream_pos) >> FRAC_BIT);

		*buf++ += signal;

		stream_pos += okim6295->source_step;
	}

	okim6295->stream_pos  = stream_pos;
	okim6295->prev_sample = prev_sample;
	okim6295->curr_sample = curr_sample;
}


/**********************************************************************************************

     OKIM6295_status_r -- read the status port of an OKIM6295-compatible chip

***********************************************************************************************/

READ8_HANDLER( OKIM6295_status_r )
{
	return okim6295->status;
}


/**********************************************************************************************

     OKIM6295_data_w -- write to the data port of an OKIM6295-compatible chip

***********************************************************************************************/

WRITE8_HANDLER( OKIM6295_data_w )
{
	if (okim6295->command != -1)
	{
		int i, start, stop;
		int volume = data & 0x0f;
		uint8_t *base;

		data >>= 4;

		for (i = 0; i < OKIM6295_VOICES; i++)
		{
			if (data & (1 << i))
			{
				ADPCMVoice *voice = &okim6295->voice[i];

				/* determine the start/stop positions */
				base = &okim6295->rom_base[okim6295->command];
				start = ((base[0] << 16) + (base[1] << 8) + base[2]) & 0x3ffff;
				stop  = ((base[3] << 16) + (base[4] << 8) + base[5]) & 0x3ffff;

				/* set up the voice to play this sample */
				if (start < stop)
				{
					if (!(okim6295->status & data))
					{
						okim6295->status |= data;

						voice->offset = start << 1;
						voice->count = (stop - start + 1) << 1;
						voice->sample = 0;

						/* also reset the ADPCM parameters */
						voice->step   = 0;
						voice->signal = -2;
						voice->volume = volume_tables[volume];
					}
				}
				else
				{
					okim6295->status &= ~data;
				}
			}
		}

		okim6295->command = -1;
	}
	else if (data & 0x80)
	{
		/* if this is the start of a command, remember the sample number for next time */
		okim6295->command = (data & 0x7f) << 3;
	}
	else
	{
		/* otherwise, see if this is a silence command */
		data >>= 3;
		okim6295->status &= ~data;
	}
}


/**********************************************************************************************

     OKIM6295_set_pin7_w -- write pin 7 value

***********************************************************************************************/

WRITE8_HANDLER( OKIM6295_set_pin7_w )
{
	int divisor = data ? 132 : 165;

	okim6295->source_step = ((okim6295->clock / divisor) << FRAC_BIT) / okim6295->rate;
	okim6295->divisor = divisor;
}


/******************************************************************************
	セーブ/ロード ステート
******************************************************************************/

#ifdef SAVE_STATE

STATE_SAVE( okim6295 )
{
	int i;

	for (i = 0; i < OKIM6295_VOICES; i++)
	{
		ADPCMVoice *voice = &okim6295->voice[i];

		state_save_long(&voice->offset, 1);
		state_save_long(&voice->sample, 1);
		state_save_long(&voice->count, 1);
		state_save_long(&voice->signal, 1);
		state_save_long(&voice->step, 1);
		state_save_long(&voice->data, 1);
		state_save_long(&voice->volume, 1);
	}

	state_save_long(&okim6295->clock, 1);
	state_save_long(&okim6295->rate, 1);
	state_save_long(&okim6295->source_step, 1);
	state_save_long(&okim6295->stream_pos, 1);
	state_save_long(&okim6295->command, 1);
	state_save_long(&okim6295->status, 1);
	state_save_long(&okim6295->divisor, 1);
}

STATE_LOAD( okim6295 )
{
	int i;

	for (i = 0; i < OKIM6295_VOICES; i++)
	{
		ADPCMVoice *voice = &okim6295->voice[i];

		state_load_long(&voice->offset, 1);
		state_load_long(&voice->sample, 1);
		state_load_long(&voice->count, 1);
		state_load_long(&voice->signal, 1);
		state_load_long(&voice->step, 1);
		state_load_long(&voice->data, 1);
		state_load_long(&voice->volume, 1);
	}

	state_load_long(&okim6295->clock, 1);
	state_load_long(&okim6295->rate, 1);
	state_load_long(&okim6295->source_step, 1);
	state_load_long(&okim6295->stream_pos, 1);
	state_load_long(&okim6295->command, 1);
	state_load_long(&okim6295->status, 1);
	state_load_long(&okim6295->divisor, 1);
}

#endif /* SAVE_STATE */
