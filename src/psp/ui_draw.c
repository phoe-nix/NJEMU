/******************************************************************************

	ui_draw.c

	PSP ユーザインタフェース描画関数

******************************************************************************/

#include "psp.h"
#include "stdarg.h"


/******************************************************************************
	定数/マクロ
******************************************************************************/

#define MAKECOL16(r, g, b)	(((b >> 4) << 8) | ((g >> 4) << 4) | (r >> 4))
#define GETR16(color)		(color & 0x0f)
#define GETG16(color)		((color >> 4) & 0x0f)
#define GETB16(color)		((color >> 8) & 0x0f)

#define CMD_RED				(1 << 8)
#define CMD_YELLOW			(2 << 8)
#define CMD_GREEN			(3 << 8)
#define CMD_BLUE			(4 << 8)
#define CMD_MAGENTA			(5 << 8)
#define CMD_PURPLE			(6 << 8)
#define CMD_ORANGE			(7 << 8)
#define CMD_GRAY			(8 << 8)
#define CMD_CYAN			(9 << 8)
#define CMD_PINK			(10 << 8)

#define CODE_HASCOLOR(c)	(c & 0xff00)
#define CODE_NOTFOUND		0xffff
#define CODE_UNDERBAR		0xfffe

#define isascii(c)			((c)  >= 0x20 && (c) <= 0x7e)
#define islatin1(c)			((c)  >= 0x80)
//#define iskana(c)			((c)  >= 0xa0 && (c) <= 0xdf)
#define isgbk1(c)			(((c) >= 0x81 && (c) <= 0xfe))
#define isgbk2(c)			((c)  >= 0x40 && (c) <= 0xfe && (c) != 0x7f && (c) != 0xff)

#define NUM_SMALL_FONTS		0x60
#define MAX_STR_LEN			256

enum
{
	FONT_TYPE_CONTROL = 0,
	FONT_TYPE_ASCII,
	FONT_TYPE_GRAPHIC,
//	FONT_TYPE_JPNHAN,
	FONT_TYPE_GBKSIMHEI,
#ifdef COMMAND_LIST
	FONT_TYPE_LATIN1,
	FONT_TYPE_COMMAND,
#endif
	FONT_TYPE_MAX
};


/******************************************************************************
	グローバル構造体
******************************************************************************/

UI_PALETTE ui_palette[UI_PAL_MAX] =
{
	{ 255, 255, 255 },	// UI_PAL_TITLE
	{ 255, 255, 255 },	// UI_PAL_SELECT
	{ 180, 180, 180 },	// UI_PAL_NORMAL
	{ 255, 255,  64 },	// UI_PAL_INFO
	{ 255,  64,  64 },	// UI_PAL_WARNING
	{  48,  48,  48 },	// UI_PAL_BG1
	{   0,   0, 160 },	// UI_PAL_BG2
	{   0,   0,   0 },	// UI_PAL_FRAME
	{  40,  40,  40 },	// UI_PAL_FILESEL1
	{ 120, 120, 120 }	// UI_PAL_FILESEL2
};


/******************************************************************************
	ローカル構造体
******************************************************************************/

static int light_level = 0;
#if VIDEO_32BPP
static int pixel_format;
#else
#define pixel_format	GU_PSM_5551
#endif

#ifdef COMMAND_LIST
static UINT16 command_font_color[11] =
{
	MAKECOL16(255,255,255),
	MAKECOL16(255, 32,  0),
	MAKECOL16(255,200,  0),
	MAKECOL16(  0,200, 80),
	MAKECOL16(  0, 64,255),
	MAKECOL16(255,  0,128),
	MAKECOL16(200,  0,200),
	MAKECOL16(255,128,  0),
	MAKECOL16(160,160,160),
	MAKECOL16( 64,200,200),
	MAKECOL16(255, 64,128)
};
#endif


static UINT16 *tex_font;
static UINT16 *tex_volicon;
static UINT16 *tex_smallfont;
static UINT16 *tex_boxshadow;

#define GAUSS_WIDTH	4

