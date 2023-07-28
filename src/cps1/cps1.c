/******************************************************************************

	cps1.c

	CPS1エミュレーションコア

******************************************************************************/

#include "cps1.h"


/******************************************************************************
	ローカル関数
******************************************************************************/

/*--------------------------------------------------------
	CPS1エミュレーション初期化
--------------------------------------------------------*/

static int cps1_init(void)
{
	if (!cps1_driver_init())
		return 0;

	msg_printf(TEXT(DONE2));
	msg_screen_clear();

	video_clear_screen();

#ifdef ADHOC
	if (!cps1_video_init())
		return 0;

	if (adhoc_enable)
	{
		sprintf(adhoc_matching, "%s_%s", PBPNAME_STR, game_name);

		Loop = LOOP_EXEC;//

		if (adhocInit(adhoc_matching) == 0)
		{
			if ((adhoc_server = adhocSelect()) >= 0)
			{
				video_clear_screen();

				if (adhoc_server)
				{
					option_controller = INPUT_PLAYER1;

					return adhoc_send_state(NULL);
				}
				else
				{
					option_controller = INPUT_PLAYER2;

					return adhoc_recv_state(NULL);
				}
			}
		}

		Loop = LOOP_BROWSER;
		return 0;
	}

	return 1;
#else
	return cps1_video_init();
#endif
}


/*--------------------------------------------------------
	CPS1エミュレーションリセット
--------------------------------------------------------*/

static void cps1_reset(void)
{
	video_set_mode(16);
	video_clear_screen();

	Loop = LOOP_EXEC;

	autoframeskip_reset();

	cps1_driver_reset();
	cps1_video_reset();

	timer_reset();
	input_reset();
	sound_reset();

	blit_clear_all_sprite();
}

/*--------------------------------------------------------
	CPSエミュレーション終了
--------------------------------------------------------*/

static void cps1_exit(void)
{
	video_set_mode(32);
	video_clear_screen();

	ui_popup_reset();

	video_clear_screen();
	msg_screen_init(WP_LOGO, ICON_SYSTEM, TEXT(EXIT_EMULATION2));

	msg_printf(TEXT(PLEASE_WAIT2));

	cps1_video_exit();
	cps1_driver_exit();

#ifdef ADHOC
	if (!adhoc_enable)
#endif
	{
#ifdef COMMAND_LIST
		free_commandlist();
#endif
		save_gamecfg(game_name);
	}

	msg_printf(TEXT(DONE2));

#ifdef ADHOC
	if (adhoc_enable) adhocTerm();
#endif

	show_exit_screen();
}

/*--------------------------------------------------------
	cheats
--------------------------------------------------------*/

extern int cheat_num;
extern gamecheat_t* gamecheat[];

static void apply_cheat()
{
	gamecheat_t *a_cheat = NULL;
	cheat_option_t *a_cheat_option = NULL;
	cheat_value_t *a_cheat_value = NULL;
	int c,j;

   for( c = 0; c < cheat_num; c++)
   { //arreglo de cheats
	a_cheat = gamecheat[c];
    if( a_cheat == NULL)
		break; //seguro

    if( a_cheat->curr_option == 0)//se asume que el option 0 es el disable
		continue;

    //Se busca cual es el option habilitado
    a_cheat_option = a_cheat->cheat_option[ a_cheat->curr_option];
    if( a_cheat_option == NULL)
		break; //seguro

		//Se ejecutan todos los value del cheat option
		for(  j = 0; j< a_cheat_option->num_cheat_values; j++)
		{
		a_cheat_value = a_cheat_option->cheat_value[j];
			if( a_cheat_value == NULL)
				break;//seguro
				m68000_write_memory_8(a_cheat_value->address,  a_cheat_value->value);

		}
    }
}

/*--------------------------------------------------------
	CPS1エミュレーション実行
--------------------------------------------------------*/

static void cps1_run(void)
{
	while (Loop >= LOOP_RESET)
	{
		cps1_reset();

		while (Loop == LOOP_EXEC)
		{
			if (Sleep)
			{
				do
				{
					usleep(5000000);
				} while (Sleep);

				autoframeskip_reset();
			}
			
			apply_cheat(); //davex cheat
			timer_update_cpu();
			update_screen();
			update_inputport();
		}

		video_clear_screen();
		sound_mute(1);
	}
}


/******************************************************************************
	グローバル関数
******************************************************************************/

/*--------------------------------------------------------
	CPS1エミュレーションメイン
--------------------------------------------------------*/

void cps1_main(void)
{
	Loop = LOOP_RESET;

	while (Loop >= LOOP_RESTART)
	{
		Loop = LOOP_EXEC;

		ui_popup_reset();

		fatal_error = 0;

		video_clear_screen();

		if (memory_init())
		{
			if (sound_init())
			{
				if (input_init())
				{
					if (cps1_init())
					{
						cps1_run();
					}
					cps1_exit();
				}
				input_shutdown();
			}
			sound_exit();
		}
		memory_shutdown();
		show_fatal_error();
	}
}
