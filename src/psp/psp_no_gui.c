/******************************************************************************

	ui.c

	•Ê©`•∂•§•Û•ø•’•ß©`•πÑI¿Ì

******************************************************************************/

#include "psp.h"
#include "emumain.h"
#include "stdarg.h"


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

int cheat_num = 0;
gamecheat_t* gamecheat[MAX_CHEATS];

#if VIDEO_32BPP
int bgimage_type;
int bgimage_blightness;
#endif

void msg_printf(const char *text, ...)
{
}

void show_progress(const char *text)
{
}

void update_progress(void)
{
}

void showmenu(void)
{
}

int draw_volume_status(int draw) {
	return 0;
}

int draw_battery_status(int draw) {
	return 0;	
}

void msg_screen_clear(void) {

}

void show_exit_screen(void) {

}

void load_background(int number)
{

}

int ui_show_popup(int draw) {
	return 0;
}

void file_browser(void) {
	Loop = LOOP_EXEC;
	strcpy(game_dir, "roms");
#if USE_CACHE
	sprintf(cache_dir, "%scache", "/");
#endif
	strcpy(game_name, "PBOBBL2N");

	emu_main();
}

void small_font_print(int sx, int sy, const char *s, int bg) {

}

void uifont_print_center(int sy, int r, int g, int b, const char *s) {

}

void uifont_print_shadow(int sx, int sy, int r, int g, int b, const char *s) {

}

void textfont_print(int sx, int sy, int r, int g, int b, const char *s, int flag) {

}

int uifont_get_string_width(const char *s) {
	return 1;
}

void uifont_print_shadow_center(int sy, int r, int g, int b, const char *s) {

}

void uifont_print(int sx, int sy, int r, int g, int b, const char *s) {

}

void small_icon_shadow(int sx, int sy, int r, int g, int b, int no) {

}

void show_background(void) {

}

void boxfill_alpha(int sx, int sy, int ex, int ey, int r, int g, int b, int alpha) {

}

void ui_popup_reset(void) {

}

void draw_dialog(int sx, int sy, int ex, int ey) {

}

int save_png(const char *path) {
	return 0;
}

void msg_screen_init(int wallpaper, int icon, const char *title) {

}

void draw_scrollbar(int sx, int sy, int ex, int ey, int disp_lines, int total_lines, int current_line) {

}

void ui_popup(const char *text, ...) {

}

int help(int number) {
	return 0;
}

void save_gamecfg(const char *name) {

}

void ui_init(void) {

}

void load_gamecfg(const char *name) {

}

void delete_files(const char *dirname, const char *pattern) {

}

void small_icon(int sx, int sy, int r, int g, int b, int no) {

}