static int gauss_sum;
static const int gauss_fact[12][12] = {
	{  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
	{  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
	{  1,  2,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
	{  1,  3,  3,  1,  0,  0,  0,  0,  0,  0,  0,  0 },
	{  1,  4,  6,  4,  1,  0,  0,  0,  0,  0,  0,  0 },
	{  1,  5, 10, 10,  5,  1,  0,  0,  0,  0,  0,  0 },
	{  1,  6, 15, 20, 15,  6,  1,  0,  0,  0,  0,  0 },
	{  1,  7, 21, 35, 35, 21,  7,  1,  0,  0,  0,  0 },
	{  1,  8, 28, 56, 70, 56, 28,  8,  1,  0,  0,  0 },
	{  1,  9, 36, 84,126,126, 84, 36,  9,  1,  0,  0 },
	{  1, 10, 45,120,210,252,210,120, 45, 10,  1,  0 },
	{  1, 11, 55,165,330,462,462,330,165, 55, 11,  1 }
};

/******************************************************************************
	ユーザインタフェース初期化
******************************************************************************/

static UINT16 *texture16_addr(int x, int y)
{
	return (UINT16 *)(0x44000000 + ((x + (y << 9)) << 1));
}


#include "font/volume_icon.c"

void ui_init(void)
{
	int code, x, y, alpha;
	UINT16 *dst;
	UINT16 color[8] = {
		MAKECOL15(248,248,248),
		MAKECOL15(240,240,240),
		MAKECOL15(232,232,232),
		MAKECOL15(224,224,224),
		MAKECOL15(216,216,216),
		MAKECOL15(208,208,208),
		MAKECOL15(200,200,200),
		MAKECOL15(192,192,192)
	};

	tex_font = texture16_addr(0, 2000);

	dst = tex_font;
	for (y = 0; y < 48; y++)
	{
		for (x = 0; x < BUF_WIDTH; x++)
		{
			dst[x] = 0;
		}
		dst += BUF_WIDTH;
	}

	if (devkit_version >= 0x03050210)
	{
		tex_volicon = texture16_addr(BUF_WIDTH - 112, 2000);

		dst = tex_volicon + SPEEKER_X;
		for (y = 0; y < 32; y++)
		{
			for (x = 0; x < 32; x++)
			{
				if (x & 1)
					alpha = icon_speeker[y][(x >> 1)] >> 4;
				else
					alpha = icon_speeker[y][(x >> 1)] & 0x0f;

				dst[x] = (alpha << 12) | 0x0fff;
			}

			dst += BUF_WIDTH;
		}

		dst = tex_volicon + SPEEKER_SHADOW_X;
		for (y = 0; y < 32; y++)
		{
			for (x = 0; x < 32; x++)
			{
				if (x & 1)
					alpha = icon_speeker_shadow[y][(x >> 1)] >> 4;
				else
					alpha = icon_speeker_shadow[y][(x >> 1)] & 0x0f;

				dst[x] = alpha << 12;
			}

			dst += BUF_WIDTH;
		}

		dst = tex_volicon + VOLUME_BAR_X;
		for (y = 0; y < 32; y++)
		{
			for (x = 0; x < 12; x++)
			{
				if (x & 1)
					alpha = icon_bar[y][(x >> 1)] >> 4;
				else
					alpha = icon_bar[y][(x >> 1)] & 0x0f;

				dst[x] = (alpha << 12) | 0x0fff;
			}

			dst += BUF_WIDTH;
		}

		dst = tex_volicon + VOLUME_BAR_SHADOW_X;
		for (y = 0; y < 32; y++)
		{
			for (x = 0; x < 12; x++)
			{
				if (x & 1)
					alpha = icon_bar_shadow[y][(x >> 1)] >> 4;
				else
					alpha = icon_bar_shadow[y][(x >> 1)] & 0x0f;

				dst[x] = alpha << 12;
			}

			dst += BUF_WIDTH;
		}

		dst = tex_volicon + VOLUME_DOT_X;
		for (y = 0; y < 32; y++)
		{
			for (x = 0; x < 12; x++)
			{
				if (x & 1)
					alpha = icon_dot[y][(x >> 1)] >> 4;
				else
					alpha = icon_dot[y][(x >> 1)] & 0x0f;

				dst[x] = (alpha << 12) | 0x0fff;
			}

			dst += BUF_WIDTH;
		}

		dst = tex_volicon + VOLUME_DOT_SHADOW_X;
		for (y = 0; y < 32; y++)
		{
			for (x = 0; x < 12; x++)
			{
				if (x & 1)
					alpha = icon_dot_shadow[y][(x >> 1)] >> 4;
				else
					alpha = icon_dot_shadow[y][(x >> 1)] & 0x0f;

				dst[x] = alpha << 12;
			}

			dst += BUF_WIDTH;
		}
	}

	tex_smallfont = texture16_addr(0, 2032);

	dst = tex_smallfont;
	for (code = 0; code < NUM_SMALL_FONTS; code++)
	{
		for (y = 0; y < 8; y++)
		{
			UINT8 data = font_s[(code << 3) + y];
			UINT8 mask = 0x80;

			for (x = 0; x < 8; x++)
			{
				*dst++ = (data & mask) ? color[y] : 0x8000;
				mask >>= 1;
			}
		}
	}


	tex_boxshadow = dst;
	for (code = 0; code < 9; code++)
	{
		for (y = 0; y < 8; y++)
		{
			for (x = 0; x < 8; x++)
			{
				if (x & 1)
					alpha = shadow[code][y][x >> 1] >> 4;
				else
					alpha = shadow[code][y][x >> 1] & 0x0f;

				alpha *= 0.8;

				*dst++ = alpha << 12;
			}
		}
	}

	gauss_sum = 0;

	for (x = 0; x < 12; x++)
		gauss_sum += gauss_fact[GAUSS_WIDTH][x];

#if VIDEO_32BPP
	pixel_format = (video_mode == 32) ? GU_PSM_8888 : GU_PSM_5551;
#endif
}


/******************************************************************************
	フォントコード取得
******************************************************************************/

#ifdef COMMAND_LIST

/*------------------------------------------------------
	フォントコード取得 (コマンドリスト用)
------------------------------------------------------*/

static UINT16 command_font_get_code(const UINT8 *s)
{
	UINT8 c1 = s[0];
	UINT8 c2 = s[1];

	if (c1 == '_')
	{
		switch (c2)
		{
		case 'A': return 0x00 | CMD_RED;
		case 'B': return 0x01 | CMD_YELLOW;
		case 'C': return 0x02 | CMD_GREEN;
		case 'D': return 0x03 | CMD_BLUE;
		case 'P': return 0x04 | CMD_MAGENTA;
		case 'K': return 0x05 | CMD_PURPLE;
		case 'G': return 0x07 | CMD_BLUE;
		case 'H': return 0x08 | CMD_PINK;
		case 'Z': return 0x09 | CMD_PURPLE;

		case 'S': return 0x14 | CMD_RED;

		case 'a': return 0x16 | CMD_RED;
		case 'b': return 0x17 | CMD_YELLOW;
		case 'c': return 0x18 | CMD_GREEN;
		case 'd': return 0x19 | CMD_BLUE;
		case 'e': return 0x1a | CMD_MAGENTA;
		case 'f': return 0x1b | CMD_PURPLE;
		case 'g': return 0x1c | CMD_CYAN;
		case 'h': return 0x1d | CMD_PINK;
		case 'i': return 0x1e | CMD_ORANGE;
		case 'j': return 0x1f | CMD_GRAY;

		case '1': return 0x20;
		case '2': return 0x21;
		case '3': return 0x22;
		case '4': return 0x23;
		case '5': return 0x24 | CMD_RED;
		case '6': return 0x25;
		case '7': return 0x26;
		case '8': return 0x27;
		case '9': return 0x28;
		case 'N': return 0x29;
		case '+': return 0x2a;
		case '.': return 0x2b;

		case '!': return 0x35;

		case 'k': return 0x36;
		case 'l': return 0x37;
		case 'm': return 0x38;
		case 'n': return 0x39;
		case 'o': return 0x3a;
		case 'p': return 0x3b;
		case 'q': return 0x3c;
		case 'r': return 0x3d;
		case 's': return 0x3e;
		case 't': return 0x3f;
		case 'u': return 0x40;
		case 'v': return 0x41;

		case 'w': return 0x42;
		case 'x': return 0x43;
		case 'y': return 0x44;
		case 'z': return 0x45;
		case 'L': return 0x46;
		case 'M': return 0x47;
		case 'Q': return 0x48;
		case 'R': return 0x49;
		case '^': return 0x4a;
		case '?': return 0x4b;
		case 'X': return 0x4c;

		case 'I': return 0x4e;
		case 'O': return 0x4f;
		case '-': return 0x50;
		case '=': return 0x51;
		case '~': return 0x54;
		case '`': return 0x57;

		case '@': return 0x58;
		case ')': return 0x59;
		case '(': return 0x5a;
		case '*': return 0x5b;
		case '&': return 0x5c;
		case '%': return 0x5d;
		case '$': return 0x5e;
		case '#': return 0x5f;
		case ']': return 0x60;
		case '[': return 0x61;
		case '{': return 0x62;
		case '}': return 0x63;
		case '<': return 0x64;
		case '>': return 0x65;
		case '_': return CODE_UNDERBAR;
		}
	}
	else if (c1 == '^')
	{
		switch (c2)
		{
		case 's': return 0x06 | CMD_ORANGE;
		case 'E': return 0x0a | CMD_YELLOW;
		case 'F': return 0x0b | CMD_ORANGE;
		case 'G': return 0x0c | CMD_RED;
		case 'H': return 0x0d | CMD_GRAY;
		case 'I': return 0x0e | CMD_CYAN;
		case 'J': return 0x0f | CMD_BLUE;
		case 'T': return 0x10 | CMD_PURPLE;
		case 'U': return 0x11 | CMD_MAGENTA;
		case 'V': return 0x12 | CMD_PURPLE;
		case 'W': return 0x13 | CMD_MAGENTA;
		case 'S': return 0x15 | CMD_YELLOW;

		case '1': return 0x2c;
		case '2': return 0x2d;
		case '3': return 0x2e;
		case '4': return 0x2f;
		case '6': return 0x30;
		case '7': return 0x31;
		case '8': return 0x32;
		case '9': return 0x33;
		case '!': return 0x34;

		case 'M': return 0x4d;

		case '-': return 0x52;
		case '=': return 0x53;
		case '*': return 0x55;
		case '?': return 0x56;
		}
	}
	return CODE_NOTFOUND;
}


/*------------------------------------------------------
	フォントコード取得 (Latin-1デコード)
------------------------------------------------------*/

static UINT16 latin1_get_code(const UINT8 *s, int *type)
{
	UINT16 code;

	if ((code = command_font_get_code(s)) != CODE_NOTFOUND)
	{
		*type = FONT_TYPE_COMMAND;
		return code;
	}
	else if (isascii(*s))
	{
		*type = FONT_TYPE_ASCII;
		return *s - 0x20;
	}
	else if (islatin1(*s))
	{
		*type = FONT_TYPE_LATIN1;
		return *s - 0x80;
	}
	*type = FONT_TYPE_CONTROL;
	return *s;
}

/*------------------------------------------------------
	フォントコード取得 (GBKデコード)
------------------------------------------------------*/

static UINT16 gbk_get_code(const UINT8 *s, int *type)
{
	UINT8 c1 = s[0];
	UINT8 c2 = s[1];
	UINT16 code;

	if ((code = command_font_get_code(s)) != CODE_NOTFOUND)
	{
		*type = FONT_TYPE_COMMAND;
		return code;
	}
	else if (isgbk1(c1) && isgbk2(c2))
	{
		*type = FONT_TYPE_GBKSIMHEI;
		return gbk_table[(c2 | (c1 << 8)) - 0x8140];
	}
	else if (isascii(c1))
	{
		if (c1 != '\\')
		{
			*type = FONT_TYPE_ASCII;
			return c1 - 0x20;
		}
//		else
//		{
//			*type = FONT_TYPE_JPNHAN;
//			return 0;
//		}
	}
//	else if (iskana(c1))
//	{
//		*type = FONT_TYPE_JPNHAN;
//		return c1 - 0xa0;
//	}
	*type = FONT_TYPE_CONTROL;
	return c1;
}

#endif /* COMMAND_LIST */


/*------------------------------------------------------
	フォントコード取得 (ユーザインタフェース)
------------------------------------------------------*/

INLINE UINT16 uifont_get_code(const UINT8 *s, int *type)
{
	UINT8 c1 = s[0];
	UINT8 c2 = s[1];

	if (isgbk1(c1) && isgbk2(c2))
	{
		*type = FONT_TYPE_GBKSIMHEI;
		return gbk_table[(c2 | (c1 << 8)) - 0x8140];
	}
//	else if (c1 == 0xa5)
//	{
//		*type = FONT_TYPE_ASCII;
//		return 0x7f - 0x20;
//	}
//	else if (c1 == 0x5c)
//	{
//		*type = FONT_TYPE_JPNHAN;
//		return 0;
//	}
//	else if (iskana(c1))
//	{
//		*type = FONT_TYPE_JPNHAN;
//		return c1 - 0xa0;
//	}
	else if (isascii(c1))
	{
		*type = FONT_TYPE_ASCII;
		return c1 - 0x20;
	}
	else if ((c1 >= 0x10 && c1 <= 0x1e) && c1 != 0x1a)
	{
		*type = FONT_TYPE_GRAPHIC;
		if (c1 < 0x1a)
			return c1 - 0x10;
		else
			return c1 - 0x11;
	}
	*type = FONT_TYPE_CONTROL;
	return c1;
}


/******************************************************************************
	フォント文字幅取得
******************************************************************************/

/*------------------------------------------------------
	フォント描画幅取得 (ユーザインタフェース)
------------------------------------------------------*/

int uifont_get_string_width(const char *s)
{
	int width, type;
	UINT16 code;
	const UINT8 *p = (const UINT8 *)s;

	width = 0;

	while (*p)
	{
		if ((code = uifont_get_code(p, &type)) != CODE_NOTFOUND)
		{
			switch (type)
			{
			case FONT_TYPE_ASCII:
				width += ascii_14p_get_pitch(code);
				p++;
				break;

			case FONT_TYPE_GRAPHIC:
				width += graphic_font_get_pitch(code);
				p++;
				break;

//			case FONT_TYPE_JPNHAN:
//				width += jpn_h14p_get_pitch(code);
//				p++;
//				break;

			case FONT_TYPE_GBKSIMHEI:
				width += gbk_s14p_get_pitch(code);
				p += 2;
				break;

			case FONT_TYPE_CONTROL:
				width += ascii_14p_get_pitch(0);
				p++;
				break;
			}
		}
		else break;
	}

	return width;
}


/******************************************************************************
	内部フォント描画関数
******************************************************************************/

/*------------------------------------------------------
	フォントテクスチャ作成
------------------------------------------------------*/

static void make_font_texture(struct font_t *font, int r, int g, int b)
{
	int x, y, p;
	UINT16 *dst, color, alpha;
	UINT8 data;

	color = (b << 8) | (g << 4) | r;

	dst = tex_font;
	p = 0;

	for (y = 0; y < font->height; y++)
	{
		for (x = 0; x < font->width;)
		{
			data = font->data[p++];

			alpha  = data & 0x0f;
			dst[x] = (alpha << 12) | color;
			x++;

			alpha = data >> 4;
			dst[x] = (alpha << 12) | color;
			x++;
		}
		dst += BUF_WIDTH;
	}
}


/*------------------------------------------------------
	フォントの影テクスチャ作成
------------------------------------------------------*/

static void make_shadow_texture(struct font_t *font)
{
	int x, y, i, sum, alpha;
	UINT16 *dst = tex_font;
	UINT8 data;
	UINT8 temp1[32][40], temp2[32][40];

	memset(temp1, 0, sizeof(temp1));
	memset(temp2, 0, sizeof(temp2));

	i = 0;
	for (y = 0; y < font->height; y++)
	{
		for (x = 0; x < font->width; x += 2)
		{
			data = font->data[i++];

			temp1[y + 4][x + 4] = (data & 0x0f) ? 0xff : 0x00;
			temp1[y + 4][x + 5] = (data >> 4) ? 0xff : 0x00;
		}
	}

	for (x = 1; x < (font->width + 4) - 1; x++)
	{
		for (y = 1; y < (font->height + 4) - 1; y++)
		{
			sum = 0;

			for (i = 0; i < GAUSS_WIDTH; i++)
			{
				alpha = temp1[2 + y][2 + x - ((GAUSS_WIDTH - 1) >> 1) + i];
				sum += alpha * gauss_fact[GAUSS_WIDTH][i];
			}

			alpha = sum / gauss_sum;

			temp2[2 + y][2 + x] = alpha;
		}
	}

	for (x = 1; x < (font->width + 4) - 1; x++)
	{
		for (y = 1; y < (font->height + 4) - 1; y++)
		{
			sum = 0;

			for (i = 0; i < GAUSS_WIDTH; i++)
			{
				alpha = temp2[2 + y - ((GAUSS_WIDTH - 1) >> 1) + i][2 + x];
				sum += alpha * gauss_fact[GAUSS_WIDTH][i];
			}

			sum /= gauss_sum;
			if (sum > 255) sum = 255;

			temp1[2 + y][2 + x] = sum;
		}
	}

	for (y = 0; y < font->height + 4; y++)
	{
		for (x = 0; x < font->width + 4; x++)
		{
			alpha = temp1[2 + y][2 + x] >> 5;
			dst[x] = (alpha & 0x0f) << 12;
		}

		dst += BUF_WIDTH;
	}
}


/*------------------------------------------------------
	フォントの光テクスチャ作成
------------------------------------------------------*/

static void make_light_texture(struct font_t *font)
{
	int x, y, p, alpha, level;
	UINT16 *dst;
	UINT8 data;

	dst = tex_font;
	level = light_level >> 1;
	p = 0;

	for (y = 0; y < font->height; y++)
	{
		for (x = 0; x < font->width;)
		{
			data = font->data[p++];

			alpha  = (data & 0x0f) - level;
			if (alpha < 0) alpha = 0;
			dst[x] = (alpha << 12) | 0x0fff;
			x++;

			alpha = (data >> 4) - level;
			if (alpha < 0) alpha = 0;
			dst[x] = (alpha << 12) | 0x0fff;
			x++;
		}
		dst += BUF_WIDTH;
	}
}


/*------------------------------------------------------
	内部フォント描画関数
------------------------------------------------------*/

static int internal_font_putc(struct font_t *font, int sx, int sy, int r, int g, int b)
{
	struct Vertex *vertices;

	if (sx + font->pitch < 0 || sx >= SCR_WIDTH)
		return 0;

	make_font_texture(font, r, g, b);

	sx += font->skipx;
	sy += font->skipy;

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuEnable(GU_BLEND);

	sceGuTexMode(GU_PSM_4444, 0, 0, GU_FALSE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_font);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);

	vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

	if (vertices)
	{
		vertices[0].u = 0;
		vertices[0].v = 0;
		vertices[0].x = sx;
		vertices[0].y = sy;

		vertices[1].u = font->width;
		vertices[1].v = font->height;
		vertices[1].x = sx + font->width;
		vertices[1].y = sy + font->height;
	}

	sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2, NULL, vertices);

	sceGuDisable(GU_BLEND);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);

	return 1;
}


