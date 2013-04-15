/***************************************************************************

	2610intf.c

	YM2610 interface

***************************************************************************/

#include "emumain.h"


/***************************************************************************
	Function prototypes
 **************************************************************************/

static void TimerHandler(int channel, int count, double stepTime);


/*------------------------------------------------------
	Start YM2610 emulation
 -----------------------------------------------------*/

void YM2610_sh_start(void)
{
#if (EMU_SYSTEM == MVS)
	void *pcmbufa, *pcmbufb;
	int pcmsizea, pcmsizeb;

	pcmbufa = (void *)memory_region_sound1;
	pcmsizea = memory_length_sound1;

	if (memory_length_sound2)
	{
		pcmbufb = (void *)memory_region_sound2;
		pcmsizeb = memory_length_sound2;
	}
	else
	{
		pcmbufb = (void *)memory_region_sound1;
		pcmsizeb = memory_length_sound1;
	}

	YM2610Init(8000000, pcmbufa, pcmsizea, pcmbufb, pcmsizeb, TimerHandler, neogeo_sound_irq);
#else
	YM2610Init(8000000, memory_region_sound1, memory_length_sound1, TimerHandler, neogeo_sound_irq);
#endif
}


/*------------------------------------------------------
	Stop YM2610 emulation
 -----------------------------------------------------*/

void YM2610_sh_stop(void)
{
}


/*------------------------------------------------------
	Reset YM2610 emulation
 -----------------------------------------------------*/

void YM2610_sh_reset(void)
{
	YM2610Reset();
}


/*------------------------------------------------------
	Read YM2610 status port A
 -----------------------------------------------------*/

READ8_HANDLER( YM2610_status_port_A_r )
{
	return YM2610Read(0);
}


/*------------------------------------------------------
	Read YM2610 status port B
 -----------------------------------------------------*/

READ8_HANDLER( YM2610_status_port_B_r )
{
	return YM2610Read(2);
}

READ8_HANDLER( YM2610_read_port_r )
{
	return YM2610Read(1);
}


/*------------------------------------------------------
	Write YM2610 control port A
 -----------------------------------------------------*/

WRITE8_HANDLER( YM2610_control_port_A_w )
{
	YM2610Write(0, data);
}


/*------------------------------------------------------
	Write YM2610 control port B
 -----------------------------------------------------*/

WRITE8_HANDLER( YM2610_control_port_B_w )
{
	YM2610Write(2, data);
}


/*------------------------------------------------------
	Write YM2610 data port A
 -----------------------------------------------------*/

WRITE8_HANDLER( YM2610_data_port_A_w )
{
	YM2610Write(1, data);
}


/*------------------------------------------------------
	Write YM2610 data port B
 -----------------------------------------------------*/

WRITE8_HANDLER( YM2610_data_port_B_w )
{
	YM2610Write(3, data);
}


/*------------------------------------------------------
	Timer callback function
 -----------------------------------------------------*/

void timer_callback_2610(int param)
{
	YM2610TimerOver(param);
}


/*------------------------------------------------------
	YM2610 timer handler
 -----------------------------------------------------*/

static void TimerHandler(int channel, int count, double stepTime)
{
	if (count == 0)
	{
		/* Reset FM Timer */
		timer_enable(channel, 0);
	}
	else
	{
		/* Start FM Timer */
#if 0
		float time_usec = (float)((double)count * SEC_TO_USEC(stepTime));
#else
		// ìsçáÇ…ÇÊÇËÅAfloatÇ≈ÇÕÇ»Ç≠intÇégóp
		int time_usec = count * SEC_TO_USEC(stepTime);
#endif

		if (!timer_enable(channel, 1))
			timer_adjust(channel, time_usec, channel, timer_callback_2610);
	}
}
