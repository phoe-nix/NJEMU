/******************************************************************************

	sprite.c

	CPS1 スプライトマネージャ

******************************************************************************/

#include "cps1.h"


/******************************************************************************
	定数/マクロ等
******************************************************************************/

#define TEXTURE_HEIGHT		512
#define SCROLLH_MAX_HEIGHT	192

#define MAKE_KEY(code, attr)		(code | ((attr & 0x0f) << 28))
#define MAKE_HIGH_KEY(code, attr)	(code | ((attr & 0x19f) << 16))
#define PSP_UNCACHE_PTR(p)			(((uint32_t)(p)) | 0x40000000)


/******************************************************************************
	プロトタイプ
******************************************************************************/

void (*blit_draw_scroll2)(int16_t x, int16_t y, uint32_t code, uint16_t attr);
static void blit_draw_scroll2_software(int16_t x, int16_t y, uint32_t code, uint16_t attr);
static void blit_draw_scroll2_hardware(int16_t x, int16_t y, uint32_t code, uint16_t attr);

void (*blit_draw_scroll2h)(int16_t x, int16_t y, uint32_t code, uint16_t attr, uint16_t tpens);
static void blit_draw_scroll2h_software(int16_t x, int16_t y, uint32_t code, uint16_t attr, uint16_t tpens);
static void blit_draw_scroll2h_hardware(int16_t x, int16_t y, uint32_t code, uint16_t attr, uint16_t tpens);


/******************************************************************************
	ローカル変数/構造体
******************************************************************************/

typedef struct sprite_t SPRITE;
typedef struct object_t OBJECT;

struct sprite_t
{
	uint32_t key;
	uint32_t used;
	int16_t index;
	uint16_t pal;
	SPRITE *next;
};

struct object_t
{
	uint32_t clut;
	struct Vertex vertices[2];
};

static RECT cps_src_clip = { 64, 16, 64 + 384, 16 + 224 };

static RECT cps_clip[6] =
{
	{ 48, 24, 48 + 384, 24 + 224 },	// option_stretch = 0  (384x224)
	{ 60,  1, 60 + 360,  1 + 270 },	// option_stretch = 1  (360x270  4:3)
	{ 48,  1, 48 + 384,  1 + 270 },	// option_stretch = 2  (384x270 24:17)
	{ 7,   0,   7+ 466,      272 },	// option_stretch = 3  (466x272 12:7)
	{ 0,   1, 480,       1 + 270 },	// option_stretch = 4  (480x270 16:9)
	{ 138, 0, 138 + 204,     272 }  	// option_stretch = 5  (204x272 3:4 vertical)
};

static int16_t scroll2_min_y;
static int16_t scroll2_max_y;
static int16_t scroll2_sy;
static int16_t scroll2_ey;

static uint8_t *pen_usage;
static uint16_t *scrbitmap;


/*------------------------------------------------------------------------
	パレット
------------------------------------------------------------------------*/

static uint8_t ALIGN_DATA palette_dirty_marks[256];


/*------------------------------------------------------------------------
	OBJECT: キャラクタ等
------------------------------------------------------------------------*/

#define OBJECT_HASH_SIZE		0x200
#define OBJECT_HASH_MASK		0x1ff
#define OBJECT_TEXTURE_SIZE		((BUF_WIDTH/16)*(TEXTURE_HEIGHT/16))
#define OBJECT_MAX_SPRITES		0x1000

static SPRITE ALIGN_DATA *object_head[OBJECT_HASH_SIZE];
static SPRITE ALIGN_DATA object_data[OBJECT_TEXTURE_SIZE];
static SPRITE ALIGN_DATA *object_free_head;

static uint8_t *gfx_object;
static uint8_t *tex_object;
static uint16_t object_texture_num;


/*------------------------------------------------------------------------
	SCROLL1: スクロール面1(テキスト等)
------------------------------------------------------------------------*/

#define SCROLL1_HASH_SIZE		0x200
#define SCROLL1_HASH_MASK		0x1ff
#define SCROLL1_TEXTURE_SIZE	((BUF_WIDTH/8)*(TEXTURE_HEIGHT/8))
#define SCROLL1_MAX_SPRITES		((384/8 + 2) * (224/8 + 2))

static SPRITE ALIGN_DATA *scroll1_head[SCROLL1_HASH_SIZE];
static SPRITE ALIGN_DATA scroll1_data[SCROLL1_TEXTURE_SIZE];
static SPRITE ALIGN_DATA *scroll1_free_head;

static uint8_t *gfx_scroll1;
static uint8_t *tex_scroll1;
static uint16_t scroll1_texture_num;


/*------------------------------------------------------------------------
	SCROLL2: スクロール面2
------------------------------------------------------------------------*/

#define SCROLL2_HASH_SIZE		0x100
#define SCROLL2_HASH_MASK		0xff
#define SCROLL2_TEXTURE_SIZE	((BUF_WIDTH/16)*(TEXTURE_HEIGHT/16))
#define SCROLL2_MAX_SPRITES		((384/16 + 2) * (224/16 + 2))

static SPRITE ALIGN_DATA *scroll2_head[SCROLL2_HASH_SIZE];
static SPRITE ALIGN_DATA scroll2_data[SCROLL2_TEXTURE_SIZE];
static SPRITE ALIGN_DATA *scroll2_free_head;

static uint8_t *gfx_scroll2;
static uint8_t *tex_scroll2;
static uint16_t scroll2_texture_num;


/*------------------------------------------------------------------------
	SCROLL3: スクロール面3
------------------------------------------------------------------------*/

#define SCROLL3_HASH_SIZE		0x40
#define SCROLL3_HASH_MASK		0x3f
#define SCROLL3_TEXTURE_SIZE	((BUF_WIDTH/32)*(TEXTURE_HEIGHT/32))
#define SCROLL3_MAX_SPRITES		((384/32 + 2) * (224/32 + 2))

static SPRITE ALIGN_DATA *scroll3_head[SCROLL3_HASH_SIZE];
static SPRITE ALIGN_DATA scroll3_data[SCROLL3_TEXTURE_SIZE];
static SPRITE ALIGN_DATA *scroll3_free_head;

static uint8_t *gfx_scroll3;
static uint8_t *tex_scroll3;
static uint16_t scroll3_texture_num;


/*------------------------------------------------------------------------
	SCROLLH: スクロール面 (ハイプライオリティ)
------------------------------------------------------------------------*/

#define SCROLLH_HASH_SIZE		0x200
#define SCROLLH_HASH_MASK		0x1ff
#define SCROLLH_TEXTURE_SIZE	((BUF_WIDTH/8)*(SCROLLH_MAX_HEIGHT/8))
#define SCROLLH_MAX_SPRITES		SCROLL1_MAX_SPRITES

#define SCROLL1H_TEXTURE_SIZE	SCROLLH_TEXTURE_SIZE
#define SCROLL1H_MAX_SPRITES	SCROLL1_MAX_SPRITES
#define SCROLL2H_TEXTURE_SIZE	((BUF_WIDTH/16)*(SCROLLH_MAX_HEIGHT/16))
#define SCROLL2H_MAX_SPRITES	SCROLL2_MAX_SPRITES
#define SCROLL3H_TEXTURE_SIZE	((BUF_WIDTH/32)*(SCROLLH_MAX_HEIGHT/32))
#define SCROLL3H_MAX_SPRITES	SCROLL3_MAX_SPRITES

static SPRITE ALIGN_DATA *scrollh_head[SCROLLH_HASH_SIZE];
static SPRITE ALIGN_DATA scrollh_data[SCROLLH_TEXTURE_SIZE];
static SPRITE ALIGN_DATA *scrollh_free_head;

static uint16_t *tex_scrollh;
static struct Vertex ALIGN_DATA vertices_scrollh[SCROLLH_MAX_SPRITES * 2];
static uint16_t scrollh_num;
static uint16_t scrollh_texture_num;
static uint8_t  scrollh_texture_clear;
static uint8_t  scrollh_layer_number;
static uint8_t scroll1_palette_is_dirty;
static uint8_t scroll2_palette_is_dirty;
static uint8_t scroll3_palette_is_dirty;


/*------------------------------------------------------------------------
	頂点データ
------------------------------------------------------------------------*/

static OBJECT ALIGN_DATA vertices_object[OBJECT_MAX_SPRITES];

static uint16_t object_num;
static uint16_t object_index;

static struct Vertex ALIGN_DATA vertices_scroll[2][SCROLL1_MAX_SPRITES * 2];


/*------------------------------------------------------------------------
	カラールックアップテーブル
------------------------------------------------------------------------*/

static uint16_t *clut;
static uint16_t clut0_num;
static uint16_t clut1_num;

static const uint32_t ALIGN_DATA color_table[16] =
{
	0x00000000, 0x10101010, 0x20202020, 0x30303030,
	0x40404040, 0x50505050, 0x60606060, 0x70707070,
	0x80808080, 0x90909090, 0xa0a0a0a0, 0xb0b0b0b0,
	0xc0c0c0c0, 0xd0d0d0d0, 0xe0e0e0e0, 0xf0f0f0f0
};


/*------------------------------------------------------------------------
	'swizzle'テクスチャアドレス計算テーブル (8bitカラー)
------------------------------------------------------------------------*/

static const int ALIGN_DATA swizzle_table_8bit[32] =
{
	   0, 16, 16, 16, 16, 16, 16, 16,
	3984, 16, 16, 16, 16, 16, 16, 16,
	3984, 16, 16, 16, 16, 16, 16, 16,
	3984, 16, 16, 16, 16, 16, 16, 16
};


/******************************************************************************
	SCROLL2 ソフトウェア描画
******************************************************************************/

static void drawgfx16_16x16(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines);
static void drawgfx16_16x16_flipx(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines);
static void drawgfx16_16x16_flipy(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines);
static void drawgfx16_16x16_flipxy(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines);

static void drawgfx16_16x16_opaque(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines);
static void drawgfx16_16x16_flipx_opaque(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines);
static void drawgfx16_16x16_flipy_opaque(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines);
static void drawgfx16_16x16_flipxy_opaque(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines);

static void drawgfx16h_16x16(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines, uint16_t tpens);
static void drawgfx16h_16x16_flipx(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines, uint16_t tpens);
static void drawgfx16h_16x16_flipy(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines, uint16_t tpens);
static void drawgfx16h_16x16_flipxy(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines, uint16_t tpens);

