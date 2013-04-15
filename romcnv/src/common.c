/*****************************************************************************

	common.c

******************************************************************************/

#include "common.h"

enum
{
	ROM_LOAD = 0,
	ROM_CONTINUE,
	ROM_WORDSWAP,
	MAP_MAX
};


/******************************************************************************
	グローバル変数
******************************************************************************/

int lsb_first;

int rom_fd;
char delimiter = '/';

char game_dir[MAX_PATH];
char zip_dir[MAX_PATH];
char launchDir[MAX_PATH];

char game_name[16];
char parent_name[16];
char cache_name[16];


/******************************************************************************
	ROMロード処理
******************************************************************************/

/*--------------------------------------------------------
	エラーメッセージ表示
--------------------------------------------------------*/

void error_memory(const char *mem_name)
{
	zip_close();
	printf("ERROR: Could not allocate %s memory.\n", mem_name);
}


void error_file(const char *rom_name)
{
	zip_close();
	printf("ERROR: File not found. \"%s\"\n", rom_name);
}


void error_crc(const char *rom_name)
{
	zip_close();
	printf("ERROR: CRC32 not correct. \"%s\"\n", rom_name);
}


/*--------------------------------------------------------
	ROMファイルを閉じる
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
	ROMファイルを開く
--------------------------------------------------------*/

int file_open(const char *fname1, const char *fname2, const UINT32 crc, char *fname)
{
	int found = 0, res = -1;
	struct zip_find_t file;
	char path[MAX_PATH];

	file_close();

	sprintf(path, "%s%c%s.zip", zip_dir, delimiter, fname1);

	if (zip_open(path, "rb") != -1)
	{
		if (zip_findfirst(&file))
		{
			if (file.crc32 == crc)
			{
				found = 1;
			}
			else
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
		if (!found)
		{
			if ((rom_fd = zopen(fname)) != -1)
			{
				file_close();
				res = -2;
			}
			zip_close();
		}
	}

	if (!found && fname2 != NULL)
	{
		sprintf(path, "%s%c%s.zip", zip_dir, delimiter, fname2);

		if (zip_open(path, "rb") != -1)
		{
			if (zip_findfirst(&file))
			{
				if (file.crc32 == crc)
				{
					found = 2;
				}
				else
				{
					while (zip_findnext(&file))
					{
						if (file.crc32 == crc)
						{
							found = 2;
							break;
						}
					}
				}
			}
			if (!found)
			{
				if ((rom_fd = zopen(fname)) != -1)
				{
					file_close();
					res = -2;
				}
				zip_close();
			}
		}
	}

	if (found)
	{
		if (fname) strcpy(fname, file.name);
		rom_fd = zopen(file.name);
		return rom_fd;
	}

	return res;
}


/*--------------------------------------------------------
	ROMファイルを指定バイト読み込む
--------------------------------------------------------*/

int file_read(void *buf, size_t length)
{
	if (rom_fd != -1)
		return zread(rom_fd, buf, length);
	return -1;
}


/*--------------------------------------------------------
	ROMファイルを1バイト読み込む
--------------------------------------------------------*/

int file_getc(void)
{
	if (rom_fd != -1)
		return zgetc(rom_fd);
	return -1;
}


/*--------------------------------------------------------
	ROMを指定メモリエリアに読み込む
--------------------------------------------------------*/

int rom_load(struct rom_t *rom, UINT8 *mem, int idx, int max)
{
	int offset, length;

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


/******************************************************************************
	その他
******************************************************************************/

/*--------------------------------------------------------
	文字列の比較
--------------------------------------------------------*/

int str_cmp(const char *s1, const char *s2)
{
	return strncasecmp(s1, s2, strlen(s2));
}


/*--------------------------------------------------------
	バイトオーダーのチェック
--------------------------------------------------------*/

void check_byte_order(void)
{
	INT32 temp = 0x12345678;
	char *p = (char *)&temp;

	if (*p == 0x78)
		lsb_first = 1;
	else
		lsb_first = 0;
}


/******************************************************************************
	Windows用関数
******************************************************************************/

#ifdef WIN32

int is_win9x = 0;

/*--------------------------------------------------------
	Windowsのバージョンチェック
--------------------------------------------------------*/

void check_windows_version(void)
{
	OSVERSIONINFO osvi;

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	if (osvi.dwMajorVersion == 4)
	{
		is_win9x  = 1;
		delimiter = '\\';
	}
}


/*--------------------------------------------------------
	ファイルダイアログ処理
--------------------------------------------------------*/

int file_dialog(HWND hwnd, LPCSTR filter, char *fname, UINT32 flags)
{
	OPENFILENAME OFN;

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner   = hwnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile   = fname;
	OFN.nMaxFile    = MAX_PATH*2;
	OFN.Flags       = flags;
	OFN.lpstrTitle  = "Select zipped ROM file.";

	return GetOpenFileName(&OFN);
}


/*--------------------------------------------------------
	フォルダダイアログを表示
--------------------------------------------------------*/

int folder_dialog(HWND hwnd, char *path)
{
	BROWSEINFO BINFO;
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;
	int res = 0;

	if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		memset(&BINFO, 0, sizeof(BINFO));
		BINFO.hwndOwner = hwnd;
		BINFO.lpszTitle = "Select ROM folder";
		BINFO.ulFlags = BIF_RETURNONLYFSDIRS;

		pidl = SHBrowseForFolder(&BINFO);
		if (pidl)
		{
			res = SHGetPathFromIDList(pidl, path);
			IMalloc_Free(pMalloc, pidl);
		}
		IMalloc_Release(pMalloc);
	}
	return res;
}


/*--------------------------------------------------------
	デリミタを変換
--------------------------------------------------------*/

#define issjis1(c)	(((c) >= 0x81 && (c) <= 0x9f) | ((c) >= 0xe0 && (c) <= 0xfc))

void convert_delimiter(char *path)
{
	if (!is_win9x)
	{
		char *p = path;
		int i, len = strlen(path);

		for (i = 0; i < len; i++)
		{
			if (*p == '\\')
			{
				if (i == 0 || !issjis1(*(UINT8 *)(p - 1)))
					*p = '/';
			}
			p++;
		}
	}
}

#endif /* WIN32 */
