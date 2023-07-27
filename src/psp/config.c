/******************************************************************************

	config.c

	アプリケーション設定ファイル管理

******************************************************************************/

#include "psp.h"
#include "emumain.h"

#define LINEBUF_SIZE	256


enum
{
	CFG_NONE = 0,
	CFG_INT,
	CFG_BOOL,
	CFG_PAD,
	CFG_STR
};

enum
{
	PAD_NONE = 0,
	PAD_UP,
	PAD_DOWN,
	PAD_LEFT,
	PAD_RIGHT,
	PAD_CIRCLE,
	PAD_CROSS,
	PAD_SQUARE,
	PAD_TRIANGLE,
	PAD_LTRIGGER,
	PAD_RTRIGGER,
	PAD_START,
	PAD_SELECT,
	PAD_MAX
};

typedef struct cfg_t
{
	int type;
	const char *name;
	int *value;
	int def;
	int max;
} cfg_type;

typedef struct cfg2_t
{
	int type;
	const char *name;
	char *value;
	int max_len;
} cfg2_type;


/******************************************************************************
	ローカル構造体/変数
******************************************************************************/

#define INIVERSION	23

static int ini_version;

#define INCLUDE_INIFILENAME

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
#include "config/cps.c"
#elif (EMU_SYSTEM == MVS)
#include "config/mvs.c"
#elif (EMU_SYSTEM == NCDZ)
#include "config/ncdz.c"
#endif

#undef INCLUDE_INIFILENAME

static cfg_type default_options[] =
{
	{ CFG_NONE,	"[System Settings]", },
	{ CFG_INT,	"INIFileVersion",	&ini_version,	INIVERSION,		INIVERSION   },
#if (EMU_SYSTEM == MVS)
	{ CFG_NONE,	"[Emulation Settings]", },
	{ CFG_INT,	"NeogeoBIOS",		&neogeo_bios,	-1,	BIOS_MAX-1 },
#elif (EMU_SYSTEM == NCDZ)
	{ CFG_NONE,	"[Emulation Settings]", },
	{ CFG_INT,	"NeogeoRegion",		&neogeo_region,	1,	2	},
#endif

#if PSP_VIDEO_32BPP
	{ CFG_NONE,	"[Color Settings]", },
	{ CFG_INT,	"BGImageType",			&bgimage_type,					  3,   3 },
	{ CFG_INT,	"BGImageBlightness",	&bgimage_blightness,			 50, 100 },
	{ CFG_INT,	"TitleFontR",			&ui_palette[UI_PAL_TITLE].r,	255, 255 },
	{ CFG_INT,	"TitleFontG",			&ui_palette[UI_PAL_TITLE].g,	255, 255 },
	{ CFG_INT,	"TitleFontB",			&ui_palette[UI_PAL_TITLE].b,	255, 255 },
	{ CFG_INT,	"SelectFontR",			&ui_palette[UI_PAL_SELECT].r,	255, 255 },
	{ CFG_INT,	"SelectFontG",			&ui_palette[UI_PAL_SELECT].g,	255, 255 },
	{ CFG_INT,	"SelectFontB",			&ui_palette[UI_PAL_SELECT].b,	255, 255 },
	{ CFG_INT,	"NormalFontR",			&ui_palette[UI_PAL_NORMAL].r,	180, 255 },
	{ CFG_INT,	"NormalFontG",			&ui_palette[UI_PAL_NORMAL].g,	180, 255 },
	{ CFG_INT,	"NormalFontB",			&ui_palette[UI_PAL_NORMAL].b,	180, 255 },
	{ CFG_INT,	"InfoFontR",			&ui_palette[UI_PAL_INFO].r,	255, 255 },
	{ CFG_INT,	"InfoFontG",			&ui_palette[UI_PAL_INFO].g,	255, 255 },
	{ CFG_INT,	"InfoFontB",			&ui_palette[UI_PAL_INFO].b,	 64, 255 },
	{ CFG_INT,	"WarningFontR",			&ui_palette[UI_PAL_WARNING].r,	255, 255 },
	{ CFG_INT,	"WarningFontG",			&ui_palette[UI_PAL_WARNING].g,	 64, 255 },
	{ CFG_INT,	"WarningFontB",			&ui_palette[UI_PAL_WARNING].b,	 64, 255 },
	{ CFG_INT,	"BGColor1R",			&ui_palette[UI_PAL_BG1].r,		 48, 255 },
	{ CFG_INT,	"BGColor1G",			&ui_palette[UI_PAL_BG1].g,		 48, 255 },
	{ CFG_INT,	"BGColor1B",			&ui_palette[UI_PAL_BG1].b,		 48, 255 },
	{ CFG_INT,	"BGColor2R",			&ui_palette[UI_PAL_BG2].r,		  0, 255 },
	{ CFG_INT,	"BGColor3G",			&ui_palette[UI_PAL_BG2].g,		  0, 255 },
	{ CFG_INT,	"BGColor2B",			&ui_palette[UI_PAL_BG2].b,		160, 255 },
	{ CFG_INT,	"FrameColorR",			&ui_palette[UI_PAL_FRAME].r,	  0, 255 },
	{ CFG_INT,	"FrameColorG",			&ui_palette[UI_PAL_FRAME].g,	  0, 255 },
	{ CFG_INT,	"FrameColorB",			&ui_palette[UI_PAL_FRAME].b,	  0, 255 },
	{ CFG_INT,	"FileSelect1R",			&ui_palette[UI_PAL_FILESEL1].r, 40, 255 },
	{ CFG_INT,	"FileSelect1G",			&ui_palette[UI_PAL_FILESEL1].g, 40, 255 },
	{ CFG_INT,	"FileSelect1B",			&ui_palette[UI_PAL_FILESEL1].b, 40, 255 },
	{ CFG_INT,	"FileSelect2R",			&ui_palette[UI_PAL_FILESEL2].r, 120, 255 },
	{ CFG_INT,	"FileSelect2G",			&ui_palette[UI_PAL_FILESEL2].g, 120, 255 },
	{ CFG_INT,	"FileSelect2B",			&ui_palette[UI_PAL_FILESEL2].b, 120, 255 },
#endif

	{ CFG_NONE, NULL, }
};

