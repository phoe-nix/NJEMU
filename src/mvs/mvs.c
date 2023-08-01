/******************************************************************************

	mvs.c

	MVSエミュレーションコア

******************************************************************************/

#include <fcntl.h>
#include "mvs.h"


/******************************************************************************
	グローバル変数
******************************************************************************/

int neogeo_bios;
int neogeo_region;
int neogeo_save_sound_flag;


/******************************************************************************
	ローカル変数
******************************************************************************/

#ifdef ADHOC
static const char *bios[] =
{
	"EURO2",
	"EURO1",
	"USA2",
	"USA1",
	"ASIA3N",
	"ASIA3",
	"JPNJ3",
	"JPN3",
	"JPN2",
	"JPN1",
	"NEOGIT",
	"ASIAES",
	"JPNAES"
};
#endif


/******************************************************************************
	ローカル関数
******************************************************************************/

/*--------------------------------------------------------
	MVSエミュレーション初期化
--------------------------------------------------------*/

static int neogeo_init(void)
{
	int32_t fd;
	char path[MAX_PATH];

#ifdef ADHOC
	if (!adhoc_enable)
#endif
	{
		sprintf(path, "%smemcard/%s.bin", launchDir, game_name);
		if ((fd = open(path, O_RDONLY, 0777)) >= 0)
		{
			read(fd, neogeo_memcard, 0x800);
			close(fd);
		}

		sprintf(path, "%snvram/%s.nv", launchDir, game_name);
		if ((fd = open(path, O_RDONLY, 0777)) >= 0)
		{
			read(fd, neogeo_sram16, 0x2000);
			close(fd);
			swab(neogeo_sram16, neogeo_sram16, 0x2000);
		}
	}

	neogeo_driver_init();
	neogeo_video_init();

	msg_printf(TEXT(DONE2));
	msg_screen_clear();

	video_driver->clearScreen(NULL);

#ifdef ADHOC
	if (adhoc_enable)
	{
		sprintf(adhoc_matching, "%s_%s_%s", TARGET_STR, game_name, bios[neogeo_bios]);

		if (adhocInit(adhoc_matching) == 0)
		{
			if ((adhoc_server = adhocSelect()) >= 0)
			{
				video_driver->clearScreen(NULL);

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
#endif

	return 1;
}


/*--------------------------------------------------------
	MVSエミュレーションリセット
--------------------------------------------------------*/

static void neogeo_reset(void)
{
	video_driver->setMode(NULL, 16);

	video_driver->clearScreen(NULL);

	timer_reset();
	input_reset();

	neogeo_driver_reset();
	neogeo_video_reset();

	sound_reset();
	blit_clear_all_sprite();
	autoframeskip_reset();

	Loop = LOOP_EXEC;
}


/*--------------------------------------------------------
	MVSエミュレーション終了
--------------------------------------------------------*/

static void neogeo_exit(void)
{
	int32_t fd;
	char path[MAX_PATH];

	video_driver->setMode(NULL, 32);
	video_driver->clearScreen(NULL);

	ui_popup_reset();

	video_driver->clearScreen(NULL);
	msg_screen_init(WP_LOGO, ICON_SYSTEM, TEXT(EXIT_EMULATION2));

	msg_printf(TEXT(PLEASE_WAIT2));

#ifdef ADHOC
	if (!adhoc_enable)
#endif
	{
		sprintf(path, "%smemcard/%s.bin", launchDir, game_name);
		if ((fd = open(path, O_WRONLY|O_CREAT, 0777)) >= 0)
		{
			write(fd, neogeo_memcard, 0x800);
			close(fd);
		}

		sprintf(path, "%snvram/%s.nv", launchDir, game_name);
		if ((fd = open(path, O_WRONLY|O_CREAT, 0777)) >= 0)
		{
			swab(neogeo_sram16, neogeo_sram16, 0x2000);
			write(fd, neogeo_sram16, 0x2000);
			close(fd);
		}


#ifdef COMMAND_LIST
		free_commandlist();
#endif

		if (neogeo_save_sound_flag) option_sound_enable = 1;
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
	MVSエミュレーション実行
--------------------------------------------------------*/

static void neogeo_run(void)
{
	while (Loop >= LOOP_RESET)
	{
		neogeo_reset();

		while (Loop == LOOP_EXEC)
		{
			if (Sleep)
			{
				cache_sleep(1);

				do
				{
					usleep(5000000);
				} while (Sleep);

				cache_sleep(0);
				autoframeskip_reset();
			}

			apply_cheat();//davex
			
			timer_update_cpu();
			update_screen();
			update_inputport();
		}

		video_driver->clearScreen(NULL);
		sound_mute(1);
	}
}


/******************************************************************************
	グローバル関数
******************************************************************************/

/*--------------------------------------------------------
	MVSエミュレーションメイン
--------------------------------------------------------*/

void neogeo_main(void)
{
	Loop = LOOP_RESET;

	while (Loop >= LOOP_RESTART)
	{
		Loop = LOOP_EXEC;

		ui_popup_reset();

		fatal_error = 0;

		video_driver->clearScreen(NULL);

		if (memory_init())
		{
			if (sound_init())
			{
				if (input_init())
				{
					if (neogeo_init())
					{
						neogeo_run();
					}
					neogeo_exit();
				}
				input_shutdown();
			}
			sound_exit();
		}
		memory_shutdown();
		show_fatal_error();
	}
}
