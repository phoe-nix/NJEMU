/******************************************************************************

	sprite.c

	CPS2 スプライトマネージャ

******************************************************************************/

#ifndef CPS2_SPRITE_H
#define CPS2_SPRITE_H

void blit_clear_all_sprite(void);

void blit_reset(void);
void blit_start(int start, int end);
void blit_finish(void);

void blit_update_object(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_draw_object(int16_t x, int16_t y, uint16_t z, int16_t priority, uint32_t code, uint16_t attr);
extern void (*blit_finish_object)(int start_pri, int end_pri);

void blit_update_scroll1(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_draw_scroll1(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_finish_scroll1(void);

void blit_set_clip_scroll2(int16_t min_y, int16_t max_y);
int blit_check_clip_scroll2(int16_t sy);
void blit_update_scroll2(int16_t x, int16_t y, uint32_t code, uint16_t attr);
extern void (*blit_draw_scroll2)(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_finish_scroll2(void);

void blit_update_scroll3(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_draw_scroll3(int16_t x, int16_t y, uint32_t code, uint16_t attr);
void blit_finish_scroll3(void);

#endif /* CPS2_SPRITE_H */
