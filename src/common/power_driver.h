/******************************************************************************

	power_driver.h

******************************************************************************/

#ifndef POWER_DRIVER_H
#define POWER_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct power_driver
{
	/* Human-readable identifier. */
	const char *ident;
	/* Creates and initializes handle to power driver.
	*
	* Returns: power driver handle on success, otherwise NULL.
	**/
	void *(*init)(void);
	/* Stops and frees driver data. */
   	void (*free)(void *data);
	int32_t (*batteryLifePercent)(void *data);
	bool (*isBatteryCharging)(void *data);

} power_driver_t;


extern power_driver_t power_psp;
extern power_driver_t power_null;

extern power_driver_t *power_drivers[];

#define power_driver power_drivers[0]

#endif /* POWER_DRIVER_H */