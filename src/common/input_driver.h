/******************************************************************************

	input_driver.h

******************************************************************************/

#ifndef INPUT_DRIVER_H
#define INPUT_DRIVER_H

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
bool pad_pressed_any(void);
void pad_wait_clear(void);
void pad_wait_press(int msec);

extern volatile int Loop;
extern volatile int Sleep;

typedef struct input_driver
{
	/* Human-readable identifier. */
	const char *ident;
	/* Creates and initializes handle to input driver.
	*
	* Returns: input driver handle on success, otherwise NULL.
	**/
	void *(*init)(void);
	/* Stops and frees driver data. */
   	void (*free)(void *data);
	uint32_t (*poll)(void *data);
	#if (EMU_SYSTEM == MVS)
	uint32_t (*pollFatfursp)(void *data);
	uint32_t (*pollAnalog)(void *data);
	#endif
} input_driver_t;


extern input_driver_t input_psp;
extern input_driver_t input_null;

extern input_driver_t *input_drivers[];

#define input_driver input_drivers[0]

#endif /* INPUT_DRIVER_H */