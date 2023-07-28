/******************************************************************************

	filer.c

	PSP ファイルブラウザ

******************************************************************************/

#include <psptypes.h>
#include <pspwlan.h>
#include <zlib.h>
#include "emumain.h"

#define MAX_ENTRY 1024
#define MAX_GAMES 512

#define GAME_NOT_WORK	0x01
#define GAME_BOOTLEG	0x02
#define GAME_HACK		0x04
#define GAME_BADROM		0x08
#define GAME_HAS_TITLE	0x10
#define GAME_IS_NEOCD	0x20

#define FTYPE_UPPERDIR	0
#define FTYPE_DIR		1
#define FTYPE_ZIP		2
#define FTYPE_FILE		3

enum PspModel
{
	PSP_MODEL_STANDARD = 0,
	PSP_MODEL_SLIM_AND_LITE = 1
};


/******************************************************************************
	グローバル変数
******************************************************************************/

char startupDir[MAX_PATH];


/******************************************************************************
	ローカル構造体/変数
******************************************************************************/

struct dirent
{
	int type;
	int flag;
	char name[128];
	char title[128];
};

static struct dirent *files[MAX_ENTRY];
static SceIoDirent dir;

static char curr_dir[MAX_PATH];
static int nfiles;

#if (EMU_SYSTEM == NCDZ)

static int neocddir;
static int has_mp3;
static int bios_error;
static char zipped_rom[MAX_PATH];

#else

static struct zipname_t
{
	char zipname[16];
	char title[128];
	int flag;
} zipname[MAX_GAMES];

static int zipname_num;

#endif


#if (EMU_SYSTEM == NCDZ)

/******************************************************************************
	ローカル関数 (NCDZPSP)
******************************************************************************/

/*--------------------------------------------------------
	title_x.sysをイメージバッファに描画
--------------------------------------------------------*/

#if VIDEO_32BPP
static void title_draw_spr(int sx, int sy, uint8_t *spr, uint32_t *palette, int tileno)
#else
static void title_draw_spr(int sx, int sy, uint8_t *spr, uint16_t *palette, int tileno)
#endif
{
	uint32_t tile, lines = 16;
	uint32_t *src = (uint32_t *)(spr + tileno * 128);
#if VIDEO_32BPP
	uint32_t *dst = (uint32_t *)video_frame_addr(tex_frame, sx, sy);
	uint32_t *pal = &palette[tileno << 4];
#else
	uint16_t *dst = (uint16_t *)video_frame_addr(tex_frame, sx, sy);
	uint16_t *pal = &palette[tileno << 4];
#endif

	while (lines--)
	{
		tile = src[0];
		dst[ 0] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 4] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 1] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 5] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 2] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 6] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 3] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 7] = pal[tile & 0x0f];
		tile = src[1];
		dst[ 8] = pal[tile & 0x0f]; tile >>= 4;
		dst[12] = pal[tile & 0x0f]; tile >>= 4;
		dst[ 9] = pal[tile & 0x0f]; tile >>= 4;
		dst[13] = pal[tile & 0x0f]; tile >>= 4;
		dst[10] = pal[tile & 0x0f]; tile >>= 4;
		dst[14] = pal[tile & 0x0f]; tile >>= 4;
		dst[11] = pal[tile & 0x0f]; tile >>= 4;
		dst[15] = pal[tile & 0x0f];
		src += 2;
		dst += BUF_WIDTH;
	}
}


/*--------------------------------------------------------
	title_x.sysを読み込む
--------------------------------------------------------*/

static int load_title(const char *path, int number)
{
	int i, fd, region, tileno, x, y, found = 0;
	uint8_t  title_spr[0x1680];
	uint16_t palette[0x5a0 >> 1];
#if VIDEO_32BPP
	uint32_t palette32[0x5a0 >> 1];
#endif
	char title_path[MAX_PATH], region_chr[3] = {'j','u','e'};

	zip_open(path);

	fd = -1;
	for (region = neogeo_region & 0x03; region >= 0; region--)
	{
		sprintf(title_path, "title_%c.sys", region_chr[region]);

		if ((fd = zopen(title_path)) != -1)
		{
			found = 1;
			break;
		}
	}

	if (!found)
	{
		zip_close();
		return 0;
	}

	zread(fd, palette, 0x5a0);
	zread(fd, title_spr, 0x1680);
	zclose(fd);

	zip_close();

	swab((uint8_t *)palette, (uint8_t *)palette, 0x5a0);

	for (i = 0; i < 0x5a0 >> 1; i++)
	{
#if VIDEO_32BPP
		int r = ((palette[i] >> 7) & 0x1e) | ((palette[i] >> 14) & 0x01);
		int g = ((palette[i] >> 3) & 0x1e) | ((palette[i] >> 13) & 0x01);
		int b = ((palette[i] << 1) & 0x1e) | ((palette[i] >> 12) & 0x01);

		r = (r << 3) | (r >> 2);
		g = (g << 3) | (g >> 2);
		b = (b << 3) | (b >> 2);

		palette32[i] = MAKECOL32(r, g, b);
#else
		int r = ((palette[i] >> 7) & 0x1e) | ((palette[i] >> 14) & 0x01);
		int g = ((palette[i] >> 3) & 0x1e) | ((palette[i] >> 13) & 0x01);
		int b = ((palette[i] << 1) & 0x1e) | ((palette[i] >> 12) & 0x01);

		palette[i] = MAKECOL15(r, g, b);
#endif
	}

	neogeo_decode_spr(title_spr, 0, 0x1680);

	tileno = 0;

	for (y = 0; y < 80; y += 16)
	{
		for (x = 0; x < 144; x += 16)
		{
#if VIDEO_32BPP
			title_draw_spr(x, y, title_spr, palette32, tileno);
#else
			title_draw_spr(x, y, title_spr, palette, tileno);
#endif
			tileno++;
		}
	}

	return 1;
}