static void ALIGN_DATA (*drawgfx16[8])(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines) =
{
	drawgfx16_16x16,
	drawgfx16_16x16_opaque,
	drawgfx16_16x16_flipx,
	drawgfx16_16x16_flipx_opaque,
	drawgfx16_16x16_flipy,
	drawgfx16_16x16_flipy_opaque,
	drawgfx16_16x16_flipxy,
	drawgfx16_16x16_flipxy_opaque
};

static void ALIGN_DATA (*drawgfx16h[4])(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines, uint16_t tpens) =
{
	drawgfx16h_16x16,
	drawgfx16h_16x16_flipx,
	drawgfx16h_16x16_flipy,
	drawgfx16h_16x16_flipxy
};


/*------------------------------------------------------------------------
	16bpp 16x16
------------------------------------------------------------------------*/

static void drawgfx16_16x16(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines)
{
	uint32_t tile, mask;

	while (lines--)
	{
		tile = src[0];
		mask = ~tile;
		if (mask)
		{
			if (mask & 0x000f) dst[ 0] = pal[(tile >>  0) & 0x0f];
			if (mask & 0x00f0) dst[ 4] = pal[(tile >>  4) & 0x0f];
			if (mask & 0x0f00) dst[ 1] = pal[(tile >>  8) & 0x0f];
			if (mask & 0xf000) dst[ 5] = pal[(tile >> 12) & 0x0f];
			mask >>= 16;
			if (mask & 0x000f) dst[ 2] = pal[(tile >> 16) & 0x0f];
			if (mask & 0x00f0) dst[ 6] = pal[(tile >> 20) & 0x0f];
			if (mask & 0x0f00) dst[ 3] = pal[(tile >> 24) & 0x0f];
			if (mask & 0xf000) dst[ 7] = pal[(tile >> 28) & 0x0f];
		}
		tile = src[1];
		mask = ~tile;
		if (mask)
		{
			if (mask & 0x000f) dst[ 8] = pal[(tile >>  0) & 0x0f];
			if (mask & 0x00f0) dst[12] = pal[(tile >>  4) & 0x0f];
			if (mask & 0x0f00) dst[ 9] = pal[(tile >>  8) & 0x0f];
			if (mask & 0xf000) dst[13] = pal[(tile >> 12) & 0x0f];
			mask >>= 16;
			if (mask & 0x000f) dst[10] = pal[(tile >> 16) & 0x0f];
			if (mask & 0x00f0) dst[14] = pal[(tile >> 20) & 0x0f];
			if (mask & 0x0f00) dst[11] = pal[(tile >> 24) & 0x0f];
			if (mask & 0xf000) dst[15] = pal[(tile >> 28) & 0x0f];
		}
		src += 2;
		dst += BUF_WIDTH;
	}
}

static void drawgfx16_16x16_flipx(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines)
{
	uint32_t tile, mask;

	while (lines--)
	{
		tile = src[0];
		mask = ~tile;
		if (mask)
		{
			if (mask & 0x000f) dst[15] = pal[(tile >>  0) & 0x0f];
			if (mask & 0x00f0) dst[11] = pal[(tile >>  4) & 0x0f];
			if (mask & 0x0f00) dst[14] = pal[(tile >>  8) & 0x0f];
			if (mask & 0xf000) dst[10] = pal[(tile >> 12) & 0x0f];
			mask >>= 16;
			if (mask & 0x000f) dst[13] = pal[(tile >> 16) & 0x0f];
			if (mask & 0x00f0) dst[ 9] = pal[(tile >> 20) & 0x0f];
			if (mask & 0x0f00) dst[12] = pal[(tile >> 24) & 0x0f];
			if (mask & 0xf000) dst[ 8] = pal[(tile >> 28) & 0x0f];
		}
		tile = src[1];
		mask = ~tile;
		if (mask)
		{
			if (mask & 0x000f) dst[ 7] = pal[(tile >>  0) & 0x0f];
			if (mask & 0x00f0) dst[ 3] = pal[(tile >>  4) & 0x0f];
			if (mask & 0x0f00) dst[ 6] = pal[(tile >>  8) & 0x0f];
			if (mask & 0xf000) dst[ 2] = pal[(tile >> 12) & 0x0f];
			mask >>= 16;
			if (mask & 0x000f) dst[ 5] = pal[(tile >> 16) & 0x0f];
			if (mask & 0x00f0) dst[ 1] = pal[(tile >> 20) & 0x0f];
			if (mask & 0x0f00) dst[ 4] = pal[(tile >> 24) & 0x0f];
			if (mask & 0xf000) dst[ 0] = pal[(tile >> 28) & 0x0f];
		}
		src += 2;
		dst += BUF_WIDTH;
	}
}

static void drawgfx16_16x16_flipy(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines)
{
	uint32_t tile, mask;

	while (lines--)
	{
		tile = src[0];
		mask = ~tile;
		if (mask)
		{
			if (mask & 0x000f) dst[ 0] = pal[(tile >>  0) & 0x0f];
			if (mask & 0x00f0) dst[ 4] = pal[(tile >>  4) & 0x0f];
			if (mask & 0x0f00) dst[ 1] = pal[(tile >>  8) & 0x0f];
			if (mask & 0xf000) dst[ 5] = pal[(tile >> 12) & 0x0f];
			mask >>= 16;
			if (mask & 0x000f) dst[ 2] = pal[(tile >> 16) & 0x0f];
			if (mask & 0x00f0) dst[ 6] = pal[(tile >> 20) & 0x0f];
			if (mask & 0x0f00) dst[ 3] = pal[(tile >> 24) & 0x0f];
			if (mask & 0xf000) dst[ 7] = pal[(tile >> 28) & 0x0f];
		}
		tile = src[1];
		mask = ~tile;
		if (mask)
		{
			if (mask & 0x000f) dst[ 8] = pal[(tile >>  0) & 0x0f];
			if (mask & 0x00f0) dst[12] = pal[(tile >>  4) & 0x0f];
			if (mask & 0x0f00) dst[ 9] = pal[(tile >>  8) & 0x0f];
			if (mask & 0xf000) dst[13] = pal[(tile >> 12) & 0x0f];
			mask >>= 16;
			if (mask & 0x000f) dst[10] = pal[(tile >> 16) & 0x0f];
			if (mask & 0x00f0) dst[14] = pal[(tile >> 20) & 0x0f];
			if (mask & 0x0f00) dst[11] = pal[(tile >> 24) & 0x0f];
			if (mask & 0xf000) dst[15] = pal[(tile >> 28) & 0x0f];
		}
		src += 2;
		dst -= BUF_WIDTH;
	}
}

static void drawgfx16_16x16_flipxy(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines)
{
	uint32_t tile, mask;

	while (lines--)
	{
		tile = src[0];
		mask = ~tile;
		if (mask)
		{
			if (mask & 0x000f) dst[15] = pal[(tile >>  0) & 0x0f];
			if (mask & 0x00f0) dst[11] = pal[(tile >>  4) & 0x0f];
			if (mask & 0x0f00) dst[14] = pal[(tile >>  8) & 0x0f];
			if (mask & 0xf000) dst[10] = pal[(tile >> 12) & 0x0f];
			mask >>= 16;
			if (mask & 0x000f) dst[13] = pal[(tile >> 16) & 0x0f];
			if (mask & 0x00f0) dst[ 9] = pal[(tile >> 20) & 0x0f];
			if (mask & 0x0f00) dst[12] = pal[(tile >> 24) & 0x0f];
			if (mask & 0xf000) dst[ 8] = pal[(tile >> 28) & 0x0f];
		}
		tile = src[1];
		mask = ~tile;
		if (mask)
		{
			if (mask & 0x000f) dst[ 7] = pal[(tile >>  0) & 0x0f];
			if (mask & 0x00f0) dst[ 3] = pal[(tile >>  4) & 0x0f];
			if (mask & 0x0f00) dst[ 6] = pal[(tile >>  8) & 0x0f];
			if (mask & 0xf000) dst[ 2] = pal[(tile >> 12) & 0x0f];
			mask >>= 16;
			if (mask & 0x000f) dst[ 5] = pal[(tile >> 16) & 0x0f];
			if (mask & 0x00f0) dst[ 1] = pal[(tile >> 20) & 0x0f];
			if (mask & 0x0f00) dst[ 4] = pal[(tile >> 24) & 0x0f];
			if (mask & 0xf000) dst[ 0] = pal[(tile >> 28) & 0x0f];
		}
		src += 2;
		dst -= BUF_WIDTH;
	}
}


/*------------------------------------------------------------------------
	16bpp 16x16 (透過なし)
------------------------------------------------------------------------*/

static void drawgfx16_16x16_opaque(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines)
{
	uint32_t tile;

	while (lines--)
	{
		tile = src[0];
		dst[ 0] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 4] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 1] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 5] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 2] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 6] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 3] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 7] = pal[tile & 0x0f];
		tile = src[1];
		dst[ 8] = pal[tile & 0x0f]; tile >>= 4;
		dst[12] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 9] = pal[tile & 0x0f]; tile >>= 4;
		dst[13] = pal[tile & 0x0f]; tile >>= 4;
		dst[10] = pal[tile & 0x0f]; tile >>= 4;
		dst[14] = pal[tile & 0x0f]; tile >>= 4;
		dst[11] = pal[tile & 0x0f]; tile >>= 4;
		dst[15] = pal[tile & 0x0f];
		src += 2;
		dst += BUF_WIDTH;
	}
}

static void drawgfx16_16x16_flipx_opaque(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines)
{
	uint32_t tile;

	while (lines--)
	{
		tile = src[0];
		dst[15] = pal[tile & 0x0f]; tile >>= 4;
		dst[11] = pal[tile & 0x0f]; tile >>= 4;
		dst[14] = pal[tile & 0x0f]; tile >>= 4;
		dst[10] = pal[tile & 0x0f]; tile >>= 4;
		dst[13] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 9] = pal[tile & 0x0f]; tile >>= 4;
		dst[12] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 8] = pal[tile & 0x0f];
		tile = src[1];
		dst[ 7] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 3] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 6] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 2] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 5] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 1] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 4] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 0] = pal[tile & 0x0f];
		src += 2;
		dst += BUF_WIDTH;
	}
}

