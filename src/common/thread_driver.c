/******************************************************************************

	thread_driver.c

******************************************************************************/

#include <stddef.h>
#include "thread_driver.h"

thread_driver_t thread_null = {
	"null",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

thread_driver_t *thread_drivers[] = {
#ifdef PSP
	&thread_psp,
#endif
	&thread_null,
	NULL,
};