/*--------------------------------------------------------
	title_x.sysを表示
--------------------------------------------------------*/

static void show_title(int sx, int sy)
{
	RECT clip1 = { 0, 0, 144, 80 };
	RECT clip2 = { sx, sy, sx + 144, sy + 80 };

	draw_box_shadow(sx, sy, sx + 144, sy + 80);
	video_copy_rect(tex_frame, draw_frame, &clip1, &clip2);
}


/*--------------------------------------------------------
	NEOGEO CDZのBIOSをチェック
--------------------------------------------------------*/

static void check_neocd_bios(void)
{
	FILE *fp;
	char path[MAX_PATH];
	uint8_t *temp_mem;

	bios_error = 0;

	if ((temp_mem = (uint8_t *)malloc(0x80000)) == NULL)
	{
		bios_error = 1;
		return;
	}

	sprintf(path, "%s%s", launchDir, "neocd.bin");

	if ((fp = fopen(path, "rb")) != NULL)
	{
		fread(temp_mem, 1, 0x80000, fp);
		fclose(fp);

		if (crc32(0, temp_mem, 0x80000) != 0xdf9de490)
			bios_error = 2;
	}
	else bios_error = 1;

	free(temp_mem);
}


#else

/******************************************************************************
	ローカル関数 (NCDZPSP以外)
******************************************************************************/

/*--------------------------------------------------------
	zipname.datからzipファイル名データベースを読み込み
--------------------------------------------------------*/

#if (EMU_SYSTEM == CPS1)
#define EXT		"cps1"
#elif (EMU_SYSTEM == CPS2)
#define EXT		"cps2"
#elif (EMU_SYSTEM == MVS)
#define EXT		"mvs"
#endif

static int load_zipname(void)
{
	FILE *fp;
	char path[MAX_PATH], buf[256];
	int found = 0;

	if (ui_text_get_language() == LANG_JAPANESE)
	{
		sprintf(path, "%szipnamej." EXT, launchDir);
		if ((fp = fopen(path, "rb")) != NULL)
		{
			fclose(fp);
			found = 1;
		}
	}
	if (ui_text_get_language() == LANG_CHINESE_SIMPLIFIED)
	{
		sprintf(path, "%szipnamech1." EXT, launchDir);
		if ((fp = fopen(path, "rb")) != NULL)
		{
			fclose(fp);
			found = 1;
		}
	}
	if (ui_text_get_language() == LANG_CHINESE_TRADITIONAL)
	{
		sprintf(path, "%szipnamech2." EXT, launchDir);
		if ((fp = fopen(path, "rb")) != NULL)
		{
			fclose(fp);
			found = 1;
		}
	}
	if (!found)
	{
		sprintf(path, "%szipname." EXT, launchDir);
	}
	if ((fp = fopen(path, "rb")) == NULL)
		return 0;

	fseek(fp, 0, SEEK_END);
	ftell(fp);
	fseek(fp, 0, SEEK_SET);

	zipname_num = 0;
	while (zipname_num < MAX_GAMES)
	{
		char *linebuf;
		char *name;
		char *title;
		char *flag;

		memset(buf, 0, 256);

		if (!fgets(buf, 255, fp)) break;

		linebuf = strtok(buf, "\r\n");

		zipname[zipname_num].flag = 0;

		name  = strtok(linebuf, ",\r\n");
		title = strtok(NULL, ",\r\n");
		flag  = strtok(NULL, ",\r\n");

		strcpy(zipname[zipname_num].zipname, name);
		strcpy(zipname[zipname_num].title, title);
		if (flag)
		{
			if (strstr(flag, "GAME_BOOTLEG"))
				zipname[zipname_num].flag |= GAME_BOOTLEG;
			if (strstr(flag, "GAME_HACK"))
				zipname[zipname_num].flag |= GAME_HACK;
			if (strstr(flag, "GAME_NOT_WORK"))
				zipname[zipname_num].flag |= GAME_NOT_WORK;
		}
		zipname_num++;
	}

	fclose(fp);

	return 1;
}


/*--------------------------------------------------------
	zipファイル名データベースを解放
--------------------------------------------------------*/

static void free_zipname(void)
{
	zipname_num = 0;
}