static void drawgfx16_16x16_flipy_opaque(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines)
{
	uint32_t tile;

	while (lines--)
	{
		tile = src[0];
		dst[ 0] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 4] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 1] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 5] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 2] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 6] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 3] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 7] = pal[tile & 0x0f];
		tile = src[1];
		dst[ 8] = pal[tile & 0x0f]; tile >>= 4;
		dst[12] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 9] = pal[tile & 0x0f]; tile >>= 4;
		dst[13] = pal[tile & 0x0f]; tile >>= 4;
		dst[10] = pal[tile & 0x0f]; tile >>= 4;
		dst[14] = pal[tile & 0x0f]; tile >>= 4;
		dst[11] = pal[tile & 0x0f]; tile >>= 4;
		dst[15] = pal[tile & 0x0f];
		src += 2;
		dst -= BUF_WIDTH;
	}
}

static void drawgfx16_16x16_flipxy_opaque(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines)
{
	uint32_t tile;

	while (lines--)
	{
		tile = src[0];
		dst[15] = pal[tile & 0x0f]; tile >>= 4;
		dst[11] = pal[tile & 0x0f]; tile >>= 4;
		dst[14] = pal[tile & 0x0f]; tile >>= 4;
		dst[10] = pal[tile & 0x0f]; tile >>= 4;
		dst[13] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 9] = pal[tile & 0x0f]; tile >>= 4;
		dst[12] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 8] = pal[tile & 0x0f];
		tile = src[1];
		dst[ 7] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 3] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 6] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 2] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 5] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 1] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 4] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 0] = pal[tile & 0x0f];
		src += 2;
		dst -= BUF_WIDTH;
	}
}


/*------------------------------------------------------------------------
	16bpp 16x16 (ハイレイヤー)
------------------------------------------------------------------------*/

static void drawgfx16h_16x16(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines, uint16_t tpens)
{
	uint32_t tile;
	uint16_t col;

	while (lines--)
	{
		tile = src[0];
		if (~tile)
		{
			col = (tile >>  0) & 0x0f; if (tpens & (1 << col)) dst[ 0] = pal[col];
			col = (tile >>  4) & 0x0f; if (tpens & (1 << col)) dst[ 4] = pal[col];
			col = (tile >>  8) & 0x0f; if (tpens & (1 << col)) dst[ 1] = pal[col];
			col = (tile >> 12) & 0x0f; if (tpens & (1 << col)) dst[ 5] = pal[col];
			col = (tile >> 16) & 0x0f; if (tpens & (1 << col)) dst[ 2] = pal[col];
			col = (tile >> 20) & 0x0f; if (tpens & (1 << col)) dst[ 6] = pal[col];
			col = (tile >> 24) & 0x0f; if (tpens & (1 << col)) dst[ 3] = pal[col];
			col = (tile >> 28) & 0x0f; if (tpens & (1 << col)) dst[ 7] = pal[col];
		}
		tile = src[1];
		if (~tile)
		{
			col = (tile >>  0) & 0x0f; if (tpens & (1 << col)) dst[ 8] = pal[col];
			col = (tile >>  4) & 0x0f; if (tpens & (1 << col)) dst[12] = pal[col];
			col = (tile >>  8) & 0x0f; if (tpens & (1 << col)) dst[ 9] = pal[col];
			col = (tile >> 12) & 0x0f; if (tpens & (1 << col)) dst[13] = pal[col];
			col = (tile >> 16) & 0x0f; if (tpens & (1 << col)) dst[10] = pal[col];
			col = (tile >> 20) & 0x0f; if (tpens & (1 << col)) dst[14] = pal[col];
			col = (tile >> 24) & 0x0f; if (tpens & (1 << col)) dst[11] = pal[col];
			col = (tile >> 28) & 0x0f; if (tpens & (1 << col)) dst[15] = pal[col];
		}
		src += 2;
		dst += BUF_WIDTH;
	}
}

static void drawgfx16h_16x16_flipx(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines, uint16_t tpens)
{
	uint32_t tile;
	uint16_t col;

	while (lines--)
	{
		tile = src[0];
		if (~tile)
		{
			col = (tile >>  0) & 0x0f; if (tpens & (1 << col)) dst[15] = pal[col];
			col = (tile >>  4) & 0x0f; if (tpens & (1 << col)) dst[11] = pal[col];
			col = (tile >>  8) & 0x0f; if (tpens & (1 << col)) dst[14] = pal[col];
			col = (tile >> 12) & 0x0f; if (tpens & (1 << col)) dst[10] = pal[col];
			col = (tile >> 16) & 0x0f; if (tpens & (1 << col)) dst[13] = pal[col];
			col = (tile >> 20) & 0x0f; if (tpens & (1 << col)) dst[ 9] = pal[col];
			col = (tile >> 24) & 0x0f; if (tpens & (1 << col)) dst[12] = pal[col];
			col = (tile >> 28) & 0x0f; if (tpens & (1 << col)) dst[ 8] = pal[col];
		}
		tile = src[1];
		if (~tile)
		{
			col = (tile >>  0) & 0x0f; if (tpens & (1 << col)) dst[ 7] = pal[col];
			col = (tile >>  4) & 0x0f; if (tpens & (1 << col)) dst[ 3] = pal[col];
			col = (tile >>  8) & 0x0f; if (tpens & (1 << col)) dst[ 6] = pal[col];
			col = (tile >> 12) & 0x0f; if (tpens & (1 << col)) dst[ 2] = pal[col];
			col = (tile >> 16) & 0x0f; if (tpens & (1 << col)) dst[ 5] = pal[col];
			col = (tile >> 20) & 0x0f; if (tpens & (1 << col)) dst[ 1] = pal[col];
			col = (tile >> 24) & 0x0f; if (tpens & (1 << col)) dst[ 4] = pal[col];
			col = (tile >> 28) & 0x0f; if (tpens & (1 << col)) dst[ 0] = pal[col];
		}
		src += 2;
		dst += BUF_WIDTH;
	}
}

static void drawgfx16h_16x16_flipy(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines, uint16_t tpens)
{
	uint32_t tile;
	uint16_t col;

	while (lines--)
	{
		tile = src[0];
		if (~tile)
		{
			col = (tile >>  0) & 0x0f; if (tpens & (1 << col)) dst[ 0] = pal[col];
			col = (tile >>  4) & 0x0f; if (tpens & (1 << col)) dst[ 4] = pal[col];
			col = (tile >>  8) & 0x0f; if (tpens & (1 << col)) dst[ 1] = pal[col];
			col = (tile >> 12) & 0x0f; if (tpens & (1 << col)) dst[ 5] = pal[col];
			col = (tile >> 16) & 0x0f; if (tpens & (1 << col)) dst[ 2] = pal[col];
			col = (tile >> 20) & 0x0f; if (tpens & (1 << col)) dst[ 6] = pal[col];
			col = (tile >> 24) & 0x0f; if (tpens & (1 << col)) dst[ 3] = pal[col];
			col = (tile >> 28) & 0x0f; if (tpens & (1 << col)) dst[ 7] = pal[col];
		}
		tile = src[1];
		if (~tile)
		{
			col = (tile >>  0) & 0x0f; if (tpens & (1 << col)) dst[ 8] = pal[col];
			col = (tile >>  4) & 0x0f; if (tpens & (1 << col)) dst[12] = pal[col];
			col = (tile >>  8) & 0x0f; if (tpens & (1 << col)) dst[ 9] = pal[col];
			col = (tile >> 12) & 0x0f; if (tpens & (1 << col)) dst[13] = pal[col];
			col = (tile >> 16) & 0x0f; if (tpens & (1 << col)) dst[10] = pal[col];
			col = (tile >> 20) & 0x0f; if (tpens & (1 << col)) dst[14] = pal[col];
			col = (tile >> 24) & 0x0f; if (tpens & (1 << col)) dst[11] = pal[col];
			col = (tile >> 28) & 0x0f; if (tpens & (1 << col)) dst[15] = pal[col];
		}
		src += 2;
		dst -= BUF_WIDTH;
	}
}

static void drawgfx16h_16x16_flipxy(uint32_t *src, uint16_t *dst, uint16_t *pal, int lines, uint16_t tpens)
{
	uint32_t tile;
	uint16_t col;

	while (lines--)
	{
		tile = src[0];
		if (~tile)
		{
			col = (tile >>  0) & 0x0f; if (tpens & (1 << col)) dst[15] = pal[col];
			col = (tile >>  4) & 0x0f; if (tpens & (1 << col)) dst[11] = pal[col];
			col = (tile >>  8) & 0x0f; if (tpens & (1 << col)) dst[14] = pal[col];
			col = (tile >> 12) & 0x0f; if (tpens & (1 << col)) dst[10] = pal[col];
			col = (tile >> 16) & 0x0f; if (tpens & (1 << col)) dst[13] = pal[col];
			col = (tile >> 20) & 0x0f; if (tpens & (1 << col)) dst[ 9] = pal[col];
			col = (tile >> 24) & 0x0f; if (tpens & (1 << col)) dst[12] = pal[col];
			col = (tile >> 28) & 0x0f; if (tpens & (1 << col)) dst[ 8] = pal[col];
		}
		tile = src[1];
		if (~tile)
		{
			col = (tile >>  0) & 0x0f; if (tpens & (1 << col)) dst[ 7] = pal[col];
			col = (tile >>  4) & 0x0f; if (tpens & (1 << col)) dst[ 3] = pal[col];
			col = (tile >>  8) & 0x0f; if (tpens & (1 << col)) dst[ 6] = pal[col];
			col = (tile >> 12) & 0x0f; if (tpens & (1 << col)) dst[ 2] = pal[col];
			col = (tile >> 16) & 0x0f; if (tpens & (1 << col)) dst[ 5] = pal[col];
			col = (tile >> 20) & 0x0f; if (tpens & (1 << col)) dst[ 1] = pal[col];
			col = (tile >> 24) & 0x0f; if (tpens & (1 << col)) dst[ 4] = pal[col];
			col = (tile >> 28) & 0x0f; if (tpens & (1 << col)) dst[ 0] = pal[col];
		}
		src += 2;
		dst -= BUF_WIDTH;
	}
}


/******************************************************************************
	OBJECTスプライト管理
******************************************************************************/

/*------------------------------------------------------------------------
	OBJECTテクスチャからスプライト番号を取得
------------------------------------------------------------------------*/

static int16_t object_get_sprite(uint32_t key)
{
	SPRITE *p = object_head[key & OBJECT_HASH_MASK];

	while (p)
	{
		if (p->key == key)
		{
			p->used = frames_displayed;
			return p->index;
	 	}
		p = p->next;
	}
	return -1;
}


/*------------------------------------------------------------------------
	OBJECTテクスチャにスプライトを登録
------------------------------------------------------------------------*/