static cfg2_type default_options2[] =
{
	{ CFG_NONE,	"[Directory Settings]", 				},
	{ CFG_STR,	"StartupDir", startupDir,	MAX_PATH	},
	{ CFG_NONE, NULL, }
};

#define INCLUDE_CONFIG_STRUCT

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
#include "config/cps.c"
#elif (EMU_SYSTEM == MVS)
#include "config/mvs.c"
#elif (EMU_SYSTEM == NCDZ)
#include "config/ncdz.c"
#endif

#undef INCLUDE_CONFIG_STRUCT

typedef struct padname_t
{
	int code;
	const char name[16];
} PADNAME;

static const PADNAME pad_name[13] =
{
	{ 0,					"PAD_NONE"		},
	{ PSP_CTRL_UP,			"PAD_UP"		},
	{ PSP_CTRL_DOWN,		"PAD_DOWN"		},
	{ PSP_CTRL_LEFT,		"PAD_LEFT"		},
	{ PSP_CTRL_RIGHT,		"PAD_RIGHT"		},
	{ PSP_CTRL_CROSS,		"PAD_CROSS"		},
	{ PSP_CTRL_CIRCLE,		"PAD_CIRCLE"	},
	{ PSP_CTRL_SQUARE,		"PAD_SQUARE"	},
	{ PSP_CTRL_TRIANGLE,	"PAD_TRIANGLE"	},
	{ PSP_CTRL_START,		"PAD_START"		},
	{ PSP_CTRL_SELECT,		"PAD_SELECT"	},
	{ PSP_CTRL_LTRIGGER,	"PAD_LTRIGGER"	},
	{ PSP_CTRL_RTRIGGER,	"PAD_RTRIGGER"	}
};


/******************************************************************************
	ローカル関数
******************************************************************************/

/*------------------------------------------------------
	CFG_BOOLの値を読み込む
------------------------------------------------------*/

static int get_config_bool(char *str)
{
	if (!strcasecmp(str, "yes"))
		return 1;
	else
		return 0;
}


/*------------------------------------------------------
	CFG_INTの値を読み込む
------------------------------------------------------*/

static int get_config_int(char *str, int maxval)
{
	int value = atoi(str);

	if (value < 0) value = 0;
	if (value > maxval) value = maxval;
	return value;
}


/*------------------------------------------------------
	CFG_PADの値を読み込む
------------------------------------------------------*/

static int get_config_pad(char *str)
{
	int i;

	for (i = 0; i < PAD_MAX; i++)
	{
		if (strcmp(str, pad_name[i].name) == 0)
			return pad_name[i].code;
	}

	return pad_name[PAD_NONE].code;
}


/*------------------------------------------------------
	CFG_BOOLの値を保存する
------------------------------------------------------*/

