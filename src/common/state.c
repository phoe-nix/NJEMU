/******************************************************************************

	state.c

	ステートセーブ/ロード

******************************************************************************/

#ifdef SAVE_STATE

#include "emumain.h"
#include "zlib/zlib.h"
#include <time.h>


/******************************************************************************
	グローバル変数
******************************************************************************/

char date_str[16];
char time_str[16];
char stver_str[16];
int state_version;
UINT8 *state_buffer;
int current_state_version;
#if (EMU_SYSTEM == MVS)
int  state_reload_bios;
#endif


/******************************************************************************
	ローカル変数
******************************************************************************/

#ifdef ADHOC
static UINT8 state_buffer_base[STATE_BUFFER_SIZE];
#endif

#if (EMU_SYSTEM == CPS1)
static const char *current_version_str = "CPS1SV23";
#elif (EMU_SYSTEM == CPS2)
static const char *current_version_str = "CPS2SV23";
#elif (EMU_SYSTEM == MVS)
static const char *current_version_str = "MVS_SV23";
#elif (EMU_SYSTEM == NCDZ)
static const char *current_version_str = "NCDZSV23";
#endif


/******************************************************************************
	ローカル関数
******************************************************************************/

/*------------------------------------------------------
	サムネイルをワーク領域からファイルに保存
------------------------------------------------------*/

static void save_thumbnail(void)
{
	int x, y, w, h;
	UINT16 *src = ((UINT16 *)UI_TEXTURE) + 152;

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
	if (machine_screen_type)
	{
		w = 112;
		h = 152;
	}
	else
#endif
	{
		w = 152;
		h = 112;
	}

	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			state_save_word(&src[x], 1);
		}
		src += BUF_WIDTH;
	}
}


/*------------------------------------------------------
	サムネイルをファイルからワーク領域に読み込み
------------------------------------------------------*/

static void load_thumbnail(FILE *fp)
{
	int x, y, w, h;
	UINT16 *dst = (UINT16 *)UI_TEXTURE;

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
	if (machine_screen_type)
	{
		w = 112;
		h = 152;
	}
	else
#endif
	{
		w = 152;
		h = 112;
	}

	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
#if (EMU_SYSTEM == NCDZ) || defined(ADHOC)
			fread(&dst[x], 1, 2, fp);
#else
			state_load_word(&dst[x], 1);
#endif
		}
		dst += BUF_WIDTH;
	}
}


/*------------------------------------------------------
	ワーク領域のサムネイルをクリア
------------------------------------------------------*/

static void clear_thumbnail(void)
{
	int x, y, w, h;
	UINT16 *dst = (UINT16 *)UI_TEXTURE;

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
	if (machine_screen_type)
	{
		w = 112;
		h = 152;
	}
	else
#endif
	{
		w = 152;
		h = 112;
	}

	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			dst[x] = 0;
		}
		dst += BUF_WIDTH;
	}
}


/******************************************************************************
	ステートセーブ/ロード関数
******************************************************************************/

/*------------------------------------------------------
	ステートセーブ
------------------------------------------------------*/

