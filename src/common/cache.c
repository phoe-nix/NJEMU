/******************************************************************************

	cache.c

	メモリキャッシュインタフェース関数

******************************************************************************/

#include "emumain.h"

#if USE_CACHE

#define MIN_CACHE_SIZE		0x40		// 下限  4MB
#ifdef PSP_SLIM
#define MAX_CACHE_SIZE		0x200		// 上限 32MB
#else
#define MAX_CACHE_SIZE		0x140		// 上限 20MB
#endif
#define CACHE_SAFETY		0x20000		// キャッシュ確保後の空きメモリサイズ
#define BLOCK_SIZE			0x10000		// 1ブロックのサイズ = 64KB
#define BLOCK_MASK			0xffff
#define BLOCK_SHIFT			16
#define BLOCK_NOT_CACHED	0xffff
#define BLOCK_EMPTY			0xffffffff

#if (EMU_SYSTEM == MVS)
#define MAX_PCM_BLOCKS		0x100
#define MAX_PCM_SIZE		0x30
#endif


/******************************************************************************
	グローバル構造体/変数
******************************************************************************/

UINT32 (*read_cache)(UINT32 offset);
void (*update_cache)(UINT32 offset);
#if (EMU_SYSTEM != MVS)
UINT32 block_offset[MAX_CACHE_BLOCKS];
UINT8  *block_empty = (UINT8 *)block_offset;
#endif


/******************************************************************************
	ローカル構造体/変数
******************************************************************************/

typedef struct cache_s
{
	int idx;
	int block;
	UINT32 frame;
	struct cache_s *prev;
	struct cache_s *next;
} cache_t;


static cache_t ALIGN_DATA cache_data[MAX_CACHE_SIZE];
static cache_t *head;
static cache_t *tail;

static int num_cache;
static UINT16 ALIGN_DATA blocks[MAX_CACHE_BLOCKS];
static int cache_fd;

#if (EMU_SYSTEM == MVS)
#ifndef PSP_SLIM
int pcm_cache_enable;

static cache_t ALIGN_DATA pcm_data[MAX_PCM_SIZE];
static cache_t *pcm_head;
static cache_t *pcm_tail;

static UINT16 ALIGN_DATA pcm_blocks[MAX_PCM_BLOCKS];
static SceUID pcm_fd;
#endif
#else
static int cache_type;
static char spr_cache_name[MAX_PATH];
#endif


/******************************************************************************
	ローカル関数
******************************************************************************/

#if (EMU_SYSTEM == MVS)
#ifndef PSP_SLIM

/*------------------------------------------------------
	PCMキャッシュを読み込む
------------------------------------------------------*/

UINT8 *pcm_cache_read(UINT16 new_block)
{
	UINT32 idx = pcm_blocks[new_block];
	cache_t *p;

	if (idx == BLOCK_NOT_CACHED)
	{
		p = pcm_head;
		pcm_blocks[p->block] = BLOCK_NOT_CACHED;

		p->block = new_block;
		pcm_blocks[new_block] = p->idx;

		sceIoLseek(pcm_fd, new_block << BLOCK_SHIFT, PSP_SEEK_SET);
		sceIoRead(pcm_fd, &memory_region_sound1[p->idx << BLOCK_SHIFT], BLOCK_SIZE);
	}
	else p = &pcm_data[idx];

	if (p->frame != frames_displayed)
	{
		p->frame = frames_displayed;

		if (p->next)
		{
			if (p->prev)
			{
				p->prev->next = p->next;
				p->next->prev = p->prev;
			}
			else
			{
				pcm_head = p->next;
				pcm_head->prev = NULL;
			}

			p->prev = pcm_tail;
			p->next = NULL;

			pcm_tail->next = p;
			pcm_tail = p;
		}
	}

	return &memory_region_sound1[p->idx << BLOCK_SHIFT];
}

#endif
#else

/*------------------------------------------------------
	ZIPキャッシュファイル内のデータファイルを開く
------------------------------------------------------*/

static int zip_cache_open(int number)
{
	static const char cnv_table[16] =
	{
		'0','1','2','3','4','5','6','7',
		'8','9','a','b','c','d','e','f'
	};
	char fname[4];

	fname[0] = cnv_table[(number >> 8) & 0x0f];
	fname[1] = cnv_table[(number >> 4) & 0x0f];
	fname[2] = cnv_table[ number       & 0x0f];
	fname[3] = '\0';

	cache_fd = zopen(fname);

	return cache_fd != -1;
}