static int16_t object_insert_sprite(uint32_t key)
{
	uint16_t hash = key & OBJECT_HASH_MASK;
	SPRITE *p = object_head[hash];
	SPRITE *q = object_free_head;

	if (!q) return -1;

	object_free_head = object_free_head->next;

	q->next = NULL;
	q->key  = key;
	q->used = frames_displayed;

	if (!p)
	{
		object_head[hash] = q;
	}
	else
	{
		while (p->next) p = p->next;
		p->next = q;
	}

	object_texture_num++;

	return q->index;
}


/*------------------------------------------------------------------------
	OBJECTテクスチャから一定時間を経過したスプライトを削除
------------------------------------------------------------------------*/

static void object_delete_sprite(void)
{
	int i;
	SPRITE *p, *prev_p;

	for (i = 0; i < OBJECT_HASH_SIZE; i++)
	{
		prev_p = NULL;
		p = object_head[i];

		while (p)
		{
			if (frames_displayed != p->used)
			{
				object_texture_num--;

				if (!prev_p)
				{
					object_head[i] = p->next;
					p->next = object_free_head;
					object_free_head = p;
					p = object_head[i];
				}
				else
				{
					prev_p->next = p->next;
					p->next = object_free_head;
					object_free_head = p;
					p = prev_p->next;
				}
			}
			else
			{
				prev_p = p;
				p = p->next;
			}
		}
	}
}


/******************************************************************************
	SCROLL1スプライト管理
******************************************************************************/

/*------------------------------------------------------------------------
	SCROLL1テクスチャからスプライト番号を取得
------------------------------------------------------------------------*/

static int16_t scroll1_get_sprite(uint32_t key)
{
	SPRITE *p = scroll1_head[key & SCROLL1_HASH_MASK];

	while (p)
	{
		if (p->key == key)
		{
			p->used = frames_displayed;
			return p->index;
	 	}
		p = p->next;
	}
	return -1;
}


/*------------------------------------------------------------------------
	SCROLL1テクスチャにスプライトを登録
------------------------------------------------------------------------*/

static int16_t scroll1_insert_sprite(uint32_t key)
{
	uint16_t hash = key & SCROLL1_HASH_MASK;
	SPRITE *p = scroll1_head[hash];
	SPRITE *q = scroll1_free_head;

	if (!q) return -1;

	scroll1_free_head = scroll1_free_head->next;

	q->next = NULL;
	q->key  = key;
	q->used = frames_displayed;

	if (!p)
	{
		scroll1_head[hash] = q;
	}
	else
	{
		while (p->next) p = p->next;
		p->next = q;
	}

	scroll1_texture_num++;

	return q->index;
}


/*------------------------------------------------------------------------
	SCROLL1テクスチャから一定時間を経過したスプライトを削除
------------------------------------------------------------------------*/

static void scroll1_delete_sprite(void)
{
	int i;
	SPRITE *p, *prev_p;

	for (i = 0; i < SCROLL1_HASH_SIZE; i++)
	{
		prev_p = NULL;
		p = scroll1_head[i];

		while (p)
		{
			if (frames_displayed != p->used)
			{
				scroll1_texture_num--;

				if (!prev_p)
				{
					scroll1_head[i] = p->next;
					p->next = scroll1_free_head;
					scroll1_free_head = p;
					p = scroll1_head[i];
				}
				else
				{
					prev_p->next = p->next;
					p->next = scroll1_free_head;
					scroll1_free_head = p;
					p = prev_p->next;
				}
			}
			else
			{
				prev_p = p;
				p = p->next;
			}
		}
	}
}


/******************************************************************************
	SCROLL2スプライト管理
******************************************************************************/

/*------------------------------------------------------------------------
	SCROLL2テクスチャからスプライト番号を取得
------------------------------------------------------------------------*/

static int16_t scroll2_get_sprite(uint32_t key)
{
	SPRITE *p = scroll2_head[key & SCROLL2_HASH_MASK];

	while (p)
	{
		if (p->key == key)
		{
			p->used = frames_displayed;
			return p->index;
	 	}
		p = p->next;
	}
	return -1;
}


/*------------------------------------------------------------------------
	SCROLL2テクスチャにスプライトを登録
------------------------------------------------------------------------*/

static int16_t scroll2_insert_sprite(uint32_t key)
{
	uint16_t hash = key & SCROLL2_HASH_MASK;
	SPRITE *p = scroll2_head[hash];
	SPRITE *q = scroll2_free_head;

	if (!q) return -1;

	scroll2_free_head = scroll2_free_head->next;

	q->next = NULL;
	q->key  = key;
	q->used = frames_displayed;

	if (!p)
	{
		scroll2_head[hash] = q;
	}
	else
	{
		while (p->next) p = p->next;
		p->next = q;
	}

	scroll2_texture_num++;

	return q->index;
}


/*------------------------------------------------------------------------
	SCROLL2テクスチャから一定時間を経過したスプライトを削除
------------------------------------------------------------------------*/

static void scroll2_delete_sprite(void)
{
	int i;
	SPRITE *p, *prev_p;

	for (i = 0; i < SCROLL2_HASH_SIZE; i++)
	{
		prev_p = NULL;
		p = scroll2_head[i];

		while (p)
		{
			if (frames_displayed != p->used)
			{
				scroll2_texture_num--;

				if (!prev_p)
				{
					scroll2_head[i] = p->next;
					p->next = scroll2_free_head;
					scroll2_free_head = p;
					p = scroll2_head[i];
				}
				else
				{
					prev_p->next = p->next;
					p->next = scroll2_free_head;
					scroll2_free_head = p;
					p = prev_p->next;
				}
			}
			else
			{
				prev_p = p;
				p = p->next;
			}
		}
	}
}


/******************************************************************************
	SCROLL3スプライト管理
******************************************************************************/

/*------------------------------------------------------------------------
	SCROLL3テクスチャからスプライト番号を取得
------------------------------------------------------------------------*/

static int16_t scroll3_get_sprite(uint32_t key)
{
	SPRITE *p = scroll3_head[key & SCROLL3_HASH_MASK];

	while (p)
	{
		if (p->key == key)
		{
			p->used = frames_displayed;
			return p->index;
	 	}
		p = p->next;
	}
	return -1;
}


/*------------------------------------------------------------------------
	SCROLL3テクスチャにスプライトを登録
------------------------------------------------------------------------*/

static int16_t scroll3_insert_sprite(uint32_t key)
{
	uint16_t hash = key & SCROLL3_HASH_MASK;
	SPRITE *p = scroll3_head[hash];
	SPRITE *q = scroll3_free_head;

	if (!q) return -1;

	scroll3_free_head = scroll3_free_head->next;

	q->next = NULL;
	q->key  = key;
	q->used = frames_displayed;

	if (!p)
	{
		scroll3_head[hash] = q;
	}
	else
	{
		while (p->next) p = p->next;
		p->next = q;
	}

	scroll3_texture_num++;

	return q->index;
}


/*------------------------------------------------------------------------
	SCROLL3テクスチャから一定時間を経過したスプライトを削除
------------------------------------------------------------------------*/

static void scroll3_delete_sprite(void)
{
	int i;
	SPRITE *p, *prev_p;

	for (i = 0; i < SCROLL3_HASH_SIZE; i++)
	{
		prev_p = NULL;
		p = scroll3_head[i];

		while (p)
		{
			if (frames_displayed != p->used)
			{
				scroll3_texture_num--;

				if (!prev_p)
				{
					scroll3_head[i] = p->next;
					p->next = scroll3_free_head;
					scroll3_free_head = p;
					p = scroll3_head[i];
				}
				else
				{
					prev_p->next = p->next;
					p->next = scroll3_free_head;
					scroll3_free_head = p;
					p = prev_p->next;
				}
			}
			else
			{
				prev_p = p;
				p = p->next;
			}
		}
	}
}


/******************************************************************************
	SCROLLHスプライト管理
******************************************************************************/

/*------------------------------------------------------------------------
	SCROLLHテクスチャをリセットする
------------------------------------------------------------------------*/

static void scrollh_reset_sprite(void)
{
	int i;

	for (i = 0; i < SCROLLH_TEXTURE_SIZE - 1; i++)
		scrollh_data[i].next = &scrollh_data[i + 1];

	scrollh_data[i].next = NULL;
	scrollh_free_head = &scrollh_data[0];

	memset(scrollh_head, 0, sizeof(SPRITE *) * SCROLLH_HASH_SIZE);

	scrollh_texture_num = 0;
	scrollh_texture_clear = 0;
	scrollh_layer_number = 0;

	scroll1_palette_is_dirty = 0;
	scroll2_palette_is_dirty = 0;
	scroll3_palette_is_dirty = 0;
}


/*------------------------------------------------------------------------
	SCROLLHテクスチャからスプライト番号を取得
------------------------------------------------------------------------*/

static int16_t scrollh_get_sprite(uint32_t key)
{
	SPRITE *p = scrollh_head[key & SCROLLH_HASH_MASK];

	while (p)
	{
		if (p->key == key)
		{
			p->used = frames_displayed;
			return p->index;
	 	}
		p = p->next;
	}
	return -1;
}


/*------------------------------------------------------------------------
	SCROLLHテクスチャにスプライトを登録
------------------------------------------------------------------------*/

static int16_t scrollh_insert_sprite(uint32_t key)
{
	uint16_t hash = key & SCROLLH_HASH_MASK;
	SPRITE *p = scrollh_head[hash];
	SPRITE *q = scrollh_free_head;

	if (!q) return -1;

	scrollh_free_head = scrollh_free_head->next;

	q->next = NULL;
	q->key  = key;
	q->pal  = (key >> 16) & 0x1f;
	q->used = frames_displayed;

	if (!p)
	{
		scrollh_head[hash] = q;
	}
	else
	{
		while (p->next) p = p->next;
		p->next = q;
	}

	scrollh_texture_num++;

	return q->index;
}


/*------------------------------------------------------------------------
	SCROLLHテクスチャから一定時間を経過したスプライトを削除
------------------------------------------------------------------------*/

static void scrollh_delete_sprite(void)
{
	int i;
	SPRITE *p, *prev_p;

	for (i = 0; i < SCROLLH_HASH_SIZE; i++)
	{
		prev_p = NULL;
		p = scrollh_head[i];

		while (p)
		{
			if (frames_displayed != p->used)
			{
				scrollh_texture_num--;

				if (!prev_p)
				{
					scrollh_head[i] = p->next;
					p->next = scrollh_free_head;
					scrollh_free_head = p;
					p = scrollh_head[i];
				}
				else
				{
					prev_p->next = p->next;
					p->next = scrollh_free_head;
					scrollh_free_head = p;
					p = prev_p->next;
				}
			}
			else
			{
				prev_p = p;
				p = p->next;
			}
		}
	}
}


