/******************************************************************************

	sprite.c

	CPS1 スプライトマネージャ

******************************************************************************/

#ifndef CPS1_SPRITE_H
#define CPS1_SPRITE_H

void blit_clear_all_sprite(void);
void blit_palette_mark_dirty(int palno);
void blit_scrollh_clear_sprite(UINT16 tpens);

void blit_reset(int bank_scroll1, int bank_scroll2, int bank_scroll3, UINT8 *pen_usage16);
void blit_start(int high_layer);
void blit_finish(void);

void blit_update_object(INT16 x, INT16 y, UINT32 code, UINT16 attr);
void blit_draw_object(INT16 x, INT16 y, UINT32 code, UINT16 attr);
void blit_finish_object(void);

void blit_update_scroll1(INT16 x, INT16 y, UINT32 code, UINT16 attr);
void blit_draw_scroll1(INT16 x, INT16 y, UINT32 code, UINT16 attr, UINT16 gfxset);
void blit_finish_scroll1(void);

void blit_draw_scroll1h(INT16 x, INT16 y, UINT32 code, UINT16 attr, UINT16 tpens, UINT16 gfxset);

void blit_set_clip_scroll2(INT16 min_y, INT16 max_y);
int blit_check_clip_scroll2(INT16 sy);
void blit_update_scroll2(INT16 x, INT16 y, UINT32 code, UINT16 attr);
extern void (*blit_draw_scroll2)(INT16 x, INT16 y, UINT32 code, UINT16 attr);
void blit_finish_scroll2(void);

void blit_update_scroll2h(INT16 x, INT16 y, UINT32 code, UINT16 attr);
extern void (*blit_draw_scroll2h)(INT16 x, INT16 y, UINT32 code, UINT16 attr, UINT16 tpens);
void blit_finish_scroll2h(void);

void blit_update_scroll3(INT16 x, INT16 y, UINT32 code, UINT16 attr);
void blit_draw_scroll3(INT16 x, INT16 y, UINT32 code, UINT16 attr);
void blit_finish_scroll3(void);

void blit_update_scroll3h(INT16 x, INT16 y, UINT32 code, UINT16 attr);
void blit_draw_scroll3h(INT16 x, INT16 y, UINT32 code, UINT16 attr, UINT16 tpens);

void blit_update_scrollh(INT16 x, INT16 y, UINT32 code, UINT16 attr);
void blit_finish_scrollh(void);

void blit_draw_stars(UINT16 stars_x, UINT16 stars_y, UINT8 *col, UINT16 *pal);

#endif /* CPS1_SPRITE_H */