int state_save(int slot)
{
	SceUID fd = -1;
	pspTime nowtime;
	char path[MAX_PATH];
	char error_mes[128];
	char buf[128];
#if (EMU_SYSTEM == NCDZ)
	UINT8 *inbuf, *outbuf;
	unsigned long insize, outsize;
#else
#ifndef ADHOC
	UINT8 *state_buffer_base;
#endif
	UINT32 size;
#endif

	sprintf(path, "%sstate/%s.sv%d", launchDir, game_name, slot);
	sceIoRemove(path);

	sprintf(buf, TEXT(STATE_SAVING), game_name, slot);
	init_progress(6, buf);

	sceRtcGetCurrentClockLocalTime(&nowtime);

	if ((fd = sceIoOpen(path, PSP_O_WRONLY|PSP_O_CREAT, 0777)) >= 0)
#if (EMU_SYSTEM == NCDZ)
	{
		if ((inbuf = memalign(MEM_ALIGN, STATE_BUFFER_SIZE)) == NULL)
		{
			strcpy(error_mes, TEXT(COULD_NOT_ALLOCATE_STATE_BUFFER));
			goto error;
		}
		memset(inbuf, 0, STATE_BUFFER_SIZE);
		state_buffer = inbuf;

		state_save_byte(current_version_str, 8);
		state_save_byte(&nowtime, 16);
		update_progress();

		save_thumbnail();
		update_progress();

		sceIoWrite(fd, inbuf, (UINT32)state_buffer - (UINT32)inbuf);
		update_progress();

		memset(inbuf, 0, STATE_BUFFER_SIZE);
		state_buffer = inbuf;

		state_save_memory();
		state_save_m68000();
		state_save_z80();
		state_save_input();
		state_save_timer();
		state_save_driver();
		state_save_video();
		state_save_ym2610();
		state_save_cdda();
		state_save_cdrom();
		update_progress();

		insize = (UINT32)state_buffer - (UINT32)inbuf;
		outsize = insize * 1.1 + 12;
		if ((outbuf = memalign(MEM_ALIGN, outsize)) == NULL)
		{
			strcpy(error_mes, TEXT(COULD_NOT_ALLOCATE_STATE_BUFFER));
			free(inbuf);
			goto error;
		}
		memset(outbuf, 0, outsize);

		if (compress(outbuf, &outsize, inbuf, insize) != Z_OK)
		{
			strcpy(error_mes, TEXT(COULD_NOT_COMPRESS_STATE_DATA));
			free(inbuf);
			free(outbuf);
			goto error;
		}
		free(inbuf);
		update_progress();

		sceIoWrite(fd, &outsize, 4);
		sceIoWrite(fd, outbuf, outsize);
		sceIoClose(fd);
		free(outbuf);
		update_progress();

		show_progress(buf);
		return 1;
	}
#else
	{
#ifdef ADHOC
		state_buffer = state_buffer_base;
#else
#if (EMU_SYSTEM == CPS1 || (EMU_SYSTEM == CPS2 && defined(PSP_SLIM)))
		state_buffer = state_buffer_base = memalign(MEM_ALIGN, STATE_BUFFER_SIZE);
#else
		state_buffer = state_buffer_base = cache_alloc_state_buffer(STATE_BUFFER_SIZE);
#endif
		if (!state_buffer)
		{
			strcpy(error_mes, TEXT(COULD_NOT_ALLOCATE_STATE_BUFFER));
			goto error;
		}
#endif
		memset(state_buffer, 0, STATE_BUFFER_SIZE);
		update_progress();

		state_save_byte(current_version_str, 8);
		state_save_byte(&nowtime, 16);
		update_progress();

		save_thumbnail();
		update_progress();

		state_save_memory();
		state_save_m68000();
		state_save_z80();
		state_save_input();
		state_save_timer();
		state_save_driver();
		state_save_video();
#if (EMU_SYSTEM == CPS1)
		state_save_coin();
		switch (machine_driver_type)
		{
		case MACHINE_qsound:
			state_save_qsound();
			state_save_eeprom();
			break;

		case MACHINE_pang3:
			state_save_eeprom();

		default:
			state_save_ym2151();
			break;
		}
#elif (EMU_SYSTEM == CPS2)
		state_save_coin();
		state_save_qsound();
		state_save_eeprom();
#elif (EMU_SYSTEM == MVS)
		state_save_ym2610();
		state_save_pd4990a();
#endif
		update_progress();

		size = (UINT32)state_buffer - (UINT32)state_buffer_base;
		sceIoWrite(fd, state_buffer_base, size);
		sceIoClose(fd);
		update_progress();

#ifndef ADHOC
#if (EMU_SYSTEM == CPS1 || (EMU_SYSTEM == CPS2 && defined(PSP_SLIM)))
		free(state_buffer_base);
#else
		cache_free_state_buffer(STATE_BUFFER_SIZE);
#endif
#endif
		update_progress();

		show_progress(buf);
		return 1;
	}
#endif
	else
	{
		sprintf(error_mes, TEXT(COULD_NOT_CREATE_STATE_FILE), game_name, slot);
	}

#if !defined(ADHOC) || (EMU_SYSTEM == NCDZ)
error:
#endif
	if (fd >= 0)
	{
		sceIoClose(fd);
		sceIoRemove(path);
	}
	show_progress(error_mes);
	pad_wait_press(PAD_WAIT_INFINITY);

	return 0;
}


/*------------------------------------------------------
	ステートロード
------------------------------------------------------*/

