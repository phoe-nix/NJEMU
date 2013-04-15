/******************************************************************************

	cdrom.c

	NEOGEO CDZ CD-ROMエミュレーション

******************************************************************************/

#include "ncdz.h"


/******************************************************************************
	グローバル変数
******************************************************************************/

int neogeo_loadscreen;
int neogeo_cdspeed_limit;
int neogeo_loadfinished;


/******************************************************************************
	ローカル構造体/変数
******************************************************************************/

static struct filelist_t
{
	char name[16];
	UINT32  offset;
	int  bank;
	int  type;
	UINT32  length;
	UINT32  sectors;
} filelist[32];


static UINT8  *cdrom_cache;
static UINT32 total_sectors;
static UINT32 loaded_sectors;
static int total_files;

static int firsttime_update;


#ifdef SAVE_STATE
typedef struct cdrom_state_t CDROM_STATE;

struct cdrom_state_t
{
	char name[16];
	UINT32 bank;
	UINT32 offset;
	UINT32 length;
	UINT32 start;
	UINT32 end;
	CDROM_STATE *prev;
	CDROM_STATE *next;
};

static CDROM_STATE cdrom_state[3][MAX_CDROM_STATE];
static CDROM_STATE *cdrom_state_head[3];
static CDROM_STATE *cdrom_state_free_head[3];
static int cdrom_state_count[3];
#endif


/******************************************************************************
	プロトタイプ
******************************************************************************/

#ifdef SAVE_STATE
static void cdrom_state_load_file(int type, const char *fname, int bank, UINT32 offset, UINT32 length);
static int cdrom_state_check_list(int type, UINT32 start, UINT32 end);
static void cdrom_state_insert_list(int type, const char *fname, int bank, UINT32 offset, UINT32 length, UINT32 start, UINT32 end);
static void cdrom_state_insert_fix(const char *fname, UINT32 offset, UINT32 length);
static void cdrom_state_insert_spr(const char *fname, int bank, UINT32 offset, UINT32 length);
static void cdrom_state_insert_pcm(const char *fname, int bank, UINT32 offset, UINT32 length);
#endif


/******************************************************************************
	ローカル関数
******************************************************************************/

/*------------------------------------------------------
	16進数→10進数変換
------------------------------------------------------*/

static int hextodec(char c)
{
	switch (tolower(c))
	{
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	case 'a': return 10;
	case 'b': return 11;
	case 'c': return 12;
	case 'd': return 13;
	case 'e': return 14;
	case 'f': return 15;
	default: return 0;
	}
}


/*------------------------------------------------------
	拡張子からファイルタイプを判別
------------------------------------------------------*/

static int get_filetype(const char *ext)
{
	int type = UNKNOWN_TYPE;

	if (stricmp(ext, "PRG") == 0) type = PRG_TYPE;
	if (stricmp(ext, "FIX") == 0) type = FIX_TYPE;
	if (stricmp(ext, "SPR") == 0) type = SPR_TYPE;
	if (stricmp(ext, "Z80") == 0) type = Z80_TYPE;
	if (stricmp(ext, "PCM") == 0) type = PCM_TYPE;
	if (stricmp(ext, "PAT") == 0) type = PAT_TYPE;
	if (ext[0] == 'A') type = AXX_TYPE;

	return type;
}


/*--------------------------------------------------------
	ロード画面無効時の "Now loading" 表示
--------------------------------------------------------*/

static void show_loading_image(void)
{
	char path[MAX_PATH];

	video_clear_screen();

	sprintf(path, "%sdata/%s", launchDir, "loading.png");

	if (load_png(path, -1) == 0)
		uifont_print_shadow_center(129, COLOR_WHITE, "Now Loading...");

	video_flip_screen(1);
}


/*------------------------------------------------------
	ロード画面の進行度を初期化
------------------------------------------------------*/

static void init_loading_progress(void)
{
	if (neogeo_loadscreen && with_image())
	{
		m68000_write_memory_32(0x108000 + 0x7694, total_sectors);
		m68000_write_memory_32(0x108000 + 0x7690, 0);
	}
}


/*------------------------------------------------------
	ロード画面の進行度を更新
------------------------------------------------------*/

