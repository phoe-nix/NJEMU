/******************************************************************************

	input_driver.c

******************************************************************************/

#include <stddef.h>
#include "input_driver.h"

input_driver_t input_null = {
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

input_driver_t *input_drivers[] = {
#ifdef PSP
	&input_psp,
#endif
	&input_null,
	NULL,
};
