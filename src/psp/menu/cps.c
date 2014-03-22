/******************************************************************************

	menu/cps.c

	PSP メニュー (CPS1/CPS2共通)

******************************************************************************/

#if defined(INCLUDE_GAMECFG_STRUCT)

/*-----------------------------------------------------------------------------
	gamecfg 構造体
-----------------------------------------------------------------------------*/

static gamecfg2_t gamecfg_normal[] =
{
#if ENABLE_RASTER_OPTION || (EMU_SYSTEM == CPS1)
	{ RASTER_EFFECTS, &cps_raster_enable,    CFG_CONTINUE, 1,  { OFF, ON } },
	MENU_BLANK,
#endif
	{ STRETCH_SCREEN, &option_stretch,       CFG_CONTINUE, 4,  { OFF, STRETCH1, STRETCH2, STRETCH3, STRETCH4 } },
	{ VIDEO_SYNC,     &option_vsync,         CFG_CONTINUE, 1,  { OFF, ON } },
	{ AUTO_FRAMESKIP, &option_autoframeskip, CFG_CONTINUE, 1,  { DISABLE, ENABLE } },
	{ FRAMESKIP,      &option_frameskip,     CFG_CONTINUE, 11, { OFF,SKIP1,SKIP2,SKIP3,SKIP4,SKIP5,SKIP6,SKIP7,SKIP8,SKIP9,SKIP10,SKIP11 } },
	{ SHOW_FPS,       &option_showfps,       CFG_CONTINUE, 1,  { OFF, ON } },
	{ FRAME_LIMIT,    &option_speedlimit,    CFG_CONTINUE, 1,  { OFF, ON } },
	MENU_BLANK,
	{ ENABLE_SOUND,   &option_sound_enable,  CFG_RESTART,  1,  { NO, YES } },
#if (EMU_SYSTEM == CPS1)
	{ SAMPLE_RATE,    &option_samplerate,    CFG_CONTINUE, 2,  { RATE11KHz,RATE22KHz,RATE44KHz } },
#endif
	{ SOUND_VOLUME,   &option_sound_volume,  CFG_CONTINUE, 10, { VOL0,VOL10,VOL20,VOL30,VOL40,VOL50,VOL60,VOL70,VOL80,VOL90,VOL100 } },
	MENU_BLANK,
	{ CONTROLLER,     &option_controller,    CFG_CONTINUE, 3,  { CONTROLLER1,CONTROLLER2,CONTROLLER3,CONTROLLER4 } },
	MENU_BLANK,
	{ PSP_CLOCK,      &psp_cpuclock,         CFG_CONTINUE, 3,  { CLK222MHz,CLK266MHz,CLK300MHz,CLK333MHz } },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

static gamecfg2_t gamecfg_vertical[] =
{
#if ENABLE_RASTER_OPTION || (EMU_SYSTEM == CPS1)
	{ RASTER_EFFECTS, &cps_raster_enable,    CFG_CONTINUE, 1,  { OFF, ON } },
	MENU_BLANK,
#endif
	{ STRETCH_SCREEN, &option_stretch,       CFG_CONTINUE, 4,  { OFF, STRETCH1, STRETCH2, STRETCH3, STRETCH4 } },
	{ ROTATE_SCREEN,  &cps_rotate_screen,    CFG_CONTINUE, 1,  { NO, YES } },
	{ VIDEO_SYNC,     &option_vsync,         CFG_CONTINUE, 1,  { OFF, ON } },
	{ AUTO_FRAMESKIP, &option_autoframeskip, CFG_CONTINUE, 1,  { DISABLE, ENABLE } },
	{ FRAMESKIP,      &option_frameskip,     CFG_CONTINUE, 11, { OFF,SKIP1,SKIP2,SKIP3,SKIP4,SKIP5,SKIP6,SKIP7,SKIP8,SKIP9,SKIP10,SKIP11 } },
	{ SHOW_FPS,       &option_showfps,       CFG_CONTINUE, 1,  { OFF, ON } },
	{ FRAME_LIMIT,    &option_speedlimit,    CFG_CONTINUE, 1,  { OFF, ON } },
	MENU_BLANK,
	{ ENABLE_SOUND,   &option_sound_enable,  CFG_RESTART,  1,  { NO, YES } },
#if (EMU_SYSTEM == CPS1)
	{ SAMPLE_RATE,    &option_samplerate,    CFG_CONTINUE, 2,  { RATE11KHz,RATE22KHz,RATE44KHz } },
#endif
	{ SOUND_VOLUME,   &option_sound_volume,  CFG_CONTINUE, 10, { VOL0,VOL10,VOL20,VOL30,VOL40,VOL50,VOL60,VOL70,VOL80,VOL90,VOL100 } },
	MENU_BLANK,
	{ CONTROLLER,     &option_controller,    CFG_CONTINUE, 1,  { CONTROLLER1,CONTROLLER2 } },
	MENU_BLANK,
	{ PSP_CLOCK,      &psp_cpuclock,         CFG_CONTINUE, 3,  { CLK222MHz,CLK266MHz,CLK300MHz,CLK333MHz } },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};
#if (EMU_SYSTEM == CPS1)
static gamecfg2_t gamecfg_mercs[] =
{
#if ENABLE_RASTER_OPTION
	{ RASTER_EFFECTS, &cps_raster_enable,    CFG_CONTINUE, 1,  { OFF, ON } },
	MENU_BLANK,
#endif
	{ STRETCH_SCREEN, &option_stretch,       CFG_CONTINUE, 4,  { OFF, STRETCH1, STRETCH2, STRETCH3, STRETCH4 } },
	{ ROTATE_SCREEN,  &cps_rotate_screen,    CFG_CONTINUE, 1,  { NO, YES } },
	{ VIDEO_SYNC,     &option_vsync,         CFG_CONTINUE, 1,  { OFF, ON } },
	{ AUTO_FRAMESKIP, &option_autoframeskip, CFG_CONTINUE, 1,  { DISABLE, ENABLE } },
	{ FRAMESKIP,      &option_frameskip,     CFG_CONTINUE, 11, { OFF,SKIP1,SKIP2,SKIP3,SKIP4,SKIP5,SKIP6,SKIP7,SKIP8,SKIP9,SKIP10,SKIP11 } },
	{ SHOW_FPS,       &option_showfps,       CFG_CONTINUE, 1,  { OFF, ON } },
	{ FRAME_LIMIT,    &option_speedlimit,    CFG_CONTINUE, 1,  { OFF, ON } },
	MENU_BLANK,
	{ ENABLE_SOUND,   &option_sound_enable,  CFG_RESTART,  1,  { NO, YES } },
	{ SAMPLE_RATE,    &option_samplerate,    CFG_CONTINUE, 2,  { RATE11KHz,RATE22KHz,RATE44KHz } },
	{ SOUND_VOLUME,   &option_sound_volume,  CFG_CONTINUE, 10, { VOL0,VOL10,VOL20,VOL30,VOL40,VOL50,VOL60,VOL70,VOL80,VOL90,VOL100 } },
	MENU_BLANK,
	{ CONTROLLER,     &option_controller,    CFG_CONTINUE, 2,  { CONTROLLER1,CONTROLLER2,CONTROLLER3 } },
	MENU_BLANK,
	{ PSP_CLOCK,      &psp_cpuclock,         CFG_CONTINUE, 3,  { CLK222MHz,CLK266MHz,CLK300MHz,CLK333MHz } },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

static gamecfg2_t gamecfg_qsound[] =
{
	{ RASTER_EFFECTS, &cps_raster_enable,    CFG_CONTINUE, 1,  { OFF, ON } },
	MENU_BLANK,
	{ STRETCH_SCREEN, &option_stretch,       CFG_CONTINUE, 4,  { OFF, STRETCH1, STRETCH2, STRETCH3, STRETCH4 } },
	{ VIDEO_SYNC,     &option_vsync,         CFG_CONTINUE, 1,  { OFF, ON } },
	{ AUTO_FRAMESKIP, &option_autoframeskip, CFG_CONTINUE, 1,  { DISABLE, ENABLE } },
	{ FRAMESKIP,      &option_frameskip,     CFG_CONTINUE, 11, { OFF,SKIP1,SKIP2,SKIP3,SKIP4,SKIP5,SKIP6,SKIP7,SKIP8,SKIP9,SKIP10,SKIP11 } },
	{ SHOW_FPS,       &option_showfps,       CFG_CONTINUE, 1,  { OFF, ON } },
	{ FRAME_LIMIT,    &option_speedlimit,    CFG_CONTINUE, 1,  { OFF, ON } },
	MENU_BLANK,
	{ ENABLE_SOUND,   &option_sound_enable,  CFG_RESTART,  1,  { NO, YES } },
	{ SOUND_VOLUME,   &option_sound_volume,  CFG_CONTINUE, 10, { VOL0,VOL10,VOL20,VOL30,VOL40,VOL50,VOL60,VOL70,VOL80,VOL90,VOL100 } },
	MENU_BLANK,
	{ CONTROLLER,     &option_controller,    CFG_CONTINUE, 3,  { CONTROLLER1,CONTROLLER2,CONTROLLER3,CONTROLLER4 } },
	MENU_BLANK,
	{ PSP_CLOCK,      &psp_cpuclock,         CFG_CONTINUE, 3,  { CLK222MHz,CLK266MHz,CLK300MHz,CLK333MHz } },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};
#endif

#elif defined(INCLUDE_GAMECFG_MENU)

/*-----------------------------------------------------------------------------
	gamecfg menu 初期化
-----------------------------------------------------------------------------*/
#if (EMU_SYSTEM == CPS2)
	if (machine_screen_type)
	{
		gamecfg2 = gamecfg_vertical;

	}
#endif
#if (EMU_SYSTEM == CPS1)
	if (machine_screen_type && machine_driver_type != MACHINE_mercs)
	{
		gamecfg2 = gamecfg_vertical;

	}
	else if (machine_driver_type == MACHINE_qsound)
	{
		gamecfg2 = gamecfg_qsound;
		gamecfg2[12].value_max = input_max_players - 1;	//fix max controller
	}
#endif
	else
	{
		gamecfg2 = gamecfg_normal;
		gamecfg2[13].value_max = input_max_players - 1;
	}

	if (option_controller >= input_max_players)
		option_controller = INPUT_PLAYER1;
#if (EMU_SYSTEM == CPS1)
	if (machine_driver_type == MACHINE_mercs)   //mercs controller3 fix
	{
		gamecfg2 = gamecfg_mercs;
	}
#endif
#elif defined(INCLUDE_KEYCFG_STRUCT)

/*-----------------------------------------------------------------------------
	keycfg 構造体
-----------------------------------------------------------------------------*/

static keycfg2_t keycfg_2buttons[] =
{
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP       },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN     },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT     },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT    },
	{ INPUT_BUTTON1,    KEYCFG_BUTTON, P1_BUTTON1  },
	{ INPUT_BUTTON2,    KEYCFG_BUTTON, P1_BUTTON2  },
	{ INPUT_START,      KEYCFG_BUTTON, P1_START    },
	{ INPUT_COIN,       KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ INPUT_SERVCOIN,   KEYCFG_BUTTON, SERV_COIN   },
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, SERV_SWITCH },
	MENU_BLANK,
	{ AUTOFIRE1,        KEYCFG_BUTTON, P1_AF_1     },
	{ AUTOFIRE2,        KEYCFG_BUTTON, P1_AF_2     },
	{ AUTOFIRE_INV,     KEYCFG_NUMBER, 0           },
	MENU_BLANK,
	{ HOTKEY_12,        KEYCFG_BUTTON, P1_12       },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER    },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

