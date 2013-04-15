/******************************************************************************

	vidhrdw.c

	MVS ビデオエミュレーション

******************************************************************************/

#include "mvs.h"


/******************************************************************************
	グローバル変数
******************************************************************************/

UINT16 ALIGN_DATA neogeo_videoram[0x20000 / 2];
UINT16 videoram_read_buffer;
UINT16 videoram_offset;
UINT16 videoram_modulo;

UINT16 ALIGN_DATA palettes[2][0x2000 / 2];
UINT8 palette_bank;

UINT16 *video_palette;
UINT16 ALIGN_PSPDATA video_palettebank[2][0x2000 / 2];
UINT16 ALIGN_DATA video_clut16[0x8000];

UINT8 *gfx_pen_usage[3];

UINT8 fix_bank;
UINT8 *fix_usage;
UINT8 *fix_memory;
int neogeo_fix_bank_type;

UINT16 max_sprite_number;


/******************************************************************************
	ローカル変数
******************************************************************************/

static UINT32 high_tile_mask;
static UINT32 sprite_gfx_code_mask;
static int next_update_first_line;

static UINT8 *spr_pen_usage;
static UINT16 *sprite_zoom_control = &neogeo_videoram[0x8000];
static UINT16 *sprite_y_control = &neogeo_videoram[0x8200];
static UINT16 *sprite_x_control = &neogeo_videoram[0x8400];

static const UINT8 *skip_fullmode0;
static const UINT8 *skip_fullmode1;
static const UINT8 *tile_fullmode0;
static const UINT8 *tile_fullmode1;

static void (*draw_fixed_layer_func[2])(void);
static void (*draw_fixed_layer)(void);


/******************************************************************************
	ローカル関数
******************************************************************************/

/*------------------------------------------------------
	FIXスプライト描画
------------------------------------------------------*/

static void draw_fixed_layer_type0(void)
{
	UINT16 x, y, code, attr;

	for (x = 8/8; x < 312/8; x++)
	{
		UINT16 *vram = &neogeo_videoram[0x7002 + (x << 5)];

		for (y = 16/8; y < 240/8; y++)
		{
			code = *vram++;
			attr = code >> 12;
			code &= 0x0fff;

			if (fix_usage[code])
				blit_draw_fix((x << 3) + 16, y << 3, code, attr);
		}
	}

	blit_finish_fix();
}


static void draw_fixed_layer_type1(void)
{
	UINT16 x, y, code, attr;
	int garouoffsets[32];
	int garoubank = 0;
	int i = 0;

	y = 0;
	while (y < 32)
	{
		if (neogeo_videoram[0x7500 + i] == 0x0200 && (neogeo_videoram[0x7580 + i] & 0xff00) == 0xff00)
		{
			garoubank = neogeo_videoram[0x7580 + i] & 3;
			garouoffsets[y++] = garoubank;
		}
		garouoffsets[y++] = garoubank;
		i += 2;
	}

	for (x = 8/8; x < 312/8; x++)
	{
		UINT16 *vram = &neogeo_videoram[0x7002 + (x << 5)];

		for (y = 16/8; y < 240/8; y++)
		{
			code = *vram++;
			attr = code >> 12;
			code &= 0x0fff;

			code += (garouoffsets[(y - 2) & 31] ^ 3) << 12;

			if (fix_usage[code])
				blit_draw_fix((x << 3) + 16, y << 3, code, attr);
		}
	}

	blit_finish_fix();
}


static void draw_fixed_layer_type2(void)
{
	UINT16 x, y, code, attr;

	for (x = 8/8; x < 312/8; x++)
	{
		UINT16 *vram = &neogeo_videoram[0x7002 + (x << 5)];

		for (y = 16/8; y < 240/8; y++)
		{
			code = *vram++;
			attr = code >> 12;
			code &= 0x0fff;

			code += (((neogeo_videoram[0x7500 + ((y - 1) & 31) + ((x / 6) << 5)] >> (5 - (x % 6)) * 2) & 3) ^ 3) << 12;

			if (fix_usage[code])
				blit_draw_fix((x << 3) + 16, y << 3, code, attr);
		}
	}

	blit_finish_fix();
}