/*------------------------------------------------------
	内部フォント影描画関数
------------------------------------------------------*/

static int internal_shadow_putc(struct font_t *font, int sx, int sy)
{
	struct Vertex *vertices;

	if (sx + font->pitch < 0 || sx >= SCR_WIDTH)
		return 0;

	make_shadow_texture(font);

	sx += font->skipx;
	sy += font->skipy;

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuEnable(GU_BLEND);

	sceGuTexMode(GU_PSM_4444, 0, 0, GU_FALSE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_font);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);

	vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

	if (vertices)
	{
		vertices[0].u = 0;
		vertices[0].v = 0;
		vertices[0].x = sx;
		vertices[0].y = sy;

		vertices[1].u = font->width + 4;
		vertices[1].v = font->height + 4;
		vertices[1].x = sx + font->width + 4;
		vertices[1].y = sy + font->height + 4;
	}

	sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2, NULL, vertices);

	sceGuDisable(GU_BLEND);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);

	return 1;
}


/*------------------------------------------------------
	内部フォントライト描画関数
------------------------------------------------------*/

static int internal_light_putc(struct font_t *font, int sx, int sy)
{
	struct Vertex *vertices;

	if (sx + font->pitch < 0 || sx >= SCR_WIDTH)
		return 0;

	make_light_texture(font);

	sx += font->skipx;
	sy += font->skipy;

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuEnable(GU_BLEND);

	sceGuTexMode(GU_PSM_4444, 0, 0, GU_FALSE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_font);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);

	vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

	if (vertices)
	{
		vertices[0].u = 0;
		vertices[0].v = 0;
		vertices[0].x = sx;
		vertices[0].y = sy;

		vertices[1].u = font->width;
		vertices[1].v = font->height;
		vertices[1].x = sx + font->width;
		vertices[1].y = sy + font->height;
	}

	sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2, NULL, vertices);

	sceGuDisable(GU_BLEND);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);

	return 1;
}