/*------------------------------------------------------
	ZIPキャッシュファイル内のデータファイル読み込み
------------------------------------------------------*/

#define zip_cache_load(offs)							\
	zread(cache_fd, &GFX_MEMORY[offs << 16], 0x10000);	\
	zclose(cache_fd);									\
	cache_fd = -1;


#endif

/*------------------------------------------------------
	キャッシュをデータで埋める

	3種類のデータが混在しているため、領域を区切って
	それぞれ適当なサイズを読み込むべきですが、手を
	抜いて先頭から読み込んでいるだけになっています。
------------------------------------------------------*/

static int fill_cache(void)
{
	int i, block;
	cache_t *p;

	i = 0;
	block = 0;

#if (EMU_SYSTEM == MVS)
	while (i < num_cache)
	{
		p = head;
		p->block = block;
		blocks[block] = p->idx;

		sceIoLseek(cache_fd, block << BLOCK_SHIFT, PSP_SEEK_SET);
		sceIoRead(cache_fd, &GFX_MEMORY[p->idx << BLOCK_SHIFT], BLOCK_SIZE);

		head = p->next;
		head->prev = NULL;

		p->prev = tail;
		p->next = NULL;

		tail->next = p;
		tail = p;
		i++;

		if (++block >= MAX_CACHE_BLOCKS)
			break;
	}
#ifndef PSP_SLIM
	if (pcm_cache_enable)
	{
		i = 0;
		block = 0;

		while (i < MAX_PCM_SIZE)
		{
			p = pcm_head;
			p->block = block;
			pcm_blocks[block] = p->idx;

			sceIoLseek(pcm_fd, block << BLOCK_SHIFT, PSP_SEEK_SET);
			sceIoRead(pcm_fd, &memory_region_sound1[p->idx << BLOCK_SHIFT], BLOCK_SIZE);

			pcm_head = p->next;
			pcm_head->prev = NULL;

			p->prev = pcm_tail;
			p->next = NULL;

			pcm_tail->next = p;
			pcm_tail = p;
			i++;
		}
	}
#endif
#else
	if (cache_type == CACHE_RAWFILE)
	{
		while (i < num_cache)
		{
			if (block_offset[block] != BLOCK_EMPTY)
			{
				p = head;
				p->block = block;
				blocks[block] = p->idx;

				sceIoLseek(cache_fd, block_offset[block], PSP_SEEK_SET);
				sceIoRead(cache_fd, &GFX_MEMORY[p->idx << BLOCK_SHIFT], BLOCK_SIZE);

				head = p->next;
				head->prev = NULL;

				p->prev = tail;
				p->next = NULL;

				tail->next = p;
				tail = p;
				i++;
			}

			if (++block >= MAX_CACHE_BLOCKS)
				break;
		}
	}
	else
	{
		while (i < num_cache)
		{
			if (!block_empty[block])
			{
				p = head;
				p->block = block;
				blocks[block] = p->idx;

				if (!zip_cache_open(p->block))
				{
					msg_printf(TEXT(COULD_NOT_OPEN_SPRITE_BLOCK_x), p->block);
					return 0;
				}
				zip_cache_load(p->idx);

				head = p->next;
				head->prev = NULL;

				p->prev = tail;
				p->next = NULL;

				tail->next = p;
				tail = p;
				i++;
			}

			if (++block >= MAX_CACHE_BLOCKS)
				break;
		}
	}
#endif

	return 1;
}


/*------------------------------------------------------
	キャッシュを使用しない

	データが全てメモリに格納されている場合
------------------------------------------------------*/

#if (EMU_SYSTEM == MVS)
static UINT32 read_cache_disable(UINT32 offset)
{
	return offset;
}
#endif


/*------------------------------------------------------
	アドレス変換のみ行う

	空き領域を削除した状態で、全てメモリに格納されて
	いる場合
------------------------------------------------------*/

#if (EMU_SYSTEM == CPS2)
static UINT32 read_cache_static(UINT32 offset)
{
	int idx = blocks[offset >> BLOCK_SHIFT];

	return ((idx << BLOCK_SHIFT) | (offset & BLOCK_MASK));
}
#endif


