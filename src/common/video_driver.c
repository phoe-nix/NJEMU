/******************************************************************************

	video_driver.c

******************************************************************************/

#include <stddef.h>
#include "video_driver.h"

#if VIDEO_32BPP
int video_mode = 0;
#endif
void *show_frame;
void *draw_frame;
void *work_frame;
void *tex_frame;

video_driver_t video_null = {
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
	NULL,
	NULL,
	NULL,
};

video_driver_t *video_drivers[] = {
#ifdef PSP
	&video_psp,
#endif
	&video_null,
	NULL,
};