static void update_loading_progress(void)
{
	if (neogeo_loadscreen && with_image())
	{
		static int draw = 0;
		UINT32 prev_progress, progress;

		prev_progress = m68000_read_memory_32(0x108000 + 0x7690);

		loaded_sectors++;
		progress = ((loaded_sectors * 0x8000) / total_sectors) << 8;

		if (progress != prev_progress)
		{
			m68000_write_memory_32(0x108000 + 0x7690, progress);

			if (progress >= 0x800000)
				m68000_write_memory_32(0x108000 + 0x7690, 0x800000);

			m68000_execute2(m68000_read_memory_32(0x11c80c), 0);

			m68000_write_memory_8(0x108000 + 0x7793, 0x00);
			m68000_execute2(0xc0c8b2, 0);

			draw = 1;
		}

		if (neogeo_ngh == NGH_ssideki3) timer_update_subcpu();
		draw = neogeo_loading_screenrefresh(firsttime_update, draw);
		firsttime_update = 0;
	}
}


/*------------------------------------------------------
	CD-ROMキャッシュからメモリへデータを転送
------------------------------------------------------*/

static void upload_file(int fileno, UINT32 offset, UINT32 length)
{
	struct filelist_t *file = &filelist[fileno];
	UINT32 base;

	switch (file->type)
	{
	case PRG_TYPE:
	case AXX_TYPE:
		swab(cdrom_cache, memory_region_cpu1 + file->offset + offset, length);
		break;

	case FIX_TYPE:
		if (neogeo_loadscreen && with_image())
		{
			UINT32 offset1, offset2, start, end;
			UINT32 length1, length2;

			start = (file->offset >> 1) + offset;
			end   = (start + length) - 1;

			if (start >= 0x6000)
			{
				offset1 = 0;
				length1 = 0;
				offset2 = start << 1;
				length2 = length;
			}
			else if (end < 0x6000)
			{
				offset1 = start << 1;
				length1 = length;
				offset2 = 0;
				length2 = 0;
			}
			else
			{
				offset1 = start << 1;
				length1 = 0x6000 - start;
				offset2 = 0x6000 << 1;
				length2 = length - length1;
			}

			if (length1)
			{
				memcpy(memory_region_cpu1 + 0x115e06 + (offset1 >> 1), cdrom_cache, length1);
			}

			if (length2)
			{
				memcpy(memory_region_gfx1 + (offset2 >> 1), cdrom_cache + length1, length2);
				neogeo_decode_fix(memory_region_gfx1, offset2, length2);
			}
		}
		else
		{
			base = file->offset >> 1;
			memcpy(memory_region_gfx1 + base + offset, cdrom_cache, length);
			neogeo_decode_fix(memory_region_gfx1, file->offset + (offset << 1), length);
		}
		break;

	case SPR_TYPE:
		base = (file->bank << 20) + file->offset;
		memcpy(memory_region_gfx2 + base + offset, cdrom_cache, length);
		neogeo_decode_spr(memory_region_gfx2, base + offset, length);
		break;

	case Z80_TYPE:
		base = file->offset >> 1;
		memcpy(memory_region_cpu2 + base + offset, cdrom_cache, length);
		break;

	case PAT_TYPE:
		swab(cdrom_cache, cdrom_cache, length);
		neogeo_apply_patch((UINT16 *)cdrom_cache, file->bank, file->offset);
		break;

	case PCM_TYPE:
		base = (file->bank << 19) + (file->offset >> 1);
		memcpy(memory_region_sound1 + base + offset, cdrom_cache, length);
		break;
	}
}


/*------------------------------------------------------
	CD-ROMキャッシュにファイルを読み込む
------------------------------------------------------*/

static int load_file(int fileno)
{
	struct filelist_t *file = &filelist[fileno];
	int f;
	UINT32 length, next = 0;

#ifdef SAVE_STATE
	switch (file->type)
	{
	case FIX_TYPE: cdrom_state_insert_fix(file->name, file->offset, file->length); break;
	case SPR_TYPE: cdrom_state_insert_spr(file->name, file->bank, file->offset, file->length); break;
	case PCM_TYPE: cdrom_state_insert_pcm(file->name, file->bank, file->offset, file->length); break;
	}
#endif

	if (file->type == Z80_TYPE)
		m68000_write_memory_8(0xff0183, 0);

	if ((f = zopen(file->name)) == -1)
	{
		fatalerror(TEXT(COULD_NOT_OPEN_FILE), file->name);
		return -1;
	}

	if (neogeo_loadscreen && with_image())
	{
		while ((length = zread(f, cdrom_cache, 0x800)) != 0)
		{
			upload_file(fileno, next, length);
			next += length;
			update_loading_progress();
		}
	}
	else
	{
		while ((length = zread(f, cdrom_cache, 0x800)) != 0)
		{
			upload_file(fileno, next, length);
			next += length;
		}
	}

	zclose(f);

	if (file->type == Z80_TYPE)
		m68000_write_memory_8(0xff0183, 0xff);

	return 0;
}