/******************************************************************************
	ユーザインタフェース用フォント描画
******************************************************************************/

/*------------------------------------------------------
	ユーザインタフェース用文字を描画
------------------------------------------------------*/

INLINE void uifont_draw(int sx, int sy, int r, int g, int b, const char *s)
{
	int type, res = 1;
	UINT16 code;
	const UINT8 *p = (const UINT8 *)s;
	struct font_t font;

	r >>= 4;
	g >>= 4;
	b >>= 4;

	while (*p && res)
	{
		code = uifont_get_code(p, &type);

		switch (type)
		{
		case FONT_TYPE_ASCII:
			if (ascii_14p_get_gryph(&font, code))
			{
				res = internal_font_putc(&font, sx, sy, r, g, b);
				sx += font.pitch;
			}
			p++;
			break;

		case FONT_TYPE_GRAPHIC:
			if (graphic_font_get_gryph(&font, code))
			{
				res = internal_font_putc(&font, sx, sy, r, g, b);
				sx += font.pitch;
			}
			p++;
			break;

//		case FONT_TYPE_JPNHAN:
//			if (jpn_h14p_get_gryph(&font, code))
//			{
//				res = internal_font_putc(&font, sx, sy, r, g, b);
//				sx += font.pitch;
//			}
//			p++;
//			break;

		case FONT_TYPE_GBKSIMHEI:
			if (gbk_s14p_get_gryph(&font, code))
			{
				res = internal_font_putc(&font, sx, sy, r, g, b);
				sx += font.pitch;
			}
			p += 2;
			break;

		default:
			p++;
			break;
		}
	}
}


/*------------------------------------------------------
	文字の影を描画 (ユーザインタフェース用)
------------------------------------------------------*/

INLINE void uifont_draw_shadow(int sx, int sy, const char *s)
{
	int type, res = 1;
	UINT16 code;
	const UINT8 *p = (const UINT8 *)s;
	struct font_t font;

	while (*p && res)
	{
		code = uifont_get_code(p, &type);

		switch (type)
		{
		case FONT_TYPE_ASCII:
			if ((res = ascii_14p_get_gryph(&font, code)) != 0)
			{
				res = internal_shadow_putc(&font, sx, sy);
				sx += font.pitch;
			}
			p++;
			break;

		case FONT_TYPE_GRAPHIC:
			if ((res = graphic_font_get_gryph(&font, code)) != 0)
			{
				res = internal_shadow_putc(&font, sx, sy);
				sx += font.pitch;
			}
			p++;
			break;

//		case FONT_TYPE_JPNHAN:
//			if ((res = jpn_h14p_get_gryph(&font, code)) != 0)
//			{
//				res = internal_shadow_putc(&font, sx, sy);
//				sx += font.pitch;
//			}
//			p++;
//			break;

		case FONT_TYPE_GBKSIMHEI:
			if ((res = gbk_s14p_get_gryph(&font, code)) != 0)
			{
				res = internal_shadow_putc(&font, sx, sy);
				sx += font.pitch;
			}
			p += 2;
			break;

		default:
			res = 0;
			break;
		}
	}
}


/*------------------------------------------------------
	文字列を描画
------------------------------------------------------*/

void uifont_print(int sx, int sy, int r, int g, int b, const char *s)
{
	uifont_draw(sx, sy, r, g, b, s);
}


/*------------------------------------------------------
	文字列を描画 / センタリング処理
------------------------------------------------------*/

void uifont_print_center(int sy, int r, int g, int b, const char *s)
{
	int width = uifont_get_string_width(s);
	int sx = (SCR_WIDTH - width) / 2;

	uifont_print(sx, sy, r, g, b, s);
}


/*------------------------------------------------------
	文字列を描画 / 影付き
------------------------------------------------------*/

void uifont_print_shadow(int sx, int sy, int r, int g, int b, const char *s)
{
	uifont_draw_shadow(sx, sy, s);
	uifont_print(sx, sy, r, g, b, s);
}


/*------------------------------------------------------
	文字列を描画 / 影付き / センタリング処理
------------------------------------------------------*/

void uifont_print_shadow_center(int sy, int r, int g, int b, const char *s)
{
	int width = uifont_get_string_width(s);
	int sx = (SCR_WIDTH - width) / 2;

	uifont_print_shadow(sx, sy, r, g, b, s);
}


/******************************************************************************
	テキスト用固定ピッチフォント描画
******************************************************************************/

#ifdef COMMAND_LIST

/*------------------------------------------------------
	欧文文字列描画
------------------------------------------------------*/

INLINE void latin1_draw(int sx, int sy, int r, int g, int b, const char *s)
{
	int type, res = 1;
	UINT16 code;
	const UINT8 *p = (const UINT8 *)s;
	struct font_t font;

	r >>= 4;
	g >>= 4;
	b >>= 4;

	while (*p && res)
	{
		code = latin1_get_code(p, &type);

		switch (type)
		{
		case FONT_TYPE_ASCII:
			if ((res = ascii_14_get_gryph(&font, code)) != 0)
			{
				res = internal_font_putc(&font, sx, sy, r, g, b);
			}
			sx += FONTSIZE / 2;
			p++;
			break;

		case FONT_TYPE_LATIN1:
			if ((res = latin1_14_get_gryph(&font, code)) != 0)
			{
				res = internal_font_putc(&font, sx, sy, r, g, b);
			}
			sx += FONTSIZE / 2;
			p++;
			break;

		case FONT_TYPE_COMMAND:
			if (code == CODE_UNDERBAR)
			{
				code = *p - 0x20;
				if ((res = ascii_14_get_gryph(&font, code)) != 0)
				{
					res = internal_font_putc(&font, sx, sy, r, g, b);
				}
				sx += FONTSIZE/2;
			}
			else
			{
				int r2, g2, b2;

				if (CODE_HASCOLOR(code))
				{
					UINT32 color = command_font_color[code >> 8];

					r2 = GETR16(color);
					g2 = GETG16(color);
					b2 = GETB16(color);
					code &= 0xff;
				}
				else
				{
					r2 = r;
					g2 = g;
					b2 = b;
				}

				if ((res = command_font_get_gryph(&font, code)) != 0)
				{
					res = internal_font_putc(&font, sx, sy, r2, g2, b2);
				}
				sx += FONTSIZE;
			}
			p += 2;
			break;

		default:
			res = 0;
			break;
		}
	}
}