/*------------------------------------------------------
	SPRスプライト描画
------------------------------------------------------*/

typedef struct
{
	INT16  x;
	UINT16 zoom_x;
	UINT16 *base;
} SPRITE_LIST;

static SPRITE_LIST sprite_list[MAX_SPRITES_PER_LINE];

static void draw_sprites_hardware(int min_y, int max_y)
{
	int y = 0;
	int x = 0;
	int rows = 0;
	int zoom_y = 0;
	int zoom_x = 0;

	UINT16 sprite_number = 0;
	UINT16 num_sprites = 0;
	UINT16 fullmode = 0;
	UINT16 attr;
	UINT32 code;

	const UINT8 *skip = NULL;
	const UINT8 *tile = NULL;

	do
	{
		for (;sprite_number < max_sprite_number; sprite_number++)
		{
			UINT16 y_control = sprite_y_control[sprite_number];
			UINT16 zoom_control = sprite_zoom_control[sprite_number];

			if (y_control & 0x40)
			{
				if (rows == 0) continue;

				x += zoom_x + 1;
			}
			else
			{
				if (num_sprites != 0) break;

				if ((rows = y_control & 0x3f) == 0) continue;

				y = 0x200 - (y_control >> 7);
				x = (sprite_x_control[sprite_number] >> 7) + 16;
				zoom_y = (zoom_control & 0xff) << 6;

				if (rows > 0x20)
				{
					rows = 0x200;
					fullmode = 1;
					skip = &skip_fullmode1[zoom_y];
					tile = &tile_fullmode1[zoom_y];
				}
				else
				{
					rows <<= 4;
					fullmode = 0;
					skip = &skip_fullmode0[zoom_y];
					tile = &tile_fullmode0[zoom_y];
				}
			}

			x &= 0x1ff;

			zoom_x = (zoom_control >> 8) & 0x0f;

			if ((x + zoom_x >= 24) && (x < 336))
			{
				sprite_list[num_sprites].x = x;
				sprite_list[num_sprites].zoom_x = zoom_x + 1;
				sprite_list[num_sprites].base = &neogeo_videoram[sprite_number << 6];
				num_sprites++;
			}
		}

		if (num_sprites)
		{
			UINT16 sprite_line = 0;
			UINT16 invert = 0;
			UINT16 sy, yskip;

			while (sprite_line < rows)
			{
				sy = (y + sprite_line) & 0x1ff;
				yskip = *skip;

				if (fullmode)
				{
					if (yskip == 0)
					{
						skip = &skip_fullmode1[zoom_y];
						tile = &tile_fullmode1[zoom_y];
						yskip = *skip;
					}
				}
				else if (yskip > 0x10)
				{
					yskip = skip_fullmode0[zoom_y + 0x3f];

					if (invert)
						sy = (sy + (*skip - yskip)) & 0x1ff;
					else
						invert = 1;
				}

				if (sy + yskip > min_y && yskip <= max_y)
				{
					SPRITE_LIST *sprite = sprite_list;

					for (x = 0; x < num_sprites; x++)
					{
						attr = sprite->base[*tile + 1];
						code = sprite->base[*tile + 0] | ((attr << 12) & high_tile_mask);

						if (!auto_animation_disabled)
						{
							if (attr & 0x0008)
								code = (code & ~0x07) | (auto_animation_counter & 0x07);
							else if (attr & 0x0004)
								code = (code & ~0x03) | (auto_animation_counter & 0x03);
						}

						code &= sprite_gfx_code_mask;

						if (spr_pen_usage[code])
							blit_draw_spr(sprite->x, sy, sprite->zoom_x, yskip, code, attr);

						sprite++;
					}
				}

				sprite_line += *skip;
				skip++;
				tile++;
			}

			num_sprites = 0;
			rows = 0;
		}
	} while (sprite_number < max_sprite_number);

	blit_finish_spr();
}