static keycfg2_t keycfg_3buttons[] =
{
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP       },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN     },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT     },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT    },
	{ INPUT_BUTTON1,    KEYCFG_BUTTON, P1_BUTTON1  },
	{ INPUT_BUTTON2,    KEYCFG_BUTTON, P1_BUTTON2  },
	{ INPUT_BUTTON3,    KEYCFG_BUTTON, P1_BUTTON3  },
	{ INPUT_START,      KEYCFG_BUTTON, P1_START    },
	{ INPUT_COIN,       KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ INPUT_SERVCOIN,   KEYCFG_BUTTON, SERV_COIN   },
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, SERV_SWITCH },
	MENU_BLANK,
	{ AUTOFIRE1,        KEYCFG_BUTTON, P1_AF_1     },
	{ AUTOFIRE2,        KEYCFG_BUTTON, P1_AF_2     },
	{ AUTOFIRE3,        KEYCFG_BUTTON, P1_AF_3     },
	{ AUTOFIRE_INV,     KEYCFG_NUMBER, 0           },
	MENU_BLANK,
	{ HOTKEY_12,        KEYCFG_BUTTON, P1_12       },
	{ HOTKEY_13,        KEYCFG_BUTTON, P1_13       },
	{ HOTKEY_23,        KEYCFG_BUTTON, P1_23       },
	{ HOTKEY_123,       KEYCFG_BUTTON, P1_123      },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER    },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