/*--------------------------------------------------------
	zipファイル名からゲームタイトル名を取得
--------------------------------------------------------*/

static char *get_zipname(const char *name, int *flag)
{
	int i, length;
	char fname[MAX_PATH];

	strcpy(fname, name);
	*strrchr(fname, '.') = '\0';

	length = strlen(fname);
	if (length > 16) return NULL;//ZIP NAME length

	for (i = 0; i < length; i++)
		fname[i] = tolower(fname[i]);

	for (i = 0; i < zipname_num; i++)
	{
		if (strcasecmp(fname, zipname[i].zipname) == 0)
		{
			*flag = zipname[i].flag;
			return zipname[i].title;
		}
	}
	*flag = 0;
	return NULL;
}

#endif


/******************************************************************************
	ローカル関数 (共通)
******************************************************************************/

/*--------------------------------------------------------
	ディレクトリの存在チェック
--------------------------------------------------------*/

static void checkDir(const char *name)
{
	int fd, found;
	char path[MAX_PATH];

	memset(&dir, 0, sizeof(dir));

	fd = sceIoDopen(launchDir);
	found = 0;

	while (!found)
	{
		if (sceIoDread(fd, &dir) <= 0) break;

		if (dir.d_stat.st_attr == FIO_SO_IFDIR)
			if (strcasecmp(dir.d_name, name) == 0)
				found = 1;
	}

	sceIoDclose(fd);

	if (!found)
	{
		sprintf(path, "%s%s", launchDir, name);
		sceIoMkdir(path, 0777);
	}
}


/*--------------------------------------------------------
	ディレクトリの作成と起動ディレクトリのチェック
--------------------------------------------------------*/

static void checkStartupDir(void)
{
	int fd;

#if USE_CACHE
	checkDir("cache");
#endif
	checkDir("roms");
	checkDir("config");
//	checkDir("snap");
#ifdef SAVE_STATE
	checkDir("state");
#endif
#if VIDEO_32BPP
	checkDir("data");
#endif
#if (EMU_SYSTEM == MVS)
	checkDir("memcard");
#endif
#if (EMU_SYSTEM != NCDZ)
	checkDir("nvram");
#endif

	fd = sceIoDopen(startupDir);
	if (fd >= 0)
	{
		strcpy(curr_dir, startupDir);
		sceIoDclose(fd);
	}
	else
	{
		strcpy(startupDir, launchDir);
		strcat(startupDir, "roms");
	}
}


/*--------------------------------------------------------
	ファイルのフラグを設定
--------------------------------------------------------*/

static int set_file_flags(const char *path, int number)
{
#if (EMU_SYSTEM == NCDZ)
	if (files[number]->type == FTYPE_ZIP)
	{
		int fd;
		char zipname[MAX_PATH];

		sprintf(zipname, "%s/%s", path, files[number]->name);
		zip_open(zipname);

		if ((fd = zopen("ipl.txt")) != -1)
		{
			zclose(fd);
			strcpy(zipped_rom, files[number]->name);
			neocddir = 2;
		}
		zip_close();
		return 0;
	}
	else
	{
		files[number]->flag = GAME_HAS_TITLE;

		strcpy(files[number]->title, files[number]->name);
		strcat(files[nfiles]->title, "/");
	}
#else
	if (files[number]->type == FTYPE_ZIP)
	{
		char *title;

		if ((title = get_zipname(dir.d_name, &files[number]->flag)) == NULL)
		{
			files[number]->flag = GAME_BADROM;
			strcpy(files[number]->title, files[number]->name);
		}
		else
		{
			strcpy(files[number]->title, title);
		}
#if RELEASE
		if (files[nfiles]->flag & GAME_BOOTLEG)
		{
			return 0;
		}
#endif
	}
	else
	{
		files[number]->flag = 0;
		strcpy(files[number]->title, files[number]->name);
		strcat(files[nfiles]->title, "/");
	}
#endif

	return 1;
}


/*--------------------------------------------------------
	ディレクトリエントリを取得
--------------------------------------------------------*/

