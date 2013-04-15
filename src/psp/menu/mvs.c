/******************************************************************************

	menu/mvs.c

	PSP メニュー (MVS)

******************************************************************************/

#if defined(INCLUDE_GAMECFG_STRUCT)

/*-----------------------------------------------------------------------------
	gamecfg 構造体
-----------------------------------------------------------------------------*/

gamecfg2_t gamecfg_mvs[] =
{
	{ MACHINE_REGION, &neogeo_region,        CFG_RESTART,  3,  { DEFAULT,REGION_JAPAN,REGION_USA,REGION_EUROPE } },
	{ MACHINE_MODE,   &neogeo_machine_mode,  CFG_RESTART,  2,  { DEFAULT,MODE_AES,MODE_MVS } },
	MENU_BLANK,
	{ RASTER_EFFECTS, &neogeo_raster_enable, CFG_RESET,    1,  { OFF, ON } },
	MENU_BLANK,
	{ STRETCH_SCREEN, &option_stretch,       CFG_CONTINUE, 5,  { OFF, STRETCH1, STRETCH2, STRETCH3, STRETCH4, STRETCH5 } },
	{ VIDEO_SYNC,     &option_vsync,         CFG_RESET,    1,  { OFF, ON } },
	{ AUTO_FRAMESKIP, &option_autoframeskip, CFG_CONTINUE, 1,  { DISABLE, ENABLE } },
	{ FRAMESKIP,      &option_frameskip,     CFG_CONTINUE, 11, { OFF,SKIP1,SKIP2,SKIP3,SKIP4,SKIP5,SKIP6,SKIP7,SKIP8,SKIP9,SKIP10,SKIP11 } },
	{ SHOW_FPS,       &option_showfps,       CFG_CONTINUE, 1,  { OFF, ON } },
	{ FRAME_LIMIT,    &option_speedlimit,    CFG_CONTINUE, 1,  { OFF, ON } },
	MENU_BLANK,
	{ ENABLE_SOUND,   &option_sound_enable,  CFG_RESTART,  1,  { NO, YES } },
	{ SAMPLE_RATE,    &option_samplerate,    CFG_CONTINUE, 2,  { RATE11KHz,RATE22KHz,RATE44KHz } },
	{ SOUND_VOLUME,   &option_sound_volume,  CFG_CONTINUE, 10, { VOL0,VOL10,VOL20,VOL30,VOL40,VOL50,VOL60,VOL70,VOL80,VOL90,VOL100 } },
	MENU_BLANK,
	{ CONTROLLER,     &option_controller,    CFG_CONTINUE, 1,  { CONTROLLER1,CONTROLLER2 } },
	MENU_BLANK,
	{ PSP_CLOCK,      &psp_cpuclock,         CFG_CONTINUE, 3,  { CLK222MHz,CLK266MHz,CLK300MHz,CLK333MHz } },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

#elif defined(INCLUDE_GAMECFG_MENU)

/*-----------------------------------------------------------------------------
	gamecfg menu 初期化
-----------------------------------------------------------------------------*/

	gamecfg2 = gamecfg_mvs;

	if (machine_init_type == INIT_ms5pcb
	||	machine_init_type == INIT_svcpcb
	||	machine_init_type == INIT_kf2k3pcb
	||	neogeo_bios >= ASIA_AES)
	{
		gamecfg[0].enable = 0;
		gamecfg[1].enable = 0;
	}
	if (neogeo_ngh == NGH_tpgolf
	||	neogeo_ngh == NGH_trally
	||	neogeo_ngh == NGH_spinmast
	||	neogeo_ngh == NGH_neodrift)
	{
		gamecfg[3].enable = 0;
	}

#elif defined(INCLUDE_KEYCFG_STRUCT)

/*-----------------------------------------------------------------------------
	keycfg 構造体
-----------------------------------------------------------------------------*/

static keycfg2_t keycfg_mvs[] =
{
	{ BUTTON_LAYOUT,    KEYCFG_LAYOUT, NEOGEO_MVS  },
	MENU_BLANK,
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP       },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN     },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT     },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT    },
	{ INPUT_BUTTON1,    KEYCFG_BUTTON, P1_BUTTONA  },
	{ INPUT_BUTTON2,    KEYCFG_BUTTON, P1_BUTTONB  },
	{ INPUT_BUTTON3,    KEYCFG_BUTTON, P1_BUTTONC  },
	{ INPUT_BUTTON4,    KEYCFG_BUTTON, P1_BUTTOND  },
	{ INPUT_START,      KEYCFG_BUTTON, P1_START    },
	{ INPUT_COIN,       KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ INPUT_SERVCOIN,   KEYCFG_BUTTON, SERV_COIN   },
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, TEST_SWITCH },
	MENU_BLANK,
	{ AUTOFIRE1,        KEYCFG_BUTTON, P1_AF_A     },
	{ AUTOFIRE2,        KEYCFG_BUTTON, P1_AF_B     },
	{ AUTOFIRE3,        KEYCFG_BUTTON, P1_AF_C     },
	{ AUTOFIRE4,        KEYCFG_BUTTON, P1_AF_D     },
	{ AUTOFIRE_INV,     KEYCFG_NUMBER, 0           },
	MENU_BLANK,
	{ HOTKEY_AB,        KEYCFG_BUTTON, P1_AB       },
	{ HOTKEY_AC,        KEYCFG_BUTTON, P1_AC       },
	{ HOTKEY_AD,        KEYCFG_BUTTON, P1_AD       },
	{ HOTKEY_BC,        KEYCFG_BUTTON, P1_BC       },
	{ HOTKEY_BD,        KEYCFG_BUTTON, P1_BD       },
	{ HOTKEY_CD,        KEYCFG_BUTTON, P1_CD       },
	{ HOTKEY_ABC,       KEYCFG_BUTTON, P1_ABC      },
	{ HOTKEY_ABD,       KEYCFG_BUTTON, P1_ABD      },
	{ HOTKEY_ACD,       KEYCFG_BUTTON, P1_ACD      },
	{ HOTKEY_BCD,       KEYCFG_BUTTON, P1_BCD      },
	{ HOTKEY_ABCD,      KEYCFG_BUTTON, P1_ABCD     },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER    },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

