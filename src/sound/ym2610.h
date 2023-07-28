/***************************************************************************

  ym2610.h

  Header file for software emulation for YAMAHA YM-2610 sound generator

***************************************************************************/

#ifndef _YM2610_H_
#define _YM2610_H_

/* for busy flag emulation , function FM_GET_TIME_NOW() should be */
/* return the present time in second unit with (double) value     */
/* in timer.c */
#define FM_GET_TIME_NOW() timer_get_time()

typedef int16_t FMSAMPLE;
typedef int32_t FMSAMPLE_MIX;

typedef void (*FM_TIMERHANDLER)(int channel, int count, double stepTime);
typedef void (*FM_IRQHANDLER)(int irq);

void YM2610Init(int baseclock, void *pcmroma, int pcmsizea,
#if (EMU_SYSTEM == MVS)
				void *pcmromb, int pcmsizeb,
#endif
				FM_TIMERHANDLER TimerHandler,
				FM_IRQHANDLER IRQHandler);

void YM2610Reset(void);
int YM2610Write(int addr, uint8_t value);
uint8_t YM2610Read(int addr);
int YM2610TimerOver(int channel);
void YM2610_set_samplerate(void);

#ifdef SAVE_STATE
STATE_SAVE( ym2610 );
STATE_LOAD( ym2610 );
#endif

#endif /* _YM2610_H_ */