/*------------------------------------------------------------------------
	SCROLLHキャッシュから指定した透過ペンのテクスチャを削除
------------------------------------------------------------------------*/

static void scrollh_delete_sprite_tpens(uint16_t tpens)
{
	int i;
	SPRITE *p, *prev_p;

	for (i = 0; i < SCROLLH_HASH_SIZE; i++)
	{
		prev_p = NULL;
		p = scrollh_head[i];

		while (p)
		{
			if ((p->key >> 23) == tpens)
			{
				scrollh_texture_num--;

				if (!prev_p)
				{
					scrollh_head[i] = p->next;
					p->next = scrollh_free_head;
					scrollh_free_head = p;
					p = scrollh_head[i];
				}
				else
				{
					prev_p->next = p->next;
					p->next = scrollh_free_head;
					scrollh_free_head = p;
					p = prev_p->next;
				}
			}
			else
			{
				prev_p = p;
				p = p->next;
			}
		}
	}
}


/*------------------------------------------------------------------------
	SCROLLHテクスチャからパレットが変更されたスプライトを削除
------------------------------------------------------------------------*/

static void scrollh_delete_dirty_palette(void)
{
	int palno;
	uint8_t *dirty_flags = NULL;

	switch (scrollh_layer_number)
	{
	case LAYER_SCROLL1: if (scroll1_palette_is_dirty) dirty_flags = &palette_dirty_marks[32]; break;
	case LAYER_SCROLL2: if (scroll2_palette_is_dirty) dirty_flags = &palette_dirty_marks[64]; break;
	case LAYER_SCROLL3: if (scroll3_palette_is_dirty) dirty_flags = &palette_dirty_marks[96]; break;
	}

	scroll1_palette_is_dirty = 0;
	scroll2_palette_is_dirty = 0;
	scroll3_palette_is_dirty = 0;

	if (!dirty_flags || !scrollh_texture_num) return;

	for (palno = 0; palno < 32; palno++)
	{
		int i;
		SPRITE *p, *prev_p;

		if (!dirty_flags[palno]) continue;

		for (i = 0; i < SCROLLH_HASH_SIZE; i++)
		{
			prev_p = NULL;
			p = scrollh_head[i];

			while (p)
			{
				if (p->pal == palno)
				{
					scrollh_texture_num--;

					if (!prev_p)
					{
						scrollh_head[i] = p->next;
						p->next = scrollh_free_head;
						scrollh_free_head = p;
						p = scrollh_head[i];
					}
					else
					{
						prev_p->next = p->next;
						p->next = scrollh_free_head;
						scrollh_free_head = p;
						p = prev_p->next;
					}
				}
				else
				{
					prev_p = p;
					p = p->next;
				}
			}
		}
	}
}


/******************************************************************************
	スプライト描画インタフェース関数
******************************************************************************/

/*------------------------------------------------------------------------
	全てのスプライトを即座にクリアする
------------------------------------------------------------------------*/

void blit_clear_all_sprite(void)
{
	int i;

	for (i = 0; i < OBJECT_TEXTURE_SIZE - 1; i++)
		object_data[i].next = &object_data[i + 1];

	object_data[i].next = NULL;
	object_free_head = &object_data[0];

	for (i = 0; i < SCROLL1_TEXTURE_SIZE - 1; i++)
		scroll1_data[i].next = &scroll1_data[i + 1];

	scroll1_data[i].next = NULL;
	scroll1_free_head = &scroll1_data[0];

	for (i = 0; i < SCROLL2_TEXTURE_SIZE - 1; i++)
		scroll2_data[i].next = &scroll2_data[i + 1];

	scroll2_data[i].next = NULL;
	scroll2_free_head = &scroll2_data[0];

	for (i = 0; i < SCROLL3_TEXTURE_SIZE - 1; i++)
		scroll3_data[i].next = &scroll3_data[i + 1];

	scroll3_data[i].next = NULL;
	scroll3_free_head = &scroll3_data[0];

	memset(object_head, 0, sizeof(SPRITE *) * OBJECT_HASH_SIZE);
	memset(scroll1_head, 0, sizeof(SPRITE *) * SCROLL1_HASH_SIZE);
	memset(scroll2_head, 0, sizeof(SPRITE *) * SCROLL2_HASH_SIZE);
	memset(scroll3_head, 0, sizeof(SPRITE *) * SCROLL3_HASH_SIZE);

	object_texture_num = 0;
	scroll1_texture_num = 0;
	scroll2_texture_num = 0;
	scroll3_texture_num = 0;

	scrollh_reset_sprite();
	memset(palette_dirty_marks, 0, sizeof(palette_dirty_marks));
}


/*------------------------------------------------------------------------
	ハイレイヤーをクリアする
------------------------------------------------------------------------*/

void blit_scrollh_clear_sprite(uint16_t tpens)
{
	scrollh_delete_sprite_tpens(tpens);
}


/*------------------------------------------------------------------------
	パレット変更フラグを立てる
------------------------------------------------------------------------*/

void blit_palette_mark_dirty(int palno)
{
	if (palno < 64) scroll1_palette_is_dirty = 1;
	else if (palno < 96) scroll2_palette_is_dirty = 1;
	else if (palno < 128) scroll3_palette_is_dirty = 1;

	palette_dirty_marks[palno] = 1;
}


/*------------------------------------------------------------------------
	スプライト処理のリセット
------------------------------------------------------------------------*/

void blit_reset(int bank_scroll1, int bank_scroll2, int bank_scroll3, uint8_t *pen_usage16)
{
	int i;

	scrbitmap   = (uint16_t *)video_driver->frameAddr(NULL, work_frame, 0, 0);
	tex_scrollh = scrbitmap + BUF_WIDTH * SCR_HEIGHT;
	tex_object  = (uint8_t *)(tex_scrollh + BUF_WIDTH * SCROLLH_MAX_HEIGHT);
	tex_scroll1 = tex_object  + BUF_WIDTH * TEXTURE_HEIGHT;
	tex_scroll2 = tex_scroll1 + BUF_WIDTH * TEXTURE_HEIGHT;
	tex_scroll3 = tex_scroll2 + BUF_WIDTH * TEXTURE_HEIGHT;

	for (i = 0; i < OBJECT_TEXTURE_SIZE; i++) object_data[i].index = i;
	for (i = 0; i < SCROLL1_TEXTURE_SIZE; i++) scroll1_data[i].index = i;
	for (i = 0; i < SCROLL2_TEXTURE_SIZE; i++) scroll2_data[i].index = i;
	for (i = 0; i < SCROLL3_TEXTURE_SIZE; i++) scroll3_data[i].index = i;
	for (i = 0; i < SCROLLH_TEXTURE_SIZE; i++) scrollh_data[i].index = i;

	gfx_object  = memory_region_gfx1;
	gfx_scroll1 = &memory_region_gfx1[bank_scroll1 << 21];
	gfx_scroll2 = &memory_region_gfx1[bank_scroll2 << 21];
	gfx_scroll3 = &memory_region_gfx1[bank_scroll3 << 21];

	pen_usage = pen_usage16;
	clut = (uint16_t *)PSP_UNCACHE_PTR(&video_palette);

	blit_clear_all_sprite();
}


/*------------------------------------------------------------------------
	スプライト描画開始
------------------------------------------------------------------------*/

void blit_start(int high_layer)
{
	if (scrollh_texture_clear || high_layer != scrollh_layer_number)
	{
		scrollh_reset_sprite();
		scrollh_layer_number = high_layer;
	}

	scrollh_delete_dirty_palette();
	memset(palette_dirty_marks, 0, sizeof(palette_dirty_marks));

	clut0_num = 0;
	clut1_num = 0;

	object_index = 0;
	object_num  = 0;

	scrollh_num = 0;

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(GU_PSM_5551, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_FAST_CLEAR_BIT);

	sceGuDrawBufferList(GU_PSM_5551, work_frame, BUF_WIDTH);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_FAST_CLEAR_BIT);

	sceGuEnable(GU_ALPHA_TEST);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------------------------
	スプライト描画終了
------------------------------------------------------------------------*/

void blit_finish(void)
{
	if (cps_rotate_screen)
	{
		if (cps_flip_screen)
		{
			video_driver->copyRectFlip(NULL, work_frame, draw_frame, &cps_src_clip, &cps_src_clip);
			video_driver->copyRect(NULL, draw_frame, work_frame, &cps_src_clip, &cps_src_clip);
			video_driver->clearFrame(NULL, draw_frame);
		}
		video_driver->copyRectRotate(NULL, work_frame, draw_frame, &cps_src_clip, &cps_clip[5]);
	}
	else
	{
		if (cps_flip_screen)
			video_driver->copyRectFlip(NULL, work_frame, draw_frame, &cps_src_clip, &cps_clip[option_stretch]);
		else
			video_driver->copyRect(NULL, work_frame, draw_frame, &cps_src_clip, &cps_clip[option_stretch]);
	}
}


/*------------------------------------------------------------------------
	OBJECTテクスチャを更新
------------------------------------------------------------------------*/

void blit_update_object(int16_t x, int16_t y, uint32_t code, uint16_t attr)
{
	if ((x > 47 && x < 448) && (y > 0 && y < 239))
	{
		uint32_t key = MAKE_KEY(code, attr);
		SPRITE *p = object_head[key & OBJECT_HASH_MASK];

		while (p)
		{
			if (p->key == key)
			{
				p->used = frames_displayed;
				return;
		 	}
			p = p->next;
		}
	}
}


/*------------------------------------------------------------------------
	OBJECTを描画リストに登録
------------------------------------------------------------------------*/

