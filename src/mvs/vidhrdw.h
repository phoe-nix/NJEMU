/***************************************************************************

	vidhrdw.c

	MVS¥Ó¥Ç¥ª¥¨¥ß¥å¥ì©`¥·¥ç¥ó

***************************************************************************/

#ifndef MVS_VIDEO_HARDWARE_H
#define MVS_VIDEO_HARDWARE_H

#define FIRST_VISIBLE_LINE		NEOGEO_VBEND
#define LAST_VISIBLE_LINE		(NEOGEO_VBSTART - 1)

#define MAX_SPRITES_PER_SCREEN	(381)
#define MAX_SPRITES_PER_LINE	(96)

extern uint16_t neogeo_videoram[0x20000 / 2];
extern uint16_t videoram_read_buffer;
extern uint16_t videoram_offset;
extern uint16_t videoram_modulo;

extern uint16_t palettes[2][0x2000 / 2];
extern uint8_t palette_bank;

extern uint16_t *video_palette;
extern uint16_t video_palettebank[2][0x2000 / 2];
extern uint16_t video_clut16[0x8000];

extern uint8_t *gfx_pen_usage[3];

extern uint8_t fix_bank;
extern uint8_t *fix_usage;
extern uint8_t *fix_memory;
extern int neogeo_fix_bank_type;

extern uint16_t max_sprite_number;

void neogeo_video_init(void);
void neogeo_video_exit(void);
void neogeo_video_reset(void);

void neogeo_set_fixed_layer_source(uint8_t data);

void neogeo_screenrefresh(void);
void neogeo_partial_screenrefresh(int current_line);

#ifdef SAVE_STATE
STATE_SAVE( video );
STATE_LOAD( video );
#endif

#endif /* MVS_VIDEO_HARDWARE_H */
