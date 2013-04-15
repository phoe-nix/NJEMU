/******************************************************************************

	timer.c

	タイマー管理

******************************************************************************/

#include "cps2.h"


/******************************************************************************
	ローカル構造体
******************************************************************************/

typedef struct timer_t
{
	float expire;
	int enable;
	int param;
	void (*callback)(int param);
} TIMER;

static TIMER ALIGN_DATA timer[MAX_TIMER];


/******************************************************************************
	ローカル変数
******************************************************************************/

static float time_slice;
static float base_time;
static float frame_base;
static float timer_ticks;
static float timer_left;

static int z80_suspended;


/******************************************************************************
	ローカル関数
******************************************************************************/

/*------------------------------------------------------
	描画割り込み
------------------------------------------------------*/

static void timer_set_raster_interrupt(int which, int scanline)
{
	int param = (which << 16) | scanline;

	timer_set(which, USECS_PER_SCANLINE * (float)scanline, param, cps2_raster_interrupt);
}


static void timer_set_vblank_interrupt(void)
{
	timer_set(VBLANK_INTERRUPT, USECS_PER_SCANLINE * 256, 0, cps2_vblank_interrupt);
}


/*------------------------------------------------------
	サウンド割り込み
------------------------------------------------------*/

static TIMER_CALLBACK( qsound_interrupt )
{
	z80_set_irq_line(0, HOLD_LINE);
	timer_set(QSOUND_INTERRUPT, TIME_IN_HZ(251), 0, qsound_interrupt);
}


/******************************************************************************
	グローバル関数
******************************************************************************/

/*------------------------------------------------------
	Z80をリセットする
------------------------------------------------------*/

void z80_set_reset_line(int state)
{
	if (z80_suspended & SUSPEND_REASON_RESET)
	{
		if (state == CLEAR_LINE)
			z80_suspended &= ~SUSPEND_REASON_RESET;
	}
	else if (state == ASSERT_LINE)
	{
		z80_suspended |= SUSPEND_REASON_RESET;
		z80_reset();
	}
}


/*------------------------------------------------------
	タイマーをリセット
------------------------------------------------------*/

void timer_reset(void)
{
	memset(&timer, 0, sizeof(timer));

	base_time     = 0;
	frame_base    = 0;
	z80_suspended = 0;

	time_slice = 1000000.0 / FPS;

	timer_set(QSOUND_INTERRUPT, TIME_IN_HZ(251), 0, qsound_interrupt);
}


/*------------------------------------------------------
	タイマーをセット
------------------------------------------------------*/

void timer_set(int which, float duration, int param, void (*callback)(int param))
{
	timer[which].expire   = (base_time + frame_base) + duration;
	timer[which].param    = param;
	timer[which].enable   = 1;
	timer[which].callback = callback;
}


/*------------------------------------------------------
	CPUを更新
------------------------------------------------------*/

void timer_update_cpu(void)
{
	int i;
	float time;
	extern int scanline1;
	extern int scanline2;

	frame_base = 0;
	timer_left = time_slice;

	if (scanline1 != RASTER_LINES) timer_set_raster_interrupt(RASTER_INTERRUPT1, scanline1);
	if (scanline2 != RASTER_LINES) timer_set_raster_interrupt(RASTER_INTERRUPT2, scanline2);
	timer_set_vblank_interrupt();
	if (driver->flags & 2) cps2_build_palette();

	while (timer_left > 0)
	{
		timer_ticks = timer_left;
		time = base_time + frame_base;

		for (i = 0; i < MAX_TIMER; i++)
		{
			if (timer[i].enable)
			{
				if (timer[i].expire - time <= 0)
				{
					timer[i].enable = 0;
					timer[i].callback(timer[i].param);
				}
			}
			if (timer[i].enable)
			{
				if (timer[i].expire - time < timer_ticks)
					timer_ticks = timer[i].expire - time;
			}
		}

		m68000_execute((int)(timer_ticks * (11800000.0 / 1000000.0)));

		if (!z80_suspended)
			z80_execute((int)(timer_ticks * (8000000.0 / 1000000.0)));

		frame_base += timer_ticks;
		timer_left -= timer_ticks;
	}

	base_time += time_slice;
	if (base_time >= 1000000.0)
	{
		base_time -= 1000000.0;

		for (i = 0; i < MAX_TIMER; i++)
		{
			if (timer[i].enable)
				timer[i].expire -= 1000000.0;
		}
	}
}


/******************************************************************************
	セーブ/ロード ステート
******************************************************************************/

#ifdef SAVE_STATE

STATE_SAVE( timer )
{
	int i;

	state_save_float(&base_time, 1);
	state_save_long(&z80_suspended, 1);

	for (i = 0; i < MAX_TIMER; i++)
	{
		state_save_float(&timer[i].expire, 1);
		state_save_long(&timer[i].enable, 1);
		state_save_long(&timer[i].param, 1);
	}
}

STATE_LOAD( timer )
{
	int i;

	state_load_float(&base_time, 1);
	state_load_long(&z80_suspended, 1);

	for (i = 0; i < MAX_TIMER; i++)
	{
		state_load_float(&timer[i].expire, 1);
		state_load_long(&timer[i].enable, 1);
		state_load_long(&timer[i].param, 1);
	}

	timer_left  = 0;
	timer_ticks = 0;
	frame_base  = 0;

	timer[QSOUND_INTERRUPT].callback  = qsound_interrupt;
	timer[VBLANK_INTERRUPT].callback  = cps2_vblank_interrupt;
	timer[RASTER_INTERRUPT1].callback = cps2_raster_interrupt;
	timer[RASTER_INTERRUPT2].callback = cps2_raster_interrupt;
}

#endif /* SAVE_STATE */
