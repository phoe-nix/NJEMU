/***************************************************************************

	vidhrdw.c

	MVSビデオエミュレーション

***************************************************************************/

#ifndef MVS_VIDEO_HARDWARE_H
#define MVS_VIDEO_HARDWARE_H

#define FIRST_VISIBLE_LINE		NEOGEO_VBEND
#define LAST_VISIBLE_LINE		(NEOGEO_VBSTART - 1)

#define MAX_SPRITES_PER_SCREEN	(381)
#define MAX_SPRITES_PER_LINE	(96)

extern UINT16 neogeo_videoram[0x20000 / 2];
extern UINT16 videoram_read_buffer;
extern UINT16 videoram_offset;
extern UINT16 videoram_modulo;

extern UINT16 palettes[2][0x2000 / 2];
extern UINT8 palette_bank;

extern UINT16 *video_palette;
extern UINT16 video_palettebank[2][0x2000 / 2];
extern UINT16 video_clut16[0x8000];

extern UINT8 *gfx_pen_usage[3];

extern UINT8 fix_bank;
extern UINT8 *fix_usage;
extern UINT8 *fix_memory;
extern int neogeo_fix_bank_type;

extern UINT16 max_sprite_number;

void neogeo_video_init(void);
void neogeo_video_exit(void);
void neogeo_video_reset(void);

void neogeo_set_fixed_layer_source(UINT8 data);

void neogeo_screenrefresh(void);
void neogeo_partial_screenrefresh(int current_line);

#ifdef SAVE_STATE
STATE_SAVE( video );
STATE_LOAD( video );
#endif

#endif /* MVS_VIDEO_HARDWARE_H */
