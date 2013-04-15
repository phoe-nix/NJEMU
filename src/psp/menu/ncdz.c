/******************************************************************************

	menu/ncdz.c

	PSP メニュー (NCDZ)

******************************************************************************/

#if defined(INCLUDE_GAMECFG_STRUCT)

/*-----------------------------------------------------------------------------
	gamecfg 構造体
-----------------------------------------------------------------------------*/

static gamecfg2_t gamecfg_ncdz[] =
{
	{ MACHINE_REGION,      &neogeo_region,        CFG_RESTART,  2,  { REGION_JAPAN,REGION_USA,REGION_EUROPE } },
	{ RASTER_EFFECTS,      &neogeo_raster_enable, CFG_RESET,    1,  { OFF, ON } },
	{ EMULATE_LOAD_SCREEN, &neogeo_loadscreen,    CFG_RESTART,  1,  { NO, YES } },
	{ CDROM_SPEED_LIMIT,   &neogeo_cdspeed_limit, CFG_CONTINUE, 1,  { NO, YES } },
	MENU_BLANK,
	{ STRETCH_SCREEN,      &option_stretch,       CFG_CONTINUE, 5,  { OFF, STRETCH1, STRETCH2, STRETCH3, STRETCH4, STRETCH5 } },
	{ VIDEO_SYNC,          &option_vsync,         CFG_CONTINUE, 1,  { OFF, ON } },
	{ AUTO_FRAMESKIP,      &option_autoframeskip, CFG_CONTINUE, 1,  { DISABLE, ENABLE } },
	{ FRAMESKIP,           &option_frameskip,     CFG_CONTINUE, 11, { OFF,SKIP1,SKIP2,SKIP3,SKIP4,SKIP5,SKIP6,SKIP7,SKIP8,SKIP9,SKIP10,SKIP11 } },
	{ SHOW_FPS,            &option_showfps,       CFG_CONTINUE, 1,  { OFF, ON } },
	{ FRAME_LIMIT,         &option_speedlimit,    CFG_CONTINUE, 1,  { OFF, ON } },
	MENU_BLANK,
	{ ENABLE_SOUND,        &option_sound_enable,  CFG_RESTART,  1,  { NO, YES } },
	{ SAMPLE_RATE,         &option_samplerate,    CFG_CONTINUE, 2,  { RATE11KHz,RATE22KHz,RATE44KHz } },
	{ SOUND_VOLUME,        &option_sound_volume,  CFG_CONTINUE, 10, { VOL0,VOL10,VOL20,VOL30,VOL40,VOL50,VOL60,VOL70,VOL80,VOL90,VOL100 } },
	{ ENABLE_CDDA,         &option_mp3_enable,    CFG_RESTART,  1,  { NO, YES } },
	{ CDDA_VOLUME,         &option_mp3_volume,    CFG_CONTINUE, 10, { VOL0,VOL10,VOL20,VOL30,VOL40,VOL50,VOL60,VOL70,VOL80,VOL90,VOL100 } },
	MENU_BLANK,
	{ CONTROLLER,          &option_controller,    CFG_CONTINUE, 1,  { CONTROLLER1,CONTROLLER2 } },
	MENU_BLANK,
	{ PSP_CLOCK,           &psp_cpuclock,         CFG_CONTINUE, 3,  { CLK222MHz,CLK266MHz,CLK300MHz,CLK333MHz } },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

#elif defined(INCLUDE_GAMECFG_MENU)

/*-----------------------------------------------------------------------------
	gamecfg menu 初期化
-----------------------------------------------------------------------------*/

	gamecfg2 = gamecfg_ncdz;

	if (neogeo_ngh == NGH_aof2
	||	neogeo_ngh == NGH_tpgolf
	||	neogeo_ngh == NGH_trally
	||	neogeo_ngh == NGH_neodrift)
		gamecfg[1].enable = 0;

#elif defined(INCLUDE_KEYCFG_STRUCT)

/*-----------------------------------------------------------------------------
	keycfg 構造体
-----------------------------------------------------------------------------*/

static keycfg2_t keycfg_ncdz[] =
{
	{ BUTTON_LAYOUT,    KEYCFG_LAYOUT, NEOGEO_PAD },
	MENU_BLANK,
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP      },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN    },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT    },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT   },
	{ INPUT_BUTTON1,    KEYCFG_BUTTON, P1_BUTTONA },
	{ INPUT_BUTTON2,    KEYCFG_BUTTON, P1_BUTTONB },
	{ INPUT_BUTTON3,    KEYCFG_BUTTON, P1_BUTTONC },
	{ INPUT_BUTTON4,    KEYCFG_BUTTON, P1_BUTTOND },
	{ INPUT_START,      KEYCFG_BUTTON, P1_START   },
	{ INPUT_COIN,       KEYCFG_BUTTON, P1_SELECT  },
	MENU_BLANK,
	{ AUTOFIRE1,        KEYCFG_BUTTON, P1_AF_A    },
	{ AUTOFIRE2,        KEYCFG_BUTTON, P1_AF_B    },
	{ AUTOFIRE3,        KEYCFG_BUTTON, P1_AF_C    },
	{ AUTOFIRE4,        KEYCFG_BUTTON, P1_AF_D    },
	{ AUTOFIRE_INV,     KEYCFG_NUMBER, 0          },
	MENU_BLANK,
	{ HOTKEY_AB,        KEYCFG_BUTTON, P1_AB      },
	{ HOTKEY_AC,        KEYCFG_BUTTON, P1_AC      },
	{ HOTKEY_AD,        KEYCFG_BUTTON, P1_AD      },
	{ HOTKEY_BC,        KEYCFG_BUTTON, P1_BC      },
	{ HOTKEY_BD,        KEYCFG_BUTTON, P1_BD      },
	{ HOTKEY_CD,        KEYCFG_BUTTON, P1_CD      },
	{ HOTKEY_ABC,       KEYCFG_BUTTON, P1_ABC     },
	{ HOTKEY_ABD,       KEYCFG_BUTTON, P1_ABD     },
	{ HOTKEY_ACD,       KEYCFG_BUTTON, P1_ACD     },
	{ HOTKEY_BCD,       KEYCFG_BUTTON, P1_BCD     },
	{ HOTKEY_ABCD,      KEYCFG_BUTTON, P1_ABCD    },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT   },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER   },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

#elif defined(INCLUDE_KEYCFG_MENU)

/*-----------------------------------------------------------------------------
	keycfg menu 初期化
-----------------------------------------------------------------------------*/

	keycfg2 = keycfg_ncdz;

#endif
