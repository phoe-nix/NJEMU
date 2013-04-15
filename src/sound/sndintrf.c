/******************************************************************************

	sndintrf.c

	サウンドインタフェース

******************************************************************************/

#include "emumain.h"


#define FRAC_BITS	14
#define FRAC_ONE	(1 << FRAC_BITS)
#define FRAC_MASK	(FRAC_ONE - 1)

#define LINEAR_INTERPORATION	0

#if (EMU_SYSTEM == CPS2)
#define SAFETY	0
#else
#define SAFETY	32
#endif


/******************************************************************************
	ローカル変数
******************************************************************************/

static INT32 ALIGN_DATA stream_buffer_left[SOUND_BUFFER_SIZE + SAFETY];
static INT32 ALIGN_DATA stream_buffer_right[SOUND_BUFFER_SIZE + SAFETY];
static INT32 ALIGN_DATA *stream_buffer[2];

#if (EMU_SYSTEM != CPS2)
static float samples_per_update;
static float samples_left_over;
static UINT32 samples_this_update;
#endif


/******************************************************************************
	ローカル関数
******************************************************************************/

#if (EMU_SYSTEM != CPS2)

/*------------------------------------------------------
	サンプルをクリップ
------------------------------------------------------*/

static void clip_stream(INT32 *buffer)
{
	UINT32 samples = samples_this_update;
	INT32 sample;

	while (samples--)
	{
		sample = *buffer;
		Limit(sample, MAXOUT, MINOUT);
		*buffer++ = sample;
	}
}


/*------------------------------------------------------
	リサンプリング
------------------------------------------------------*/

static void resample_stream(INT32 *src, INT16 *dst)
{
	UINT32 pos = 0;
	UINT32 src_step = (samples_this_update << FRAC_BITS) / sound->samples;
	UINT32 samples = sound->samples;
#if LINEAR_INTERPORATION || (EMU_SYSTEM == CPS1)
	INT32 sample;
#endif

#if LINEAR_INTERPORATION
	src[samples_this_update] = src[samples_this_update - 1];
#endif

#if (EMU_SYSTEM == CPS1)
	if (sound->channels == 1)
	{
		while (samples--)
		{
#if LINEAR_INTERPORATION
			sample  = src[(pos >> FRAC_BITS) + 0] * (FRAC_ONE - (pos & FRAC_MASK));
			sample += src[(pos >> FRAC_BITS) + 1] * (pos & FRAC_MASK);
			sample >>= FRAC_BITS;
			*dst++ = sample;
			*dst++ = sample;
#else
			sample = src[pos >> FRAC_BITS];
			*dst++ = sample;
			*dst++ = sample;
#endif
			pos += src_step;
		}
	}
	else
#endif
	{
		while (samples--)
		{
#if LINEAR_INTERPORATION
			sample  = src[(pos >> FRAC_BITS) + 0] * (FRAC_ONE - (pos & FRAC_MASK));
			sample += src[(pos >> FRAC_BITS) + 1] * (pos & FRAC_MASK);
			sample >>= FRAC_BITS;
			*dst = sample;
#else
			*dst = src[pos >> FRAC_BITS];
#endif
			dst += 2;
			pos += src_step;
		}
	}
}

#endif


/*------------------------------------------------------
	サウンド更新(ステレオ)
------------------------------------------------------*/

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)

static void sound_update_stereo(INT16 *buffer)
{
	UINT32 samples = sound->samples;
	INT32 *srcL, *srcR, sample;
	INT16 *dst = buffer;

	(*sound->callback)(stream_buffer, samples);

	srcL = stream_buffer[0];
	srcR = stream_buffer[1];

	while (samples--)
	{
		sample = *srcL++;
		Limit(sample, MAXOUT, MINOUT);
		*dst++ = sample;

		sample = *srcR++;
		Limit(sample, MAXOUT, MINOUT);
		*dst++ = sample;
	}

	memset(stream_buffer[0], 0, sound->samples * sizeof(INT32));
	memset(stream_buffer[1], 0, sound->samples * sizeof(INT32));
}

#else

