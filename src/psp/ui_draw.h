/******************************************************************************

	ui_draw.c

	PSP ユーザインタフェース描画関数

******************************************************************************/

#ifndef PSP_UI_DRAW_H
#define PSP_UI_DRAW_H

struct font_t
{
	const UINT8 *data;
	int width;
	int height;
	int pitch;
	int skipx;
	int skipy;
};

#define FONTSIZE			14

#define FONT_UPARROW		"\x10"
#define FONT_DOWNARROW		"\x11"
#define FONT_LEFTARROW		"\x12"
#define FONT_RIGHTARROW		"\x13"
#define FONT_CIRCLE			"\x14"
#define FONT_CROSS			"\x15"
#define FONT_SQUARE			"\x16"
#define FONT_TRIANGLE		"\x17"
#define FONT_LTRIGGER		"\x18"
#define FONT_RTRIGGER		"\x19"
#define FONT_UPTRIANGLE		"\x1b"
#define FONT_DOWNTRIANGLE	"\x1c"
#define FONT_LEFTTRIANGLE	"\x1d"
#define FONT_RIGHTTRIANGLE	"\x1e"

enum
{
	ICON_CONFIG = 0,
	ICON_KEYCONFIG,
	ICON_FOLDER,
	ICON_SYSTEM,
	ICON_RETURN,
	ICON_EXIT,
	ICON_DIPSWITCH,
	ICON_CMDLIST,
	ICON_UPPERDIR,
	ICON_MEMSTICK,
	ICON_ZIPFILE,
	ICON_BATTERY1,
	ICON_BATTERY2,
	ICON_BATTERY3,
	ICON_BATTERY4,
	ICON_COMMANDDAT,
	ICON_COLORCFG,
	MAX_ICONS
};

typedef struct ui_palette_t
{
	int r;
	int g;
	int b;
} UI_PALETTE;

enum
{
	UI_PAL_TITLE = 0,
	UI_PAL_SELECT,
	UI_PAL_NORMAL,
	UI_PAL_INFO,
	UI_PAL_WARNING,
	UI_PAL_BG1,
	UI_PAL_BG2,
	UI_PAL_FRAME,
	UI_PAL_FILESEL1,
	UI_PAL_FILESEL2,
	UI_PAL_MAX
};

#define CHARSET_DEFAULT		0
#define CHARSET_ISO8859_1	1
#define CHARSET_LATIN1		1
#define CHARSET_SHIFTJIS	2
#define CHARSET_GBK			2

#define UI_COLOR(no)	ui_palette[no].r,ui_palette[no].g,ui_palette[no].b

#define ui_fill_frame(frame, no)		video_fill_frame(frame, MAKECOL32(ui_palette[no].r,ui_palette[no].g,ui_palette[no].b))
#define ui_fill_rect(frame, no, rect)	video_fill_rect(frame, MAKECOL32(ui_palette[no].r,ui_palette[no].g,ui_palette[no].b), rect)
#define ui_clear_rect(frame, no, rect)	video_fill_rect(frame, MAKECOL32(ui_palette[no].r,ui_palette[no].g,ui_palette[no].b0), rect)


/*------------------------------------------------------
	テーブル等
------------------------------------------------------*/

// ユーザインタフェースカラーデータ
extern UI_PALETTE ui_palette[UI_PAL_MAX];

// ゲーム画面用スモールフォント
extern const UINT8 font_s[];

// ダイアログ等の影データ
extern const UINT8 shadow[9][8][4];

// GBK フォントテーブル
extern const UINT16 gbk_table[];


/*------------------------------------------------------
	ユーザインタフェース初期化
------------------------------------------------------*/

void ui_init(void);


/*------------------------------------------------------
	フォント管理
------------------------------------------------------*/

// プロポーショナルフォント
int graphic_font_get_gryph(struct font_t *font, UINT16 code);
int graphic_font_get_pitch(UINT16 code);

int ascii_14p_get_gryph(struct font_t *font, UINT16 code);
int ascii_14p_get_pitch(UINT16 code);

//int jpn_h14p_get_gryph(struct font_t *font, UINT16 code);
//int jpn_h14p_get_pitch(UINT16 code);

int gbk_s14p_get_gryph(struct font_t *font, UINT16 code);
int gbk_s14p_get_pitch(UINT16 code);

