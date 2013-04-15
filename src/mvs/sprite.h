/******************************************************************************

	sprite.c

	MVS スプライトマネージャ

******************************************************************************/

#ifndef MVS_SPRITE_H
#define MVS_SPRITE_H

#define SPRITE_BLANK		0x00
#define SPRITE_TRANSPARENT	0x01
#define SPRITE_OPAQUE		0x02

void blit_clear_all_sprite(void);
void blit_set_spr_clear_flag(void);
void blit_set_fix_clear_flag(void);

void blit_reset(void);
void blit_start(int start, int end);
void blit_finish(void);

void blit_draw_fix(int x, int y, UINT32 code, UINT16 attr);
void blit_finish_fix(void);

void blit_draw_spr(int x, int y, int w, int h, UINT32 code, UINT16 attr);
void blit_finish_spr(void);

void blit_draw_spr_line(int x, int y, int zoom_x, int sprite_y, UINT32 code, UINT16 attr, UINT8 opaque);

#endif /* MVS_SPRITE_H */
