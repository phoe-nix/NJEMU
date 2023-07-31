/******************************************************************************

	input.c

    COMMON¥³¥ó¥È¥í©`¥éÈëÁ¦ÖÆÓù

******************************************************************************/

#ifndef COMMON_INPUT_H
#define COMMON_INPUT_H

#include <stdint.h>
#include <stdbool.h>

// Platform Controls
#define PLATFORM_PAD_UP 0
#define PLATFORM_PAD_DOWN 1
#define PLATFORM_PAD_LEFT 2
#define PLATFORM_PAD_RIGHT 3
#define PLATFORM_PAD_B1 4
#define PLATFORM_PAD_B2 5
#define PLATFORM_PAD_B3 6
#define PLATFORM_PAD_B4 7
#define PLATFORM_PAD_L 8
#define PLATFORM_PAD_R 9
#define PLATFORM_PAD_SELECT 10
#define PLATFORM_PAD_START 11

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
void pad_exit(void);
uint32_t poll_gamepad(void);
#if (EMU_SYSTEM == MVS)
uint32_t poll_gamepad_fatfursp(void);
uint32_t poll_gamepad_analog(void);
#endif
void pad_update(void);
bool pad_pressed(uint32_t code);
bool pad_pressed_any(uint32_t disable_code);
void pad_wait_clear(void);
void pad_wait_press(int msec);

extern volatile int Loop;
extern volatile int Sleep;

#endif /* COMMON_INPUT_H */