INLINE int sprite_on_scanline(int scanline, int y, int rows)
{
	/* check if the current scanline falls inside this sprite,
       two possible scenerios, wrap around or not */
	int max_y = (y + rows - 1) & 0x1ff;

	return (((max_y >= y) &&  (scanline >= y) && (scanline <= max_y)) ||
			((max_y <  y) && ((scanline >= y) || (scanline <= max_y))));
}


static void draw_sprites_software(int min_y, int max_y)
{
	int y = 0;
	int x = 0;
	int rows = 0;
	int zoom_y = 0;
	int zoom_x = 0;

	UINT16 sprite_number = 0;
	UINT16 num_sprites = 0;
	UINT8 fullmode = 0;
	UINT16 attr;
	UINT32 code;

	do
	{
		for (;sprite_number < max_sprite_number; sprite_number++)
		{
			UINT16 y_control = sprite_y_control[sprite_number];
			UINT16 zoom_control = sprite_zoom_control[sprite_number];

			if (y_control & 0x40)
			{
				if (rows == 0) continue;

				x += zoom_x + 1;
			}
			else
			{
				if (num_sprites != 0) break;

				if ((rows = y_control & 0x3f) == 0) continue;

				y = 0x200 - (y_control >> 7);
				x = (sprite_x_control[sprite_number] >> 7) + 16;
				zoom_y = zoom_control & 0xff;

				if (rows > 0x20)
				{
					rows = 0x200;
					fullmode = 1;
				}
				else
				{
					rows <<= 4;
					fullmode = 0;
				}
			}

			x &= 0x1ff;

			zoom_x = (zoom_control >> 8) & 0x0f;

			if ((x + zoom_x >= 24) && (x < 336))
			{
				sprite_list[num_sprites].x = x;
				sprite_list[num_sprites].zoom_x = zoom_x + 1;
				sprite_list[num_sprites].base = &neogeo_videoram[sprite_number << 6];
				num_sprites++;
			}
		}

		if (num_sprites)
		{
			UINT16 scanline;
			UINT8 *zoom_y_table = memory_region_gfx4 + (zoom_y << 8);
			UINT8 sprite_y;
			UINT8 tile;
			UINT8 sprite_y_and_tile;
			UINT8 attr_and_code_offs;

			for (scanline = min_y; scanline <= max_y; scanline++)
			{
				SPRITE_LIST *sprite = sprite_list;

				if (sprite_on_scanline(scanline, y, rows))
				{
					UINT16 sprite_line = (scanline - y) & 0x1ff;
					UINT16 zoom_line = sprite_line & 0xff;
					UINT16 invert = sprite_line & 0x100;

					if (invert)
						zoom_line ^= 0xff;

					if (fullmode)
					{
						zoom_line = zoom_line % ((zoom_y + 1) << 1);

						if (zoom_line > zoom_y)
						{
							zoom_line = ((zoom_y + 1) << 1) - 1 - zoom_line;
							invert = !invert;
						}
					}

					sprite_y_and_tile = zoom_y_table[zoom_line];
					sprite_y = sprite_y_and_tile & 0x0f;
					tile = sprite_y_and_tile >> 4;

					if (invert)
					{
						sprite_y ^= 0x0f;
						tile ^= 0x1f;
					}

					attr_and_code_offs = tile << 1;

					for (x = 0; x < num_sprites; x++)
					{
						attr = sprite->base[attr_and_code_offs + 1];
						code = sprite->base[attr_and_code_offs + 0] | ((attr << 12) & high_tile_mask);

						if (!auto_animation_disabled)
						{
							if (attr & 0x0008)
								code = (code & ~0x07) | (auto_animation_counter & 0x07);
							else if (attr & 0x0004)
								code = (code & ~0x03) | (auto_animation_counter & 0x03);
						}

						code &= sprite_gfx_code_mask;

						if (spr_pen_usage[code])
							blit_draw_spr_line(sprite->x, scanline, sprite->zoom_x, sprite_y, code, attr, spr_pen_usage[code]);

						sprite++;
					}
				}
			}

			num_sprites = 0;
			rows = 0;
		}
	} while (sprite_number < max_sprite_number);
}