#if (EMU_SYSTEM == CPS2)
static keycfg2_t keycfg_4buttons[] =
{
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP       },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN     },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT     },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT    },
	{ INPUT_BUTTON1,    KEYCFG_BUTTON, P1_BUTTON1  },
	{ INPUT_BUTTON2,    KEYCFG_BUTTON, P1_BUTTON2  },
	{ INPUT_BUTTON3,    KEYCFG_BUTTON, P1_BUTTON3  },
	{ INPUT_BUTTON4,    KEYCFG_BUTTON, P1_BUTTON4  },
	{ INPUT_START,      KEYCFG_BUTTON, P1_START    },
	{ INPUT_COIN,       KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ INPUT_SERVCOIN,   KEYCFG_BUTTON, SERV_COIN   },
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, SERV_SWITCH },
	MENU_BLANK,
	{ AUTOFIRE1,        KEYCFG_BUTTON, P1_AF_1     },
	{ AUTOFIRE2,        KEYCFG_BUTTON, P1_AF_2     },
	{ AUTOFIRE3,        KEYCFG_BUTTON, P1_AF_3     },
	{ AUTOFIRE4,        KEYCFG_BUTTON, P1_AF_4     },
	{ AUTOFIRE_INV,     KEYCFG_NUMBER, 0           },
	MENU_BLANK,
	{ HOTKEY_12,        KEYCFG_BUTTON, P1_12       },
	{ HOTKEY_13,        KEYCFG_BUTTON, P1_13       },
	{ HOTKEY_14,        KEYCFG_BUTTON, P1_14       },
	{ HOTKEY_23,        KEYCFG_BUTTON, P1_23       },
	{ HOTKEY_24,        KEYCFG_BUTTON, P1_24       },
	{ HOTKEY_34,        KEYCFG_BUTTON, P1_34       },
	{ HOTKEY_123,       KEYCFG_BUTTON, P1_123      },
	{ HOTKEY_124,       KEYCFG_BUTTON, P1_124      },
	{ HOTKEY_134,       KEYCFG_BUTTON, P1_134      },
	{ HOTKEY_234,       KEYCFG_BUTTON, P1_234      },
	{ HOTKEY_1234,      KEYCFG_BUTTON, P1_1234     },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER    },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};
