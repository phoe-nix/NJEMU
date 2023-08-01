/******************************************************************************

	sprite.c

	CPS1 スプライトマネージャ

******************************************************************************/

#ifndef CPS1_SPRITE_H
#define CPS1_SPRITE_H

void blit_clear_all_sprite(void);
void blit_palette_mark_dirty(int palno);
void blit_scrollh_clear_sprite(uint16_t tpens);

void blit_reset(int bank_scroll1, int bank_scroll2, int bank_scroll3, uint8_t *pen_usage16);
void blit_start(int high_layer);
void blit_finish(void);

void blit_update_object(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_draw_object(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_finish_object(void);

void blit_update_scroll1(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_draw_scroll1(int16_t x, int16_t y, uint32_t code, uint16_t attr, uint16_t gfxset);
void blit_finish_scroll1(void);

void blit_draw_scroll1h(int16_t x, int16_t y, uint32_t code, uint16_t attr, uint16_t tpens, uint16_t gfxset);

void blit_set_clip_scroll2(int16_t min_y, int16_t max_y);
int blit_check_clip_scroll2(int16_t sy);
void blit_update_scroll2(int16_t x, int16_t y, uint32_t code, uint16_t attr);
extern void (*blit_draw_scroll2)(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_finish_scroll2(void);

void blit_update_scroll2h(int16_t x, int16_t y, uint32_t code, uint16_t attr);
extern void (*blit_draw_scroll2h)(int16_t x, int16_t y, uint32_t code, uint16_t attr, uint16_t tpens);
void blit_finish_scroll2h(void);

void blit_update_scroll3(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_draw_scroll3(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_finish_scroll3(void);

void blit_update_scroll3h(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_draw_scroll3h(int16_t x, int16_t y, uint32_t code, uint16_t attr, uint16_t tpens);

void blit_update_scrollh(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_finish_scrollh(void);

void blit_draw_stars(uint16_t stars_x, uint16_t stars_y, uint8_t *col, uint16_t *pal);

#endif /* CPS1_SPRITE_H */
