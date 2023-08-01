/******************************************************************************

	video.c

	PSP�r�f�I����֐�

******************************************************************************/

#ifndef PSP_VIDEO_H
#define PSP_VIDEO_H

#include "psp.h"

#define SCR_WIDTH			480
#define SCR_HEIGHT			272
#define BUF_WIDTH			512
#define	FRAMESIZE			(BUF_WIDTH * SCR_HEIGHT * sizeof(uint16_t))
#define	FRAMESIZE32			(BUF_WIDTH * SCR_HEIGHT * sizeof(uint32_t))

#define SLICE_SIZE			64 // change this to experiment with different page-cache sizes
#define TEXTURE_FLAGS		(GU_TEXTURE_16BIT | GU_COLOR_5551 | GU_VERTEX_16BIT | GU_TRANSFORM_2D)
#define PRIMITIVE_FLAGS		(GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D)

extern uint8_t gulist[GULIST_SIZE];

#endif /* PSP_VIDE_H */