#endif

static keycfg2_t keycfg_6buttons[] =
{
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP       },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN     },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT     },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT    },
	{ INPUT_BUTTON1,    KEYCFG_BUTTON, P1_BUTTON1  },
	{ INPUT_BUTTON2,    KEYCFG_BUTTON, P1_BUTTON2  },
	{ INPUT_BUTTON3,    KEYCFG_BUTTON, P1_BUTTON3  },
	{ INPUT_BUTTON4,    KEYCFG_BUTTON, P1_BUTTON4  },
	{ INPUT_BUTTON5,    KEYCFG_BUTTON, P1_BUTTON5  },
	{ INPUT_BUTTON6,    KEYCFG_BUTTON, P1_BUTTON6  },
	{ INPUT_START,      KEYCFG_BUTTON, P1_START    },
	{ INPUT_COIN,       KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ INPUT_SERVCOIN,   KEYCFG_BUTTON, SERV_COIN   },
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, SERV_SWITCH },
	MENU_BLANK,
	{ AUTOFIRE1,        KEYCFG_BUTTON, P1_AF_1     },
	{ AUTOFIRE2,        KEYCFG_BUTTON, P1_AF_2     },
	{ AUTOFIRE3,        KEYCFG_BUTTON, P1_AF_3     },
	{ AUTOFIRE4,        KEYCFG_BUTTON, P1_AF_4     },
	{ AUTOFIRE5,        KEYCFG_BUTTON, P1_AF_5     },
	{ AUTOFIRE6,        KEYCFG_BUTTON, P1_AF_6     },
	{ AUTOFIRE_INV,     KEYCFG_NUMBER, 0           },
	MENU_BLANK,
	{ HOTKEY_123,       KEYCFG_BUTTON, P1_123      },
	{ HOTKEY_456,       KEYCFG_BUTTON, P1_456      },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER    },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