/*------------------------------------------------------
	日本語フォント描画
------------------------------------------------------*/

INLINE void gbk_draw(int sx, int sy, int r, int g, int b, const char *s)
{
	int type, res = 1;
	UINT16 code;
	const UINT8 *p = (const UINT8 *)s;
	struct font_t font;

	r >>= 4;
	g >>= 4;
	b >>= 4;

	while (*p && res)
	{
		code = gbk_get_code(p, &type);

		switch (type)
		{
		case FONT_TYPE_ASCII:
			if ((res = ascii_14_get_gryph(&font, code)) != 0)
			{
				res = internal_font_putc(&font, sx, sy, r, g, b);
			}
			sx += FONTSIZE / 2;
			p++;
			break;

//		case FONT_TYPE_JPNHAN:
//			if ((res = jpn_h14_get_gryph(&font, code)) != 0)
//			{
//				res = internal_font_putc(&font, sx, sy, r, g, b);
//			}
//			sx += FONTSIZE / 2;
//			p++;
//			break;

		case FONT_TYPE_GBKSIMHEI:
			if ((res = gbk_s14_get_gryph(&font, code)) != 0)
			{
				res = internal_font_putc(&font, sx, sy, r, g, b);
			}
			sx += FONTSIZE;
			p += 2;
			break;

		case FONT_TYPE_COMMAND:
			if (code == CODE_UNDERBAR)
			{
				code = *p - 0x20;
				if ((res = ascii_14_get_gryph(&font, code)) != 0)
				{
					res = internal_font_putc(&font, sx, sy, r, g, b);
				}
				sx += FONTSIZE/2;
			}
			else
			{
				int r2, g2, b2;

				if (CODE_HASCOLOR(code))
				{
					UINT32 color = command_font_color[code >> 8];

					r2 = GETR16(color);
					g2 = GETG16(color);
					b2 = GETB16(color);
					code &= 0xff;
				}
				else
				{
					r2 = r;
					g2 = g;
					b2 = b;
				}

				if ((res = command_font_get_gryph(&font, code)) != 0)
				{
					res = internal_font_putc(&font, sx, sy, r2, g2, b2);
				}
				sx += FONTSIZE;
			}
			p += 2;
			break;

		default:
			res = 0;
			break;
		}
	}
}


/*------------------------------------------------------
	フォント描画
------------------------------------------------------*/

void textfont_print(int sx, int sy, int r, int g, int b, const char *s, int flag)
{
	if (flag & CHARSET_GBK)
		gbk_draw(sx, sy, r, g, b, s);
	else
		latin1_draw(sx, sy, r, g, b, s);
}

#endif /* COMMAND_LIST */


/******************************************************************************
	アイコン描画
******************************************************************************/

/*------------------------------------------------------
	アイコン(小)を描画
------------------------------------------------------*/

void small_icon(int sx, int sy, int r, int g, int b, int no)
{
	struct font_t font;

	r >>= 4;
	g >>= 4;
	b >>= 4;

	if (icon_s_get_gryph(&font, no))
		internal_font_putc(&font, sx, sy, r, g, b);
}


/*------------------------------------------------------
	アイコン(小)を描画  (影エフェクトあり)
------------------------------------------------------*/

void small_icon_shadow(int sx, int sy, int r, int g, int b, int no)
{
	struct font_t font;

	r >>= 4;
	g >>= 4;
	b >>= 4;

	if (icon_s_get_gryph(&font, no))
	{
		internal_shadow_putc(&font, sx, sy);
		internal_font_putc(&font, sx, sy, r, g, b);
	}
}


/*------------------------------------------------------
	アイコン(小)を描画  (影、光エフェクトあり)
------------------------------------------------------*/

void small_icon_light(int sx, int sy, int r, int g, int b, int no)
{
	struct font_t font;
	struct font_t font_light;

	r >>= 4;
	g >>= 4;
	b >>= 4;

	if (icon_s_get_gryph(&font, no) && icon_s_get_light(&font_light, no))
	{
		internal_shadow_putc(&font, sx, sy);
		internal_light_putc(&font_light, sx - 4, sy - 4);
		internal_font_putc(&font, sx, sy, r, g, b);
	}
}


/*------------------------------------------------------
	アイコン(大)を描画
------------------------------------------------------*/

void large_icon(int sx, int sy, int r, int g, int b, int no)
{
	struct font_t font;

	r >>= 4;
	g >>= 4;
	b >>= 4;

	if (icon_l_get_gryph(&font, no))
		internal_font_putc(&font, sx, sy, r, g, b);
}


/*------------------------------------------------------
	アイコン(大)を描画 (影エフェクトあり)
------------------------------------------------------*/

void large_icon_shadow(int sx, int sy, int r, int g, int b, int no)
{
	struct font_t font;

	r >>= 4;
	g >>= 4;
	b >>= 4;

	if (icon_l_get_gryph(&font, no))
	{
		internal_shadow_putc(&font, sx, sy);
		internal_font_putc(&font, sx, sy, r, g, b);
	}
}


/*------------------------------------------------------
	アイコン(大)を描画  (影、光エフェクトあり)
------------------------------------------------------*/

void large_icon_light(int sx, int sy, int r, int g, int b, int no)
{
	struct font_t font;
	struct font_t font_light;

	r >>= 4;
	g >>= 4;
	b >>= 4;

	if (icon_l_get_gryph(&font, no) && icon_l_get_light(&font_light, no))
	{
		internal_shadow_putc(&font, sx, sy);
		internal_light_putc(&font_light, sx - 4, sy - 4);
		internal_font_putc(&font, sx, sy, r, g, b);
	}
}


/*------------------------------------------------------
	アイコンの光
------------------------------------------------------*/

int ui_light_update(void)
{
	static int light_dir = 1;
	int prev_level;

	prev_level = light_level >> 1;

	light_level += light_dir;
	if (light_level > 31)
	{
		light_level = 31;
		light_dir = -1;
	}
	else if (light_level < 0)
	{
		light_level = 0;
		light_dir = 1;
	}

	return (prev_level != (light_level >> 1)) ? UI_PARTIAL_REFRESH : 0;
}


/******************************************************************************
	ボリューム描画
******************************************************************************/

/*------------------------------------------------------
	ボリュームを描画 (CFW 3.52以降のユーザーモードのみ)
------------------------------------------------------*/

