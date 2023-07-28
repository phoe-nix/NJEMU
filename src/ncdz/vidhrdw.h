/******************************************************************************

	vidhrdw.c

	NEOGEO CDZ ビデオエミュレーション

******************************************************************************/

#ifndef VIDHRDW_H
#define VIDHRDW_H

#define FIRST_VISIBLE_LINE		NEOGEO_VBEND
#define LAST_VISIBLE_LINE		(NEOGEO_VBSTART - 1)

#define MAX_SPRITES_PER_SCREEN	(381)
#define MAX_SPRITES_PER_LINE	(96)

extern uint16_t neogeo_videoram[0x20000 / 2];
extern uint16_t videoram_read_buffer;
extern uint16_t videoram_offset;
extern uint16_t videoram_modulo;

extern uint16_t palettes[2][0x2000 / 2];
extern uint32_t palette_bank;

extern uint16_t *video_palette;
extern uint16_t video_palettebank[2][0x2000 / 2];
extern uint16_t video_clut16[0x8000];

extern uint8_t  fix_pen_usage[0x20000 / 32];
extern uint8_t  spr_pen_usage[0x400000 / 128];

extern int video_enable;
extern int spr_disable;
extern int fix_disable;

void neogeo_video_init(void);
void neogeo_video_exit(void);
void neogeo_video_reset(void);

void neogeo_screenrefresh(void);
void neogeo_partial_screenrefresh(int current_line);

int neogeo_loading_screenrefresh(int flag, int draw);

#ifdef SAVE_STATE
STATE_SAVE( video );
STATE_LOAD( video );
#endif

#endif /* VIDHRDW_H */
