/******************************************************************************

	input_driver.c

******************************************************************************/

#include <stddef.h>
#include "input_driver.h"
#include <stddef.h>
#include "input_driver.h"
#include "ticker_driver.h"
#include "video_driver.h"

// TODO: Use video driver function instead
void video_wait_vsync(void);

/******************************************************************************
	ローカル変数
******************************************************************************/

static uint32_t pad;
static uint8_t pressed_check;
static uint8_t pressed_count;
static uint8_t pressed_delay;
static TICKER curr_time;
static TICKER prev_time;

void *input_info;


/******************************************************************************
	グローバル関数
******************************************************************************/

/*--------------------------------------------------------
	パッドを初期化
--------------------------------------------------------*/

void pad_init(void)
{
	pad = 0;
	pressed_check = 0;
	pressed_count = 0;
	pressed_delay = 0;
	input_info = input_driver->init();
}

void pad_exit(void)
{
	input_driver->free(input_info);
	input_info = NULL;
}

/*--------------------------------------------------------
	パッドの押下状態取得
--------------------------------------------------------*/

uint32_t poll_gamepad(void)
{
	return input_driver->poll(input_info);
}


/*--------------------------------------------------------
	パッドの押下状態取得 (MVS / fatfursp専用)
--------------------------------------------------------*/

#if (EMU_SYSTEM == MVS)
uint32_t poll_gamepad_fatfursp(void)
{
	return input_driver->pollFatfursp(input_info);
}
#endif


/*--------------------------------------------------------
	パッドの押下状態取得(アナログ)
--------------------------------------------------------*/

#if (EMU_SYSTEM == MVS)
uint32_t poll_gamepad_analog(void)
{
	return input_driver->pollAnalog(input_info);
}
#endif


/*--------------------------------------------------------
	パッド押下情報更新
--------------------------------------------------------*/

void pad_update(void)
{
	uint32_t data;

	data = poll_gamepad();

	if (data)
	{
		if (!pressed_check)
		{
			pressed_check = 1;
			pressed_count = 0;
			pressed_delay = 8;
			prev_time = ticker_driver->ticker(NULL);
		}
		else
		{
			int count;

			curr_time = ticker_driver->ticker(NULL);
			count = (int)((curr_time - prev_time) / (TICKS_PER_SEC / 60));
			prev_time = curr_time;

			pressed_count += count;

			if (pressed_count > pressed_delay)
			{
				pressed_count = 0;
				if (pressed_delay > 2) pressed_delay -= 2;
			}
			else data = 0;
		}
	}
	else pressed_check = 0;

	pad = data;
}


/*--------------------------------------------------------
	ボタン押下状態の取得
--------------------------------------------------------*/

bool pad_pressed(uint32_t code)
{
	return (pad & code) != 0;
}


#define PLATFORM_PAD_ANY			\
	(								\
		PLATFORM_PAD_SELECT |	\
	 	PLATFORM_PAD_START |	\
	 	PLATFORM_PAD_UP |		\
	 	PLATFORM_PAD_RIGHT |	\
	 	PLATFORM_PAD_DOWN |	\
	 	PLATFORM_PAD_LEFT |	\
	 	PLATFORM_PAD_L |		\
	 	PLATFORM_PAD_R |		\
	 	PLATFORM_PAD_B4 |		\
	 	PLATFORM_PAD_B1 |		\
	 	PLATFORM_PAD_B2 |		\
	 	PLATFORM_PAD_B3 		\
	)

bool pad_pressed_any(void)
{
	return (pad & PLATFORM_PAD_ANY) != 0;
}

/*--------------------------------------------------------
	ボタンの押下状態がクリアされるまで待つ
--------------------------------------------------------*/

void pad_wait_clear(void)
{
	while (poll_gamepad())
	{
		video_driver->waitVsync(video_data);
		if (!Loop) break;
	}

	pad = 0;
	pressed_check = 0;
}


/*--------------------------------------------------------
	何かボタンが押されるまで待つ
--------------------------------------------------------*/

void pad_wait_press(int msec)
{
	pad_wait_clear();

	if (msec == PAD_WAIT_INFINITY)
	{
		while (!poll_gamepad())
		{
			video_driver->waitVsync(video_data);
			if (!Loop) break;
		}
	}
	else
	{
		TICKER target = ticker_driver->ticker(NULL) + msec * (TICKS_PER_SEC / 1000);

		while (ticker_driver->ticker(NULL) < target)
		{
			video_driver->waitVsync(video_data);
			if (poll_gamepad()) break;
			if (!Loop) break;
		}
	}

	pad_wait_clear();
}

input_driver_t input_null = {
	"null",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

input_driver_t *input_drivers[] = {
#ifdef PSP
	&input_psp,
#endif
	&input_null,
	NULL,
};