/*--------------------------------------------------------
	ロード画面のデータをチェック
--------------------------------------------------------*/

static UINT32 check_offset;

static int check_screen_data(UINT32 offset, int type)
{
	UINT32 data_offset;
	int data_type;

	check_offset = offset;

	do
	{
		data_type = m68000_read_memory_32(check_offset);
		check_offset += 4;

		data_offset = m68000_read_memory_32(check_offset);
		check_offset += 4;

		if (data_type == type + 1)
			return -1;

		if (data_type == -1)
			return -1;

	} while (data_type != type);

	return data_offset;
}


/*--------------------------------------------------------
	ロード画面のFIXスプライトを転送
--------------------------------------------------------*/

static void loading_upload_fix(void)
{
	UINT8 *src, *dst;
	UINT32 offset;

	// 現在のFIXスプライトを保存
	src = memory_region_gfx1;
	dst = memory_region_cpu1 + 0x115e06;
	memcpy(dst, src, 0x6000);
	neogeo_undecode_fix(dst, 0, 0x6000);

	// デフォルトのロード画面のFIXスプライトを転送
	src = memory_region_user1 + 0x7c000;
	dst = memory_region_gfx1;
	memcpy(dst, src, 0x4000);
	neogeo_decode_fix(dst, 0, 0x4000);

	offset = 0x120002;

	// ゲーム独自のロード画面のFIXスプライトを転送
	while (1)
	{
		UINT16 fix_offs, size;

		offset = check_screen_data(offset, 1);
		if (offset == 0xffffffff)
			break;

		fix_offs = m68000_read_memory_32(offset);
		offset += 4;

		size = m68000_read_memory_32(offset);
		offset += 4;

		src = memory_region_cpu1;
		dst = memory_region_gfx1;
		memcpy(dst + (fix_offs >> 1), src + offset, size);
		neogeo_decode_fix(dst, fix_offs, size);

		offset = check_offset;
	}
}


/*--------------------------------------------------------
	ロード画面のパレットを転送
--------------------------------------------------------*/

static void loading_upload_palette(void)
{
	int i;
	UINT32 src, dst, offset;

	// 現在のパレットを保存
	src = 0x400000;
	dst = 0x11be06;

	for (i = 0; i < 0x200; i += 4)
	{
		m68000_write_memory_32(dst, m68000_read_memory_32(src));
		src += 4;
		dst += 4;
	}

	// BIOSのパレットを転送
	src = 0xc1701c;
	dst = 0x400000;

	for (i = 0; i < 0x200; i += 4)
	{
		m68000_write_memory_32(dst, m68000_read_memory_32(src));
		src += 4;
		dst += 4;
	}

	offset = 0x120002;

	// ゲーム独自のパレットを転送
	while (1)
	{
		UINT16 palno;

		src = check_screen_data(offset, 2);
		if (src == 0xffffffff)
			break;

		palno = m68000_read_memory_16(src);
		src += 2;

		dst = 0x400000 + (palno << 5);

		for (i = 0; i < 16; i++)
		{
			m68000_write_memory_16(dst, m68000_read_memory_16(src));
			src += 2;
			dst += 2;
		}

		offset = check_offset;
	}
}


/*--------------------------------------------------------
	ロード画面表示開始
--------------------------------------------------------*/

static void loading_screen_start(void)
{
	if (neogeo_loadscreen)
	{
		firsttime_update = 1;

		if (with_image())
		{
			int i;
			UINT32 offset;

			video_clear_screen();

			// Save FIX plane
			for (i = 0; i < 0x500; i++)
				m68000_write_memory_16(0x110804 + i * 2, neogeo_videoram[0x7000 + i]);

			// Setup load screen
			m68000_execute2(m68000_read_memory_32(0x11c808), 0);

			loading_upload_fix();
			loading_upload_palette();

			offset = m68000_read_memory_32(0x11c80c);
			if (offset != 0xc0c814)
			{
				i = 64;	 // for safety

				if (NGH_NUMBER(0x0096))	// aof3
				{
					while (i--)
					{
						offset = m68000_read_memory_32(0x11c80c);
						m68000_execute2(offset, 0);

						if (offset == 0x1244f0 || offset == 0x124534
						||  offset == 0x1245b2 || offset == 0x1245ce)
							break;
					}
				}
				else if (NGH_NUMBER(0x0234))	// lastblad
				{
					while (i--)
					{
						offset = m68000_read_memory_32(0x11c80c);
						m68000_execute2(offset, 0);

						if (offset == 0x124550 || offset == 0x124b28)
							break;
					}
				}
				else if (NGH_NUMBER(0x0243))	// lastbld2
				{
					while (i--)
					{
						offset = m68000_read_memory_32(0x11c80c);
						m68000_execute2(offset, 0);

						if (offset == 0x1244aa || offset == 0x124d9c)
							break;
					}
				}
			}

			fix_disable_w(0);
			spr_disable_w(1);
			video_enable_w(1);
		}
		else if (NGH_NUMBER(0x0212))	// overtop
		{
			UINT8 *src, *dst;

			video_clear_screen();

			src = memory_region_cpu1 + 0xe0000;
			dst = memory_region_gfx1;
			swab(src, dst, 0x20000);
			neogeo_decode_fix(dst, 0, 0x20000);

			m68000_write_memory_32(0x11c80c, 0x8db0);
		}
	}
}


