/******************************************************************************

	timer.c

	タイマー管理

******************************************************************************/

#ifndef TIMER_H
#define TIMER_H

#define QSOUND_INTERRUPT		0
#define VBLANK_INTERRUPT		1
#define RASTER_INTERRUPT1		2
#define RASTER_INTERRUPT2		3
#define MAX_TIMER				4

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
#define SUSPEND_ANY_REASON		((UINT32)-1)

#define TIMER_CALLBACK(name)	void name(int param)

void timer_reset(void);
void timer_set(int which, float duration, int param, void (*callback)(int param));
void timer_update_cpu(void);

void z80_set_reset_line(int state);

#ifdef SAVE_STATE
STATE_SAVE( timer );
STATE_LOAD( timer );
#endif

#endif /* TIMER_H */