static void getDir(const char *path)
{
	int i, j, type, fd;

	memset(&dir, 0, sizeof(dir));

	nfiles = 0;
#if (EMU_SYSTEM == NCDZ)
	neocddir = 0;
	has_mp3 = 0;
#endif

	if (strcmp(path, "ms0:/") != 0)
	{
		strcpy(files[nfiles]->name, "..");
		strcpy(files[nfiles]->title, "..");
		files[nfiles]->type = FTYPE_UPPERDIR;
		files[nfiles]->flag = 0;
		nfiles++;
	}

	fd = sceIoDopen(path);

	while (nfiles < MAX_ENTRY)
	{
		char *ext;

		if (sceIoDread(fd, &dir) <= 0)
		{
			break;
		}
		if (dir.d_name[0] == '.')
		{
			continue;
		}
#if (EMU_SYSTEM == MVS)
		if (strcasecmp(dir.d_name, "neogeo.zip") == 0)
		{
			continue;
		}
#elif (EMU_SYSTEM == NCDZ)
		if (strcasecmp(dir.d_name, "ipl.txt") == 0)
		{
			neocddir = 1;
			continue;
		}
#endif
		if ((ext = strrchr(dir.d_name, '.')) != NULL)
		{
#ifdef COMMAND_LIST
			if (strcasecmp(dir.d_name, "command.dat") == 0)
			{
				strcpy(files[nfiles]->name, dir.d_name);
				strcpy(files[nfiles]->title, dir.d_name);
				files[nfiles]->type = FTYPE_FILE;
				files[nfiles]->flag = 0;
				nfiles++;
				continue;
			}
#endif
			if (strcasecmp(ext, ".zip") == 0)
			{
				strcpy(files[nfiles]->name, dir.d_name);
				files[nfiles]->type = FTYPE_ZIP;
				if (set_file_flags(path, nfiles))
				{
					nfiles++;
				}
				continue;
			}
		}
		if (dir.d_stat.st_attr == FIO_SO_IFDIR)
		{
#if USE_CACHE
			if (strcasecmp(dir.d_name, "cache") == 0) continue;
#endif
			if (strcasecmp(dir.d_name, "config") == 0) continue;
//			if (strcasecmp(dir.d_name, "snap") == 0) continue;
#if (EMU_SYSTEM != NCDZ)
			if (strcasecmp(dir.d_name, "nvram") == 0) continue;
#endif
#ifdef SAVE_STATE
			if (strcasecmp(dir.d_name, "state") == 0) continue;
#endif
#if VIDEO_32BPP
			if (strcasecmp(dir.d_name, "data") == 0) continue;
#endif
#if (EMU_SYSTEM == MVS)
			if (strcasecmp(dir.d_name, "memcard") == 0) continue;
#elif (EMU_SYSTEM == NCDZ)
			if (strcasecmp(dir.d_name, "mp3") == 0)
			{
				has_mp3 = 1;
				continue;
			}
#endif
			strcpy(files[nfiles]->name, dir.d_name);
			files[nfiles]->type = FTYPE_DIR;
			set_file_flags(path, nfiles);
			nfiles++;
		}
	}

	sceIoDclose(fd);

	for (i = 0; i < nfiles - 1; i++)
	{
		for (j = i + 1; j < nfiles; j++)
		{
			if (files[i]->type > files[j]->type)
			{
				struct dirent tmp;

				tmp = *files[i];
				*files[i] = *files[j];
				*files[j] = tmp;
			}
		}
	}

	for (type = 1; type < 4; type++)
	{
		int start = nfiles, end = 0;

		for (i = 0; i < nfiles; i++)
		{
			if (files[i]->type == type)
			{
				start = i;
				break;
			}
		}
		for (; i < nfiles; i++)
		{
			if (files[i]->type != type)
			{
				end = i;
				break;
			}
		}

		if (start == nfiles) continue;
		if (end == 0) end = nfiles;

		for (i = start; i < end - 1; i++)
		{
			for (j = i + 1; j < end; j++)
			{
				if (strcmp(files[i]->title, files[j]->title) > 0)
				{
					struct dirent tmp;

					tmp = *files[i];
					*files[i] = *files[j];
					*files[j] = tmp;
				}
			}
		}
	}
}


/*--------------------------------------------------------
	パス文字列を表示領域に合わせて修正
--------------------------------------------------------*/

static void modify_display_path(char *path, char *org_path, int max_width)
{
	strcpy(path, org_path);
	strcat(path, "/");

	if (uifont_get_string_width(path) > max_width)
	{
		int i, j, num_dir = 0;
		char temp[MAX_PATH], *dir[256];

		strcpy(temp, path);

		dir[num_dir++] = strtok(temp, "/");

		while ((dir[num_dir] = strtok(NULL, "/")) != NULL)
			num_dir++;

		j = num_dir - 1;

		do
		{
			j--;

			path[0] = '\0';

			for (i = 0; i < j; i++)
			{
				strcat(path, dir[i]);
				strcat(path, "/");
			}

			strcat(path, ".../");
			strcat(path, dir[num_dir - 1]);
			strcat(path, "/");

		} while (uifont_get_string_width(path) > max_width);
	}
}


/******************************************************************************
	グローバル関数
******************************************************************************/

/*--------------------------------------------------------
	ファイルの存在チェック
--------------------------------------------------------*/

int file_exist(const char *path)
{
	SceUID fd;

	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
	sceIoClose(fd);

	return ((fd >= 0) ? 1 : 0);
}


/*--------------------------------------------------------
	指定したパターンと一致するファイルを検索
--------------------------------------------------------*/

char *find_file(char *pattern, char *path)
{
	static struct dirent file;
	int fd, i, found, len1, len2;

	memset(&dir, 0, sizeof(dir));

	fd = sceIoDopen(path);
	found = 0;

	len1 = strlen(pattern);

	while (!found)
	{
		if (sceIoDread(fd, &dir) <= 0) break;

		len2 = strlen(dir.d_name);

		for (i = 0; i < len2; i++)
		{
			if (strncasecmp(&dir.d_name[i], pattern, len1) == 0)
			{
				strcpy(file.name, dir.d_name);
				found = 1;
				break;
			}
		}
	}

	sceIoDclose(fd);

	return found ? file.name : NULL;
}


