/******************************************************************************

	timer.c

	タイマー管理

******************************************************************************/

#ifndef TIMER_H
#define TIMER_H

#define YM2610_TIMERA			0
#define YM2610_TIMERB			1
#define SOUNDLATCH_TIMER		3
#define SOUNDUPDATE_TIMER		4
#define CPUSPIN_TIMER			5
#define MAX_TIMER				6

#define TIME_NOW				(0)
#define TIME_NEVER				(0x7fffffff)
#define TIME_IN_HZ(hz)			(1000000.0 / (hz))

#define SEC_TO_USEC(secs)		(int)((secs) * 1000000.0)

#define USECS_PER_SCANLINE		64

#define SUSPEND_REASON_HALT		0x0001
#define SUSPEND_REASON_RESET	0x0002
#define SUSPEND_REASON_SPIN		0x0004
#define SUSPEND_REASON_TRIGGER	0x0008
#define SUSPEND_REASON_DISABLE	0x0010
#define SUSPEND_ANY_REASON		((uint32_t)-1)

#define TIMER_CALLBACK(name)	void name(int param)

void z80_set_reset_line(int state);

void timer_reset(void);
void timer_set_update_handler(void);
void timer_suspend_cpu(int cpunum, int state, int reason);
int timer_enable(int which, int enable);
void timer_adjust(int which, int duration, int param, void (*callback)(int raram));
void timer_set(int which, int duration, int param, void (*callback)(int param));
float timer_get_time(void);
int timer_getscanline(void);

#define video_get_vpos()	(timer_getscanline() - (NEOGEO_VBEND + (RASTER_COUNTER_RELOAD - NEOGEO_VBSTART)))

extern void (*timer_update_cpu)(void);
void timer_update_subcpu(void);

#ifdef SAVE_STATE
STATE_SAVE( timer );
STATE_LOAD( timer );
#endif

#endif /* TIMER_H */
