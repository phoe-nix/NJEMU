/******************************************************************************

	audio_driver.h

******************************************************************************/

#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct audio_driver
{
	/* Human-readable identifier. */
	const char *ident;
	/* Creates and initializes handle to audio driver.
	*
	* Returns: audio driver handle on success, otherwise NULL.
	**/
	void *(*init)(void);
	/* Stops and frees driver data. */
   	void (*free)(void *data);
	int32_t (*volumeMax)(void *data);
	bool (*chSRCReserve)(void *data, int32_t samples, int32_t freqency, int32_t channels);
	bool (*chReserve)(void *data, int32_t channel, int32_t samplecount, int32_t format);
	void (*srcOutputBlocking)(void *data, int32_t volume, void *buffer);
	void (*outputPannedBlocking)(void *data, int leftvol, int rightvol, void *buf);
	void (*release)(void *data);
} audio_driver_t;


extern audio_driver_t audio_psp;
extern audio_driver_t audio_null;

extern audio_driver_t *audio_drivers[];

#define audio_driver audio_drivers[0]

#endif /* AUDIO_DRIVER_H */