static const char *set_config_bool(int value)
{
	if (value)
		return "yes";
	else
		return "no";
}


/*------------------------------------------------------
	CFG_INTの値を保存する
------------------------------------------------------*/

static char *set_config_int(int value, int maxval)
{
	static char buf[16];

	if (value < 0) value = 0;
	if (value > maxval) value = maxval;

	sprintf(buf, "%d", value);

	return buf;
}


/*------------------------------------------------------
	CFG_PADの値を保存する
------------------------------------------------------*/

static const char *set_config_pad(int value)
{
	int i;

	for (i = 0; i < PAD_MAX; i++)
	{
		if (value == pad_name[i].code)
			return pad_name[i].name;
	}

	return pad_name[PAD_NONE].name;
}


/*------------------------------------------------------
	.iniファイルから設定を読み込む
------------------------------------------------------*/

static int load_inifile(const char *path, cfg_type *cfg, cfg2_type *cfg2)
{
	FILE *fp;

	if ((fp = fopen(path, "r")) != NULL)
	{
		int i;
		char linebuf[LINEBUF_SIZE];

		while (1)
		{
			char *name, *value;

			memset(linebuf, LINEBUF_SIZE, 0);
			if (fgets(linebuf, LINEBUF_SIZE - 1, fp) == NULL)
				break;

			if (linebuf[0] == ';' || linebuf[0] == '[')
				continue;

			name = strtok(linebuf, " =\r\n");
			if (name == NULL)
				continue;

			value = strtok(NULL, " =\r\n");
			if (value == NULL)
				continue;

			/* check name and value */
			for (i = 0; cfg[i].name; i++)
			{
				if (!strcmp(name, cfg[i].name))
				{
					switch (cfg[i].type)
					{
					case CFG_INT:  *cfg[i].value = get_config_int(value, cfg[i].max); break;
					case CFG_BOOL: *cfg[i].value = get_config_bool(value); break;
					case CFG_PAD:  *cfg[i].value = get_config_pad(value); break;
					}
				}
			}
		}

		if (cfg2)
		{
			fseek(fp, 0, SEEK_SET);

			while (1)
			{
				char *name, *value;
				char *p1, *p2, temp[LINEBUF_SIZE];

				memset(linebuf, LINEBUF_SIZE, 0);
				if (fgets(linebuf, LINEBUF_SIZE - 1, fp) == NULL)
					break;

				strcpy(temp, linebuf);

				if (linebuf[0] == ';' || linebuf[0] == '[')
					continue;

				name = strtok(linebuf, " =\r\n");
				if (name == NULL)
					continue;

				value = strtok(NULL, " =\r\n");
				if (value == NULL)
					continue;

				p1 = strchr(temp, '\"');
				if (p1)
				{
					p2 = strchr(p1 + 1, '\"');
					if (p2)
					{
						value = p1 + 1;
						*p2 = '\0';
					}
				}

				/* check name and value */
				for (i = 0; cfg2[i].name; i++)
				{
					if (!strcmp(name, cfg2[i].name))
					{
						if (cfg2[i].type == CFG_STR)
						{
							memset(cfg2[i].value, 0, cfg2[i].max_len);
							strncpy(cfg2[i].value, value, cfg2[i].max_len - 1);
						}
					}
				}
			}
		}

		fclose(fp);

		return 1;
	}

	return 0;
}


/*------------------------------------------------------
	.iniファイルに設定を保存
------------------------------------------------------*/

static int save_inifile(const char *path, cfg_type *cfg, cfg2_type *cfg2)
{
	FILE *fp;

	if ((fp = fopen(path, "w")) != NULL)
	{
		int i;

		fprintf(fp, ";-------------------------------------------\r\n");
		fprintf(fp, "; " APPNAME_STR " " VERSION_STR "\r\n");
		fprintf(fp, ";-------------------------------------------\r\n");

		for (i = 0; cfg[i].name; i++)
		{
			switch (cfg[i].type)
			{
			case CFG_NONE: if (cfg[i].name) fprintf(fp, "\r\n%s\r\n", cfg[i].name); break;
			case CFG_INT:  fprintf(fp, "%s = %s\r\n", cfg[i].name, set_config_int(*cfg[i].value, cfg[i].max)); break;
			case CFG_BOOL: fprintf(fp, "%s = %s\r\n", cfg[i].name, set_config_bool(*cfg[i].value)); break;
			case CFG_PAD:  fprintf(fp, "%s = %s\r\n", cfg[i].name, set_config_pad(*cfg[i].value)); break;
			}
		}

		if (cfg2)
		{
			for (i = 0; cfg2[i].name; i++)
			{
				switch (cfg2[i].type)
				{
				case CFG_NONE: if (cfg2[i].name) fprintf(fp, "\r\n%s\r\n", cfg2[i].name); break;
				case CFG_STR:  fprintf(fp, "%s = \"%s\"\r\n", cfg2[i].name, cfg2[i].value); break;
				}
			}
		}

		fclose(fp);

		return 1;
	}

	return 0;
}