/*------------------------------------------------------
	無圧縮キャッシュを使用

	無圧縮のキャッシュファイルからデータを読み込む
------------------------------------------------------*/

static UINT32 read_cache_rawfile(UINT32 offset)
{
	INT16 new_block = offset >> BLOCK_SHIFT;
	UINT32 idx = blocks[new_block];
	cache_t *p;

	if (idx == BLOCK_NOT_CACHED)
	{
		p = head;
		blocks[p->block] = BLOCK_NOT_CACHED;

		p->block = new_block;
		blocks[new_block] = p->idx;

#if (EMU_SYSTEM == MVS)
		sceIoLseek(cache_fd, new_block << BLOCK_SHIFT, PSP_SEEK_SET);
#else
		sceIoLseek(cache_fd, block_offset[new_block], PSP_SEEK_SET);
#endif
		sceIoRead(cache_fd, &GFX_MEMORY[p->idx << BLOCK_SHIFT], BLOCK_SIZE);
	}
	else p = &cache_data[idx];

	if (p->next)
	{
		if (p->prev)
		{
			p->prev->next = p->next;
			p->next->prev = p->prev;
		}
		else
		{
			head = p->next;
			head->prev = NULL;
		}

		p->prev = tail;
		p->next = NULL;

		tail->next = p;
		tail = p;
	}

	return ((tail->idx << BLOCK_SHIFT) | (offset & BLOCK_MASK));
}


/*------------------------------------------------------
	ZIP圧縮キャッシュを使用

	ZIP圧縮のキャッシュファイルからデータを読み込む
------------------------------------------------------*/

#if (EMU_SYSTEM == CPS2)
static UINT32 read_cache_zipfile(UINT32 offset)
{
	INT16 new_block = offset >> BLOCK_SHIFT;
	UINT32 idx = blocks[new_block];
	cache_t *p;

	if (idx == BLOCK_NOT_CACHED)
	{
		if (!zip_cache_open(new_block))
			return 0;

		p = head;
		blocks[p->block] = BLOCK_NOT_CACHED;

		p->block = new_block;
		blocks[new_block] = p->idx;

		zip_cache_load(p->idx);
	}
	else p = &cache_data[idx];

	if (p->next)
	{
		if (p->prev)
		{
			p->prev->next = p->next;
			p->next->prev = p->prev;
		}
		else
		{
			head = p->next;
			head->prev = NULL;
		}

		p->prev = tail;
		p->next = NULL;

		tail->next = p;
		tail = p;
	}

	return ((tail->idx << BLOCK_SHIFT) | (offset & BLOCK_MASK));
}
#endif


/*------------------------------------------------------
	キャッシュのデータを更新する

	キャッシュを使用しない、または全て読み込んだ場合。
------------------------------------------------------*/

static void update_cache_disable(UINT32 offset)
{
}


/*------------------------------------------------------
	キャッシュのデータを更新する

	指定されたデータをキャッシュの最後尾に回します。
	キャッシュを管理しない場合は不要。
------------------------------------------------------*/

static void update_cache_dynamic(UINT32 offset)
{
	INT16 new_block = offset >> BLOCK_SHIFT;
	int idx = blocks[new_block];

	if (idx != BLOCK_NOT_CACHED)
	{
		cache_t *p = &cache_data[idx];

		if (p->next)
		{
			if (p->prev)
			{
				p->prev->next = p->next;
				p->next->prev = p->prev;
			}
			else
			{
				head = p->next;
				head->prev = NULL;
			}

			p->prev = tail;
			p->next = NULL;

			tail->next = p;
			tail = p;
		}
	}
}


/******************************************************************************
	キャッシュインタフェース関数
******************************************************************************/

/*------------------------------------------------------
	キャッシュを初期化する
------------------------------------------------------*/

void cache_init(void)
{
	int i;

	num_cache = 0;
	cache_fd = -1;

#if (EMU_SYSTEM == MVS)
	read_cache = read_cache_disable;
#else
	cache_type = CACHE_NOTFOUND;
	read_cache = read_cache_static;
#endif
	update_cache = update_cache_disable;

	for (i = 0; i < MAX_CACHE_BLOCKS; i++)
		blocks[i] = BLOCK_NOT_CACHED;

#if (EMU_SYSTEM == MVS)
#ifndef PSP_SLIM
	pcm_cache_enable = 0;
	pcm_fd = -1;

	for (i = 0; i < MAX_PCM_BLOCKS; i++)
		pcm_blocks[i] = BLOCK_NOT_CACHED;
#endif
#endif
}