/*--------------------------------------------------------
	ロード画面表示終了
--------------------------------------------------------*/

static void loading_screen_stop(void)
{
	if (neogeo_loadscreen)
	{
		if (with_image())
		{
			UINT8 *src, *dst;
			UINT32 src_offs, dst_offs;
			int i;

			// Restore palettes
			src_offs = 0x11be06;
			dst_offs = 0x400000;

			for (i = 0; i < 0x200; i += 4)
			{
				m68000_write_memory_32(dst_offs, m68000_read_memory_32(src_offs));
				src_offs += 4;
				dst_offs += 4;
			}

			// Restore FIX data
			src = memory_region_cpu1 + 0x115e06;
			dst = memory_region_gfx1;
			memcpy(dst, src, 0x6000);
			neogeo_decode_fix(dst, 0, 0x6000);

			// Restore FIX plane
			for (i = 0; i < 0x500; i++)
				neogeo_videoram[0x7000 + i] = m68000_read_memory_16(0x110804 + i * 2);

			fix_disable_w(0);
			spr_disable_w(0);
			video_enable_w(1);
		}
		else if (NGH_NUMBER(0x0212))	// overtop
		{
			m68000_write_memory_32(0x11c80c, 0x8854);
		}
	}

	raster_line = 0;
	raster_counter = RASTER_COUNTER_RELOAD;
}


/******************************************************************************
	CD-ROMインタフェース関数
******************************************************************************/

/*------------------------------------------------------
	CD-ROMインタフェース初期化
------------------------------------------------------*/

void cdrom_init(void)
{
#ifdef SAVE_STATE
	int i, j;

	for (i = 0; i < 3; i++)
	{
		memset(&cdrom_state[i], 0, sizeof(CDROM_STATE) * MAX_CDROM_STATE);

		for (j = 0; j < MAX_CDROM_STATE - 1; j++)
			cdrom_state[i][j].next = &cdrom_state[i][j + 1];

		cdrom_state[i][MAX_CDROM_STATE - 1].next = NULL;

		cdrom_state_head[i] = NULL;
		cdrom_state_free_head[i] = &cdrom_state[i][0];
		cdrom_state_count[i] = 0;
	}
#endif

	cdrom_cache = memory_region_cpu1 + 0x111204;
	neogeo_loadfinished = 0;
}


/*------------------------------------------------------
	CD-ROMインタフェース終了
------------------------------------------------------*/

void cdrom_shutdown(void)
{
}


/*------------------------------------------------------
	IPL.TXT処理
------------------------------------------------------*/

