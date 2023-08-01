/******************************************************************************

	ticker_driver.h

******************************************************************************/

#ifndef TICKER_DRIVER_H
#define TICKER_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t TICKER;

//TICKER TICKS_PER_SEC;
#define TICKS_PER_SEC	1000000

typedef struct ticker_driver
{
	/* Human-readable identifier. */
	const char *ident;
	/* Creates and initializes handle to ticker driver.
	*
	* Returns: ticker driver handle on success, otherwise NULL.
	**/
	void *(*init)(void);
	/* Stops and frees driver data. */
   	void (*free)(void *data);
	TICKER (*ticker)(void *data);

} ticker_driver_t;


extern ticker_driver_t ticker_psp;
extern ticker_driver_t ticker_null;

extern ticker_driver_t *ticker_drivers[];

#define ticker_driver ticker_drivers[0]

#endif /* TICKER_DRIVER_H */