/*------------------------------------------------------
	キャッシュ処理開始
------------------------------------------------------*/

int cache_start(void)
{
	int i, found;
	UINT32 size = 0;
	char version_str[8];
#if (EMU_SYSTEM == MVS)
	SceUID fd;
#endif

	zip_close();

#if (EMU_SYSTEM == MVS)

	msg_printf(TEXT(LOADING_CACHE_INFORMATION_DATA));

	found = 0;
	if ((fd = cachefile_open(CACHE_INFO)) >= 0)
	{
		sceIoRead(fd, version_str, 8);

		if (strcmp(version_str, "MVS_" CACHE_VERSION) == 0)
		{
			sceIoRead(fd, gfx_pen_usage[2], memory_length_gfx3 / 128);
			found = 1;
		}
		sceIoClose(fd);

		if (!found)
		{
			msg_printf(TEXT(UNSUPPORTED_VERSION_OF_CACHE_FILE), version_str[5], version_str[6]);
			msg_printf(TEXT(CURRENT_REQUIRED_VERSION_IS_x));
			msg_printf(TEXT(PLEASE_REBUILD_CACHE_FILE));
			return 0;
		}
	}
	else
	{
		msg_printf(TEXT(COULD_NOT_OPEN_CACHE_FILE));
		return 0;
	}

#ifndef PSP_SLIM
	if (option_sound_enable && disable_sound)
	{
		if ((pcm_fd = cachefile_open(CACHE_VROM)) >= 0)
		{
			if ((memory_region_sound1 = memalign(MEM_ALIGN, MAX_PCM_SIZE * BLOCK_SIZE)) != NULL)
			{
				pcm_cache_enable = 1;
				disable_sound = 0;
				msg_printf(TEXT(PCM_CACHE_ENABLED));
			}
		}
		if (!pcm_cache_enable)
		{
			if (pcm_fd >= 0)
			{
				sceIoClose(pcm_fd);
				pcm_fd = -1;
			}
			memory_length_sound1 = 0;
		}
	}
#endif

	if ((cache_fd = cachefile_open(CACHE_CROM)) < 0)
	{
		msg_printf(TEXT(COULD_NOT_OPEN_CACHE_FILE));
		return 0;
	}

#elif (EMU_SYSTEM == CPS2)
	found = 1;
	cache_type = CACHE_RAWFILE;

	sprintf(spr_cache_name, "%s/%s.cache", cache_dir, game_name);
	if ((cache_fd = sceIoOpen(spr_cache_name, PSP_O_RDONLY, 0777)) < 0)
	{
		sprintf(spr_cache_name, "%s/%s.cache", cache_dir, cache_parent_name);
		if ((cache_fd = sceIoOpen(spr_cache_name, PSP_O_RDONLY, 0777)) < 0)
		{
			found = 0;
		}
	}
	if (!found)
	{
		found = 1;
		cache_type = CACHE_ZIPFILE;

		sprintf(spr_cache_name, "%s/%s_cache.zip", cache_dir, game_name);
		if (zip_open(spr_cache_name) == -1)
		{
			sprintf(spr_cache_name, "%s/%s_cache.zip", cache_dir, cache_parent_name);
			if (zip_open(spr_cache_name) == -1)
			{
				found = 0;
				zip_close();
			}
		}
	}
	if (!found)
	{
		cache_type = CACHE_NOTFOUND;
		msg_printf(TEXT(COULD_NOT_OPEN_CACHE_FILE));
		return 0;
	}

	msg_printf(TEXT(LOADING_CACHE_INFORMATION_DATA));

	if (cache_type == CACHE_RAWFILE)
	{
		sceIoRead(cache_fd, version_str, 8);

		if (strcmp(version_str, "CPS2" CACHE_VERSION) == 0)
		{
			sceIoRead(cache_fd, gfx_pen_usage[TILE08], gfx_total_elements[TILE08]);
			sceIoRead(cache_fd, gfx_pen_usage[TILE16], gfx_total_elements[TILE16]);
			sceIoRead(cache_fd, gfx_pen_usage[TILE32], gfx_total_elements[TILE32]);
			sceIoRead(cache_fd, block_offset, MAX_CACHE_BLOCKS * sizeof(UINT32));
		}
		else
		{
			sceIoClose(cache_fd);
			found = 0;
		}
	}
	else
	{
		if ((cache_fd = zopen("cache_info")) != -1)
		{
			zread(cache_fd, version_str, 8);

			if (strcmp(version_str, "CPS2" CACHE_VERSION) == 0)
			{
				zread(cache_fd, gfx_pen_usage[TILE08], gfx_total_elements[TILE08]);
				zread(cache_fd, gfx_pen_usage[TILE16], gfx_total_elements[TILE16]);
				zread(cache_fd, gfx_pen_usage[TILE32], gfx_total_elements[TILE32]);
				zread(cache_fd, block_empty, MAX_CACHE_BLOCKS);
				zclose(cache_fd);
			}
			else
			{
				zclose(cache_fd);
				found = 0;
			}
		}
		else
		{
			found = 0;
		}
		if (!found) zip_close();
	}
	if (!found)
	{
		msg_printf(TEXT(UNSUPPORTED_VERSION_OF_CACHE_FILE), version_str[5], version_str[6]);
		msg_printf(TEXT(CURRENT_REQUIRED_VERSION_IS_x));
		msg_printf(TEXT(PLEASE_REBUILD_CACHE_FILE));
		return 0;
	}

	if ((GFX_MEMORY = (UINT8 *)memalign(MEM_ALIGN, GFX_SIZE + CACHE_SAFETY)) != NULL)
	{
		free(GFX_MEMORY);
		GFX_MEMORY = (UINT8 *)memalign(MEM_ALIGN, GFX_SIZE);
		memset(GFX_MEMORY, 0, GFX_SIZE);

		num_cache = GFX_SIZE >> 16;
	}
	else
#endif
	{
#if (EMU_SYSTEM == MVS)
		read_cache = read_cache_rawfile;
#else
		if (cache_type == CACHE_RAWFILE)
			read_cache = read_cache_rawfile;
		else
			read_cache = read_cache_zipfile;
#endif
		update_cache = update_cache_dynamic;

		// 確保可能なサイズをチェック
#ifdef PSP_SLIM
		if (psp2k_mem_left == PSP2K_MEM_SIZE)
		{
			GFX_MEMORY = (UINT8 *)PSP2K_MEM_TOP;
			i = MAX_CACHE_SIZE;
			size = i << BLOCK_SHIFT;
		}
		else
#endif
		{
			for (i = GFX_SIZE >> BLOCK_SHIFT; i >= MIN_CACHE_SIZE; i--)
			{
				if ((GFX_MEMORY = (UINT8 *)memalign(MEM_ALIGN, (i << BLOCK_SHIFT) + CACHE_SAFETY)) != NULL)
				{
					size = i << BLOCK_SHIFT;
					free(GFX_MEMORY);
					GFX_MEMORY = NULL;
					break;
				}
			}

			if (i < MIN_CACHE_SIZE)
			{
				msg_printf(TEXT(MEMORY_NOT_ENOUGH));
				return 0;
			}

			if ((GFX_MEMORY = (UINT8 *)memalign(MEM_ALIGN, size)) == NULL)
			{
				msg_printf(TEXT(COULD_NOT_ALLOCATE_CACHE_MEMORY));
				return 0;
			}
		}

		memset(GFX_MEMORY, 0, size);

		num_cache = i;
	}

	msg_printf(TEXT(xKB_CACHE_ALLOCATED), (num_cache << BLOCK_SHIFT) / 1024);

	for (i = 0; i < num_cache; i++)
		cache_data[i].idx = i;

	for (i = 1; i < num_cache; i++)
		cache_data[i].prev = &cache_data[i - 1];

	for (i = 0; i < num_cache - 1; i++)
		cache_data[i].next = &cache_data[i + 1];

	cache_data[0].prev = NULL;
	cache_data[num_cache - 1].next = NULL;

	head = &cache_data[0];
	tail = &cache_data[num_cache - 1];

#if (EMU_SYSTEM == MVS)
#ifndef PSP_SLIM
	for (i = 0; i < MAX_PCM_SIZE; i++)
		pcm_data[i].idx = i;

	for (i = 1; i < MAX_PCM_SIZE; i++)
		pcm_data[i].prev = &pcm_data[i - 1];

	for (i = 0; i < MAX_PCM_SIZE - 1; i++)
		pcm_data[i].next = &pcm_data[i + 1];

	pcm_data[0].prev = NULL;
	pcm_data[MAX_PCM_SIZE - 1].next = NULL;

	pcm_head = &pcm_data[0];
	pcm_tail = &pcm_data[MAX_PCM_SIZE - 1];
#endif
#endif

	if (!fill_cache())
	{
		msg_printf(TEXT(CACHE_LOAD_ERROR));
		pad_wait_press(PAD_WAIT_INFINITY);
		Loop = LOOP_BROWSER;
		return 0;
	}

	if (size == 0) cache_shutdown();

	return 1;
}