int cdrom_process_ipl(void)
{
	struct filelist_t *file = &filelist[0];
    int i, j, f;
    UINT32 length;
    char linebuf[32], *buf, *p, *ext;
	char region_chr[3] = { 'J','U','E' };

	zip_open(game_dir);

	video_clear_screen();
	neogeo_loadfinished = 0;

	if (neogeo_loadscreen)
	{
		char fname[16];

		for (i = neogeo_region & 3; i >= 0; i--)
		{
			sprintf(fname, "LOGO_%c.PRG", region_chr[i]);

			if ((f = zopen(fname)) != -1)
			{
				UINT8 *mem = memory_region_cpu1 + 0x120000;

				length = zread(f, mem, 0x10000);
				zclose(f);

				swab(mem, mem, length);
			}
		}
	}
	else
	{
		show_loading_image();
	}

	memset(cdrom_cache, 0, 0x800);
	length = zlength("IPL.TXT");

    if ((f = zopen("IPL.TXT")) == -1)
    {
		zip_close();
		return 0;
	}

	zread(f, cdrom_cache, length);
	zclose(f);

	total_sectors = 0;
	loaded_sectors = 0;
	total_files = 0;

	p = buf = (char *)cdrom_cache;

	while ((UINT32)p - (UINT32)cdrom_cache < length)
	{
		memset(linebuf, 0, 32);

		p = strchr(buf, 0x0a);
		if (p == NULL) break;

		*p++ = 0;

		strcpy(linebuf, buf);

		buf = p;

		if (strlen(linebuf) < 3) break;

		file->bank   = 0;
		file->offset = 0;

		i = 0;
		j = 0;
		while (linebuf[i] != ',')
		{
			file->name[j++] = toupper(linebuf[i++]);
		}
		file->name[j] = '\0';

		ext = &file->name[j - 3];
		if (ext[0] == 'O') strcpy(ext, "SPR");
		file->type = get_filetype(ext);

		i++;
		while (linebuf[i] != ',')
		{
			file->bank *= 10;
			file->bank += linebuf[i++] - '0';
		}
		file->bank &= 0x03;

		i++;
		while (linebuf[i] != 0x0d)
		{
			file->offset *= 16;
			file->offset += hextodec(linebuf[i++]);
		}

		file->length = zlength(file->name);
		file->sectors = (file->length + 0x7ff) / 0x800;

		total_sectors += file->sectors;
		total_files++;

		file++;
	}

	m68000_write_memory_8(0x108000 + 0x7ddc, 0x01);

	init_loading_progress();
	loading_screen_start();

	for (i = 0; i < total_files; i++)
	{
		if (Loop != LOOP_EXEC) return 1;
		load_file(i);
	}

	loading_screen_stop();

	m68000_write_memory_8(0x108000 + 0x7ddc, 0x00);
	m68000_write_memory_32(0x108000 + 0x7690, 0);

	video_clear_screen();

	if (!neogeo_loadscreen) neogeo_loadfinished = 1;

	autoframeskip_reset();
	blit_clear_fix_sprite();

	zip_close();

	return 1;
}


/*------------------------------------------------------
	ファイル読み込み
------------------------------------------------------*/

void cdrom_load_files(void)
{
	struct filelist_t *file = &filelist[0];
	UINT32 offset, src, dst, data, save1, save2;
	int i;

	neogeo_loadfinished = 0;

    src = m68000_get_reg(M68K_A0);

    if (m68000_read_memory_8(src) == 0)
		return;

	zip_open(game_dir);

	if (with_image() && !neogeo_loadscreen)
		show_loading_image();

	save1 = m68000_read_memory_8(0x108000 + 0x7d80);
	save2 = m68000_read_memory_8(0x108000 + 0x7dc2);

    cdda_stop();

	offset = 0x115a06;
	for (i = 0; i < 0x20; i++)
	{
		memset(memory_region_cpu1 + offset, 0, 0x20);
		m68000_write_memory_16(offset + 0x1c, 0xffff);
		offset += 0x20;
	}

	offset = 0x115a06;

	do
	{
		dst = offset;

		while ((data = m68000_read_memory_8(src++)) != 0)
		{
			if (data >= 'a') data -= 0x20;
			m68000_write_memory_8(dst++, data);
		}

		if (m68000_read_memory_8(dst - 3) == 'O')
		{
			m68000_write_memory_8(dst - 3, 'S');
			m68000_write_memory_8(dst - 2, 'P');
			m68000_write_memory_8(dst - 1, 'R');
		}

		dst = offset + 0x10;
		data = m68000_read_memory_8(src++) & 0x03;
		m68000_write_memory_8(dst++, data);

		src = (src + 1) & 0xfffffe;
		dst = (dst + 1) & 0xfffffe;

		m68000_write_memory_32(dst, m68000_read_memory_32(src));

		src += 4;
		offset += 0x20;

	} while (m68000_read_memory_8(src) != 0);

	offset = 0x115a06;

	total_sectors = 0;
	loaded_sectors = 0;
	total_files = 0;

	do
	{
		char *p;

		swab(memory_region_cpu1 + offset, file->name, 16);

		for (i = 0; file->name[i]; i++)
			if (!isprint(file->name[i]))
				break;

		if ((p = strchr(file->name, ';')) != NULL) *p = '\0';

		file->bank   = m68000_read_memory_8(offset + 0x10);
		file->offset = m68000_read_memory_32(offset + 0x12);
		file->type   = get_filetype(strrchr(file->name, '.') + 1);

		file->length = zlength(file->name);
		total_sectors += (file->length + 0x7ff) / 0x800;
		total_files++;

		offset += 0x20;
		file++;

	} while (memory_region_cpu1[offset] != 0);

	init_loading_progress();
	loading_screen_start();

	for (i = 0; i < total_files; i++)
	{
		if (Loop != LOOP_EXEC) return;
		load_file(i);
	}

	loading_screen_stop();

	if (with_image() && !neogeo_loadscreen)
		video_clear_screen();

	m68000_write_memory_8(0x108000 + 0x7e88, 0x00);	// unknown, need to clear by '0'
	m68000_write_memory_8(0x108000 + 0x7ddd, 0x00);	// unknown, need to clear by '0'
	m68000_write_memory_8(0x108000 + 0x76db, 0x01);	// unknown, need to set '1'
	m68000_write_memory_8(0x108000 + 0x7ec4, 0x01);	// unknown, need to set '1'

	m68000_write_memory_32(0x10f742, 0);	// unknown, need to clear by '0'
	m68000_write_memory_32(0x10f746, 0);	// unknown, need to clear by '0'

	m68000_write_memory_8(0x108000 + 0x7657, 0x00);	// CD-ROM loading flag
	m68000_write_memory_8(0x108000 + 0x76d9, 0x01);	// Input enable (?)
	m68000_write_memory_8(0x108000 + 0x76c3, 0x01);	// CDDA enable
	m68000_write_memory_8(0x108000 + 0x76f6, 0xff);	// CDDA command
	m68000_write_memory_8(0x108000 + 0x764b, 0xff);	// CDDA track

	m68000_write_memory_8(0x108000 + 0x7ddc, 0x00);	// with/without loading screen
	m68000_write_memory_8(0x108000 + 0x7e85, 0xff);	// unknown

	m68000_write_memory_8(0x108000 + 0x7dc2, save2);
	m68000_write_memory_8(0x108000 + 0x7d80, save1);

	autoframeskip_reset();
	blit_clear_fix_sprite();

	if (!neogeo_loadscreen) neogeo_loadfinished = 1;

	zip_close();
}


