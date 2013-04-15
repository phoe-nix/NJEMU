/******************************************************************************

	cdda.c

	NEOGEO CDZ CDDAエミュレーション

******************************************************************************/

#include "ncdz.h"


/******************************************************************************
	グローバル変数
******************************************************************************/

int cdda_current_track = 0;
volatile int cdda_playing = 0;
volatile int cdda_autoloop = 0;
volatile int cdda_command_ack = 0;


/******************************************************************************
	ローカル変数
******************************************************************************/

static UINT32 cdda_play_start = 0;


/******************************************************************************
	プロトタイプ
******************************************************************************/

static void neogeo_cdda_command(int command, int track);


/******************************************************************************
	グローバル関数
******************************************************************************/

/*------------------------------------------------------
	CDDAエミュレーション初期化
------------------------------------------------------*/

int cdda_init(void)
{
	cdda_current_track = 0;
	cdda_playing = CDDA_STOP;
	cdda_play_start = 0;

	return mp3_thread_start();
}


/*------------------------------------------------------
	CDDAエミュレーション終了
------------------------------------------------------*/

void cdda_shutdown(void)
{
	mp3_thread_stop();
}


/*------------------------------------------------------
	CDDA再生
------------------------------------------------------*/

void cdda_play(int track)
{
	cdda_current_track = track;
	cdda_playing = CDDA_PLAY;
	cdda_command_ack = 0;

	if (option_mp3_enable)
	{
		char pattern[16], *fname;

		sprintf(pattern, "%02d.mp3", track);

		if ((fname = find_file(pattern, mp3_dir)))
		{
			char path[MAX_PATH];

			sprintf(path, "%s/%s", mp3_dir, fname);
			mp3_play(path);
			autoframeskip_reset();
		}
		else
		{
			cdda_command_ack = 1;
		}
	}
}


/*------------------------------------------------------
	CDDA停止
------------------------------------------------------*/

void cdda_stop(void)
{
	cdda_playing = CDDA_STOP;
	cdda_current_track = 0;

	if (option_mp3_enable) mp3_stop();
}


/*------------------------------------------------------
	CDDAポーズ
------------------------------------------------------*/

void cdda_pause(void)
{
	cdda_playing = CDDA_PAUSE;

	if (option_mp3_enable) mp3_pause(1);
}


/*------------------------------------------------------
	CDDAレジューム
------------------------------------------------------*/

void cdda_resume(void)
{
	cdda_playing = CDDA_PLAY;

	if (option_mp3_enable) mp3_pause(0);
}


/*------------------------------------------------------
	CDDAコントロール (BIOSファンクション)
------------------------------------------------------*/

void neogeo_cdda_control(void)
{
	int command, track;
	UINT32 offset;

	command = m68000_get_reg(M68K_D0);
	track   = command & 0xff;
	command = (command >> 8) & 0xff;

	if ((command & 2) == 0)
	{
		m68000_write_memory_8(0x108000 + 0x764b, track);
		m68000_write_memory_8(0x108000 + 0x76f8, track);
		m68000_write_memory_8(0x108000 + 0x76f7, command);
	}
	m68000_write_memory_8(0x108000 + 0x76f6, command);

	if (command <= 7)
	{
		if (option_mp3_enable && (command || track))
			neogeo_cdda_command(command, track);
	}

	offset = m68000_read_memory_32(0x108000 + 0x76ea);

	if (offset)
	{
		m68000_write_memory_8(0x108000 + 0x7678, 0x01);

		if (command <= 7)
		{
			offset  = (offset - 0xe00000) >> 1;
			memory_region_cpu2[offset + 0] = 0;
			memory_region_cpu2[offset + 1] = 0;
		}
	}
}


/*------------------------------------------------------
	CDDAコマンドチェック
------------------------------------------------------*/

void neogeo_cdda_check(void)
{
	UINT32 offset = m68000_read_memory_32(0x108000 + 0x76ea);

	if (offset)
	{
		UINT8 command, track;

		offset  = (offset - 0xe00000) >> 1;

		command = memory_region_cpu2[offset + 0];
		track   = memory_region_cpu2[offset + 1];

		memory_region_cpu2[offset + 0] = 0;
		memory_region_cpu2[offset + 1] = 0;

		if (command <= 7)
		{
			if (option_mp3_enable && (command || track))
				neogeo_cdda_command(command, track);
		}
	}
}


/*------------------------------------------------------
	CDDAコマンド送信
------------------------------------------------------*/

static void neogeo_cdda_command(int command, int track)
{
	int loop, mode, flag, _track;

	_track = track;
	track = ((track >> 4) * 10) + (track & 0x0f);
	loop  = (command & 1) ? 0 : 1;
	mode  = (command & 2) ? 1 : 0;
	flag  = (command & 4) ? 1 : 0;

	switch (command)
	{
	case 0:
	case 1:
	case 4:
	case 5:
		if (track > 1 && track <= 99)
		{
			cdda_play_start = frames_displayed;
			cdda_autoloop = loop;
			cdda_play(track);
			if (option_mp3_enable)
			{
				if (flag)
				{
					do
					{
						sceKernelDelayThread(10);
					} while (!cdda_command_ack);
				}
			}
		}
		break;

	case 2:
	case 6:
		if (cdda_playing == CDDA_PLAY && cdda_play_start != frames_displayed)
		{
			cdda_pause();
		}
		break;

	case 3:
	case 7:
		if (cdda_playing == CDDA_PAUSE)
		{
			cdda_resume();
		}
		break;
	}
}


#ifdef SAVE_STATE

/*------------------------------------------------------
	セーブ/ロードステート
------------------------------------------------------*/

STATE_SAVE( cdda )
{
	int playing = cdda_playing;
	int autoloop = cdda_autoloop;
	UINT32 frame = mp3_get_current_frame();

	state_save_long(&cdda_current_track, 1);
	state_save_long(&playing,  1);
	state_save_long(&autoloop, 1);
	state_save_long(&frame, 1);
}

STATE_LOAD( cdda )
{
	if (option_mp3_enable)
	{
		int playing;
		int autoloop;
		UINT32 frame;

		state_load_long(&cdda_current_track, 1);
		state_load_long(&playing, 1);
		state_load_long(&autoloop, 1);
		state_load_long(&frame, 1);

		cdda_playing = playing;
		cdda_autoloop = autoloop;

		if (cdda_playing)
		{
			char pattern[16], *fname;

			sprintf(pattern, "%02d.mp3", cdda_current_track);

			if ((fname = find_file(pattern, mp3_dir)))
			{
				char path[MAX_PATH];

				sprintf(path, "%s/%s", mp3_dir, fname);
				mp3_seek_set(path, frame);
			}
			else
			{
				mp3_stop();
			}
		}
		else cdda_stop();
	}
	else cdda_stop();
}

#endif
