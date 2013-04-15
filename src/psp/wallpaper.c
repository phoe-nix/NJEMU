/******************************************************************************

	wallpaper.c

	壁紙データ

******************************************************************************/

#include "psp.h"


/******************************************************************************
	グローバル変数
******************************************************************************/

UINT8 *wallpaper[NUM_WALLPAPERS];
UINT32 wallpaper_size[NUM_WALLPAPERS];


/******************************************************************************
	ローカル変数
******************************************************************************/

static UINT8 *wp_user[NUM_WALLPAPERS];
static UINT32 wp_user_size[NUM_WALLPAPERS];

#define INCLUDE_WALLPAPER

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
#include "psp/wallpaper/cps.c"
#elif (EMU_SYSTEM == MVS)
#include "psp/wallpaper/mvs.c"
#elif (EMU_SYSTEM == NCDZ)
#include "psp/wallpaper/ncdz.c"
#endif

#undef INCLUDE_WALLPAPER


/******************************************************************************
	グローバル関数
******************************************************************************/

/*------------------------------------------------------
	壁紙を初期化
------------------------------------------------------*/

void set_wallpaper(void)
{
	int i;

	wallpaper[WP_LOGO]     = wallpaper_logo;
	wallpaper[WP_FILER]    = wallpaper_filer;
	wallpaper[WP_GAMECFG]  = wallpaper_gamecfg;
	wallpaper[WP_KEYCFG]   = wallpaper_keycfg;
	wallpaper[WP_STATE]    = wallpaper_state;
	wallpaper[WP_COLORCFG] = wallpaper_colorcfg;
	wallpaper[WP_DIPSW]    = wallpaper_dipsw;
	wallpaper[WP_CMDLIST]  = wallpaper_cmdlist;

#define INCLUDE_WALLPAPER_SIZE

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
#include "psp/wallpaper/cps.c"
#elif (EMU_SYSTEM == MVS)
#include "psp/wallpaper/mvs.c"
#elif (EMU_SYSTEM == NCDZ)
#include "psp/wallpaper/ncdz.c"
#endif

#undef INCLUDE_WALLPAPER_SIZE

	if (bgimage_type != BG_DEFAULT)
	{
		for (i = 0; i < NUM_WALLPAPERS; i++)
		{
			if (wp_user[i])
			{
				wallpaper[i] = wp_user[i];
				wallpaper_size[i] = wp_user_size[i];
			}
		}
	}
}


/*------------------------------------------------------
	メモリに壁紙を読み込み
------------------------------------------------------*/

void load_wallpaper(void)
{
	int i, size;
	char path[MAX_PATH];
	FILE *fp;
	const char *wp_name[NUM_WALLPAPERS] =
	{
		"logo",
		"filer",
		"gamecfg",
		"keycfg",
		"state",
		"colorcfg",
		"dipsw",
		"cmdlist"
	};

	for (i = 0; i < NUM_WALLPAPERS; i++)
	{
		sprintf(path, "%sdata/%s.png", launchDir, wp_name[i]);

		wp_user[i] = NULL;
		wp_user_size[i] = 0;

		if ((fp = fopen(path, "rb")) != NULL)
		{
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			if ((wp_user[i] = (UINT8 *)malloc(size)) != NULL)
			{
				wp_user_size[i] = size;
				fread(wp_user[i], 1, size, fp);
			}

			fclose(fp);
		}
	}
	set_wallpaper();
}


/*------------------------------------------------------
	壁紙を開放
------------------------------------------------------*/

void free_wallpaper(void)
{
	int i;

	for (i = 0; i < NUM_WALLPAPERS; i++)
	{
		if (wp_user[i])
		{
			free(wp_user[i]);
			wp_user[i] = NULL;
			wp_user_size[i] = 0;
		}
	}
}