/******************************************************************************
	MVS ビデオ描画処理
******************************************************************************/

/*------------------------------------------------------
	ビデオエミュレーション初期化
------------------------------------------------------*/

void neogeo_video_init(void)
{
	int i, r, g, b;
	UINT32 no_of_tiles, bit;

	for (r = 0; r < 32; r++)
	{
		for (g = 0; g < 32; g++)
		{
			for (b = 0; b < 32; b++)
			{
				int r1 = (r << 3) | (r >> 2);
				int g1 = (g << 3) | (g >> 2);
				int b1 = (b << 3) | (b >> 2);

				UINT16 color = ((r & 1) << 14) | ((r & 0x1e) << 7)
						  | ((g & 1) << 13) | ((g & 0x1e) << 3)
						  | ((b & 1) << 12) | ((b & 0x1e) >> 1);

				video_clut16[color] = MAKECOL15(r1, g1, b1);
			}
		}
	}

	no_of_tiles = memory_length_gfx3 / 128;
	high_tile_mask  = (no_of_tiles > 0x10000) ? 0x10000 : 0;
	high_tile_mask |= (no_of_tiles > 0x20000) ? 0x20000 : 0;
	high_tile_mask |= (no_of_tiles > 0x40000) ? 0x40000 : 0;

	sprite_gfx_code_mask = 0xffffffff;

	for (bit = 0x80000000; bit != 0; bit >>= 1)
	{
		if ((no_of_tiles - 1) & bit)
			break;

		sprite_gfx_code_mask >>= 1;
	}

	skip_fullmode0 = &memory_region_user3[0x100*0x40*0];
	tile_fullmode0 = &memory_region_user3[0x100*0x40*1];
	skip_fullmode1 = &memory_region_user3[0x100*0x40*2];
	tile_fullmode1 = &memory_region_user3[0x100*0x40*3];

	memset(neogeo_videoram, 0, sizeof(neogeo_videoram));
	memset(palettes, 0, sizeof(palettes));
	memset(video_palettebank, 0, sizeof(video_palettebank));

	for (i = 0; i < 0x1000; i += 16)
	{
		video_palettebank[0][i] = 0x8000;
		video_palettebank[1][i] = 0x8000;
	}

	neogeo_video_reset();
}


/*------------------------------------------------------
	ビデオエミュレーション終了
------------------------------------------------------*/

void neogeo_video_exit(void)
{
}


/*------------------------------------------------------
	ビデオエミュレーションリセット
------------------------------------------------------*/

void neogeo_video_reset(void)
{
	video_palette = video_palettebank[0];
	palette_bank = 0;
	videoram_read_buffer = 0;
	videoram_modulo = 0;
	videoram_offset = 0;

	next_update_first_line = FIRST_VISIBLE_LINE;

	if (neogeo_bios >= UNI_V10 && neogeo_bios < DEBUG_BIOS)
		max_sprite_number = 32;
	else if (neogeo_bios == ASIA_AES || neogeo_bios == DEBUG_BIOS)
		max_sprite_number = MAX_SPRITES_PER_SCREEN;
	else
		max_sprite_number = 0;

	draw_fixed_layer_func[0] = draw_fixed_layer_type0;

	switch (neogeo_fix_bank_type)
	{
	case 1:  draw_fixed_layer_func[1] = draw_fixed_layer_type1; break;
	case 2:  draw_fixed_layer_func[1] = draw_fixed_layer_type2; break;
	default: draw_fixed_layer_func[1] = draw_fixed_layer_type0; break;
	}

	if (neogeo_bios == ASIA_AES || neogeo_bios == DEBUG_BIOS)
		neogeo_set_fixed_layer_source(1);
	else
		neogeo_set_fixed_layer_source(0);

	spr_pen_usage = gfx_pen_usage[2];

	blit_reset();
}


