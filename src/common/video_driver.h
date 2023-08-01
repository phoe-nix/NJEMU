/******************************************************************************

	power_driver.h

******************************************************************************/

#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#define MAKECOL15(r, g, b)	(((b & 0xf8) << 7) | ((g & 0xf8) << 2) | ((r & 0xf8) >> 3))
#define GETR15(col)			(((col << 3) & 0xf8) | ((col >>  2) & 0x07))
#define GETG15(col)			(((col >> 2) & 0xf8) | ((col >>  7) & 0x07))
#define GETB15(col)			(((col >> 7) & 0xf8) | ((col >> 12) & 0x07))

#define MAKECOL32(r, g, b)	(0xff000000 | ((b & 0xff) << 16) | ((g & 0xff) << 8) | (r & 0xff))
#define GETR32(col)			((col >>  0) & 0xff)
#define GETG32(col)			((col >>  8) & 0xff)
#define GETB32(col)			((col >> 16) & 0xff)

#define MAKECOL32A(r, g, b, a)	(((a & 0xff) << 24) | ((b & 0xff) << 16) | ((g & 0xff) << 8) | (r & 0xff))

#define COLOR_BLACK			  0,  0,  0
#define COLOR_RED			255,  0,  0
#define COLOR_GREEN			  0,255,  0
#define COLOR_BLUE			  0,  0,255
#define COLOR_YELLOW		255,255,  0
#define COLOR_PURPLE		255,  0,255
#define COLOR_CYAN			  0,255,255
#define COLOR_WHITE			255,255,255
#define COLOR_GRAY			127,127,127
#define COLOR_DARKRED		127,  0,  0
#define COLOR_DARKGREEN		  0,127,  0
#define COLOR_DARKBLUE		  0,  0,127
#define COLOR_DARKYELLOW	127,127,  0
#define COLOR_DARKPURPLE	127,  0,127
#define COLOR_DARKCYAN		  0,127,127
#define COLOR_DARKGRAY		 63, 63, 63

#define GU_FRAME_ADDR(frame)		(uint16_t *)((uint32_t)frame | 0x44000000)
#define CNVCOL15TO32(c)				(GETR15(c) | (GETG15(c) << 8) | (GETB15(c) << 16))
#define CNVCOL32TO15(c)				(((GETR32(c) & 0xf8) >> 3) | ((GETG32(c) & 0xf8) << 2) | ((GETB32(src[x]) & 0xf8) << 7))

#define SWIZZLED_8x8(tex, idx)		&tex[(idx) << 6]
#define SWIZZLED_16x16(tex, idx)	&tex[((idx & ~31) << 8) | ((idx & 31) << 7)]
#define SWIZZLED_32x32(tex, idx)	&tex[((idx & ~15) << 10) | ((idx & 15) << 8)]

#define NONE_SWIZZLED_8x8(tex, idx)		&tex[((idx & ~63) << 6) | ((idx & 63) << 3)]
#define NONE_SWIZZLED_16x16(tex, idx)	&tex[((idx & ~31) << 8) | ((idx & 31) << 4)]
#define NONE_SWIZZLED_32x32(tex, idx)	&tex[((idx & ~15) << 10) | ((idx & 15) << 5)]

#define SWIZZLED8_8x8(tex, idx)		&tex[((idx & ~1) << 6) | ((idx & 1) << 3)]
#define SWIZZLED8_16x16(tex, idx)	&tex[((idx & ~31) << 8) | ((idx & 31) << 7)]
#define SWIZZLED8_32x32(tex, idx)	&tex[((idx & ~15) << 10) | ((idx & 15) << 8)]

struct Vertex
{
	uint16_t u, v;
	uint16_t color;
	int16_t x, y, z;
};

typedef struct Vertex16_t
{
	uint32_t color;
	int16_t x, y, z;
} Vertex16;

struct rectangle
{
	int min_x;
	int max_x;
	int min_y;
	int max_y;
};

typedef struct rect_t
{
	int16_t left;
	int16_t top;
	int16_t right;
	int16_t bottom;
} RECT;

typedef struct video_driver
{
	/* Human-readable identifier. */
	const char *ident;
	/* Creates and initializes handle to video driver.
	*
	* Returns: video driver handle on success, otherwise NULL.
	**/
	void *(*init)(void);
	/* Stops and frees driver data. */
   	void (*free)(void *data);
	void (*setMode)(void *data, int mode);
	void (*waitVsync)(void *data);
	void (*flipScreen)(void *data, bool vsync);
	void *(*frameAddr)(void *data, void *frame, int x, int y);
	void (*clearScreen)(void *data);
	void (*clearFrame)(void *data, void *frame);
	void (*fillFrame)(void *data, void *frame, uint32_t color);
	void (*fillRect)(void *data, void *frame, uint32_t color, RECT *rect);
	void (*copyRect)(void *data, void *src, void *dst, RECT *src_rect, RECT *dst_rect);
	void (*copyRectFlip)(void *data, void *src, void *dst, RECT *src_rect, RECT *dst_rect);
	void (*copyRectRotate)(void *data, void *src, void *dst, RECT *src_rect, RECT *dst_rect);
	void (*drawTexture)(void *data, uint32_t src_fmt, uint32_t dst_fmt, void *src, void *dst, RECT *src_rect, RECT *dst_rect);

} video_driver_t;

extern int platform_cpuclock;

extern video_driver_t video_psp;
extern video_driver_t video_null;

extern video_driver_t *video_drivers[];

#define video_driver video_drivers[0]

extern int video_mode;
extern void *show_frame;
extern void *draw_frame;
extern void *work_frame;
extern void *tex_frame;
extern RECT full_rect;

#endif /* VIDEO_DRIVER_H */