/******************************************************************************
	グローバル関数
******************************************************************************/

/*------------------------------------------------------
	アプリケーションの設定を読み込む
------------------------------------------------------*/

void load_settings(void)
{
	int i;
	char path[MAX_PATH];

	for (i = 0; default_options[i].name; i++)
	{
		if (default_options[i].value)
			*default_options[i].value = default_options[i].def;
	}
#if (EMU_SYSTEM == NCDZ)
	if (ui_text_get_language() == LANG_JAPANESE)
	{
		for (i = 0; default_options[i].name; i++)
		{
			if (strcmp(default_options[i].name, "NeogeoRegion") == 0)
			{
				*default_options[i].value = 0;
				break;
			}
		}
	}
#endif

	sprintf(path, "%s%s", launchDir, inifile_name);

	if (load_inifile(path, default_options, default_options2) == 0)
	{
		save_settings();
	}
	else if (ini_version != INIVERSION)
	{
		for (i = 0; default_options[i].name; i++)
		{
			if (default_options[i].value)
				*default_options[i].value = default_options[i].def;
		}
#if (EMU_SYSTEM == NCDZ)
		if (ui_text_get_language() == LANG_JAPANESE)
		{
			for (i = 0; default_options[i].name; i++)
			{
				if (strcmp(default_options[i].name, "NeogeoRegion") == 0)
				{
					*default_options[i].value = 0;
					break;
				}
			}
		}
#endif

		sprintf(startupDir, "%sroms", launchDir);

		sceIoRemove(inifile_name);
		delete_files("nvram", "nv");
		delete_files("config", "ini");

		save_settings();
	}
}


/*------------------------------------------------------
	アプリケーションの設定を保存する
------------------------------------------------------*/

void save_settings(void)
{
	char path[MAX_PATH];

	sprintf(path, "%s%s", launchDir, inifile_name);

	save_inifile(path, default_options, default_options2);
}


/*------------------------------------------------------
	ゲームの設定を読み込む
------------------------------------------------------*/

void load_gamecfg(const char *name)
{
	int i;
	char path[MAX_PATH];
	cfg_type *gamecfg;

	sprintf(path, "%sconfig/%s.ini", launchDir, name);

	memset(input_map, 0, sizeof(input_map));

#define INCLUDE_SETUP_CONFIG_STRUCT

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
#include "config/cps.c"
#elif (EMU_SYSTEM == MVS)
#include "config/mvs.c"
#elif (EMU_SYSTEM == NCDZ)
#include "config/ncdz.c"
#endif

#undef INCLUDE_SETUP_CONFIG_STRUCT

	for (i = 0; gamecfg[i].name; i++)
	{
		if (gamecfg[i].value)
			*gamecfg[i].value = gamecfg[i].def;
	}

#define INCLUDE_SETUP_DIPSWITCH

#if (EMU_SYSTEM == CPS1)
#include "config/cps.c"
#elif (EMU_SYSTEM == MVS)
#include "config/mvs.c"
#endif

#undef INCLUDE_SETUP_DIPSWITCH

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
	if (!machine_screen_type) cps_rotate_screen = 0;
#endif

	if (load_inifile(path, gamecfg, NULL) == 0)
	{
#ifdef ADHOC
		if (adhoc_enable)
#endif
			save_gamecfg(name);
	}
}


/*------------------------------------------------------
	ゲームの設定を保存する
------------------------------------------------------*/

void save_gamecfg(const char *name)
{
	char path[MAX_PATH];
	cfg_type *gamecfg;

	sprintf(path, "%sconfig/%s.ini", launchDir, name);

#define INCLUDE_SETUP_CONFIG_STRUCT

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
#include "config/cps.c"
#elif (EMU_SYSTEM == MVS)
#include "config/mvs.c"
#elif (EMU_SYSTEM == NCDZ)
#include "config/ncdz.c"
#endif

#undef INCLUDE_SETUP_CONFIG_STRUCT

	save_inifile(path, gamecfg, NULL);
}