static keycfg2_t keycfg_quiz[] =
{
	{ INPUT_BUTTON1,    KEYCFG_BUTTON, P1_BUTTON1  },
	{ INPUT_BUTTON2,    KEYCFG_BUTTON, P1_BUTTON2  },
	{ INPUT_BUTTON3,    KEYCFG_BUTTON, P1_BUTTON3  },
	{ INPUT_BUTTON4,    KEYCFG_BUTTON, P1_BUTTON4  },
	{ INPUT_START,      KEYCFG_BUTTON, P1_START    },
	{ INPUT_COIN,       KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ INPUT_SERVCOIN,   KEYCFG_BUTTON, SERV_COIN   },
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, SERV_SWITCH },
	MENU_BLANK,
	{ AUTOFIRE1,        KEYCFG_BUTTON, P1_AF_1     },
	{ AUTOFIRE2,        KEYCFG_BUTTON, P1_AF_2     },
	{ AUTOFIRE3,        KEYCFG_BUTTON, P1_AF_3     },
	{ AUTOFIRE4,        KEYCFG_BUTTON, P1_AF_4     },
	{ AUTOFIRE_INV,     KEYCFG_NUMBER, 0           },
	MENU_BLANK,
	{ HOTKEY_12,        KEYCFG_BUTTON, P1_12       },
	{ HOTKEY_13,        KEYCFG_BUTTON, P1_13       },
	{ HOTKEY_14,        KEYCFG_BUTTON, P1_14       },
	{ HOTKEY_23,        KEYCFG_BUTTON, P1_23       },
	{ HOTKEY_24,        KEYCFG_BUTTON, P1_24       },
	{ HOTKEY_34,        KEYCFG_BUTTON, P1_34       },
	{ HOTKEY_123,       KEYCFG_BUTTON, P1_123      },
	{ HOTKEY_124,       KEYCFG_BUTTON, P1_124      },
	{ HOTKEY_134,       KEYCFG_BUTTON, P1_134      },
	{ HOTKEY_234,       KEYCFG_BUTTON, P1_234      },
	{ HOTKEY_1234,      KEYCFG_BUTTON, P1_1234     },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER    },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