void blit_draw_object(int16_t x, int16_t y, uint32_t code, uint16_t attr)
{
	if ((x > 47 && x < 448) && (y > 0 && y < 239))
	{
		int16_t idx;
		OBJECT *object;
		struct Vertex *vertices;
		uint32_t key = MAKE_KEY(code, attr);

		if ((idx = object_get_sprite(key)) < 0)
		{
			uint32_t col, tile;
			uint8_t *src, *dst, lines = 16;

			if (object_texture_num == OBJECT_TEXTURE_SIZE - 1)
			{
				cps1_scan_object();
				object_delete_sprite();
			}

			idx = object_insert_sprite(key);
			dst = SWIZZLED8_16x16(tex_object, idx);
			src = &gfx_object[code << 7];
			col = color_table[attr & 0x0f];

			while (lines--)
			{
				tile = *(uint32_t *)(src + 0);
				*(uint32_t *)(dst +  0) = ((tile >> 0) & 0x0f0f0f0f) | col;
				*(uint32_t *)(dst +  4) = ((tile >> 4) & 0x0f0f0f0f) | col;
				tile = *(uint32_t *)(src + 4);
				*(uint32_t *)(dst +  8) = ((tile >> 0) & 0x0f0f0f0f) | col;
				*(uint32_t *)(dst + 12) = ((tile >> 4) & 0x0f0f0f0f) | col;
				src += 8;
				dst += swizzle_table_8bit[lines];
			}
		}

		object = &vertices_object[object_index++];
		object->clut = attr & 0x10;

		vertices = object->vertices;

		vertices[0].x = vertices[1].x = x;
		vertices[0].y = vertices[1].y = y;
		vertices[0].u = vertices[1].u = (idx & 0x001f) << 4;
		vertices[0].v = vertices[1].v = (idx & 0x03e0) >> 1;

		attr ^= 0x60;
		vertices[(attr & 0x20) >> 5].u += 16;
		vertices[(attr & 0x40) >> 6].v += 16;

		vertices[1].x += 16;
		vertices[1].y += 16;

		object_num += 2;
	}
}


/*------------------------------------------------------------------------
	OBJECT描画終了
------------------------------------------------------------------------*/

void blit_finish_object(void)
{
	int i, total_sprites = 0;
	uint8_t color = 0;
	struct Vertex *vertices, *vertices_tmp;
	OBJECT *object;

	if (!object_num) return;

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(GU_PSM_5551, work_frame, BUF_WIDTH);
	sceGuScissor(64, 16, 448, 240);
	sceGuTexMode(GU_PSM_T8, 0, 0, GU_TRUE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_object);
	sceGuClutLoad(256/8, clut);

	vertices_tmp = vertices = (struct Vertex *)sceGuGetMemory(object_num * sizeof(struct Vertex));

	object = vertices_object;

	for (i = 0; i < object_index; i++)
	{
		if (color != object->clut)
		{
			color = object->clut;

			if (total_sprites)
			{
				sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, total_sprites, NULL, vertices);
				total_sprites = 0;
				vertices = vertices_tmp;
			}

			sceGuClutLoad(256/8, &clut[color << 4]);
		}

		vertices_tmp[0] = object->vertices[0];
		vertices_tmp[1] = object->vertices[1];

		total_sprites += 2;
		vertices_tmp += 2;
		object++;
	}

	if (total_sprites)
		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, total_sprites, NULL, vertices);

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------------------------
	SCROLL1テクスチャを更新
------------------------------------------------------------------------*/

void blit_update_scroll1(int16_t x, int16_t y, uint32_t code, uint16_t attr)
{
	uint32_t key = MAKE_KEY(code, attr);
	SPRITE *p = scroll1_head[key & SCROLL1_HASH_MASK];

	while (p)
	{
		if (p->key == key)
		{
			p->used = frames_displayed;
			return;
		}
		p = p->next;
	}
}


/*------------------------------------------------------------------------
	SCROLL1を描画リストに登録
------------------------------------------------------------------------*/

void blit_draw_scroll1(int16_t x, int16_t y, uint32_t code, uint16_t attr, uint16_t gfxset)
{
	int16_t idx;
	struct Vertex *vertices;
	uint32_t key = MAKE_KEY(code, attr);

	if ((idx = scroll1_get_sprite(key)) < 0)
	{
		uint32_t col, tile;
		uint8_t *src, *dst, lines = 8;

		if (scroll1_texture_num == SCROLL1_TEXTURE_SIZE - 1)
		{
			cps1_scan_scroll1();
			scroll1_delete_sprite();
		}

		idx = scroll1_insert_sprite(key);
		dst = SWIZZLED8_8x8(tex_scroll1, idx);
		src = &gfx_scroll1[(code << 6) + (gfxset << 2)];
		col = color_table[attr & 0x0f];

		while (lines--)
		{
			tile = *(uint32_t *)(src + 0);
			*(uint32_t *)(dst +  0) = ((tile >> 0) & 0x0f0f0f0f) | col;
			*(uint32_t *)(dst +  4) = ((tile >> 4) & 0x0f0f0f0f) | col;
			src += 8;
			dst += 16;
		}
	}

	if (attr & 0x10)
	{
		vertices = &vertices_scroll[1][clut1_num];
		clut1_num += 2;
	}
	else
	{
		vertices = &vertices_scroll[0][clut0_num];
		clut0_num += 2;
	}

	vertices[0].x = vertices[1].x = x;
	vertices[0].y = vertices[1].y = y;
	vertices[0].u = vertices[1].u = (idx & 0x003f) << 3;
	vertices[0].v = vertices[1].v = (idx & 0x0fc0) >> 3;

	attr ^= 0x60;
	vertices[(attr & 0x20) >> 5].u += 8;
	vertices[(attr & 0x40) >> 6].v += 8;

	vertices[1].x += 8;
	vertices[1].y += 8;
}


/*------------------------------------------------------------------------
	SCROLL1描画終了
------------------------------------------------------------------------*/

void blit_finish_scroll1(void)
{
	struct Vertex *vertices;

	if (clut0_num + clut1_num == 0) return;

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(GU_PSM_5551, work_frame, BUF_WIDTH);
	sceGuScissor(64, 16, 448, 240);
	sceGuTexMode(GU_PSM_T8, 0, 0, GU_TRUE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_scroll1);

	vertices = (struct Vertex *)sceGuGetMemory((clut0_num + clut1_num) * sizeof(struct Vertex));

	if (clut0_num)
	{
		sceGuClutLoad(256/8, &clut[32 << 4]);

		memcpy(vertices, vertices_scroll[0], clut0_num * sizeof(struct Vertex));
		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, clut0_num, NULL, vertices);
		vertices += clut0_num;

		clut0_num = 0;
	}
	if (clut1_num)
	{
		sceGuClutLoad(256/8, &clut[48 << 4]);

		memcpy(vertices, vertices_scroll[1], clut1_num * sizeof(struct Vertex));
		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, clut1_num, NULL, vertices);

		clut1_num = 0;
	}

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------------------------
	SCROLL2クリップ範囲設定
------------------------------------------------------------------------*/

void blit_set_clip_scroll2(int16_t min_y, int16_t max_y)
{
	scroll2_min_y = min_y;
	scroll2_max_y = max_y + 1;

	if (scroll2_max_y - scroll2_min_y >= 16)
	{
		blit_draw_scroll2  = blit_draw_scroll2_hardware;
		blit_draw_scroll2h = blit_draw_scroll2h_hardware;
	}
	else
	{
		blit_draw_scroll2  = blit_draw_scroll2_software;
		blit_draw_scroll2h = blit_draw_scroll2h_software;
	}
}


/*------------------------------------------------------------------------
	SCROLL2の描画範囲チェック
------------------------------------------------------------------------*/

int blit_check_clip_scroll2(int16_t sy)
{
	scroll2_sy = sy;
	scroll2_ey = sy + 16;

	if (scroll2_min_y > scroll2_sy) scroll2_sy = scroll2_min_y;
	if (scroll2_max_y < scroll2_ey) scroll2_ey = scroll2_max_y;

	return (scroll2_sy < scroll2_ey);
}


/*------------------------------------------------------------------------
	SCROLL2テクスチャを更新
------------------------------------------------------------------------*/

void blit_update_scroll2(int16_t x, int16_t y, uint32_t code, uint16_t attr)
{
	if (y + 16 > 0 && y < 239)
	{
		uint32_t key = MAKE_KEY(code, attr);
		SPRITE *p = scroll2_head[key & SCROLL2_HASH_MASK];

		while (p)
		{
			if (p->key == key)
			{
				p->used = frames_displayed;
				return;
			}
			p = p->next;
		}
	}
}


/*------------------------------------------------------------------------
	SCROLL2(ラインスクロール)を直接VRAMに描画
------------------------------------------------------------------------*/

static void blit_draw_scroll2_software(int16_t x, int16_t y, uint32_t code, uint16_t attr)
{
	uint32_t src, dst;
	uint8_t func;

	src = code << 7;

	if (attr & 0x40)
	{
		src += ((y + 16) - scroll2_ey) << 3;
		dst = ((scroll2_ey - 1) << 9) + x;
	}
	else
	{
		src += (scroll2_sy - y) << 3;
		dst = (scroll2_sy << 9) + x;
	}

	func = pen_usage[code] | ((attr & 0x60) >> 4);

	(*drawgfx16[func])((uint32_t *)&gfx_scroll2[src],
					&scrbitmap[dst],
					&video_palette[((attr & 0x1f) + 64) << 4],
					scroll2_ey - scroll2_sy);
}


/*------------------------------------------------------------------------
	SCROLL2を描画リストに登録
------------------------------------------------------------------------*/

static void blit_draw_scroll2_hardware(int16_t x, int16_t y, uint32_t code, uint16_t attr)
{
	int16_t idx;
	struct Vertex *vertices;
	uint32_t key = MAKE_KEY(code, attr);

	if ((idx = scroll2_get_sprite(key)) < 0)
	{
		uint32_t col, tile;
		uint8_t *src, *dst, lines = 16;

		if (scroll2_texture_num == SCROLL2_TEXTURE_SIZE - 1)
		{
			cps1_scan_scroll2();
			scroll2_delete_sprite();
		}

		idx = scroll2_insert_sprite(key);
		dst = SWIZZLED8_16x16(tex_scroll2, idx);
		src = &gfx_scroll2[code << 7];
		col = color_table[attr & 0x0f];

		while (lines--)
		{
			tile = *(uint32_t *)(src + 0);
			*(uint32_t *)(dst +  0) = ((tile >> 0) & 0x0f0f0f0f) | col;
			*(uint32_t *)(dst +  4) = ((tile >> 4) & 0x0f0f0f0f) | col;
			tile = *(uint32_t *)(src + 4);
			*(uint32_t *)(dst +  8) = ((tile >> 0) & 0x0f0f0f0f) | col;
			*(uint32_t *)(dst + 12) = ((tile >> 4) & 0x0f0f0f0f) | col;
			src += 8;
			dst += swizzle_table_8bit[lines];
		}
	}

	if (attr & 0x10)
	{
		vertices = &vertices_scroll[1][clut1_num];
		clut1_num += 2;
	}
	else
	{
		vertices = &vertices_scroll[0][clut0_num];
		clut0_num += 2;
	}

	vertices[0].x = vertices[1].x = x;
	vertices[0].y = vertices[1].y = y;
	vertices[0].u = vertices[1].u = (idx & 0x001f) << 4;
	vertices[0].v = vertices[1].v = (idx & 0x03e0) >> 1;

	attr ^= 0x60;
	vertices[(attr & 0x20) >> 5].u += 16;
	vertices[(attr & 0x40) >> 6].v += 16;

	vertices[1].x += 16;
	vertices[1].y += 16;
}


