/******************************************************************************

	ui_draw.c

	PSP ��`�����󥿥ե��`���軭�v��

******************************************************************************/

#ifndef PSP_UI_DRAW_H
#define PSP_UI_DRAW_H

struct font_t
{
	const uint8_t *data;
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

#define ui_fill_frame(frame, no)		video_driver->fillFrame(NULL, frame, MAKECOL32(ui_palette[no].r,ui_palette[no].g,ui_palette[no].b))
#define ui_fill_rect(frame, no, rect)	video_driver->fillRect(NULL, frame, MAKECOL32(ui_palette[no].r,ui_palette[no].g,ui_palette[no].b), rect)


/*------------------------------------------------------
	�Ʃ`�֥��
------------------------------------------------------*/

// ��`�����󥿥ե��`������`�ǩ`��
extern UI_PALETTE ui_palette[UI_PAL_MAX];

// ���`�໭���å���`��ե����
extern const uint8_t font_s[];

// �����������Ȥ�Ӱ�ǩ`��
extern const uint8_t shadow[9][8][4];

// GBK �ե���ȥƩ`�֥�
extern const uint16_t gbk_table[];


/*------------------------------------------------------
	��`�����󥿥ե��`�����ڻ�
------------------------------------------------------*/

void ui_init(void);


/*------------------------------------------------------
	�ե���ȹ���
------------------------------------------------------*/

// �ץ��ݩ`����ʥ�ե����
int graphic_font_get_gryph(struct font_t *font, uint16_t code);
int graphic_font_get_pitch(uint16_t code);

int ascii_14p_get_gryph(struct font_t *font, uint16_t code);
int ascii_14p_get_pitch(uint16_t code);

//int jpn_h14p_get_gryph(struct font_t *font, uint16_t code);
//int jpn_h14p_get_pitch(uint16_t code);

int gbk_s14p_get_gryph(struct font_t *font, uint16_t code);
int gbk_s14p_get_pitch(uint16_t code);

// ��������(С)
int icon_s_get_gryph(struct font_t *font, uint16_t code);
int icon_s_get_light(struct font_t *font, uint16_t code);

// ��������(��)
int icon_l_get_gryph(struct font_t *font, uint16_t code);
int icon_l_get_light(struct font_t *font, uint16_t code);

// �ȷ��ե����
#ifdef COMMAND_LIST
int command_font_get_gryph(struct font_t *font, uint16_t code);
int ascii_14_get_gryph(struct font_t *font, uint16_t code);
int latin1_14_get_gryph(struct font_t *font, uint16_t code);
//int jpn_h14_get_gryph(struct font_t *font, uint16_t code);
int gbk_s14_get_gryph(struct font_t *font, uint16_t code);
#endif


/*------------------------------------------------------
	�ե�����軭��ȡ�� (��`�����󥿥ե��`����)
------------------------------------------------------*/

int uifont_get_string_width(const char *s);


/*------------------------------------------------------
	�ե�����軭 (��`�����󥿥ե��`����)
------------------------------------------------------*/

void uifont_print(int sx, int sy, int r, int g, int b, const char *s);
void uifont_print_center(int sy, int r, int g, int b, const char *s);
void uifont_print_shadow(int sx, int sy, int r, int g, int b, const char *s);
void uifont_print_shadow_center(int sy, int r, int g, int b, const char *s);


/*------------------------------------------------------
	�ե�����軭 (�ƥ����ȱ�ʾ��)
------------------------------------------------------*/

#ifdef COMMAND_LIST
void textfont_print(int sx, int sy, int r, int g, int b, const char *s, int flag);
#endif


/*------------------------------------------------------
	���������軭 (��`�����󥿥ե��`����)
------------------------------------------------------*/

void small_icon(int sx, int sy, int r, int g, int b, int no);
void small_icon_shadow(int sx, int sy, int r, int g, int b, int no);
void small_icon_light(int sx, int sy, int r, int g, int b, int no);

void large_icon(int sx, int sy, int r, int g, int b, int no);
void large_icon_shadow(int sx, int sy, int r, int g, int b, int no);
void large_icon_light(int sx, int sy, int r, int g, int b, int no);

int ui_light_update(void);


/*------------------------------------------------------
	�ܥ��`���軭 (CFW 3.52�Խ��Υ�`���`��`�ɤΤ�)
------------------------------------------------------*/

void draw_volume(int volume);


/*------------------------------------------------------
	�ե�����軭 (���`�໭����)
------------------------------------------------------*/

void small_font_print(int sx, int sy, const char *s, int bg);
void small_font_printf(int x, int y, const char *text, ...);

void debug_font_printf(void *frame, int x, int y, const char *text, ...);


/*------------------------------------------------------
	�����軭 (��`�����󥿥ե��`����)
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
	��`�����󥿥ե��`��ɫ�O��
------------------------------------------------------*/

#if VIDEO_32BPP
void get_ui_color(UI_PALETTE *pal, int *r, int *g, int *b);
void set_ui_color(UI_PALETTE *pal, int r, int g, int b);
#endif

/*------------------------------------------------------
	�����軭
------------------------------------------------------*/

void logo(int sx, int sy, int r, int g, int b);

#endif /* PSP_UI_DRAW_H */