void draw_volume(int volume)
{
	struct Vertex *vertices, *vertices_tmp;

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuEnable(GU_BLEND);
	sceGuTexMode(GU_PSM_4444, 0, 0, GU_FALSE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_volicon);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);

	vertices = (struct Vertex *)sceGuGetMemory(2 * 31 * 2 * sizeof(struct Vertex));

	if (vertices)
	{
		int i, x;

		memset(vertices, 0, 2 * 31 * 2 * sizeof(struct Vertex));
		vertices_tmp = vertices;

		x = 24;

		vertices_tmp[0].u = SPEEKER_SHADOW_X;
		vertices_tmp[0].v = 0;
		vertices_tmp[0].x = 3 + x;
		vertices_tmp[0].y = 3 + 230;

		vertices_tmp[1].u = SPEEKER_SHADOW_X + 32;
		vertices_tmp[1].v = 32;
		vertices_tmp[1].x = 3 + x + 32;
		vertices_tmp[1].y = 3 + 230 + 32;

		vertices_tmp += 2;

		vertices_tmp[0].u = SPEEKER_X;
		vertices_tmp[0].v = 0;
		vertices_tmp[0].x = x;
		vertices_tmp[0].y = 230;

		vertices_tmp[1].u = SPEEKER_X + 32;
		vertices_tmp[1].v = 32;
		vertices_tmp[1].x = x + 32;
		vertices_tmp[1].y = 230 + 32;

		vertices_tmp += 2;

		x = 64;

		for (i = 0; i < volume; i++)
		{
			vertices_tmp[0].u = VOLUME_BAR_SHADOW_X;
			vertices_tmp[0].v = 0;
			vertices_tmp[0].x = 3 + x;
			vertices_tmp[0].y = 3 + 230;

			vertices_tmp[1].u = VOLUME_BAR_SHADOW_X + 12;
			vertices_tmp[1].v = 32;
			vertices_tmp[1].x = 3 + x + 12;
			vertices_tmp[1].y = 3 + 230 + 32;

			vertices_tmp += 2;

			vertices_tmp[0].u = VOLUME_BAR_X;
			vertices_tmp[0].v = 0;
			vertices_tmp[0].x = x;
			vertices_tmp[0].y = 230;

			vertices_tmp[1].u = VOLUME_BAR_X + 12;
			vertices_tmp[1].v = 32;
			vertices_tmp[1].x = x + 12;
			vertices_tmp[1].y = 230 + 32;

			vertices_tmp += 2;

			x += 12;
		}

		for (; i < 30; i++)
		{
			vertices_tmp[0].u = VOLUME_DOT_SHADOW_X;
			vertices_tmp[0].v = 0;
			vertices_tmp[0].x = 3 + x;
			vertices_tmp[0].y = 3 + 230;

			vertices_tmp[1].u = VOLUME_DOT_SHADOW_X + 12;
			vertices_tmp[1].v = 32;
			vertices_tmp[1].x = 3 + x + 12;
			vertices_tmp[1].y = 3 + 230 + 32;

			vertices_tmp += 2;

			vertices_tmp[0].u = VOLUME_DOT_X;
			vertices_tmp[0].v = 0;
			vertices_tmp[0].x = x;
			vertices_tmp[0].y = 230;

			vertices_tmp[1].u = VOLUME_DOT_X + 12;
			vertices_tmp[1].v = 32;
			vertices_tmp[1].x = x + 12;
			vertices_tmp[1].y = 230 + 32;

			vertices_tmp += 2;

			x += 12;
		}

		sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_COLOR_5551 | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2 * 31 * 2, NULL, vertices);
	}

	sceGuDisable(GU_BLEND);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/******************************************************************************
	ゲーム画面用フォント描画
******************************************************************************/

/*------------------------------------------------------
	文字列描画
------------------------------------------------------*/

void small_font_print(int sx, int sy, const char *s, int bg)
{
	struct Vertex *vertices;
	int len = strlen(s);

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(sx, sy, sx + 8 * len, sy + 8);

	if (bg)
		sceGuDisable(GU_ALPHA_TEST);
	else
		sceGuEnable(GU_ALPHA_TEST);

	sceGuTexMode(GU_PSM_5551, 0, 0, GU_TRUE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_smallfont);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);

	vertices = (struct Vertex *)sceGuGetMemory(len * 2 * sizeof(struct Vertex));

	if (vertices)
	{
		int i;
		struct Vertex *vertices_tmp = vertices;

		for (i = 0; i < len; i++)
		{
			UINT8 code = isascii((UINT8)s[i]) ? s[i] - 0x20 : 0x20;
			int u = (code & 63) << 3;
			int v = (code >> 6) << 3;

			vertices_tmp[0].u = u;
			vertices_tmp[0].v = v;
			vertices_tmp[0].x = sx;
			vertices_tmp[0].y = sy;

			vertices_tmp[1].u = u + 8;
			vertices_tmp[1].v = v + 8;
			vertices_tmp[1].x = sx + 8;
			vertices_tmp[1].y = sy + 8;

			vertices_tmp += 2;
			sx += 8;
		}

		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2 * len, NULL, vertices);
	}

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------
	書式付文字列描画
------------------------------------------------------*/

void small_font_printf(int x, int y, const char *text, ...)
{
	char buf[256];
	va_list arg;

	va_start(arg, text);
	vsprintf(buf, text, arg);
	va_end(arg);

	small_font_print(x << 3, y << 3, buf, 1);
}


/*------------------------------------------------------
	文字列描画
------------------------------------------------------*/

static void debug_font_print(void *frame, int sx, int sy, const char *s, int bg)
{
	int len = strlen(s);
	struct Vertex *vertices = (struct Vertex *)sceGuGetMemory(len * 2 * sizeof(struct Vertex));

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, frame, BUF_WIDTH);
	sceGuScissor(sx, sy, sx + 8 * len, sy + 8);

	if (bg)
		sceGuDisable(GU_ALPHA_TEST);
	else
		sceGuEnable(GU_ALPHA_TEST);

	sceGuTexMode(GU_PSM_5551, 0, 0, GU_TRUE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_smallfont);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);

	if (vertices)
	{
		int i;
		struct Vertex *vertices_tmp = vertices;

		for (i = 0; i < len; i++)
		{
			UINT8 code = isascii((UINT8)s[i]) ? s[i] - 0x20 : 0x20;
			int u = (code & 63) << 3;
			int v = (code >> 6) << 3;

			vertices_tmp[0].u = u;
			vertices_tmp[0].v = v;
			vertices_tmp[0].x = sx;
			vertices_tmp[0].y = sy;

			vertices_tmp[1].u = u + 8;
			vertices_tmp[1].v = v + 8;
			vertices_tmp[1].x = sx + 8;
			vertices_tmp[1].y = sy + 8;

			vertices_tmp += 2;
			sx += 8;
		}

		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2 * len, NULL, vertices);
	}

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------
	書式付文字列描画
------------------------------------------------------*/

void debug_font_printf(void *frame, int x, int y, const char *text, ...)
{
	char buf[256];
	va_list arg;

	va_start(arg, text);
	vsprintf(buf, text, arg);
	va_end(arg);

	debug_font_print(frame, x << 3, y << 3, buf, 1);
}


/******************************************************************************
	図形描画
******************************************************************************/

/*------------------------------------------------------
	水平線描画
------------------------------------------------------*/