// アイコン(小)
int icon_s_get_gryph(struct font_t *font, UINT16 code);
int icon_s_get_light(struct font_t *font, UINT16 code);

// アイコン(大)
int icon_l_get_gryph(struct font_t *font, UINT16 code);
int icon_l_get_light(struct font_t *font, UINT16 code);

// 等幅フォント
#ifdef COMMAND_LIST
int command_font_get_gryph(struct font_t *font, UINT16 code);
int ascii_14_get_gryph(struct font_t *font, UINT16 code);
int latin1_14_get_gryph(struct font_t *font, UINT16 code);
//int jpn_h14_get_gryph(struct font_t *font, UINT16 code);
int gbk_s14_get_gryph(struct font_t *font, UINT16 code);
#endif


/*------------------------------------------------------
	フォント描画幅取得 (ユーザインタフェース用)
------------------------------------------------------*/

int uifont_get_string_width(const char *s);


/*------------------------------------------------------
	フォント描画 (ユーザインタフェース用)
------------------------------------------------------*/

void uifont_print(int sx, int sy, int r, int g, int b, const char *s);
void uifont_print_center(int sy, int r, int g, int b, const char *s);
void uifont_print_shadow(int sx, int sy, int r, int g, int b, const char *s);
void uifont_print_shadow_center(int sy, int r, int g, int b, const char *s);


/*------------------------------------------------------
	フォント描画 (テキスト表示用)
------------------------------------------------------*/

#ifdef COMMAND_LIST
void textfont_print(int sx, int sy, int r, int g, int b, const char *s, int flag);
#endif


/*------------------------------------------------------
	アイコン描画 (ユーザインタフェース用)
------------------------------------------------------*/

void small_icon(int sx, int sy, int r, int g, int b, int no);
void small_icon_shadow(int sx, int sy, int r, int g, int b, int no);
void small_icon_light(int sx, int sy, int r, int g, int b, int no);

void large_icon(int sx, int sy, int r, int g, int b, int no);
void large_icon_shadow(int sx, int sy, int r, int g, int b, int no);
void large_icon_light(int sx, int sy, int r, int g, int b, int no);

int ui_light_update(void);


/*------------------------------------------------------
	ボリューム描画 (CFW 3.52以降のユーザーモードのみ)
------------------------------------------------------*/

void draw_volume(int volume);


/*------------------------------------------------------
	フォント描画 (ゲーム画面用)
------------------------------------------------------*/

void small_font_print(int sx, int sy, const char *s, int bg);
void small_font_printf(int x, int y, const char *text, ...);

void debug_font_printf(void *frame, int x, int y, const char *text, ...);


/*------------------------------------------------------
	図形描画 (ユーザインタフェース用)
------------------------------------------------------*/

void hline(int sx, int ex, int y, int r, int g, int b);
void hline_alpha(int sx, int ex, int y, int r, int g, int b, int alpha);
void hline_gradation(int sx, int ex, int y, int r1, int g1, int b1, int r2, int g2, int b2, int alpha);

void vline(int x, int sy, int ey, int r, int g, int b);
void vline_alpha(int x, int sy, int ey, int r, int g, int b, int alpha);
void vline_gradation(int x, int sy, int ey, int r1, int g1, int b1, int r2, int g2, int b2, int alpha);

void box(int sx, int sy, int ex, int ey, int r, int g, int b);

void boxfill(int sx, int sy, int ex, int ey, int r, int g, int b);
void boxfill_alpha(int sx, int sy, int ex, int ey, int r, int g, int b, int alpha);
void boxfill_gradation(int sx, int sy, int ex, int ey, int r1, int g1, int b1, int r2, int g2, int b2, int alpha, int dir);

void draw_bar_shadow(void);
void draw_box_shadow(int sx, int sy, int ex, int ey);

/*------------------------------------------------------
	ユーザインタフェース色設定
------------------------------------------------------*/

#if VIDEO_32BPP
void get_ui_color(UI_PALETTE *pal, int *r, int *g, int *b);
void set_ui_color(UI_PALETTE *pal, int r, int g, int b);
#endif

/*------------------------------------------------------
	ロゴ描画
------------------------------------------------------*/

void logo(int sx, int sy, int r, int g, int b);

#endif /* PSP_UI_DRAW_H */
