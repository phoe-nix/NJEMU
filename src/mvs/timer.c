/******************************************************************************

	timer.c

	タイマー管理

	※速度を向上させるため、精度を落としてint型で全て処理しています。
	  float/doubleだと、クロススォードの速度が大幅に低下します。
	  (YM2610のタイマ割り込み回数が極端に多いため)

******************************************************************************/

#include "mvs.h"


#define CPU_NOTACTIVE	-1


/******************************************************************************
	マクロ
******************************************************************************/

/*------------------------------------------------------
	CPUの消費した時間を取得 (単位:マイクロ秒)
------------------------------------------------------*/

#define cpu_elapsed_time(cpunum)	\
	(cpu[cpunum].cycles - *cpu[cpunum].icount) / cpu[cpunum].cycles_per_usec


/******************************************************************************
	ローカル構造体
******************************************************************************/

typedef struct timer_t
{
	int expire;
	int enable;
	int param;
	void (*callback)(int param);
} TIMER;

typedef struct cpuinfo_t
{
	int (*execute)(int cycles);
	int *icount;
	int cycles_per_usec;
	int cycles;
	int suspended;
} CPUINFO;


static TIMER ALIGN_DATA timer[MAX_TIMER];
static CPUINFO ALIGN_DATA cpu[MAX_CPU];


/******************************************************************************
	ローカル変数
******************************************************************************/

static int global_offset;
static int base_time;
static int frame_base;
static int timer_ticks;
static int timer_left;
static int active_cpu;
static int scanline;


/******************************************************************************
	プロトタイプ
******************************************************************************/

void (*timer_update_cpu)(void);
static void timer_update_cpu_normal(void);
static void timer_update_cpu_raster(void);


/******************************************************************************
	ローカル関数
******************************************************************************/

/*------------------------------------------------------
	CPUを実行
------------------------------------------------------*/

static void cpu_execute(int cpunum)
{
	if (!cpu[cpunum].suspended)
	{
		active_cpu = cpunum;
		cpu[cpunum].cycles = timer_ticks * cpu[cpunum].cycles_per_usec;
		cpu[cpunum].execute(cpu[cpunum].cycles);
		active_cpu = CPU_NOTACTIVE;
	}
}


/*------------------------------------------------------
	CPUのスピンを解除(トリガ)
------------------------------------------------------*/

static void cpu_spin_trigger(int param)
{
	timer_suspend_cpu(param, 1, SUSPEND_REASON_SPIN);
}


/*------------------------------------------------------
	現在の秒以下の時間を取得 (単位:マイクロ秒)
------------------------------------------------------*/

static int getabsolutetime(void)
{
	int time = base_time + frame_base;

	if (active_cpu != CPU_NOTACTIVE)
		time += cpu_elapsed_time(active_cpu);

	return time;
}


/******************************************************************************
	グローバル関数
******************************************************************************/

/*------------------------------------------------------
	タイマーをリセット
------------------------------------------------------*/

void timer_reset(void)
{
	global_offset = 0;
	base_time     = 0;
	frame_base    = 0;

	active_cpu = CPU_NOTACTIVE;
	memset(&timer, 0, sizeof(timer));

	cpu[CPU_M68000].execute   = m68000_execute;
	cpu[CPU_M68000].icount    = &C68K.ICount;
	cpu[CPU_M68000].cycles    = 0;
	cpu[CPU_M68000].suspended = 0;
	cpu[CPU_M68000].cycles_per_usec = 12000000 / 1000000;

	cpu[CPU_Z80].execute   = z80_execute;
	cpu[CPU_Z80].icount    = &CZ80.ICount;
	cpu[CPU_Z80].cycles    = 0;
	cpu[CPU_Z80].suspended = 0;
	cpu[CPU_Z80].cycles_per_usec = 4000000 / 1000000;
}


/*------------------------------------------------------
	CPU更新ハンドラを設定
------------------------------------------------------*/

void timer_set_update_handler(void)
{
	if (neogeo_driver_type == NORMAL)
		timer_update_cpu = timer_update_cpu_normal;
	else
		timer_update_cpu = timer_update_cpu_raster;
}


/*------------------------------------------------------
	CPUをサスペンドする
------------------------------------------------------*/

void timer_suspend_cpu(int cpunum, int state, int reason)
{
	if (state == 0)
		cpu[cpunum].suspended |= reason;
	else
		cpu[cpunum].suspended &= ~reason;
}


/*------------------------------------------------------
	タイマーを有効/無効にする
------------------------------------------------------*/

int timer_enable(int which, int enable)
{
	int old = timer[which].enable;

	timer[which].enable = enable;
	return old;
}


/*------------------------------------------------------
	タイマーをセット
------------------------------------------------------*/