/*------------------------------------------------------
	SPRスプライトのデコード
------------------------------------------------------*/

void neogeo_decode_spr(UINT8 *mem, UINT32 offset, UINT32 length)
{
	UINT32 tileno, numtiles = length / 128;
	UINT8 *base  = mem + offset;
	UINT8 *usage = spr_pen_usage + (offset >> 7);

	for (tileno = 0; tileno < numtiles; tileno++)
	{
		UINT8 swap[128];
		UINT8 *gfxdata;
		int x,y;
		UINT32 pen;
		int opaque = 0;

		gfxdata = &base[tileno << 7];

		memcpy(swap, gfxdata, 128);

		for (y = 0;y < 16;y++)
		{
			UINT32 dw, data;

			dw = 0;
			for (x = 0;x < 8;x++)
			{
				pen  = ((swap[64 + 4*y + 2] >> x) & 1) << 3;
				pen |= ((swap[64 + 4*y + 3] >> x) & 1) << 2;
				pen |= ((swap[64 + 4*y + 0] >> x) & 1) << 1;
				pen |= ((swap[64 + 4*y + 1] >> x) & 1) << 0;
				opaque += (pen & 0x0f) != 0;
				dw |= pen << 4*x;
			}

			data = ((dw & 0x0000000f) >>  0) | ((dw & 0x000000f0) <<  4)
				 | ((dw & 0x00000f00) <<  8) | ((dw & 0x0000f000) << 12)
				 | ((dw & 0x000f0000) >> 12) | ((dw & 0x00f00000) >>  8)
				 | ((dw & 0x0f000000) >>  4) | ((dw & 0xf0000000) >>  0);

			*(gfxdata++) = data >>  0;
			*(gfxdata++) = data >>  8;
			*(gfxdata++) = data >> 16;
			*(gfxdata++) = data >> 24;

			dw = 0;
			for (x = 0;x < 8;x++)
			{
				pen  = ((swap[4*y + 2] >> x) & 1) << 3;
				pen |= ((swap[4*y + 3] >> x) & 1) << 2;
				pen |= ((swap[4*y + 0] >> x) & 1) << 1;
				pen |= ((swap[4*y + 1] >> x) & 1) << 0;
				opaque += (pen & 0x0f) != 0;
				dw |= pen << 4*x;
			}

			data = ((dw & 0x0000000f) >>  0) | ((dw & 0x000000f0) <<  4)
				 | ((dw & 0x00000f00) <<  8) | ((dw & 0x0000f000) << 12)
				 | ((dw & 0x000f0000) >> 12) | ((dw & 0x00f00000) >>  8)
				 | ((dw & 0x0f000000) >>  4) | ((dw & 0xf0000000) >>  0);

			*(gfxdata++) = data >>  0;
			*(gfxdata++) = data >>  8;
			*(gfxdata++) = data >> 16;
			*(gfxdata++) = data >> 24;
		}

		if (opaque)
			*usage = (opaque == 256) ? SPRITE_OPAQUE : SPRITE_TRANSPARENT;
		else
			*usage = 0;
		usage++;
	}

	blit_clear_spr_sprite();
}


