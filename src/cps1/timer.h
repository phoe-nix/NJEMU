/******************************************************************************

	timer.c

	Timer functions.

******************************************************************************/

#ifndef TIMER_H
#define TIMER_H

#define YM2151_TIMERA		0
#define YM2151_TIMERB		1
#define QSOUND_INTERRUPT	0
#define CPU1_SPIN_TIMER		2
#define CPU2_SPIN_TIMER		3
#define VBLANK_INTERRUPT	4
#define MAX_TIMER			5

#define TIME_NOW				(0.0)
#define TIME_NEVER				(1.0e30)
#define TIME_IN_HZ(hz)			(1000000.0 / (hz))

#define SEC_TO_USEC(secs)		((secs) * 1000000.0)

#define USECS_PER_SCANLINE		((1000000.0/FPS)/(float)RASTER_LINES)

#define SUSPEND_REASON_HALT		0x0001
#define SUSPEND_REASON_RESET	0x0002
#define SUSPEND_REASON_SPIN		0x0004
#define SUSPEND_REASON_TRIGGER	0x0008
#define SUSPEND_REASON_DISABLE	0x0010
#define SUSPEND_ANY_REASON		((uint32_t)-1)

#define TIMER_CALLBACK(name)	void name(int param)

void timer_reset(void);
void timer_suspend_cpu(int cpunum, int state, int reason);
void timer_set_resetline(int cpunum, int state);
int timer_get_cpu_status(int cpunum);
int timer_enable(int which, int enable);
void timer_adjust(int which, float duration, int param, void (*callback)(int raram));
void timer_set(int which, float duration, int param, void (*callback)(int param));
uint32_t timer_get_currentframe(void);
void timer_update_cpu(void);

#ifdef SAVE_STATE
STATE_SAVE( timer );
STATE_LOAD( timer );
#endif

#endif /* TIMER_H */