int state_load(int slot)
{
#if defined(ADHOC) || (EMU_SYSTEM == NCDZ)
	SceUID fd;
#else
	FILE *fp;
#endif
	char path[MAX_PATH];
	char error_mes[128];
	char buf[128];
#if (EMU_SYSTEM == NCDZ)
	UINT8 *inbuf, *outbuf;
	unsigned long insize, outsize;
#endif

	sprintf(path, "%sstate/%s.sv%d", launchDir, game_name, slot);

#if (EMU_SYSTEM == MVS)
	state_reload_bios = 0;
#endif

	sprintf(buf, TEXT(STATE_LOADING), game_name, slot);
#if (EMU_SYSTEM == NCDZ)
	init_progress(6, buf);
#else
	init_progress(4, buf);
#endif

#if (EMU_SYSTEM == NCDZ)
	if ((fd = sceIoOpen(path, PSP_O_RDONLY, 0777)) >= 0)
	{
		sceIoLseek(fd, (8+16) + (152*112*2), SEEK_SET);
		update_progress();

		sceIoRead(fd, &insize, 4);
		if ((inbuf = memalign(MEM_ALIGN, insize)) == NULL)
		{
			strcpy(error_mes, TEXT(COULD_NOT_ALLOCATE_STATE_BUFFER));
			sceIoClose(fd);
			goto error;
		}
		memset(inbuf, 0, insize);
		update_progress();

		sceIoRead(fd, inbuf, insize);
		sceIoClose(fd);
		update_progress();

		outsize = STATE_BUFFER_SIZE;
		if ((outbuf = memalign(MEM_ALIGN, outsize)) == NULL)
		{
			strcpy(error_mes, TEXT(COULD_NOT_ALLOCATE_STATE_BUFFER));
			free(inbuf);
			goto error;
		}
		memset(outbuf, 0, outsize);

		if (uncompress(outbuf, &outsize, inbuf, insize) != Z_OK)
		{
			strcpy(error_mes, TEXT(COULD_NOT_UNCOMPRESS_STATE_DATA));
			free(inbuf);
			free(outbuf);
			goto error;
		}
		free(inbuf);
		update_progress();

		state_buffer = outbuf;

		state_load_memory();
		state_load_m68000();
		state_load_z80();
		state_load_input();
		state_load_timer();
		state_load_driver();
		state_load_video();
		state_load_ym2610();
		state_load_cdda();
		state_load_cdrom();
		update_progress();

		free(outbuf);

		if (mp3_get_status() == MP3_SEEK)
		{
			mp3_seek_start();

			while (mp3_get_status() == MP3_SEEK)
				video_wait_vsync();
		}
		update_progress();

		show_progress(buf);
		return 1;
	}
#else
#ifdef ADHOC
	if ((fd = sceIoOpen(path, PSP_O_RDONLY, 0777)) >= 0)
	{
		int size;

		size = sceIoLseek(fd, 0, SEEK_END);
		sceIoLseek(fd, 0, SEEK_SET);
		sceIoRead(fd, state_buffer_base, size);
		sceIoClose(fd);

		state_buffer = state_buffer_base;

		state_load_skip((8+16));
		update_progress();

		state_load_skip((152*112*2));
		update_progress();

		state_load_memory();
		state_load_m68000();
		state_load_z80();
		state_load_input();
		state_load_timer();
		state_load_driver();
		state_load_video();
#if (EMU_SYSTEM == CPS1)

		state_load_coin();
		switch (machine_driver_type)
		{
		case MACHINE_qsound:
			state_load_qsound();
			state_load_eeprom();
			break;

		case MACHINE_pang3:
			state_load_eeprom();

		default:
			state_load_ym2151();
			break;
		}
#elif (EMU_SYSTEM == CPS2)
		state_load_coin();
		state_load_qsound();
		state_load_eeprom();
#elif (EMU_SYSTEM == MVS)
		state_load_ym2610();
		state_load_pd4990a();

		if (state_reload_bios)
		{
			state_reload_bios = 0;

			if (!reload_bios())
			{
				show_progress(TEXT(COULD_NOT_RELOAD_BIOS));
				pad_wait_press(PAD_WAIT_INFINITY);
				Loop = LOOP_BROWSER;
				return 0;
			}
		}
#endif
#else
	if ((fp = fopen(path, "rb")) != NULL)
	{
		state_load_skip((8+16));
		update_progress();

		state_load_skip((152*112*2));
		update_progress();

		state_load_memory(fp);
		state_load_m68000(fp);
		state_load_z80(fp);
		state_load_input(fp);
		state_load_timer(fp);
		state_load_driver(fp);
		state_load_video(fp);
#if (EMU_SYSTEM == CPS1)

		state_load_coin(fp);
		switch (machine_driver_type)
		{
		case MACHINE_qsound:
			state_load_qsound(fp);
			state_load_eeprom(fp);
			break;

		case MACHINE_pang3:
			state_load_eeprom(fp);

		default:
			state_load_ym2151(fp);
			break;
		}
		fclose(fp);
#elif (EMU_SYSTEM == CPS2)
		state_load_coin(fp);
		state_load_qsound(fp);
		state_load_eeprom(fp);
		fclose(fp);
#elif (EMU_SYSTEM == MVS)
		state_load_ym2610(fp);
		state_load_pd4990a(fp);
		fclose(fp);

		if (state_reload_bios)
		{
			state_reload_bios = 0;

			if (!reload_bios())
			{
				show_progress(TEXT(COULD_NOT_RELOAD_BIOS));
				pad_wait_press(PAD_WAIT_INFINITY);
				Loop = LOOP_BROWSER;
				return 0;
			}
		}
#endif
#endif

		update_progress();

		show_progress(buf);
		return 1;
	}
#endif
	else
	{
		sprintf(error_mes, TEXT(COULD_NOT_OPEN_STATE_FILE), game_name, slot);
	}

#if (EMU_SYSTEM == NCDZ)
error:
#endif
	show_progress(error_mes);
	pad_wait_press(PAD_WAIT_INFINITY);

	return 0;
}


/*------------------------------------------------------
	サムネイル作成
------------------------------------------------------*/

void state_make_thumbnail(void)
{
	UINT16 *tex = UI_TEXTURE;

	{
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		RECT clip1 = { 64, 16, 64 + 384, 16 + 224 };

		if (machine_screen_type)
		{
			RECT clip2 = { 152, 0, 152 + 112, 152 };
			video_copy_rect_rotate(work_frame, tex, &clip1, &clip2);
		}
		else
		{
			RECT clip2 = { 152, 0, 152 + 152, 112 };
			video_copy_rect(work_frame, tex, &clip1, &clip2);
		}
#elif (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		RECT clip1 = { 24, 16, 336, 240 };
		RECT clip2 = { 152, 0, 152 + 152, 112 };

		video_copy_rect(work_frame, tex, &clip1, &clip2);
#endif
	}
}


/*------------------------------------------------------
	サムネイル読み込み
------------------------------------------------------*/

int state_load_thumbnail(int slot)
{
	FILE *fp;
	char path[MAX_PATH];

	clear_thumbnail();

	sprintf(path, "%sstate/%s.sv%d", launchDir, game_name, slot);

	if ((fp = fopen(path, "rb")) != NULL)
	{
		pspTime t;

		memset(stver_str, 0, 16);

		fread(stver_str, 1, 8, fp);
		fread(&t, 1, 16, fp);
		load_thumbnail(fp);
		fclose(fp);

		current_state_version = current_version_str[7] - '0';
		state_version = stver_str[7] - '0';

		sprintf(date_str, "%04d/%02d/%02d", t.year, t.month, t.day);
		sprintf(time_str, "%02d:%02d:%02d", t.hour, t.minutes, t.seconds);

		return 1;
	}

	ui_popup(TEXT(COULD_NOT_OPEN_STATE_FILE), game_name, slot);

	return 0;
}


/*------------------------------------------------------
	サムネイル消去
------------------------------------------------------*/

void state_clear_thumbnail(void)
{
	strcpy(date_str, "----/--/--");
	strcpy(time_str, "--:--:--");
	strcpy(stver_str, "--------");

	state_version = 0;

	clear_thumbnail();
}

/******************************************************************************
	AdHoc用ステート送受信関数
******************************************************************************/

#ifdef ADHOC

/*
	データサイズは ((実データサイズ / 0x3ff) + 1) * 0x3ff で求めること

	0x3ff = 0x400 bytes (送信バッファサイズ) - 1 byte(データ識別コード)
*/

#if (EMU_SYSTEM == CPS1)
#define ADHOC_STATE_SIZE	0x452eb		// CPS1PSP adhoc: 0x450d3
#elif (EMU_SYSTEM == CPS2)
#define ADHOC_STATE_SIZE	0x4b2d3		// CPS2PSP adhoc: 0x4b1f7
#elif (EMU_SYSTEM == MVS)
#define ADHOC_STATE_SIZE	0x46ee4		// MVSPSP adhoc: 0x46da2
#endif

/*------------------------------------------------------
	ステート送信
------------------------------------------------------*/

int adhoc_send_state(UINT32 *frame)
{
	int error = 0;
	int retry_count = 10;

	state_buffer = state_buffer_base;

	memset(state_buffer, 0, STATE_BUFFER_SIZE);

	if (frame != NULL)
		*(UINT32 *)state_buffer = *frame;

	state_buffer += 4;

	state_save_memory();
	state_save_m68000();
	state_save_z80();
	state_save_input();
	state_save_timer();
	state_save_driver();
	state_save_video();

#if (EMU_SYSTEM == CPS1)
	state_save_coin();
	switch (machine_driver_type)
	{
	case MACHINE_qsound:
		state_save_qsound();
		state_save_eeprom();
		break;

	case MACHINE_pang3:
		state_save_eeprom();

	default:
		state_save_ym2151();
		break;
	}

#elif (EMU_SYSTEM == CPS2)
	state_save_coin();
	state_save_qsound();
	state_save_eeprom();

#elif (EMU_SYSTEM == MVS)
	state_save_ym2610();
	state_save_pd4990a();
#endif

#if 0
	{
		int size = (UINT32)state_buffer - (UINT32)state_buffer_base;
		ui_popup("size = %08x (%08x)", size, ((size / 0x3ff) + 1) * 0x3ff);
	}
#endif

retry:
	adhocWait(ADHOC_DATASIZE_SYNC);
	if (adhocSync() < 0)
	{
		return 0;
	}
	if ((error = adhocSendRecvAck(state_buffer_base, ADHOC_STATE_SIZE, ADHOC_TIMEOUT, ADHOC_DATATYPE_STATE)) != ADHOC_STATE_SIZE)
	{
		if (error == (int)0x80410715)	// タイムアウト
		{
			if (Loop != LOOP_EXEC) return 1;
			if (--retry_count) goto retry;
		}
		return 0;
	}

	return 1;
}


/*------------------------------------------------------
	ステート受信
------------------------------------------------------*/

int adhoc_recv_state(UINT32 *frame)
{
	int error = 0;
	int retry_count = 10;

retry:
	adhocWait(ADHOC_DATASIZE_SYNC);
	if (adhocSync() < 0)
	{
		return 0;
	}

	adhocWait(ADHOC_DATASIZE_STATE);
	if ((error = adhocRecvSendAck(state_buffer_base, ADHOC_STATE_SIZE, ADHOC_TIMEOUT, ADHOC_DATATYPE_STATE)) != ADHOC_STATE_SIZE)
	{
		if (error == -1)
		{
			if (Loop != LOOP_EXEC) return 1;
			goto retry;	// データタイプ不一致
		}
		else if (error == (int)0x80410715)	// タイムアウト
		{
			if (Loop != LOOP_EXEC) return 1;
			if (--retry_count) goto retry;
		}
		return 0;
	}

	state_buffer = state_buffer_base;

	if (frame != NULL)
		*frame = *(UINT32 *)state_buffer;

	state_buffer += 4;

	state_load_memory();
	state_load_m68000();
	state_load_z80();
	state_load_input();
	state_load_timer();
	state_load_driver();
	state_load_video();

#if (EMU_SYSTEM == CPS1)
	state_load_coin();
	switch (machine_driver_type)
	{
	case MACHINE_qsound:
		state_load_qsound();
		state_load_eeprom();
		break;

	case MACHINE_pang3:
		state_load_eeprom();

	default:
		state_load_ym2151();
		break;
	}

#elif (EMU_SYSTEM == CPS2)
	state_load_coin();
	state_load_qsound();
	state_load_eeprom();

#elif (EMU_SYSTEM == MVS)
	state_load_ym2610();
	state_load_pd4990a();
#endif

	if (adhoc_server)
		option_controller = INPUT_PLAYER1;
	else
		option_controller = INPUT_PLAYER2;

	return 1;
}

#endif

#endif /* SAVE_STATE */
