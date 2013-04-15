/******************************************************************************

	vidhrdw.c

	CPS1 ビデオエミュレーション

******************************************************************************/

#ifndef CPS1_VIDEO_HARDWARE_H
#define CPS1_VIDEO_HARDWARE_H

#define FIRST_VISIBLE_LINE	16
#define LAST_VISIBLE_LINE	239

#define SPRITE_BLANK		0x00
#define SPRITE_OPAQUE		0x01
#define SPRITE_TRANSPARENT	0x02

enum
{
	LAYER_SKIP = -1,
	LAYER_OBJECT = 0,
	LAYER_SCROLL1,
	LAYER_SCROLL2,
	LAYER_SCROLL3,
	LAYER_MAX
};

enum
{
	TILE08 = 0,
	TILE16,
	TILE32
};

extern int cps_rotate_screen;
extern int cps_flip_screen;
#if ENABLE_RASTER_OPTION
extern int cps_raster_enable;
#endif
extern UINT16 video_palette[3072];

void cps1_scan_object(void);
void cps1_scan_scroll1(void);
void cps1_scan_scroll2(void);
void cps1_scan_scroll3(void);
void cps1_scan_scroll1_foreground(void);
void cps1_scan_scroll2_foreground(void);
void cps1_scan_scroll3_foreground(void);

int cps1_video_init(void);
void cps1_video_exit(void);
void cps1_video_reset(void);
void cps1_screenrefresh(void);
void cps1_objram_latch(void);

READ16_HANDLER( cps1_output_r );
WRITE16_HANDLER( cps1_output_w );

#ifdef SAVE_STATE
STATE_SAVE( video );
STATE_LOAD( video );
#endif

#endif /* CPS1_VIDEO_HARDWARE_H */
