/******************************************************************************

	input.c

    PSP¥³¥ó¥È¥í©`¥éÈëÁ¦ÖÆÓù

******************************************************************************/

#ifndef PSP_INPUT_H
#define PSP_INPUT_H

// Platform Controls
#define PLATFORM_PAD_UP PSP_CTRL_UP
#define PLATFORM_PAD_DOWN PSP_CTRL_DOWN
#define PLATFORM_PAD_LEFT PSP_CTRL_LEFT
#define PLATFORM_PAD_RIGHT PSP_CTRL_RIGHT
#define PLATFORM_PAD_B1 PSP_CTRL_CIRCLE
#define PLATFORM_PAD_B2 PSP_CTRL_CROSS
#define PLATFORM_PAD_B3 PSP_CTRL_CIRCLE
#define PLATFORM_PAD_B4 PSP_CTRL_TRIANGLE
#define PLATFORM_PAD_L PSP_CTRL_LTRIGGER
#define PLATFORM_PAD_R PSP_CTRL_RTRIGGER
#define PLATFORM_PAD_SELECT PSP_CTRL_SELECT
#define PLATFORM_PAD_START PSP_CTRL_START

#define PLATFORM_PAD_ANY	\
	(PLATFORM_PAD_SELECT |		\
	 PLATFORM_PAD_START |		\
	 PLATFORM_PAD_UP |			\
	 PLATFORM_PAD_RIGHT |		\
	 PLATFORM_PAD_DOWN |		\
	 PLATFORM_PAD_LEFT |		\
	 PLATFORM_PAD_L |	\
	 PLATFORM_PAD_R |	\
	 PLATFORM_PAD_B4 |	\
	 PLATFORM_PAD_B1 |		\
	 PLATFORM_PAD_B2 |		\
	 PLATFORM_PAD_B3)

#define PAD_WAIT_INFINITY	-1

void pad_init(void);
uint32_t poll_gamepad(void);
#if (EMU_SYSTEM == MVS)
uint32_t poll_gamepad_fatfursp(void);
uint32_t poll_gamepad_analog(void);
#endif
void pad_update(void);
int pad_pressed(uint32_t code);
int pad_pressed_any(uint32_t disable_code);
void pad_wait_clear(void);
void pad_wait_press(int msec);

#endif /* PSP_INPUT_H */
