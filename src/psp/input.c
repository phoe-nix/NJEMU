/******************************************************************************

	input.c

    PSPコントローラ入力制御

******************************************************************************/

#include "psp.h"


/******************************************************************************
	ローカル変数
******************************************************************************/

static uint32_t pad;
static uint8_t pressed_check;
static uint8_t pressed_count;
static uint8_t pressed_delay;
static TICKER curr_time;
static TICKER prev_time;


/******************************************************************************
	グローバル関数
******************************************************************************/

/*--------------------------------------------------------
	パッドを初期化
--------------------------------------------------------*/

void pad_init(void)
{
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	pad = 0;
	pressed_check = 0;
	pressed_count = 0;
	pressed_delay = 0;
}


/*--------------------------------------------------------
	パッドの押下状態取得
--------------------------------------------------------*/

uint32_t poll_gamepad(void)
{
	SceCtrlData paddata;

	sceCtrlPeekBufferPositive(&paddata, 1);

	paddata.Buttons &= PSP_CTRL_ANY;

	if (paddata.Ly >= 0xd0) paddata.Buttons |= PSP_CTRL_DOWN;
	if (paddata.Ly <= 0x30) paddata.Buttons |= PSP_CTRL_UP;
	if (paddata.Lx <= 0x30) paddata.Buttons |= PSP_CTRL_LEFT;
	if (paddata.Lx >= 0xd0) paddata.Buttons |= PSP_CTRL_RIGHT;

	return paddata.Buttons;
}


/*--------------------------------------------------------
	パッドの押下状態取得 (MVS / fatfursp専用)
--------------------------------------------------------*/

#if (EMU_SYSTEM == MVS)
uint32_t poll_gamepad_fatfursp(void)
{
	SceCtrlData paddata;

	sceCtrlPeekBufferPositive(&paddata, 1);

	paddata.Buttons &= PSP_CTRL_ANY;

	if (!(paddata.Buttons & PSP_CTRL_UP)    && paddata.Ly >= 0xd0) paddata.Buttons |= PSP_CTRL_DOWN;
	if (!(paddata.Buttons & PSP_CTRL_DOWN)  && paddata.Ly <= 0x30) paddata.Buttons |= PSP_CTRL_UP;
	if (!(paddata.Buttons & PSP_CTRL_RIGHT) && paddata.Lx <= 0x30) paddata.Buttons |= PSP_CTRL_LEFT;
	if (!(paddata.Buttons & PSP_CTRL_LEFT)  && paddata.Lx >= 0xd0) paddata.Buttons |= PSP_CTRL_RIGHT;

	return paddata.Buttons;
}
#endif


/*--------------------------------------------------------
	パッドの押下状態取得(アナログ)
--------------------------------------------------------*/

#if (EMU_SYSTEM == MVS)
uint32_t poll_gamepad_analog(void)
{
	uint32_t data;
	SceCtrlData paddata;

	sceCtrlPeekBufferPositive(&paddata, 1);

	paddata.Buttons &= PSP_CTRL_ANY;

	if (paddata.Ly >= 0xd0) paddata.Buttons |= PSP_CTRL_DOWN;
	if (paddata.Ly <= 0x30) paddata.Buttons |= PSP_CTRL_UP;
	if (paddata.Lx <= 0x30) paddata.Buttons |= PSP_CTRL_LEFT;
	if (paddata.Lx >= 0xd0) paddata.Buttons |= PSP_CTRL_RIGHT;

	data  = paddata.Buttons & 0xffff;
	data |= paddata.Lx << 16;
	data |= paddata.Ly << 24;

	return data;
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
			prev_time = ticker();
		}
		else
		{
			int count;

			curr_time = ticker();
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

int pad_pressed(uint32_t code)
{
	return (pad & code) != 0;
}


/*--------------------------------------------------------
	指定コード以外の全ボタンの押下状態取得
--------------------------------------------------------*/

int pad_pressed_any(uint32_t disable_code)
{
	return (pad & (PSP_CTRL_ANY ^ disable_code)) != 0;
}


/*--------------------------------------------------------
	ボタンの押下状態がクリアされるまで待つ
--------------------------------------------------------*/

void pad_wait_clear(void)
{
	while (poll_gamepad())
	{
		video_wait_vsync();
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
			video_wait_vsync();
			if (!Loop) break;
		}
	}
	else
	{
		TICKER target = ticker() + msec * (TICKS_PER_SEC / 1000);

		while (ticker() < target)
		{
			video_wait_vsync();
			if (poll_gamepad()) break;
			if (!Loop) break;
		}
	}

	pad_wait_clear();
}
