/******************************************************************************

	loadrom.c

	ROMイメージファイルロード関数

******************************************************************************/

#include "emumain.h"
#include <sys/unistd.h>


#if (EMU_SYSTEM != NCDZ)

/******************************************************************************
	ローカル変数
******************************************************************************/

static int rom_fd = -1;


/******************************************************************************
	ROMファイル読み込み
******************************************************************************/

/*--------------------------------------------------------
	ZIPファイルからファイルを検索し開く
--------------------------------------------------------*/

int file_open(const char *fname1, const char *fname2, const UINT32 crc, char *fname)
{
	int i, found = 0;
	struct zip_find_t file;
	char path[MAX_PATH];

	for (i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0: sprintf(path, "%s/%s.zip", game_dir, fname1); break;
		case 1: sprintf(path, "%s/%s.zip", game_dir, fname2); break;
		case 2: sprintf(path, "%sroms/%s.zip", launchDir, fname2); break;
		}

		if (zip_open(path) != -1)
		{
			if (zip_findfirst(&file))
			{
				if (file.crc32 == crc)
				{
					found = 1;
				}
				else
				{
					if (!found)
					{
						while (zip_findnext(&file))
						{
							if (file.crc32 == crc)
							{
								found = 1;
								break;
							}
						}
					}
				}
			}

			if (!found)
			{
				if (fname)
				{
					int fd;

					if ((fd = zopen(fname)) != -1)
					{
						zclose(fd);
						found = 2;
					}
				}
				zip_close();
			}
		}

		if (found || fname2 == NULL) break;
	}

	if (found == 1)
	{
		if (fname) strcpy(fname, file.name);
		rom_fd = zopen(file.name);
		return rom_fd;
	}
	else if (found == 2)
	{
		return -2;	// CRC error
	}

	return -1;	// not found
}


/*--------------------------------------------------------
	ファイルを閉じる
--------------------------------------------------------*/

void file_close(void)
{
	if (rom_fd != -1)
	{
		zclose(rom_fd);
		zip_close();
		rom_fd = -1;
	}
}


/*--------------------------------------------------------
	ファイルから指定バイト読み込む
--------------------------------------------------------*/

int file_read(void *buf, size_t length)
{
	if (rom_fd != -1)
		return zread(rom_fd, buf, length);
	return -1;
}


/*--------------------------------------------------------
	ファイルから1文字読み込む
--------------------------------------------------------*/

int file_getc(void)
{
	if (rom_fd != -1)
		return zgetc(rom_fd);
	return -1;
}


/*--------------------------------------------------------
	キャッシュファイルを開く
--------------------------------------------------------*/

#if USE_CACHE && (EMU_SYSTEM == MVS)
int cachefile_open(int type)
{
	SceUID fd = -1;
	char path[MAX_PATH];

	switch (type)
	{
	case CACHE_INFO:
		if (use_parent_crom && use_parent_srom && use_parent_vrom)
		{
			sprintf(path, "%s/%s_cache/cache_info", cache_dir, parent_name);
			fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
		}
		else
		{
			sprintf(path, "%s/%s_cache/cache_info", cache_dir, game_name);
			fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
		}
		break;

	case CACHE_CROM:
		if (use_parent_crom)
		{
			sprintf(path, "%s/%s_cache/crom", cache_dir, parent_name);
			fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
		}
		if (fd < 0)
		{
			sprintf(path, "%s/%s_cache/crom", cache_dir, game_name);
			fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
		}
		break;

	case CACHE_SROM:
		if (use_parent_srom)
		{
			sprintf(path, "%s/%s_cache/srom", cache_dir, parent_name);
			fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
		}
		if (fd < 0)
		{
			sprintf(path, "%s/%s_cache/srom", cache_dir, game_name);
			fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
		}
		break;

	case CACHE_VROM:
		if (use_parent_vrom)
		{
			sprintf(path, "%s/%s_cache/vrom", cache_dir, parent_name);
			fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
		}
		if (fd < 0)
		{
			sprintf(path, "%s/%s_cache/vrom", cache_dir, game_name);
			fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
		}
		break;
	}

	return fd;
}
#endif


/*--------------------------------------------------------
	ROMをロードする
--------------------------------------------------------*/

int rom_load(struct rom_t *rom, UINT8 *mem, int idx, int max)
{
	UINT32 offset, length;

_continue:
	offset = rom[idx].offset;

	if (rom[idx].skip == 0)
	{
		file_read(&mem[offset], rom[idx].length);

		if (rom[idx].type == ROM_WORDSWAP)
			swab(&mem[offset], &mem[offset], rom[idx].length);
	}
	else
	{
		int c;
		int skip = rom[idx].skip + rom[idx].group;

		length = 0;

		if (rom[idx].group == 1)
		{
			if (rom[idx].type == ROM_WORDSWAP)
				offset ^= 1;

			while (length < rom[idx].length)
			{
				if ((c = file_getc()) == EOF) break;
				mem[offset] = c;
				offset += skip;
				length++;
			}
		}
		else
		{
			while (length < rom[idx].length)
			{
				if ((c = file_getc()) == EOF) break;
				mem[offset + 0] = c;
				if ((c = file_getc()) == EOF) break;
				mem[offset + 1] = c;
				offset += skip;
				length += 2;
			}
		}
	}

	if (++idx != max)
	{
		if (rom[idx].type == ROM_CONTINUE)
		{
			goto _continue;
		}
	}

	return idx;
}

#endif /* EMU_SYSTEM */


/******************************************************************************
	エラーメッセージ表示
******************************************************************************/

/*------------------------------------------------------
	メモリ確保エラーメッセージ表示
------------------------------------------------------*/

void error_memory(const char *mem_name)
{
	zip_close();
	msg_printf(TEXT(COULD_NOT_ALLOCATE_x_MEMORY), mem_name);
	msg_printf(TEXT(PRESS_ANY_BUTTON2));
	pad_wait_press(PAD_WAIT_INFINITY);
	Loop = LOOP_BROWSER;
}


/*------------------------------------------------------
	CRCエラーメッセージ表示
------------------------------------------------------*/

void error_crc(const char *rom_name)
{
	zip_close();
	msg_printf(TEXT(CRC32_NOT_CORRECT_x), rom_name);
	msg_printf(TEXT(PRESS_ANY_BUTTON2));
	pad_wait_press(PAD_WAIT_INFINITY);
	Loop = LOOP_BROWSER;
}


/*------------------------------------------------------
	ROMファイルエラーメッセージ表示
------------------------------------------------------*/

void error_file(const char *rom_name)
{
	zip_close();
	msg_printf(TEXT(FILE_NOT_FOUND_x), rom_name);
	msg_printf(TEXT(PRESS_ANY_BUTTON2));
	pad_wait_press(PAD_WAIT_INFINITY);
	Loop = LOOP_BROWSER;
}
