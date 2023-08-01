/******************************************************************************

	audio_driver.c

******************************************************************************/

#include <stddef.h>
#include "audio_driver.h"

audio_driver_t audio_null = {
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

audio_driver_t *audio_drivers[] = {
#ifdef PSP
	&audio_psp,
#endif
	&audio_null,
	NULL,
};