/*------------------------------------------------------
	キャッシュ処理終了
------------------------------------------------------*/

void cache_shutdown(void)
{
#if (EMU_SYSTEM == MVS)
#ifndef PSP_SLIM
	if (pcm_cache_enable)
	{
		if (pcm_fd != -1)
		{
			sceIoClose(pcm_fd);
		}
		pcm_cache_enable = 0;
	}
#endif
	if (cache_fd != -1)
	{
		sceIoClose(cache_fd);
		cache_fd = -1;
	}
#else
	if (cache_type == CACHE_RAWFILE)
	{
		if (cache_fd != -1)
		{
			sceIoClose(cache_fd);
			cache_fd = -1;
		}
	}
	else
	{
		zip_close();
	}
#endif
	num_cache = 0;
}


/*------------------------------------------------------
	キャッシュを一時的に停止/再開する
------------------------------------------------------*/

void cache_sleep(int flag)
{
	if (num_cache)
	{
		if (flag)
		{
#if (EMU_SYSTEM == MVS)
			sceIoClose(cache_fd);
#ifndef PSP_SLIM
			if (pcm_cache_enable) sceIoClose(pcm_fd);
#endif
#else
			if (cache_type == CACHE_RAWFILE)
				sceIoClose(cache_fd);
			else
				zip_close();
#endif
		}
		else
		{
#if (EMU_SYSTEM == MVS)
			cache_fd = cachefile_open(CACHE_CROM);
#ifndef PSP_SLIM
			if (pcm_cache_enable)
				pcm_fd = cachefile_open(CACHE_VROM);
#endif
#else
			if (cache_type == CACHE_RAWFILE)
				cache_fd = sceIoOpen(spr_cache_name, PSP_O_RDONLY, 0777);
			else
				zip_open(spr_cache_name);
#endif
		}
	}
}