void timer_adjust(int which, int duration, int param, void (*callback)(int param))
{
	int time = getabsolutetime();

	timer[which].expire = time + duration;
	timer[which].param = param;
	timer[which].callback = callback;

	if (active_cpu != CPU_NOTACTIVE)
	{
		// CPU実行中の場合は、残りサイクルを破棄
		int cycles_left = *cpu[active_cpu].icount;
		int time_left = cycles_left / cpu[active_cpu].cycles_per_usec;

		if (duration < timer_left)
		{
			timer_ticks -= time_left;
			cpu[active_cpu].cycles -= cycles_left;
			*cpu[active_cpu].icount = 0;

			if (active_cpu == CPU_Z80)
			{
				// CPU2の場合はCPU1を停止しCPU1が消費した余分なサイクルを調整する
				if (!timer[CPUSPIN_TIMER].enable)
				{
					timer_suspend_cpu(CPU_M68000, 0, SUSPEND_REASON_SPIN);
					timer[CPUSPIN_TIMER].enable = 1;
					timer[CPUSPIN_TIMER].expire = time + time_left;
					timer[CPUSPIN_TIMER].param = CPU_M68000;
					timer[CPUSPIN_TIMER].callback = cpu_spin_trigger;
				}
			}
		}
	}
}


/*------------------------------------------------------
	タイマーをセット
------------------------------------------------------*/

void timer_set(int which, int duration, int param, void (*callback)(int param))
{
	timer[which].enable = 1;
	timer_adjust(which, duration, param, callback);
}


/*------------------------------------------------------
	現在のエミュレーション時間を取得 (単位:秒)
------------------------------------------------------*/

float timer_get_time(void)
{
	int time = getabsolutetime();

	return (float)global_offset + (float)time / 1000000.0;
}


/*------------------------------------------------------
	現在のスキャンラインを取得
------------------------------------------------------*/

int timer_getscanline(void)
{
	if (neogeo_driver_type == RASTER)
		return scanline;
	else
		return 1 + (frame_base >> 6);
}


/*------------------------------------------------------
	CPUを更新
------------------------------------------------------*/

static void timer_update_cpu_normal(void)
{
	int i, time;

	frame_base = 0;
	timer_left = TICKS_PER_FRAME;

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

		if (Loop != LOOP_EXEC) return;

		cpu_execute(CPU_M68000);
		cpu_execute(CPU_Z80);

		frame_base += timer_ticks;
		timer_left -= timer_ticks;
	}

	neogeo_vblank_interrupt();

	base_time += TICKS_PER_FRAME;
	if (base_time >= 1000000)
	{
		global_offset++;
		base_time -= 1000000;

		for (i = 0; i < MAX_TIMER; i++)
		{
			if (timer[i].enable)
				timer[i].expire -= 1000000;
		}
	}

	if (!skip_this_frame()) neogeo_screenrefresh();
}


/*------------------------------------------------------
	CPUを更新 (ラスタドライバ用)
------------------------------------------------------*/

static void timer_update_cpu_raster(void)
{
	int i, time;

	frame_base = 0;
	timer_left = 0;

	for (scanline = 1; scanline <= RASTER_LINES; scanline++)
	{
		timer_left += USECS_PER_SCANLINE;

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

			if (Loop != LOOP_EXEC) return;

			cpu_execute(CPU_M68000);
			cpu_execute(CPU_Z80);

			frame_base += timer_ticks;
			timer_left -= timer_ticks;
		}

		neogeo_raster_interrupt(scanline);
	}

	base_time += TICKS_PER_FRAME;
	if (base_time >= 1000000)
	{
		global_offset++;
		base_time -= 1000000;

		for (i = 0; i < MAX_TIMER; i++)
		{
			if (timer[i].enable)
				timer[i].expire -= 1000000;
		}
	}

	if (!skip_this_frame()) neogeo_screenrefresh();
}


/******************************************************************************
	セーブ/ロード ステート
******************************************************************************/

#ifdef SAVE_STATE

STATE_SAVE( timer )
{
	int i;

	state_save_long(&global_offset, 1);
	state_save_long(&base_time, 1);

	state_save_long(&cpu[0].suspended, 1);
	state_save_long(&cpu[1].suspended, 1);

	for (i = 0; i < MAX_TIMER; i++)
	{
		state_save_long(&timer[i].expire, 1);
		state_save_long(&timer[i].enable, 1);
		state_save_long(&timer[i].param, 1);
	}
}

STATE_LOAD( timer )
{
	int i;

	state_load_long(&global_offset, 1);
	state_load_long(&base_time, 1);

	state_load_long(&cpu[0].suspended, 1);
	state_load_long(&cpu[1].suspended, 1);

	for (i = 0; i < MAX_TIMER; i++)
	{
		state_load_long(&timer[i].expire, 1);
		state_load_long(&timer[i].enable, 1);
		state_load_long(&timer[i].param, 1);
	}

	timer_left  = 0;
	timer_ticks = 0;
	frame_base  = 0;
	active_cpu = CPU_NOTACTIVE;

	timer[YM2610_TIMERA].callback    = timer_callback_2610;
	timer[YM2610_TIMERB].callback    = timer_callback_2610;
	timer[SOUNDLATCH_TIMER].callback = neogeo_sound_write;
	timer[CPUSPIN_TIMER].callback    = cpu_spin_trigger;
	timer[WATCHDOG_TIMER].callback   = watchdog_callback;
}

#endif /* SAVE_STATE */
