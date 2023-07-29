/******************************************************************************

	thread_driver.h

******************************************************************************/

#ifndef THREAD_DRIVER_H
#define THREAD_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct thread_driver
{
	/* Human-readable identifier. */
	const char *ident;
	/* Creates and initializes handle to thread driver.
	*
	* Returns: thread driver handle on success, otherwise NULL.
	**/
	void *(*init)(void);
	/* Stops and frees driver data. */
   	void (*free)(void *data);
	bool (*createThread)(void *data, const char *name, void (*threadFunc)(void *), uint32_t priority, uint32_t stackSize);
	void (*startThread)(void *data);
	void (*waitThreadEnd)(void *data);
	void (*wakeupThread)(void *data);
	void (*deleteThread)(void *data);
	void (*resumeThread)(void *data);
	void (*suspendThread)(void *data);
	void (*sleepThread)(void *data);
	void (*exitThread)(void *data, int exit_code);
} thread_driver_t;


extern thread_driver_t thread_psp;
extern thread_driver_t thread_null;

extern thread_driver_t *thread_drivers[];

#define thread_driver thread_drivers[0]

#endif /* THREAD_DRIVER_H */