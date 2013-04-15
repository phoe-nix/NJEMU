/***************************************************************************

  ym2151.h

  Header file for software emulation for YAMAHA YM-2151 sound generator

***************************************************************************/

#ifndef YM2151_H
#define YM2151_H

typedef INT16 FMSAMPLE;
typedef INT32 FMSAMPLE_MIX;

typedef void (*FM_IRQHANDLER)(int irq);

void YM2151Init(int clock, FM_IRQHANDLER IRQHandler);
void YM2151Reset(void);
void YM2151_set_samplerate(void);
void YM2151Update(int p);
void YM2151WriteReg(int reg, int value);
int YM2151ReadStatus(void);

void timer_callback_2151_a(int param);
void timer_callback_2151_b(int param);

#ifdef SAVE_STATE
STATE_SAVE( ym2151 );
STATE_LOAD( ym2151 );
#endif

#endif /* YM2151_H */