/*------------------------------------------------------------------------
	SCROLL2描画終了
------------------------------------------------------------------------*/

void blit_finish_scroll2(void)
{
	struct Vertex *vertices;

	if (clut0_num + clut1_num == 0) return;

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(GU_PSM_5551, work_frame, BUF_WIDTH);
	sceGuScissor(64, scroll2_min_y, 448, scroll2_max_y);
	sceGuTexMode(GU_PSM_T8, 0, 0, GU_TRUE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_scroll2);

	vertices = (struct Vertex *)sceGuGetMemory((clut0_num + clut1_num) * sizeof(struct Vertex));

	if (clut0_num)
	{
		sceGuClutLoad(256/8, &clut[64 << 4]);

		memcpy(vertices, vertices_scroll[0], clut0_num * sizeof(struct Vertex));
		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, clut0_num, NULL, vertices);
		vertices += clut0_num;

		clut0_num = 0;
	}
	if (clut1_num)
	{
		sceGuClutLoad(256/8, &clut[80 << 4]);

		memcpy(vertices, vertices_scroll[1], clut1_num * sizeof(struct Vertex));
		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, clut1_num, NULL, vertices);

		clut1_num = 0;
	}

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------------------------
	SCROLL3テクスチャを更新
------------------------------------------------------------------------*/

void blit_update_scroll3(int16_t x, int16_t y, uint32_t code, uint16_t attr)
{
	uint32_t key = MAKE_KEY(code, attr);
	SPRITE *p = scroll3_head[key & SCROLL3_HASH_MASK];

	while (p)
	{
		if (p->key == key)
		{
			p->used = frames_displayed;
			return;
		}
		p = p->next;
	}
}


/*------------------------------------------------------------------------
	SCROLL3を描画リストに登録
------------------------------------------------------------------------*/

void blit_draw_scroll3(int16_t x, int16_t y, uint32_t code, uint16_t attr)
{
	int16_t idx;
	struct Vertex *vertices;
	uint32_t key = MAKE_KEY(code, attr);

	if ((idx = scroll3_get_sprite(key)) < 0)
	{
		uint32_t col, tile;
		uint8_t *src, *dst, lines = 32;

		if (scroll3_texture_num == SCROLL3_TEXTURE_SIZE - 1)
		{
			cps1_scan_scroll3();
			scroll3_delete_sprite();
		}

		idx = scroll3_insert_sprite(key);
		dst = SWIZZLED8_32x32(tex_scroll3, idx);
		src = &gfx_scroll3[code << 9];
		col = color_table[attr & 0x0f];

		while (lines--)
		{
			tile = *(uint32_t *)(src + 0);
			*(uint32_t *)(dst +  0) = ((tile >> 0) & 0x0f0f0f0f) | col;
			*(uint32_t *)(dst +  4) = ((tile >> 4) & 0x0f0f0f0f) | col;
			tile = *(uint32_t *)(src + 4);
			*(uint32_t *)(dst +  8) = ((tile >> 0) & 0x0f0f0f0f) | col;
			*(uint32_t *)(dst + 12) = ((tile >> 4) & 0x0f0f0f0f) | col;
			tile = *(uint32_t *)(src + 8);
			*(uint32_t *)(dst + 128) = ((tile >> 0) & 0x0f0f0f0f) | col;
			*(uint32_t *)(dst + 132) = ((tile >> 4) & 0x0f0f0f0f) | col;
			tile = *(uint32_t *)(src + 12);
			*(uint32_t *)(dst + 136) = ((tile >> 0) & 0x0f0f0f0f) | col;
			*(uint32_t *)(dst + 140) = ((tile >> 4) & 0x0f0f0f0f) | col;
			src += 16;
			dst += swizzle_table_8bit[lines];
		}
	}

	if (attr & 0x10)
	{
		vertices = &vertices_scroll[1][clut1_num];
		clut1_num += 2;
	}
	else
	{
		vertices = &vertices_scroll[0][clut0_num];
		clut0_num += 2;
	}

	vertices[0].x = vertices[1].x = x;
	vertices[0].y = vertices[1].y = y;
	vertices[0].u = vertices[1].u = (idx & 0x000f) << 5;
	vertices[0].v = vertices[1].v = (idx & 0x00f0) << 1;

	attr ^= 0x60;
	vertices[(attr & 0x20) >> 5].u += 32;
	vertices[(attr & 0x40) >> 6].v += 32;

	vertices[1].x += 32;
	vertices[1].y += 32;
}


/*------------------------------------------------------------------------
	SCROLL3描画終了
------------------------------------------------------------------------*/

void blit_finish_scroll3(void)
{
	struct Vertex *vertices;

	if (clut0_num + clut1_num == 0) return;

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(GU_PSM_5551, work_frame, BUF_WIDTH);
	sceGuScissor(64, 16, 448, 240);
	sceGuTexMode(GU_PSM_T8, 0, 0, GU_TRUE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_scroll3);

	vertices = (struct Vertex *)sceGuGetMemory((clut0_num + clut1_num) * sizeof(struct Vertex));

	if (clut0_num)
	{
		sceGuClutLoad(256/8, &clut[96 << 4]);

		memcpy(vertices, vertices_scroll[0], clut0_num * sizeof(struct Vertex));
		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, clut0_num, NULL, vertices);
		vertices += clut0_num;

		clut0_num = 0;
	}
	if (clut1_num)
	{
		sceGuClutLoad(256/8, &clut[112 << 4]);

		memcpy(vertices, vertices_scroll[1], clut1_num * sizeof(struct Vertex));
		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, clut1_num, NULL, vertices);

		clut1_num = 0;
	}

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------------------------
	SCROLL1(ハイレイヤー)を描画リストに登録
------------------------------------------------------------------------*/

void blit_draw_scroll1h(int16_t x, int16_t y, uint32_t code, uint16_t attr, uint16_t tpens, uint16_t gfxset)
{
	int16_t idx;
	struct Vertex *vertices;
	uint32_t key = MAKE_HIGH_KEY(code, attr);

	if ((idx = scrollh_get_sprite(key)) < 0)
	{
		uint32_t *src, tile, lines = 8;
		uint16_t *dst, *pal, pal2[16];

		if (scrollh_texture_num == SCROLL1H_TEXTURE_SIZE - 1)
		{
			cps1_scan_scroll1_foreground();
			scrollh_delete_sprite();
		}

		idx = scrollh_insert_sprite(key);
		dst = NONE_SWIZZLED_8x8(tex_scrollh, idx);
		src = (uint32_t *)&gfx_scroll1[(code << 6) + (gfxset << 2)];
		pal = &video_palette[((attr & 0x1f) + 32) << 4];

		if (tpens != 0x7fff)
		{
			int i;

			for (i = 0; i < 15; i++)
				pal2[i] = (tpens & (1 << i)) ? pal[i] : 0x8000;
			pal2[15] = 0x8000;
			pal = pal2;
		}

		while (lines--)
		{
			tile = src[0];
			dst[0] = pal[tile & 0x0f]; tile >>= 4;
			dst[4] = pal[tile & 0x0f]; tile >>= 4;
			dst[1] = pal[tile & 0x0f]; tile >>= 4;
			dst[5] = pal[tile & 0x0f]; tile >>= 4;
			dst[2] = pal[tile & 0x0f]; tile >>= 4;
			dst[6] = pal[tile & 0x0f]; tile >>= 4;
			dst[3] = pal[tile & 0x0f]; tile >>= 4;
			dst[7] = pal[tile & 0x0f];
			src += 2;
			dst += BUF_WIDTH;
		}
	}

	vertices = &vertices_scrollh[scrollh_num];

	vertices[0].u = vertices[1].u = (idx & 0x003f) << 3;
	vertices[0].v = vertices[1].v = (idx & 0x0fc0) >> 3;

	attr ^= 0x60;
	vertices[(attr & 0x20) >> 5].u += 8;
	vertices[(attr & 0x40) >> 6].v += 8;

	vertices[0].x = x;
	vertices[0].y = y;

	vertices[1].x = x + 8;
	vertices[1].y = y + 8;

	scrollh_num += 2;
}


/*------------------------------------------------------------------------
	SCROLL2(ハイレイヤー/ラインスクロール)を直接VRAMに描画
------------------------------------------------------------------------*/

static void blit_draw_scroll2h_software(int16_t x, int16_t y, uint32_t code, uint16_t attr, uint16_t tpens)
{
	uint32_t src, dst;
	uint8_t func;

	src = code << 7;

	if (attr & 0x40)
	{
		src += ((y + 16) - scroll2_ey) << 3;
		dst = ((scroll2_ey - 1) << 9) + x;
	}
	else
	{
		src += (scroll2_sy - y) << 3;
		dst = (scroll2_sy << 9) + x;
	}

	if (tpens == 0x7fff)
	{
		func = pen_usage[code] | ((attr & 0x60) >> 4);

		(*drawgfx16[func])((uint32_t *)&gfx_scroll2[src],
						&scrbitmap[dst],
						&video_palette[((attr & 0x1f) + 64) << 4],
						scroll2_ey - scroll2_sy);
	}
	else
	{
		func = (attr & 0x60) >> 5;

		(*drawgfx16h[func])((uint32_t *)&gfx_scroll2[src],
						&scrbitmap[dst],
						&video_palette[((attr & 0x1f) + 64) << 4],
						scroll2_ey - scroll2_sy,
						tpens);
	}
}


/*------------------------------------------------------------------------
	SCROLL2(ハイレイヤー)テクスチャを更新
------------------------------------------------------------------------*/