void hline(int sx, int ex, int y, int r, int g, int b)
{
	Vertex16 *vertices;
	UINT32 color = MAKECOL32(r, g, b);

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuDisable(GU_TEXTURE_2D);

	vertices = (Vertex16 *)sceGuGetMemory(2 * sizeof(Vertex16));

	if (vertices)
	{
		vertices[0].x = sx;
		vertices[0].y = y;
		vertices[0].color = color;

		vertices[1].x = ex + 1;
		vertices[1].y = y;
		vertices[1].color = color;

		sceGuDrawArray(GU_LINES, PRIMITIVE_FLAGS, 2, NULL, vertices);
	}

	sceGuEnable(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------
	水平線描画 (アルファブレンド)
------------------------------------------------------*/

void hline_alpha(int sx, int ex, int y, int r, int g, int b, int alpha)
{
	Vertex16 *vertices;
	UINT32 color = MAKECOL32A(r, g, b, ((alpha << 4) - 1));

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuDisable(GU_TEXTURE_2D);
	sceGuEnable(GU_BLEND);

	vertices = (Vertex16 *)sceGuGetMemory(2 * sizeof(Vertex16));

	if (vertices)
	{
		vertices[0].x = sx;
		vertices[0].y = y;
		vertices[0].color = color;

		vertices[1].x = ex + 1;
		vertices[1].y = y;
		vertices[1].color = color;

		sceGuDrawArray(GU_LINES, PRIMITIVE_FLAGS, 2, NULL, vertices);
	}

	sceGuDisable(GU_BLEND);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------
	水平線描画 (アルファブレンド/グラデーション)
------------------------------------------------------*/

void hline_gradation(int sx, int ex, int y, int r1, int g1, int b1, int r2, int g2, int b2, int alpha)
{
	Vertex16 *vertices;
	UINT32 color1 = MAKECOL32A(r1, g1, b1, ((alpha << 4) - 1));
	UINT32 color2 = MAKECOL32A(r2, g2, b2, ((alpha << 4) - 1));

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuDisable(GU_TEXTURE_2D);
	sceGuEnable(GU_BLEND);
	sceGuShadeModel(GU_SMOOTH);

#if VIDEO_32BPP
	if (video_mode == 16)
#endif
		sceGuEnable(GU_DITHER);

	vertices = (Vertex16 *)sceGuGetMemory(2 * sizeof(Vertex16));

	if (vertices)
	{
		vertices[0].x = sx;
		vertices[0].y = y;
		vertices[0].color = color1;

		vertices[1].x = ex + 1;
		vertices[1].y = y;
		vertices[1].color = color2;

		sceGuDrawArray(GU_LINES, PRIMITIVE_FLAGS, 2, NULL, vertices);
	}

#if VIDEO_32BPP
	if (video_mode == 16)
#endif
		sceGuDisable(GU_DITHER);

	sceGuShadeModel(GU_FLAT);
	sceGuDisable(GU_BLEND);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------
	垂直線描画
------------------------------------------------------*/

void vline(int x, int sy, int ey, int r, int g, int b)
{
	Vertex16 *vertices;
	UINT32 color = MAKECOL32(r, g, b);

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuDisable(GU_TEXTURE_2D);

	vertices = (Vertex16 *)sceGuGetMemory(2 * sizeof(Vertex16));

	if (vertices)
	{
		vertices[0].x = x;
		vertices[0].y = sy;
		vertices[0].color = color;

		vertices[1].x = x;
		vertices[1].y = ey + 1;
		vertices[1].color = color;

		sceGuDrawArray(GU_LINES, PRIMITIVE_FLAGS, 2, NULL, vertices);
	}

	sceGuEnable(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------
	垂直線描画 (アルファブレンド)
------------------------------------------------------*/

void vline_alpha(int x, int sy, int ey, int r, int g, int b, int alpha)
{
	Vertex16 *vertices;
	UINT32 color = MAKECOL32A(r, g, b, ((alpha << 4) - 1));

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuDisable(GU_TEXTURE_2D);
	sceGuEnable(GU_BLEND);

	vertices = (Vertex16 *)sceGuGetMemory(2 * sizeof(Vertex16));

	if (vertices)
	{
		vertices[0].x = x;
		vertices[0].y = sy;
		vertices[0].color = color;

		vertices[1].x = x;
		vertices[1].y = ey + 1;
		vertices[1].color = color;

		sceGuDrawArray(GU_LINES, PRIMITIVE_FLAGS, 2, NULL, vertices);
	}

	sceGuDisable(GU_BLEND);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------
	垂直線描画 (アルファブレンド/グラデーション)
------------------------------------------------------*/

void vline_gradation(int x, int sy, int ey, int r1, int g1, int b1, int r2, int g2, int b2, int alpha)
{
	Vertex16 *vertices;
	UINT32 color1 = MAKECOL32A(r1, g1, b1, ((alpha << 4) - 1));
	UINT32 color2 = MAKECOL32A(r2, g2, b2, ((alpha << 4) - 1));

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuDisable(GU_TEXTURE_2D);
	sceGuEnable(GU_BLEND);
	sceGuShadeModel(GU_SMOOTH);

#if VIDEO_32BPP
	if (video_mode == 16)
#endif
		sceGuEnable(GU_DITHER);

	vertices = (Vertex16 *)sceGuGetMemory(2 * sizeof(Vertex16));

	if (vertices)
	{
		vertices[0].x = x;
		vertices[0].y = sy;
		vertices[0].color = color1;

		vertices[1].x = x;
		vertices[1].y = ey + 1;
		vertices[1].color = color2;

		sceGuDrawArray(GU_LINES, PRIMITIVE_FLAGS, 2, NULL, vertices);
	}

#if VIDEO_32BPP
	if (video_mode == 16)
#endif
		sceGuDisable(GU_DITHER);

	sceGuShadeModel(GU_FLAT);
	sceGuDisable(GU_BLEND);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------
	矩形描画
------------------------------------------------------*/

void box(int sx, int sy, int ex, int ey, int r, int g, int b)
{
	Vertex16 *vertices;
	UINT32 color = MAKECOL32(r, g, b);

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuDisable(GU_TEXTURE_2D);

	vertices = (Vertex16 *)sceGuGetMemory(5 * sizeof(Vertex16));

	if (vertices)
	{
		// 左上
		vertices[0].x = sx;
		vertices[0].y = sy;
		vertices[0].color = color;

		// 右上
		vertices[1].x = ex;
		vertices[1].y = sy;
		vertices[1].color = color;

		// 右下
		vertices[2].x = ex;
		vertices[2].y = ey + 1;
		vertices[2].color = color;

		// 左下
		vertices[3].x = sx;
		vertices[3].y = ey;
		vertices[3].color = color;

		// 左上
		vertices[4].x = sx;
		vertices[4].y = sy;
		vertices[4].color = color;

		sceGuDrawArray(GU_LINE_STRIP, PRIMITIVE_FLAGS, 5, NULL, vertices);
	}

	sceGuEnable(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------
	矩形塗りつぶし
------------------------------------------------------*/

void boxfill(int sx, int sy, int ex, int ey, int r, int g, int b)
{
	Vertex16 *vertices;
	UINT32 color = MAKECOL32(r, g, b);

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuDisable(GU_TEXTURE_2D);

	vertices = (Vertex16 *)sceGuGetMemory(4 * sizeof(Vertex16));

	if (vertices)
	{
		// 左上
		vertices[0].x = sx;
		vertices[0].y = sy;
		vertices[0].color = color;

		// 右上
		vertices[1].x = ex + 1;
		vertices[1].y = sy;
		vertices[1].color = color;

		// 左下
		vertices[2].x = sx;
		vertices[2].y = ey + 1;
		vertices[2].color = color;

		// 右下
		vertices[3].x = ex + 1;
		vertices[3].y = ey + 1;
		vertices[3].color = color;

		sceGuDrawArray(GU_TRIANGLE_STRIP, PRIMITIVE_FLAGS, 4, NULL, vertices);
	}

	sceGuEnable(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------
	矩形塗りつぶし (アルファブレンド)
------------------------------------------------------*/

void boxfill_alpha(int sx, int sy, int ex, int ey, int r, int g, int b, int alpha)
{
	Vertex16 *vertices;
	UINT32 color = MAKECOL32A(r, g, b, ((alpha << 4) - 1));

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuDisable(GU_TEXTURE_2D);
	sceGuEnable(GU_BLEND);

	vertices = (Vertex16 *)sceGuGetMemory(4 * sizeof(Vertex16));

	if (vertices)
	{
		// 左上
		vertices[0].x = sx;
		vertices[0].y = sy;
		vertices[0].color = color;

		// 右上
		vertices[1].x = ex + 1;
		vertices[1].y = sy;
		vertices[1].color = color;

		// 左下
		vertices[2].x = sx;
		vertices[2].y = ey + 1;
		vertices[2].color = color;

		// 右下
		vertices[3].x = ex + 1;
		vertices[3].y = ey + 1;
		vertices[3].color = color;

		sceGuDrawArray(GU_TRIANGLE_STRIP, PRIMITIVE_FLAGS, 4, NULL, vertices);
	}

	sceGuDisable(GU_BLEND);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*---------------------------------------------------------
	矩形塗りつぶし (アルファブレンド/グラデーション)
---------------------------------------------------------*/

void boxfill_gradation(int sx, int sy, int ex, int ey, int r1, int g1, int b1, int r2, int g2, int b2, int alpha, int dir)
{
	Vertex16 *vertices;
	UINT32 color1 = MAKECOL32A(r1, g1, b1, ((alpha << 4) - 1));
	UINT32 color2 = MAKECOL32A(r2, g2, b2, ((alpha << 4) - 1));

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuDisable(GU_TEXTURE_2D);
	sceGuEnable(GU_BLEND);
	sceGuShadeModel(GU_SMOOTH);

#if VIDEO_32BPP
	if (video_mode == 16)
#endif
		sceGuEnable(GU_DITHER);

	vertices = (Vertex16 *)sceGuGetMemory(4 * sizeof(Vertex16));

	if (vertices)
	{
		// 左上
		vertices[0].x = sx;
		vertices[0].y = sy;

		// 右上
		vertices[1].x = ex + 1;
		vertices[1].y = sy;

		// 左下
		vertices[2].x = sx;
		vertices[2].y = ey + 1;

		// 右下
		vertices[3].x = ex + 1;
		vertices[3].y = ey + 1;

		if (dir)
		{
			// 縦方向にグラデーション

			// 上
			vertices[0].color = color1;
			vertices[1].color = color1;

			// 下
			vertices[2].color = color2;
			vertices[3].color = color2;
		}
		else
		{
			// 横方向にグラデーション

			// 左
			vertices[0].color = color1;
			vertices[2].color = color1;

			// 右
			vertices[1].color = color2;
			vertices[3].color = color2;
		}

		sceGuDrawArray(GU_TRIANGLE_STRIP, PRIMITIVE_FLAGS, 4, NULL, vertices);
	}

#if VIDEO_32BPP
	if (video_mode == 16)
#endif
		sceGuDisable(GU_DITHER);

	sceGuShadeModel(GU_FLAT);
	sceGuDisable(GU_BLEND);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/******************************************************************************
	ユーザインタフェースのパーツを描画
******************************************************************************/

/*------------------------------------------------------
	影をテクスチャとして描画
------------------------------------------------------*/

static void draw_boxshadow(int sx, int sy, int w, int h, int code)
{
	struct Vertex *vertices;

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(sx, sy, sx + w, sy + h);
	sceGuDisable(GU_ALPHA_TEST);
	sceGuEnable(GU_BLEND);

	sceGuTexMode(GU_PSM_4444, 0, 0, GU_TRUE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_boxshadow);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);

	vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

	if (vertices)
	{
		int u = code << 3;

		vertices[0].u = u;
		vertices[0].v = 0;
		vertices[0].x = sx;
		vertices[0].y = sy;

		vertices[1].u = u + 8;
		vertices[1].v = 8;
		vertices[1].x = sx + 8;
		vertices[1].y = sy + 8;

		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2, NULL, vertices);
	}

	sceGuDisable(GU_BLEND);

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*------------------------------------------------------
	矩形の影を描画
------------------------------------------------------*/

void draw_box_shadow(int sx, int sy, int ex, int ey)
{
	int i, j, x, y, width, height;
	int w, h, nw, nh;

	width = (ex - sx) + 1;
	height = (ey - sy) + 1;

	width  -= 14;
	height -= 14;

	nw = width / 8;
	nh = height / 8;

	w = width % 8;
	h = height % 8;

	sx += 2;
	sy += 2;

	x = sx;
	y = sy;

	draw_boxshadow(x, y, 8, 8, 0);
	x += 8;

	for (i = 0; i < nw; i++)
	{
		draw_boxshadow(x, y, 8, 8, 1);
		x += 8;
	}

	draw_boxshadow(x, y, w, 8, 1);
	x += w;

	draw_boxshadow(x, y, 8, 8, 2);
	y += 8;

	for (j = 0; j < nh; j++)
	{
		x = sx;

		draw_boxshadow(x, y, 8, 8, 3);
		x += 8;

		for (i = 0; i < nw; i++)
		{
			draw_boxshadow(x, y, 8, 8, 4);
			x += 8;
		}

		draw_boxshadow(x, y, w, 8, 4);
		x += w;

		draw_boxshadow(x, y, 8, 8, 5);
		y += 8;
	}

	x = sx;

	draw_boxshadow(x, y, 8, h, 3);
	x += 8;

	for (i = 0; i < nw; i++)
	{
		draw_boxshadow(x, y, 8, h, 4);
		x += 8;
	}

	draw_boxshadow(x, y, w, h, 4);
	x += w;

	draw_boxshadow(x, y, 8, h, 5);
	y += h;

	x = sx;

	draw_boxshadow(x, y, 8, 8, 6);
	x += 8;

	for (i = 0; i < nw; i++)
	{
		draw_boxshadow(x, y, 8, 8, 7);
		x += 8;
	}

	draw_boxshadow(x, y, w, 8, 7);
	x += w;

	draw_boxshadow(x, y, 8, 8, 8);
}


/*------------------------------------------------------
	上部バーの影を描画
------------------------------------------------------*/

void draw_bar_shadow(void)
{
	int x;

	for (x = 0; x < SCR_WIDTH; x += 8)
	{
		draw_boxshadow(x,  0, 8, 8, 4);
		draw_boxshadow(x,  8, 8, 8, 4);
		draw_boxshadow(x, 16, 8, 4, 4);
		draw_boxshadow(x, 20, 8, 8, 7);
	}
}


#if VIDEO_32BPP

/*******************************************************
	ユーザインタフェース色設定
*******************************************************/

/*------------------------------------------------------
	UI_PALETTE取得
------------------------------------------------------*/

void get_ui_color(UI_PALETTE *pal, int *r, int *g, int *b)
{
	*r = pal->r;
	*g = pal->g;
	*b = pal->b;
}


/*------------------------------------------------------
	UI_PALETTE設定
------------------------------------------------------*/

void set_ui_color(UI_PALETTE *pal, int r, int g, int b)
{
	pal->r = r;
	pal->g = g;
	pal->b = b;
}

#endif /* VIDEO_32BPP */


/******************************************************************************
	ロゴ描画
******************************************************************************/

#include "psp/font/logo.c"

/*------------------------------------------------------
	ロゴ描画
------------------------------------------------------*/

void logo(int sx, int sy, int r, int g, int b)
{
	struct Vertex *vertices;
	int x, y, alpha;
	UINT16 color, *dst;

	r >>= 4;
	g >>= 4;
	b >>= 4;

	dst = tex_font;
	color = (b << 8) | (g << 4) | r;

	for (y = 0; y < 14; y++)
	{
#if (EMU_SYSTEM == MVS)
		for (x = 0; x < 208; x++)
#else
		for (x = 0; x < 232; x++)
#endif
		{
			if (x & 1)
				alpha = logo_data[y][x >> 1] >> 4;
			else
				alpha = logo_data[y][x >> 1] & 0x0f;

			dst[x] = (alpha << 12) | color;
		}
		dst += BUF_WIDTH;
	}

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, draw_frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuEnable(GU_BLEND);

	sceGuTexMode(GU_PSM_4444, 0, 0, GU_FALSE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, tex_font);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);

	vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

	if (vertices)
	{
		vertices[0].u = 0;
		vertices[0].v = 0;
		vertices[0].x = sx;
		vertices[0].y = sy;

#if (EMU_SYSTEM == MVS)
		vertices[1].u = 208;
		vertices[1].v = 14;
		vertices[1].x = sx + 208;
		vertices[1].y = sy + 14;
#else
		vertices[1].u = 232;
		vertices[1].v = 14;
		vertices[1].x = sx + 232;
		vertices[1].y = sy + 14;
#endif
	}

	sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2, NULL, vertices);

	sceGuDisable(GU_BLEND);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}