#if (EMU_SYSTEM == CPS1)
static keycfg2_t keycfg_sfzch[] =
{
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP       },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN     },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT     },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT    },
	{ INPUT_BUTTON1,    KEYCFG_BUTTON, P1_BUTTON1  },
	{ INPUT_BUTTON2,    KEYCFG_BUTTON, P1_BUTTON2  },
	{ INPUT_BUTTON3,    KEYCFG_BUTTON, P1_BUTTON3  },
	{ INPUT_BUTTON4,    KEYCFG_BUTTON, P1_BUTTON4  },
	{ INPUT_BUTTON5,    KEYCFG_BUTTON, P1_BUTTON5  },
	{ INPUT_BUTTON6,    KEYCFG_BUTTON, P1_BUTTON6  },
	{ INPUT_START,      KEYCFG_BUTTON, P1_START    },
	{ INPUT_PAUSE,      KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ INPUT_SERVCOIN,   KEYCFG_BUTTON, SERV_COIN   },
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, SERV_SWITCH },
	MENU_BLANK,
	{ AUTOFIRE1,        KEYCFG_BUTTON, P1_AF_1     },
	{ AUTOFIRE2,        KEYCFG_BUTTON, P1_AF_2     },
	{ AUTOFIRE3,        KEYCFG_BUTTON, P1_AF_3     },
	{ AUTOFIRE4,        KEYCFG_BUTTON, P1_AF_4     },
	{ AUTOFIRE5,        KEYCFG_BUTTON, P1_AF_5     },
	{ AUTOFIRE6,        KEYCFG_BUTTON, P1_AF_6     },
	{ AUTOFIRE_INV,     KEYCFG_NUMBER, 0           },
	MENU_BLANK,
	{ HOTKEY_123,       KEYCFG_BUTTON, P1_123      },
	{ HOTKEY_456,       KEYCFG_BUTTON, P1_456      },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER    },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

static keycfg2_t keycfg_wofch[] =
{
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP       },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN     },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT     },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT    },
	{ INPUT_BUTTON1,    KEYCFG_BUTTON, P1_BUTTON1  },
	{ INPUT_BUTTON2,    KEYCFG_BUTTON, P1_BUTTON2  },
	{ INPUT_BUTTON3,    KEYCFG_BUTTON, P1_BUTTON3  },
	{ INPUT_BUTTON4,    KEYCFG_BUTTON, P1_BUTTON4  },
	{ INPUT_BUTTON5,    KEYCFG_BUTTON, P1_BUTTON5  },
	{ INPUT_BUTTON6,    KEYCFG_BUTTON, P1_BUTTON6  },
	{ INPUT_START,      KEYCFG_BUTTON, P1_START    },
	{ INPUT_PAUSE,      KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ INPUT_SERVCOIN,   KEYCFG_BUTTON, SERV_COIN   },
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, SERV_SWITCH },
	MENU_BLANK,
	{ AUTOFIRE1,        KEYCFG_BUTTON, P1_AF_1     },
	{ AUTOFIRE2,        KEYCFG_BUTTON, P1_AF_2     },
	{ AUTOFIRE3,        KEYCFG_BUTTON, P1_AF_3     },
	{ AUTOFIRE4,        KEYCFG_BUTTON, P1_AF_4     },
	{ AUTOFIRE5,        KEYCFG_BUTTON, P1_AF_5     },
	{ AUTOFIRE6,        KEYCFG_BUTTON, P1_AF_6     },
	{ AUTOFIRE_INV,     KEYCFG_NUMBER, 0           },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER    },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