/*------------------------------------------------------
	FIXスプライトのデコード
------------------------------------------------------*/

#define decode_fix(n)				\
{									\
	tile = buf[n];					\
	*p++ = tile;					\
	opaque += (tile & 0x0f) != 0;	\
	opaque += (tile >> 4) != 0;		\
}

void neogeo_decode_fix(UINT8 *mem, UINT32 offset, UINT32 length)
{
	UINT32 i, j;
	UINT8 tile, opaque;
	UINT8 *p, buf[32];
	UINT8 *usage = &fix_pen_usage[offset >> 6];
	UINT8 *base  = &mem[offset >> 1];

	for (i = 0; i < length; i += 32)
	{
		opaque  = 0;

		memcpy(buf, &base[i], 32);
		p = &base[i];

		for (j = 0; j < 8; j++)
		{
			decode_fix(j + 16);
			decode_fix(j + 24);
			decode_fix(j +  0);
			decode_fix(j +  8);
		}

		if (opaque)
			*usage = (opaque == 64) ? SPRITE_OPAQUE : SPRITE_TRANSPARENT;
		else
			*usage = 0;
		usage++;
	}

	blit_clear_fix_sprite();
}


/*------------------------------------------------------
	FIXスプライトを展開前の状態に戻す
------------------------------------------------------*/

#define undecode_fix(n)				\
{									\
	buf[n] = *p++;					\
}

void neogeo_undecode_fix(UINT8 *mem, UINT32 offset, UINT32 length)
{
	UINT32 i, j;
	UINT8 *p, buf[32];
	UINT8 *base = &mem[offset >> 1];

	for (i = 0; i < length; i += 32)
	{
		memcpy(buf, &base[i], 32);
		p = &base[i];

		for (j = 0; j < 8; j++)
		{
			undecode_fix(j + 16);
			undecode_fix(j + 24);
			undecode_fix(j +  0);
			undecode_fix(j +  8);
		}
	}

	blit_clear_fix_sprite();
}


/*------------------------------------------------------
	Z80パッチ処理
------------------------------------------------------*/

#define PATCH_Z80(a, b)					\
{										\
	dst[((a) + 0)] =  (b)       & 0xff;	\
	dst[((a) + 1)] = ((b) >> 8) & 0xff;	\
}

void neogeo_apply_patch(UINT16 *src, int bank, UINT32 offset)
{
	UINT8 *dst = memory_region_cpu2;

	offset = (((bank * 0x100000) + offset) >> 8) & 0xffff;

	while (*src)
	{
		PATCH_Z80(src[0] + 0,  (src[1] + offset) >> 1);
		PATCH_Z80(src[0] + 2, ((src[2] + offset) >> 1) - 1);

		if (src[3] && src[4])
		{
			PATCH_Z80(src[0] + 5,  (src[3] + offset) >> 1);
			PATCH_Z80(src[0] + 7, ((src[4] + offset) >> 1) - 1);
		}

		src += 5;
	}
}


/******************************************************************************
	セーブ/ロードステート
******************************************************************************/

#ifdef SAVE_STATE

STATE_SAVE( cdrom )
{
	int i, j;

	for (i = 0; i < 3; i++)
	{
		CDROM_STATE *p = cdrom_state_head[i];

		state_save_long(&cdrom_state_count[i], 1);

		for (j = 0; j < cdrom_state_count[i]; j++)
		{
			state_save_byte(p->name, 16);
			state_save_long(&p->bank, 1);
			state_save_long(&p->offset, 1);
			state_save_long(&p->length, 1);
			state_save_long(&p->start, 1);
			state_save_long(&p->end, 1);
			p = p->next;
		}
	}
}