/*--------------------------------------------------------
	指定したパターンのファイルを削除
--------------------------------------------------------*/

void delete_files(const char *dirname, const char *pattern)
{
	int fd, i, len1, len2;
	char path[MAX_PATH];

	memset(&dir, 0, sizeof(dir));

	sprintf(path, "%s%s", launchDir, dirname);

	fd = sceIoDopen(path);
	len1 = strlen(pattern);

	while (1)
	{
		if (sceIoDread(fd, &dir) <= 0) break;

		len2 = strlen(dir.d_name);

		for (i = 0; i < len2; i++)
		{
			if (strncasecmp(&dir.d_name[i], pattern, len1) == 0)
			{
				char path2[MAX_PATH];

				sprintf(path2, "%s/%s", path, dir.d_name);
				sceIoRemove(path2);
			}
		}
	}

	sceIoDclose(fd);
}


/*--------------------------------------------------------
	ステートファイルを検索
--------------------------------------------------------*/

#ifdef SAVE_STATE

void find_state_file(uint8_t *slot)
{
	int fd, len;
	char path[MAX_PATH], pattern[16];

	memset(&dir, 0, sizeof(dir));

	sprintf(path, "%sstate", launchDir);
	sprintf(pattern, "%s.sv", game_name);

	len = strlen(pattern);
	fd = sceIoDopen(path);

	while (sceIoDread(fd, &dir) > 0)
	{
		if (strncasecmp(dir.d_name, pattern, len) == 0)
		{
			int number = dir.d_name[len] - '0';

			if (number >= 0 && number <= 9)
				slot[number] = 1;
		}
	}

	sceIoDclose(fd);
}

#endif


/*--------------------------------------------------------
	アプリケーション終了画面を表示
--------------------------------------------------------*/

void show_exit_screen(void)
{
	if (Loop == LOOP_EXIT)
	{
		video_set_mode(32);
		video_clear_screen();
		boxfill(0, 0, SCR_WIDTH - 1, SCR_HEIGHT - 1, COLOR_DARKGRAY);
		uifont_print_shadow_center(129, COLOR_WHITE, TEXT(PLEASE_WAIT));
		video_flip_screen(1);
	}
}


/*--------------------------------------------------------
	ファイルブラウザ実行
--------------------------------------------------------*/

