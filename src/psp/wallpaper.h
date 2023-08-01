/******************************************************************************

	wallpaper.c

	NCDZPSP用壁紙データ

******************************************************************************/

#ifndef PSP_WALLPAPER_H
#define PSP_WALLPAPER_H

enum
{
	WP_LOGO = 0,
	WP_FILER,
	WP_GAMECFG,
	WP_KEYCFG,
	WP_STATE,
	WP_COLORCFG,
	WP_DIPSW,
	WP_CMDLIST,
	NUM_WALLPAPERS
};

extern uint8_t *wallpaper[NUM_WALLPAPERS];
extern uint32_t wallpaper_size[NUM_WALLPAPERS];

void set_wallpaper(void);
void load_wallpaper(void);
void free_wallpaper(void);

#endif /* PSP_WALLPAPER_H */