void blit_update_scroll2h(int16_t x, int16_t y, uint32_t code, uint16_t attr)
{
	if (y + 16 > 0 && y < 239)
	{
		uint32_t key = MAKE_HIGH_KEY(code, attr);
		SPRITE *p = scrollh_head[key & SCROLLH_HASH_MASK];

		while (p)
		{
			if (p->key == key)
			{
				p->used = frames_displayed;
				return;
			}
			p = p->next;
		}
	}
}


/*------------------------------------------------------------------------
	SCROLL2(ハイレイヤー)を描画リストに登録
------------------------------------------------------------------------*/

static void blit_draw_scroll2h_hardware(int16_t x, int16_t y, uint32_t code, uint16_t attr, uint16_t tpens)
{
	int16_t idx;
	struct Vertex *vertices;
	uint32_t key = MAKE_HIGH_KEY(code, attr);

	if ((idx = scrollh_get_sprite(key)) < 0)
	{
		uint32_t *src, tile, lines = 16;
		uint16_t *dst, *pal, pal2[16];

		if (scrollh_texture_num == SCROLL2H_TEXTURE_SIZE - 1)
		{
			cps1_scan_scroll2_foreground();
			scrollh_delete_sprite();
		}

		idx = scrollh_insert_sprite(key);
		dst = NONE_SWIZZLED_16x16(tex_scrollh, idx);
		src = (uint32_t *)&gfx_scroll2[code << 7];
		pal = &video_palette[((attr & 0x1f) + 64) << 4];

		if (tpens != 0x7fff)
		{
			int i;

			for (i = 0; i < 15; i++)
				pal2[i] = (tpens & (1 << i)) ? pal[i] : 0x8000;
			pal2[15] = 0x8000;
			pal = pal2;
		}

		while (lines--)
		{
			tile = src[0];
			dst[ 0] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 4] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 1] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 5] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 2] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 6] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 3] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 7] = pal[tile & 0x0f];
			tile = src[1];
			dst[ 8] = pal[tile & 0x0f]; tile >>= 4;
			dst[12] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 9] = pal[tile & 0x0f]; tile >>= 4;
			dst[13] = pal[tile & 0x0f]; tile >>= 4;
			dst[10] = pal[tile & 0x0f]; tile >>= 4;
			dst[14] = pal[tile & 0x0f]; tile >>= 4;
			dst[11] = pal[tile & 0x0f]; tile >>= 4;
			dst[15] = pal[tile & 0x0f];
			src += 2;
			dst += BUF_WIDTH;
		}
	}

	vertices = &vertices_scrollh[scrollh_num];

	vertices[0].u = vertices[1].u = (idx & 0x001f) << 4;
	vertices[0].v = vertices[1].v = (idx & 0x03e0) >> 1;

	attr ^= 0x60;
	vertices[(attr & 0x20) >> 5].u += 16;
	vertices[(attr & 0x40) >> 6].v += 16;

	vertices[0].x = x;
	vertices[0].y = y;

	vertices[1].x = x + 16;
	vertices[1].y = y + 16;

	scrollh_num += 2;
}


/*------------------------------------------------------------------------
	SCROLL2(ハイレイヤー)描画終了
------------------------------------------------------------------------*/

void blit_finish_scroll2h(void)
{
	struct Vertex *vertices;

	if (!scrollh_num) return;

	sceGuStart(GU_DIRECT, gulist);

	sceGuDrawBufferList(GU_PSM_5551, work_frame, BUF_WIDTH);
	sceGuScissor(64, scroll2_min_y, 448, scroll2_max_y);
	sceGuTexMode(GU_PSM_5551, 0, 0, GU_FALSE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_scrollh);

	vertices = (struct Vertex *)sceGuGetMemory(scrollh_num * sizeof(struct Vertex));
	memcpy(vertices, vertices_scrollh, scrollh_num * sizeof(struct Vertex));
	sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, scrollh_num, NULL, vertices);

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);

	scrollh_num = 0;
}


/*------------------------------------------------------------------------
	SCROLL3(ハイレイヤー)を描画リストに登録
------------------------------------------------------------------------*/

void blit_draw_scroll3h(int16_t x, int16_t y, uint32_t code, uint16_t attr, uint16_t tpens)
{
	int16_t idx;
	struct Vertex *vertices;
	uint32_t key = MAKE_HIGH_KEY(code, attr);

	if ((idx = scrollh_get_sprite(key)) < 0)
	{
		uint32_t *src, tile, lines = 32;
		uint16_t *dst, *pal, pal2[16];

		if (scrollh_texture_num == SCROLL3H_TEXTURE_SIZE - 1)
		{
			cps1_scan_scroll3_foreground();
			scrollh_delete_sprite();
		}

		idx = scrollh_insert_sprite(key);
		dst = NONE_SWIZZLED_32x32(tex_scrollh, idx);
		src = (uint32_t *)&gfx_scroll3[code << 9];
		pal = &video_palette[((attr & 0x1f) + 96) << 4];

		if (tpens != 0x7fff)
		{
			int i;

			for (i = 0; i < 15; i++)
				pal2[i] = (tpens & (1 << i)) ? pal[i] : 0x8000;
			pal2[15] = 0x8000;
			pal = pal2;
		}

		while (lines--)
		{
			tile = src[0];
			dst[ 0] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 4] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 1] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 5] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 2] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 6] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 3] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 7] = pal[tile & 0x0f];
			tile = src[1];
			dst[ 8] = pal[tile & 0x0f]; tile >>= 4;
			dst[12] = pal[tile & 0x0f]; tile >>= 4;
			dst[ 9] = pal[tile & 0x0f]; tile >>= 4;
			dst[13] = pal[tile & 0x0f]; tile >>= 4;
			dst[10] = pal[tile & 0x0f]; tile >>= 4;
			dst[14] = pal[tile & 0x0f]; tile >>= 4;
			dst[11] = pal[tile & 0x0f]; tile >>= 4;
			dst[15] = pal[tile & 0x0f];
			tile = src[2];
			dst[16] = pal[tile & 0x0f]; tile >>= 4;
			dst[20] = pal[tile & 0x0f]; tile >>= 4;
			dst[17] = pal[tile & 0x0f]; tile >>= 4;
			dst[21] = pal[tile & 0x0f]; tile >>= 4;
			dst[18] = pal[tile & 0x0f]; tile >>= 4;
			dst[22] = pal[tile & 0x0f]; tile >>= 4;
			dst[19] = pal[tile & 0x0f]; tile >>= 4;
			dst[23] = pal[tile & 0x0f];
			tile = src[3];
			dst[24] = pal[tile & 0x0f]; tile >>= 4;
			dst[28] = pal[tile & 0x0f]; tile >>= 4;
			dst[25] = pal[tile & 0x0f]; tile >>= 4;
			dst[29] = pal[tile & 0x0f]; tile >>= 4;
			dst[26] = pal[tile & 0x0f]; tile >>= 4;
			dst[30] = pal[tile & 0x0f]; tile >>= 4;
			dst[27] = pal[tile & 0x0f]; tile >>= 4;
			dst[31] = pal[tile & 0x0f];
			src += 4;
			dst += BUF_WIDTH;
		}
	}

	vertices = &vertices_scrollh[scrollh_num];

	vertices[0].u = vertices[1].u = (idx & 0x000f) << 5;
	vertices[0].v = vertices[1].v = (idx & 0x00f0) << 1;

	attr ^= 0x60;
	vertices[(attr & 0x20) >> 5].u += 32;
	vertices[(attr & 0x40) >> 6].v += 32;

	vertices[0].x = x;
	vertices[1].x = x + 32;

	vertices[0].y = y;
	vertices[1].y = y + 32;

	scrollh_num += 2;
}


/*------------------------------------------------------------------------
	SCROLL1,3(ハイレイヤー)テクスチャを更新
------------------------------------------------------------------------*/

void blit_update_scrollh(int16_t x, int16_t y, uint32_t code, uint16_t attr)
{
	uint32_t key = MAKE_HIGH_KEY(code, attr);
	SPRITE *p = scrollh_head[key & SCROLLH_HASH_MASK];

	while (p)
	{
		if (p->key == key)
		{
			p->used = frames_displayed;
			return;
		}
		p = p->next;
	}
}


/*------------------------------------------------------------------------
	SCROLL1,3(ハイレイヤー)描画終了
------------------------------------------------------------------------*/

void blit_finish_scrollh(void)
{
	struct Vertex *vertices;

	if (!scrollh_num) return;

	sceGuStart(GU_DIRECT, gulist);

	sceGuDrawBufferList(GU_PSM_5551, work_frame, BUF_WIDTH);
	sceGuScissor(64, 16, 448, 240);
	sceGuTexMode(GU_PSM_5551, 0, 0, GU_FALSE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_scrollh);

	vertices = (struct Vertex *)sceGuGetMemory(scrollh_num * sizeof(struct Vertex));
	memcpy(vertices, vertices_scrollh, scrollh_num * sizeof(struct Vertex));
	sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, scrollh_num, NULL, vertices);

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------------------------
	STARSレイヤー描画
------------------------------------------------------------------------*/

void blit_draw_stars(uint16_t stars_x, uint16_t stars_y, uint8_t *col, uint16_t *pal)
{
	typedef struct Vertex_t
	{
		uint16_t color;
		int16_t x, y, z;
	} Vertex;

	Vertex *vertices = (Vertex *)sceGuGetMemory(0x1000 * sizeof(Vertex));

	if (vertices)
	{
		Vertex *vertices_tmp = vertices;
		uint16_t offs;
		int stars_num = 0;

		for (offs = 0; offs < 0x1000; offs++, col += 8)
		{
			if (*col != 0x0f)
			{
				vertices_tmp->x     = (((offs >> 8) << 5) - stars_x + (*col & 0x1f)) & 0x1ff;
				vertices_tmp->y     = ((offs & 0xff) - stars_y) & 0xff;
				vertices_tmp->color = pal[(*col & 0xe0) >> 1];
				vertices_tmp++;
				stars_num++;
			}
		}

		if (stars_num)
		{
			sceGuStart(GU_DIRECT, gulist);
			sceGuDrawBufferList(GU_PSM_5551, work_frame, BUF_WIDTH);
			sceGuScissor(64, 16, 448, 240);
			sceGuDisable(GU_TEXTURE_2D);
			sceGuDisable(GU_ALPHA_TEST);

			sceGuDrawArray(GU_POINTS, GU_COLOR_5551 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, stars_num, NULL, vertices);

			sceGuEnable(GU_TEXTURE_2D);
			sceGuEnable(GU_ALPHA_TEST);
			sceGuFinish();
			sceGuSync(0, GU_SYNC_FINISH);
		}
	}
}