void file_browser(void)
{
	int i, sel = 0, rows = 11, top = 0;
	int run_emulation = 0, update = 1, prev_sel = 0;
	char *p;
#if (EMU_SYSTEM == NCDZ)
	int title_counter = 60, title_image = -1;
#endif

	Loop = LOOP_BROWSER;

#if (VIDEO_32BPP && USE_CACHE)
	GFX_MEMORY = NULL;
#endif

	for (i = 0; i < MAX_ENTRY; i++)
		files[i] = (struct dirent *)malloc(sizeof(struct dirent));

#if (EMU_SYSTEM != NCDZ)
	memset(zipname, 0, sizeof(zipname));
	zipname_num = 0;
#endif

	strcpy(curr_dir, launchDir);
	strcat(curr_dir, "roms");
	strcpy(startupDir, curr_dir);
	load_settings();

#if VIDEO_32BPP
	load_wallpaper();
#else
	ui_fill_frame(draw_frame, UI_PAL_BG2);
#endif

	load_background(WP_LOGO);
	show_background();
	small_icon_shadow(6, 3, UI_COLOR(UI_PAL_TITLE), ICON_SYSTEM);
	logo(32, 5, UI_COLOR(UI_PAL_TITLE));

	i = uifont_get_string_width(APPNAME_STR " " VERSION_STR) / 2;
#ifdef LARGE_MEMORY
	draw_dialog(240-(i+62), 136-48, 240+(i+62), 136+48);
	uifont_print_shadow_center(136-30, 255,255,120, APPNAME_STR " " VERSION_STR);
	uifont_print_shadow_center(136-07, 255,255,255, "for PSP Slim");
	uifont_print_shadow_center(136+06, 200,200,200, "NJ (http://nj-emu.tfact.net)");
	uifont_print_shadow_center(136+20, 200,200,200, "2011-2016 (https://github.com/phoe-nix/NJEMU)");
#else
	draw_dialog(240-(i+62), 136-48, 240+(i+62), 136+48);
	uifont_print_shadow_center(136-30, 255,255,120, APPNAME_STR " " VERSION_STR);
	uifont_print_shadow_center(136-07, 255,255,255, "for PSP");
	uifont_print_shadow_center(136+ 6, 200,200,200, "NJ (http://nj-emu.tfact.net)");
	uifont_print_shadow_center(136+20, 200,200,200, "2011-2016 (https://github.com/phoe-nix/NJEMU)");
#endif
	video_flip_screen(1);

#if (EMU_SYSTEM != NCDZ)
	if (!load_zipname())
	{
		fatalerror(TEXT(COULD_NOT_OPEN_ZIPNAME_DAT), EXT);
		show_fatal_error();
		show_exit_screen();
		goto error;
	}
#endif
	checkStartupDir();
	getDir(curr_dir);

#if defined(LARGE_MEMORY) && ((EMU_SYSTEM == CPS2) || (EMU_SYSTEM == MVS))
	if (devkit_version < 0x03070110 || kuKernelGetModel() == PSP_MODEL_STANDARD)
	{
		show_background();
		small_icon_shadow(6, 3, UI_COLOR(UI_PAL_TITLE), ICON_SYSTEM);
		logo(32, 5, UI_COLOR(UI_PAL_TITLE));
		messagebox(MB_PSPVERSIONERROR);
		show_exit_screen();
		goto error;
	}
#endif

#if (EMU_SYSTEM == NCDZ)
	check_neocd_bios();
#endif

//	pad_wait_press(3000);
	pad_wait_press(PAD_WAIT_INFINITY);

#if (EMU_SYSTEM == NCDZ)
	if (bios_error)
	{
		show_background();
		small_icon_shadow(6, 3, UI_COLOR(UI_PAL_TITLE), ICON_SYSTEM);
		logo(32, 5, UI_COLOR(UI_PAL_TITLE));
		video_flip_screen(1);

		switch (bios_error)
		{
		case 1: messagebox(MB_BIOSNOTFOUND); break;
		case 2: messagebox(MB_BIOSINVALID); break;
		}
	}
#endif

	ui_popup_reset();
	load_background(WP_FILER);

	while (Loop)
	{
		int icon[4] = { ICON_UPPERDIR, ICON_FOLDER, ICON_ZIPFILE, ICON_COMMANDDAT };

		if (run_emulation)
		{
			run_emulation = 0;
			Loop = LOOP_EXEC;

#if (EMU_SYSTEM != NCDZ)
			strcpy(game_dir, curr_dir);
#if USE_CACHE
			sprintf(cache_dir, "%scache", launchDir);
#endif
			strcpy(game_name, files[sel]->name);
			*strchr(game_name, '.') = '\0';
			i = 0;
			while (game_name[i])
			{
				game_name[i] = tolower(game_name[i]);
				i++;
			}
			free_zipname();
#endif

			for (i = 0; i < MAX_ENTRY; i++)
			{
				free(files[i]);
				files[i] = NULL;
			}

			emu_main();
			set_cpu_clock(PSPCLOCK_222);

#ifdef ADHOC
			if (adhoc_enable)
			{
				adhocUnloadModules();
				adhoc_enable = 0;
			}
#endif

			if (Loop)
			{
				for (i = 0; i < MAX_ENTRY; i++)
					files[i] = (struct dirent *)malloc(sizeof(struct dirent));

#if (EMU_SYSTEM == NCDZ)
				title_counter = 60;
				title_image = -1;
#else
				load_zipname();
#endif
				load_background(WP_FILER);
				getDir(curr_dir);
				update = 1;
			}
			else break;
		}

		if (update & UI_FULL_REFRESH)
		{
			char path[MAX_PATH];

			modify_display_path(path, curr_dir, 368);

			show_background();
			small_icon_shadow(6, 3, UI_COLOR(UI_PAL_TITLE), ICON_MEMSTICK);
			uifont_print_shadow(32, 5, UI_COLOR(UI_PAL_TITLE), path);

			for (i = 0; i < rows; i++)
			{
				if (top + i >= nfiles) break;

				if (top + i == sel)
				{
					boxfill_gradation(4, 37 + i * 20, 464, 56 + i * 20, UI_COLOR(UI_PAL_FILESEL1), UI_COLOR(UI_PAL_FILESEL2), 8, 0);
					small_icon_light(6, 38 + i * 20, UI_COLOR(UI_PAL_SELECT), icon[files[sel]->type]);

					if (files[sel]->flag & GAME_BADROM)
						uifont_print_shadow(36, 40 + i * 20, COLOR_RED, files[sel]->title);
					else if (files[sel]->flag & GAME_NOT_WORK)
						uifont_print_shadow(36, 40 + i * 20, COLOR_GRAY, files[sel]->title);
					else if (files[sel]->flag & GAME_BOOTLEG)
						uifont_print_shadow(36, 40 + i * 20, COLOR_YELLOW, files[sel]->title);
					else if (files[sel]->flag & GAME_HACK)
						uifont_print_shadow(36, 40 + i * 20, COLOR_GREEN, files[sel]->title);
					else
						uifont_print_shadow(36, 40 + i * 20, UI_COLOR(UI_PAL_SELECT), files[sel]->title);

#if (EMU_SYSTEM == NCDZ)
					if (!title_counter && title_image == -1)
					{
						if (files[sel]->flag & GAME_HAS_TITLE)
						{
							int flag;
							char name[MAX_PATH];

							sprintf(path, "%s/%s", curr_dir, files[sel]->name);

							getDir(path);

							flag = neocddir;
							if (flag == 2)
								strcpy(name, zipped_rom);
							else
								name[0] = '\0';

							getDir(curr_dir);

							if (flag)
							{
								if (flag == 2)
									sprintf(path, "%s/%s/%s", curr_dir, files[sel]->name, name);
								else
									sprintf(path, "%s/%s", curr_dir, files[sel]->name);

								if (!load_title(path, sel))
								{
									files[sel]->flag &= ~GAME_HAS_TITLE;
									title_image = -1;
								}
								else
								{
									title_image = sel;
								}
							}
							else
							{
								files[sel]->flag &= ~GAME_HAS_TITLE;
								title_image = -1;
							}
						}
					}
					if (title_image != -1)
					{
						if (sel < top + rows / 2)
							show_title(315, 169);
						else
							show_title(315, 50);
					}
#endif
				}
				else
				{
					small_icon(6, 38 + i * 20, UI_COLOR(UI_PAL_NORMAL), icon[files[top + i]->type]);

					if (files[top + i]->flag & GAME_BADROM)
						uifont_print(36, 40 + i * 20, COLOR_DARKRED, files[top + i]->title);
					else if (files[top + i]->flag & GAME_NOT_WORK)
						uifont_print(36, 40 + i * 20, COLOR_DARKGRAY, files[top + i]->title);
					else if (files[top + i]->flag & GAME_BOOTLEG)
						uifont_print(36, 40 + i * 20, COLOR_DARKYELLOW, files[top + i]->title);
					else if (files[top + i]->flag & GAME_HACK)
						uifont_print(36, 40 + i * 20, COLOR_DARKGREEN, files[top + i]->title);
					else
						uifont_print(36, 40 + i * 20, UI_COLOR(UI_PAL_NORMAL), files[top + i]->title);
				}
			}

			draw_scrollbar(469, 26, 479, 270, rows, nfiles, sel);

			update  = draw_battery_status(1);
			update |= draw_volume_status(1);
			update |= ui_show_popup(1);
			video_flip_screen(1);
		}
		else if (update & UI_PARTIAL_REFRESH)
		{
			int x, y, w, h;
			RECT clip1, clip2;

			show_background();

			for (i = 0; i < rows; i++)
				if (top + i == sel) break;

			boxfill_gradation(4, 37 + i * 20, 464, 56 + i * 20, UI_COLOR(UI_PAL_FILESEL1), UI_COLOR(UI_PAL_FILESEL2), 8, 0);
			small_icon_light(6, 38 + i * 20, UI_COLOR(UI_PAL_SELECT), icon[files[sel]->type]);

			x = 4;
			y = 38 + i * 20;
			w = 24 + 8;
			h = 18 + 8;

			clip1.left   = x - 4;
			clip1.top    = y - 4;
			clip1.right  = clip1.left + w;
			clip1.bottom = clip1.top  + h;

			clip2.left   = 0;
			clip2.top    = 112;
			clip2.right  = clip2.left + w;
			clip2.bottom = clip2.top  + h;

			video_copy_rect(draw_frame, tex_frame, &clip1, &clip2);
			video_copy_rect(show_frame, draw_frame, &full_rect, &full_rect);
			video_copy_rect(tex_frame, draw_frame, &clip2, &clip1);

			update = draw_battery_status(0);
			update |= draw_volume_status(0);
			update |= ui_show_popup(0);
			video_flip_screen(1);
		}
		else
		{
			update = draw_battery_status(0);
			update |= draw_volume_status(0);
			update |= ui_show_popup(0);
			video_wait_vsync();
		}

		update |= ui_light_update();
		prev_sel = sel;

		if (pad_pressed(PSP_CTRL_UP))
		{
			sel--;
		}
		else if (pad_pressed(PSP_CTRL_DOWN))
		{
			sel++;
		}
		else if (pad_pressed(PSP_CTRL_LEFT))
		{
			sel -= rows;
			if (sel < 0) sel = 0;
		}
		else if (pad_pressed(PSP_CTRL_RIGHT))
		{
			sel += rows;
			if (sel >= nfiles) sel = nfiles - 1;
		}
#ifdef ADHOC
		else if (pad_pressed(PSP_CTRL_CIRCLE) || (adhoc_enable = pad_pressed(PSP_CTRL_SQUARE)))
#else
		else if (pad_pressed(PSP_CTRL_CIRCLE))
#endif
		{
#ifdef ADHOC
			if (files[sel]->type != FTYPE_ZIP)
				adhoc_enable = 0;
#endif

			switch (files[sel]->type)
			{
			case FTYPE_UPPERDIR:
				if (strcmp(curr_dir, "ms0:") != 0)
				{
					char old_dir[MAX_PATH];

					p = strrchr(curr_dir, '/');
					strcpy(old_dir, p + 1);
					*p = '\0';

					getDir(curr_dir);
					sel = 0;
					prev_sel = -1;

					for (i = 0; i < nfiles; i++)
					{
						if (!strcmp(old_dir, files[i]->name))
						{
							sel = i;
							top = sel - 3;
							break;
						}
					}
				}
				break;

			case FTYPE_DIR:
				strcat(curr_dir, "/");
				strcat(curr_dir, files[sel]->name);
				getDir(curr_dir);
#if (EMU_SYSTEM == NCDZ)
				if (neocddir && !bios_error)
				{
					int launch;

					if (has_mp3)
						launch = messagebox(MB_STARTEMULATION);
					else
						launch = messagebox(MB_STARTEMULATION_NOMP3);

					if (launch)
					{
						if (neocddir == 1)
						{
							strcpy(game_dir, curr_dir);
						}
						else
						{
							sprintf(game_dir, "%s/%s", curr_dir, zipped_rom);
						}

						sprintf(mp3_dir, "%s/mp3", curr_dir);

						run_emulation = 1;
						neogeo_boot_bios = 0;
					}

					*strrchr(curr_dir, '/') = '\0';
					neocddir = 0;
				}
				if (!run_emulation)
#endif
				{
					sel = 0;
					prev_sel = -1;
					pad_wait_clear();
				}
				break;

#ifdef COMMAND_LIST
			case FTYPE_FILE:
				update = commandlist_size_reduction();
				break;
#endif

#if (EMU_SYSTEM != NCDZ)
			case FTYPE_ZIP:
				if (!(files[sel]->flag & GAME_BADROM))
				{
					if (files[sel]->flag & GAME_NOT_WORK)
					{
						messagebox(MB_GAMENOTWORK);
					}
#ifdef ADHOC
					else if (adhoc_enable)
					{
						if (!sceWlanGetSwitchState())
						{
							ui_popup(TEXT(PLEASE_TURN_ON_THE_WLAN_SWITCH));
							adhoc_enable = 0;
						}
						else if (messagebox(MB_STARTEMULATION_ADHOC))
						{
							if (adhocLoadModules() >= 0)
							{
								run_emulation = 1;
							}
							else
							{
								ui_popup(TEXT(FAILED_TO_LOAD_ADHOC_MODULES));
								adhoc_enable = 0;
							}
						}
					}
#endif
					else if (messagebox(MB_STARTEMULATION))
					{
						run_emulation = 1;
					}
				}
				update = 1;
				break;
#endif
			}
			pad_wait_clear();
		}
/*PRESS TRIANGLE OR HOME TO EXIT IN FILEBROWSER */
//		else if (pad_pressed(PSP_CTRL_TRIANGLE))
		else if (pad_pressed(PSP_CTRL_TRIANGLE) || (readHomeButton()))
		{
			if (messagebox(MB_EXITEMULATION))
			{
				Loop = LOOP_EXIT;
				show_exit_screen();
				break;
			}
			update = 1;
			pad_wait_clear();
		}
#if (EMU_SYSTEM == MVS)
		else if (pad_pressed(PSP_CTRL_RTRIGGER))
		{
			strcpy(game_dir, curr_dir);
			bios_select(0);
			update = 1;
		}
#elif (EMU_SYSTEM == NCDZ)
		else if (pad_pressed(PSP_CTRL_RTRIGGER))
		{
			if (!bios_error)
			{
				if (messagebox(MB_BOOTBIOS))
				{
					strcpy(game_dir, curr_dir);
					sprintf(mp3_dir, "%s/mp3", curr_dir);
					run_emulation = 1;
					neogeo_boot_bios = 1;
				}
				update = 1;
			}
			pad_wait_clear();
		}
#endif
		else if (pad_pressed(PSP_CTRL_START))
		{
			if (files[sel]->type == FTYPE_DIR)
			{
				if (messagebox(MB_SETSTARTUPDIR))
					sprintf(startupDir, "%s/%s", curr_dir, files[sel]->name);
			}
		}
#if VIDEO_32BPP
		else if (pad_pressed(PSP_CTRL_LTRIGGER))
		{
			show_color_menu();
			update = 1;
		}
#endif
		else if (pad_pressed(PSP_CTRL_SELECT))
		{
			help(HELP_FILEBROWSER);
			update = 1;
		}

		if (top > nfiles - rows) top = nfiles - rows;
		if (top < 0) top = 0;
		if (sel >= nfiles) sel = 0;
		if (sel < 0) sel = nfiles - 1;
		if (sel >= top + rows) top = sel - rows + 1;
		if (sel < top) top = sel;

		if (prev_sel != sel)
		{
#if (EMU_SYSTEM == NCDZ)
			title_counter = 60;
			title_image = -1;
#endif
			update = 1;
		}
#if (EMU_SYSTEM == NCDZ)
		else if (title_counter)
		{
			title_counter--;
			if (!title_counter)
				update = 1;
		}
#endif

		pad_update();
	}

	save_settings();
#if (EMU_SYSTEM != NCDZ)
error:
	for (i = 0; i < MAX_ENTRY; i++)
	{
		if (files[i]) free(files[i]);
	}
	free_zipname();
#endif
#if VIDEO_32BPP
	free_wallpaper();
#endif
}