static keycfg2_t keycfg_forgottn[] =
{
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP       },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN     },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT     },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT    },
	{ INPUT_BUTTON1,    KEYCFG_BUTTON, P1_BUTTON1  },
	{ INPUT_START,      KEYCFG_BUTTON, P1_START    },
	{ INPUT_COIN,       KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ DIAL_LEFT,        KEYCFG_BUTTON, P1_DIAL_L   },
	{ DIAL_RIGHT,       KEYCFG_BUTTON, P1_DIAL_R   },
	{ ANALOG_SENS,      KEYCFG_ANALOG, 0           },
	MENU_BLANK,
	{ INPUT_SERVCOIN,   KEYCFG_BUTTON, SERV_COIN   },
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, SERV_SWITCH },
	MENU_BLANK,
	{ AUTOFIRE1,        KEYCFG_BUTTON, P1_AF_1     },
	{ AUTOFIRE_INV,     KEYCFG_NUMBER, 0           },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER    },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};
#else
static keycfg2_t keycfg_progear[] =
{
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP       },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN     },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT     },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT    },
	{ INPUT_BUTTON1,    KEYCFG_BUTTON, P1_BUTTON1  },
	{ INPUT_BUTTON2,    KEYCFG_BUTTON, P1_BUTTON2  },
	{ INPUT_BUTTON3,    KEYCFG_BUTTON, P1_BUTTON3  },
	{ INPUT_START,      KEYCFG_BUTTON, P1_START    },
	{ INPUT_COIN,       KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ PLAYER2_START,    KEYCFG_BUTTON, P2_START    },
	MENU_BLANK,
	{ INPUT_SERVCOIN,   KEYCFG_BUTTON, SERV_COIN   },
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, SERV_SWITCH },
	MENU_BLANK,
	{ AUTOFIRE1,        KEYCFG_BUTTON, P1_AF_1     },
	{ AUTOFIRE2,        KEYCFG_BUTTON, P1_AF_2     },
	{ AUTOFIRE3,        KEYCFG_BUTTON, P1_AF_3     },
	{ AUTOFIRE_INV,     KEYCFG_NUMBER, 0           },
	MENU_BLANK,
	{ HOTKEY_12,        KEYCFG_BUTTON, P1_12       },
	{ HOTKEY_13,        KEYCFG_BUTTON, P1_13       },
	{ HOTKEY_23,        KEYCFG_BUTTON, P1_23       },
	{ HOTKEY_123,       KEYCFG_BUTTON, P1_123      },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER    },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};

static keycfg2_t keycfg_pzloop2[] =
{
	{ INPUT_UP,         KEYCFG_BUTTON, P1_UP       },
	{ INPUT_DOWN,       KEYCFG_BUTTON, P1_DOWN     },
	{ INPUT_LEFT,       KEYCFG_BUTTON, P1_LEFT     },
	{ INPUT_RIGHT,      KEYCFG_BUTTON, P1_RIGHT    },
	{ INPUT_BUTTON1,    KEYCFG_BUTTON, P1_BUTTON1  },
	{ INPUT_START,      KEYCFG_BUTTON, P1_START    },
	{ INPUT_COIN,       KEYCFG_BUTTON, P1_COIN     },
	MENU_BLANK,
	{ PADDLE_LEFT,      KEYCFG_BUTTON, P1_DIAL_L   },
	{ PADDLE_RIGHT,     KEYCFG_BUTTON, P1_DIAL_R   },
	{ ANALOG_SENS,      KEYCFG_ANALOG, 0           },
	MENU_BLANK,
	{ INPUT_SERVCOIN,   KEYCFG_BUTTON, SERV_COIN   },
	{ INPUT_SERVSWITCH, KEYCFG_BUTTON, SERV_SWITCH },
	MENU_BLANK,
	{ SAVE_SCREENSHOT,  KEYCFG_BUTTON, SNAPSHOT    },
	{ SWITCH_PLAYER,    KEYCFG_BUTTON, SWPLAYER    },
	{ _COMMAND_LIST,    KEYCFG_BUTTON, COMMANDLIST },
	MENU_BLANK,
	MENU_RETURN,
	MENU_END
};
#endif