static keycfg2_t keycfg_analog[] =
{
	{ BUTTON_LAYOUT,    KEYCFG_LAYOUT, NEOGEO_MVS  },
	MENU_BLANK,
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP       },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN     },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT     },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT    },
	{ INPUT_BUTTON1,    KEYCFG_BUTTON, P1_BUTTONA  },
	{ INPUT_BUTTON2,    KEYCFG_BUTTON, P1_BUTTONB  },
	{ INPUT_BUTTON3,    KEYCFG_BUTTON, P1_BUTTONC  },
	{ INPUT_BUTTON4,    KEYCFG_BUTTON, P1_BUTTOND  },
	{ INPUT_START,      KEYCFG_BUTTON, P1_START    },
	{ INPUT_COIN,       KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ ANALOG_SENS,      KEYCFG_ANALOG, 0           },
	MENU_BLANK,
	{ INPUT_SERVCOIN,   KEYCFG_BUTTON, SERV_COIN   },
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, TEST_SWITCH },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER    },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

static keycfg2_t keycfg_vliner[] =
{
	{ BUTTON_LAYOUT,    KEYCFG_LAYOUT, NEOGEO_MVS  },
	MENU_BLANK,
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP       },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN     },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT     },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT    },
	{ BIG,              KEYCFG_BUTTON, P1_BUTTONA  },
	{ SMALL,            KEYCFG_BUTTON, P1_BUTTONB  },
	{ DOUBLE_UP,        KEYCFG_BUTTON, P1_BUTTONC  },
	{ START_COLLECT,    KEYCFG_BUTTON, P1_BUTTOND  },
	{ PAYOUT,           KEYCFG_BUTTON, P1_START    },
	{ INPUT_COIN,       KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ OPERATOR_MENU,    KEYCFG_BUTTON, OTHER1      },
	{ CLEAR_CREDIT,     KEYCFG_BUTTON, OTHER2      },
	{ HOPPER_OUT,       KEYCFG_BUTTON, OTHER3      },
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, TEST_SWITCH },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

static keycfg2_t keycfg_jockeygp[] =
{
	{ BUTTON_LAYOUT,    KEYCFG_LAYOUT, NEOGEO_MVS  },
	MENU_BLANK,
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP       },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN     },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT     },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT    },
	{ BET,              KEYCFG_BUTTON, P1_BUTTONA  },
	{ CANCEL,           KEYCFG_BUTTON, P1_BUTTONB  },
	{ BET_CANCEL_ALL,   KEYCFG_BUTTON, P1_BUTTONC  },
	{ PAYOUT,           KEYCFG_BUTTON, P1_BUTTOND  },
	{ INPUT_COIN,       KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, TEST_SWITCH },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER    },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

#elif defined(INCLUDE_KEYCFG_MENU)

/*-----------------------------------------------------------------------------
	keycfg menu 初期化
-----------------------------------------------------------------------------*/

	switch (neogeo_ngh)
	{
	case NGH_irrmaze:
	case NGH_popbounc:
		keycfg2 = keycfg_analog;
		break;

	case NGH_vliner:
		keycfg2 = keycfg_vliner;
		break;

	case NGH_jockeygp:
		keycfg2 = keycfg_jockeygp;
		break;

	default:
		keycfg2 = keycfg_mvs;
		break;
	}

#elif defined(INCLUDE_LOAD_DIPSWITCH)

/*-----------------------------------------------------------------------------
	dipswitch menu 初期化
-----------------------------------------------------------------------------*/

	int old_value = neogeo_dipswitch & 0xff;
	int old_harddip = neogeo_hard_dipsw;

	dipswitch = load_dipswitch();

#elif defined(INCLUDE_SAVE_DIPSWITCH)

/*-----------------------------------------------------------------------------
	dipswitch menu 終了
-----------------------------------------------------------------------------*/

	save_dipswitch();

	if (neogeo_dipswitch != old_value)
	{
		menu_restart();
		return 1;
	}
	if (neogeo_hard_dipsw != old_harddip)
	{
		menu_reset();
		return 1;
	}

#endif
