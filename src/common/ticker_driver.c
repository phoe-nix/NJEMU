/******************************************************************************

	ticker_driver.c

******************************************************************************/

#include <stddef.h>
#include "ticker_driver.h"

ticker_driver_t ticker_null = {
	"null",
	NULL,
	NULL,
	NULL,
};

ticker_driver_t *ticker_drivers[] = {
#ifdef PSP
	&ticker_psp,
#endif
	&ticker_null,
	NULL,
};