static void sound_update_stereo(INT16 *buffer)
{
	(*sound->callback)(stream_buffer, samples_this_update);

	clip_stream(stream_buffer[0]);
	clip_stream(stream_buffer[1]);

	resample_stream(stream_buffer[0], &buffer[0]);
	resample_stream(stream_buffer[1], &buffer[1]);

	samples_left_over  += samples_per_update;
	samples_this_update = (UINT32)samples_left_over;
	samples_left_over  -= samples_this_update;
}

#endif


/*------------------------------------------------------
	サウンド更新(モノラル)
------------------------------------------------------*/

#if (EMU_SYSTEM == CPS1)

static void sound_update_mono(INT16 *buffer)
{
	(*sound->callback)(stream_buffer, samples_this_update);

	clip_stream(stream_buffer[0]);

	resample_stream(stream_buffer[0], buffer);

	samples_left_over  += samples_per_update;
	samples_this_update = (UINT32)samples_left_over;
	samples_left_over  -= samples_this_update;
}

#endif


/******************************************************************************
	サウンドインタフェース関数
******************************************************************************/

/*------------------------------------------------------
	サウンドエミュレーション初期化
------------------------------------------------------*/
int sound_init(void)
{
#if (EMU_SYSTEM != CPS2)
	int sample_shift;
#endif

#if (EMU_SYSTEM == CPS1)
	if (machine_sound_type == SOUND_QSOUND)
		qsound_sh_start();
	else
		YM2151_sh_start(machine_sound_type);
#elif (EMU_SYSTEM == CPS2)
	qsound_sh_start();
#elif (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
	YM2610_sh_start();
#endif

#if (EMU_SYSTEM == CPS1)
	if (sound->channels == 1)
		sound->update = sound_update_mono;
	else
#endif
		sound->update = sound_update_stereo;

	memset(stream_buffer_left, 0, sizeof(stream_buffer_left));
	memset(stream_buffer_right, 0, sizeof(stream_buffer_right));

	stream_buffer[0] = stream_buffer_left;
	stream_buffer[1] = stream_buffer_right;

#if (EMU_SYSTEM != CPS2)
	sample_shift = 2 - option_samplerate;
	samples_per_update = (((float)sound->frequency / FPS) * 2) / (1 << sample_shift);

	samples_left_over   = samples_per_update;
	samples_this_update = (UINT32)samples_per_update;
	samples_left_over  -= samples_this_update;
#endif

	return sound_thread_start();
}


/*------------------------------------------------------
	サウンドエミュレーション終了
------------------------------------------------------*/

void sound_exit(void)
{
#if (EMU_SYSTEM == CPS1)
	if (machine_sound_type == SOUND_QSOUND)
		qsound_sh_stop();
	else
		YM2151_sh_stop();
#elif (EMU_SYSTEM == CPS2)
	qsound_sh_stop();
#elif (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
	YM2610_sh_stop();
#endif

	sound_thread_stop();
}


/*------------------------------------------------------
	サウンドエミュレーションリセット
------------------------------------------------------*/

void sound_reset(void)
{
#if (EMU_SYSTEM == CPS1)
	if (machine_sound_type == SOUND_QSOUND)
		qsound_sh_reset();
	else
		YM2151_sh_reset();
#elif (EMU_SYSTEM == CPS2)
	qsound_sh_reset();
#elif (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
	YM2610_sh_reset();
#endif

	sound_mute(0);
}


/*------------------------------------------------------
	サウンド再生レート設定
------------------------------------------------------*/

#if (EMU_SYSTEM != CPS2)
void sound_set_samplerate(void)
{
	int sample_shift;

#if (EMU_SYSTEM == CPS1)
	if (machine_sound_type != SOUND_QSOUND)
		YM2151_set_samplerate();
#elif (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
	YM2610_set_samplerate();
#endif

	sample_shift = 2 - option_samplerate;
	samples_per_update = (((float)sound->frequency / FPS) * 2) / (1 << sample_shift);

	samples_left_over   = samples_per_update;
	samples_this_update = (UINT32)samples_per_update;
	samples_left_over  -= samples_this_update;
}
#endif


/*------------------------------------------------------
	サウンドミュート
------------------------------------------------------*/

void sound_mute(int mute)
{
	if (mute)
		sound_thread_enable(0);
	else
		sound_thread_enable(option_sound_enable);
}