#ifdef STATE_SAVE

/*------------------------------------------------------
	ステートセーブ領域を一時的に確保する
------------------------------------------------------*/

#ifdef PSP_SLIM
static int cache_alloc_type = 0;
#endif

UINT8 *cache_alloc_state_buffer(INT32 size)
{
#ifdef PSP_SLIM
	if (size < psp2k_mem_left)
	{
		cache_alloc_type = 1;
		return (UINT8 *)psp2k_mem_offset;
	}
	else
#endif
	{
		SceUID fd;
		char path[MAX_PATH];

#ifdef PSP_SLIM
		cache_alloc_type = 0;
#endif

		sprintf(path, "%sstate/cache.tmp", launchDir);

		if ((fd = sceIoOpen(path, PSP_O_WRONLY|PSP_O_CREAT, 0777)) >= 0)
		{
			sceIoWrite(fd, GFX_MEMORY, size);
			sceIoClose(fd);
			return GFX_MEMORY;
		}
		return NULL;
	}
}

/*------------------------------------------------------
	セーブ領域を解放し、退避したキャッシュを戻す
------------------------------------------------------*/

void cache_free_state_buffer(INT32 size)
{
#ifdef PSP_SLIM
	if (!cache_alloc_type)
#endif
	{
		SceUID fd;
		char path[MAX_PATH];

		sprintf(path, "%sstate/cache.tmp", launchDir);

		if ((fd = sceIoOpen(path, PSP_O_RDONLY, 0777)) >= 0)
		{
			sceIoRead(fd, GFX_MEMORY, size);
			sceIoClose(fd);
		}
		sceIoRemove(path);
	}

#ifdef PSP_SLIM
	cache_alloc_type = 0;
#endif
}

#endif /* STATE_SAVE */

#endif /* USE_CACHE */