#elif defined(INCLUDE_KEYCFG_MENU)

/*-----------------------------------------------------------------------------
	keycfg menu 初期化
-----------------------------------------------------------------------------*/

#if (EMU_SYSTEM == CPS1)
	switch (machine_input_type)
	{
	case INPTYPE_forgottn:
		keycfg2 = keycfg_forgottn;
		break;

	case INPTYPE_dynwar:
	case INPTYPE_ffight:
	case INPTYPE_mtwins:
//	case INPTYPE_3wonders:
	case INPTYPE_pnickj:
	case INPTYPE_pang3:
	case INPTYPE_megaman:
	case INPTYPE_rockmanj:
	case INPTYPE_slammast:
#if !RELEASE
	case INPTYPE_kodh:
	case INPTYPE_knightsh:
	case INPTYPE_wofh:
	case INPTYPE_wof3js:
	case INPTYPE_wofsj:
	case INPTYPE_dinoh:
	case INPTYPE_punisherbz:
#endif
		keycfg2 = keycfg_3buttons;
		break;

	case INPTYPE_sf2:
	case INPTYPE_sf2j:
		keycfg2 = keycfg_6buttons;
		break;

	case INPTYPE_sfzch:
		keycfg2 = keycfg_sfzch;
		break;

	case INPTYPE_wofch:
		keycfg2 = keycfg_wofch;
		break;

	case INPTYPE_cworld2j:
	case INPTYPE_qad:
	case INPTYPE_qadj:
	case INPTYPE_qtono2:
		keycfg2 = keycfg_quiz;
		break;

	default:
		keycfg2 = keycfg_2buttons;
		break;
	}
#else
	switch (machine_input_type)
	{
	case INPTYPE_19xx:
	case INPTYPE_batcir:
		keycfg2 = keycfg_2buttons;
		break;

	case INPTYPE_cybots:
	case INPTYPE_ddtod:
		keycfg2 = keycfg_4buttons;
		break;

	case INPTYPE_cps2:
	case INPTYPE_ssf2:
		keycfg2 = keycfg_6buttons;
		break;

	case INPTYPE_qndream:
		keycfg2 = keycfg_quiz;
		break;

	case INPTYPE_pzloop2:
		keycfg2 = keycfg_pzloop2;
		break;

	default:
		keycfg2 = keycfg_3buttons;
		break;
	}

	if (!strcmp(driver->name, "progear"))
		keycfg2 =  keycfg_progear;
#endif

#elif defined(INCLUDE_LOAD_DIPSWITCH)

/*-----------------------------------------------------------------------------
	dipswitch menu 初期化
-----------------------------------------------------------------------------*/

#if (EMU_SYSTEM == CPS1)
	int old_value1, old_value2, old_value3;

	if ((dipswitch = load_dipswitch(&sx)) == NULL)
	{
		ui_popup(TEXT(THIS_GAME_HAS_NO_DIP_SWITCHES));
		return 0;
	}

	old_value1 = cps1_dipswitch[DIP_A] & 0xff;
	old_value2 = cps1_dipswitch[DIP_B] & 0xff;
	old_value3 = cps1_dipswitch[DIP_C] & 0xff;
#endif

#elif defined(INCLUDE_SAVE_DIPSWITCH)

/*-----------------------------------------------------------------------------
	dipswitch menu 終了
-----------------------------------------------------------------------------*/

#if (EMU_SYSTEM == CPS1)
	save_dipswitch();

	cps1_dipswitch[DIP_A] &= 0xff;
	cps1_dipswitch[DIP_B] &= 0xff;
	cps1_dipswitch[DIP_C] &= 0xff;

	if (cps1_dipswitch[DIP_A] != old_value1
	||	cps1_dipswitch[DIP_B] != old_value2
	||	cps1_dipswitch[DIP_C] != old_value3)
	{
		menu_resetdip();
//		menu_restart();
		return 1;
	}
#endif

#endif