/******************************************************************************
	FIXレイヤーバンク処理
******************************************************************************/

/*------------------------------------------------------
	Select fix bank (0x3a000a / 9x3001a)
------------------------------------------------------*/

void neogeo_set_fixed_layer_source(UINT8 data)
{
	fix_bank   = data;
	fix_usage  = gfx_pen_usage[data];
	fix_memory = data ? memory_region_gfx2 : memory_region_gfx1;

	draw_fixed_layer = draw_fixed_layer_func[data];

	blit_set_fix_clear_flag();
}


/******************************************************************************
	画面更新処理
******************************************************************************/

/*------------------------------------------------------
	スクリーン更新
------------------------------------------------------*/

void neogeo_screenrefresh(void)
{
	if (next_update_first_line <= LAST_VISIBLE_LINE)
	{
		blit_start(next_update_first_line, LAST_VISIBLE_LINE);

		if (LAST_VISIBLE_LINE - next_update_first_line < 15)
			draw_sprites_software(next_update_first_line, LAST_VISIBLE_LINE);
		else
			draw_sprites_hardware(next_update_first_line, LAST_VISIBLE_LINE);
	}

	draw_fixed_layer();

	blit_finish();

	next_update_first_line = FIRST_VISIBLE_LINE;
}


/*------------------------------------------------------
	スクリーン部分更新
------------------------------------------------------*/

void neogeo_partial_screenrefresh(int current_line)
{
	if (current_line >= FIRST_VISIBLE_LINE)
	{
		if (current_line >= next_update_first_line)
		{
			if (current_line > LAST_VISIBLE_LINE)
				current_line = LAST_VISIBLE_LINE;

			blit_start(next_update_first_line, current_line);

			if (current_line - next_update_first_line < 15)
				draw_sprites_software(next_update_first_line, current_line);
			else
				draw_sprites_hardware(next_update_first_line, current_line);
		}

		next_update_first_line = current_line + 1;
	}
}


/******************************************************************************
	セーブ/ロード ステート
******************************************************************************/

#ifdef SAVE_STATE

STATE_SAVE( video )
{
	state_save_word(neogeo_videoram, 0x10000);
	state_save_word(palettes[0], 0x1000);
	state_save_word(palettes[1], 0x1000);
	state_save_word(&videoram_read_buffer, 1);
	state_save_word(&videoram_offset, 1);
	state_save_word(&videoram_modulo, 1);

	state_save_byte(&palette_bank, 1);
	state_save_byte(&fix_bank, 1);
}

STATE_LOAD( video )
{
	int i;

	state_load_word(neogeo_videoram, 0x10000);
	state_load_word(palettes[0], 0x1000);
	state_load_word(palettes[1], 0x1000);
	state_load_word(&videoram_read_buffer, 1);
	state_load_word(&videoram_offset, 1);
	state_load_word(&videoram_modulo, 1);

	state_load_byte(&palette_bank, 1);
	state_load_byte(&fix_bank, 1);

	for (i = 0; i < 0x1000; i++)
	{
		if (i & 0x0f)
		{
			video_palettebank[0][i] = video_clut16[palettes[0][i] & 0x7fff];
			video_palettebank[1][i] = video_clut16[palettes[1][i] & 0x7fff];
		}
	}

	video_palette = video_palettebank[palette_bank];

	neogeo_set_fixed_layer_source(fix_bank);
}

#endif /* SAVE_STATE */
