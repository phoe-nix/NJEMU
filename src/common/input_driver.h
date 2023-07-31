/******************************************************************************

	input_driver.h

******************************************************************************/

#ifndef INPUT_DRIVER_H
#define INPUT_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

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