STATE_LOAD( cdrom )
{
	int i, j;

	memset(memory_region_gfx1, 0, memory_length_gfx1);
	memset(memory_region_gfx2, 0, memory_length_gfx2);
	memset(memory_region_sound1, 0, memory_length_sound1);

	zip_open(game_dir);

	cdrom_init();

	for (i = 0; i < 3; i++)
	{
		CDROM_STATE *p, *last = NULL;

		state_load_long(&cdrom_state_count[i], 1);

		for (j = 0; j < cdrom_state_count[i]; j++)
		{
			p = cdrom_state_free_head[i];

			if (last)
				last->next = p;
			else
				cdrom_state_head[i] = p;

			cdrom_state_free_head[i] = cdrom_state_free_head[i]->next;

			p->prev = last;
			p->next = NULL;

			state_load_byte(p->name, 16);
			state_load_long(&p->bank, 1);
			state_load_long(&p->offset, 1);
			state_load_long(&p->length, 1);
			state_load_long(&p->start, 1);
			state_load_long(&p->end, 1);

			cdrom_state_load_file(i, p->name, p->bank, p->offset, p->length);

			last = p;
		}
	}

	zip_close();
}


/*------------------------------------------------------
	CD-ROMからファイルを読み込む
------------------------------------------------------*/

static void cdrom_state_load_file(int type, const char *fname, int bank, UINT32 offset, UINT32 length)
{
	struct filelist_t *file = &filelist[0];
	int ftype[3] = { FIX_TYPE, SPR_TYPE, PCM_TYPE };
	int f;
	UINT32 next = 0;

	if ((f = zopen(fname)) == -1)
	{
		fatalerror(TEXT(COULD_NOT_OPEN_FILE), file->name);
		return;
	}

	file->type   = ftype[type];
	file->bank   = bank;
	file->offset = offset;
	file->length = length;

	while ((length = zread(f, cdrom_cache, 0x800)) != 0)
	{
		upload_file(0, next, length);
		next += length;
	}

	zclose(f);
}

/*------------------------------------------------------
	CD-ROMステートデータをチェック
------------------------------------------------------*/

static int cdrom_state_check_list(int type, UINT32 start, UINT32 end)
{
	if (cdrom_state_count[type])
	{
		CDROM_STATE *p = cdrom_state_head[type];
		CDROM_STATE *q;

		while (p)
		{
			if (start <= p->start && end >= p->end)
			{
				if (p->prev)
					p->prev->next = p->next;
				else
					cdrom_state_head[type] = p->next;

				if (p->next)
					p->next->prev = p->prev;

				q = p->next;
				p->next = cdrom_state_free_head[type];
				cdrom_state_free_head[type] = p;
				cdrom_state_count[type]--;
				p = q;
				continue;
			}
			p = p->next;
		}

		if (cdrom_state_count[type] == MAX_CDROM_STATE)
			return 0;
	}
	return 1;
}


/*------------------------------------------------------
	CD-ROMステートデータにファイル情報を追加
------------------------------------------------------*/

static void cdrom_state_insert_list(int type, const char *fname, int bank, UINT32 offset, UINT32 length, UINT32 start, UINT32 end)
{
	CDROM_STATE *p, *last = NULL;

	if (cdrom_state_check_list(type, start, end) == 0)
	{
		ui_popup(TEXT(COULD_NOT_INSERT_CDROM_STATE_DATA));
		return;
	}

	p = cdrom_state_head[type];
	while (p)
	{
		last = p;
		p = p->next;
	}

	p = cdrom_state_free_head[type];

	if (last)
		last->next = p;
	else
		cdrom_state_head[type] = p;

	cdrom_state_free_head[type] = cdrom_state_free_head[type]->next;

	memset(p->name, 0, 16);
	strcpy(p->name, fname);
	p->prev   = last;
	p->next   = NULL;
	p->bank   = bank;
	p->offset = offset;
	p->length = length;
	p->start  = start;
	p->end    = end;

	cdrom_state_count[type]++;
}


/*------------------------------------------------------
	FIXファイルをリストに追加
------------------------------------------------------*/

static void cdrom_state_insert_fix(const char *fname, UINT32 offset, UINT32 length)
{
	UINT32 start, end;

	start = offset >> 1;
	end   = start + length - 1;

	cdrom_state_insert_list(0, fname, 0, offset, length, start, end);
}


/*------------------------------------------------------
	SPRファイルをリストに追加
------------------------------------------------------*/

static void cdrom_state_insert_spr(const char *fname, int bank, UINT32 offset, UINT32 length)
{
	UINT32 start, end;

	start = offset + (bank << 20);
	end   = start + length - 1;

	cdrom_state_insert_list(1, fname, bank, offset, length, start, end);
}

/*------------------------------------------------------
	PCMファイルをリストに追加
------------------------------------------------------*/

static void cdrom_state_insert_pcm(const char *fname, int bank, UINT32 offset, UINT32 length)
{
	UINT32 start, end;

	start = (offset >> 1) + (bank << 19);
	end   = start + length - 1;

	cdrom_state_insert_list(2, fname, bank, offset, length, start, end);
}

#endif /* SAVE_STATE */
