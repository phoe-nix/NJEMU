/******************************************************************************

	cache.c

	メモリキャッシュインタフェース関数

******************************************************************************/

#ifndef MEMORY_CACHE_H
#define MEMORY_CACHE_H

#if USE_CACHE

#if (EMU_SYSTEM == CPS2)
#define GFX_MEMORY			memory_region_gfx1
#define GFX_SIZE			memory_length_gfx1
#define CHECK_FNAME			"block_empty"
#define MAX_CACHE_BLOCKS	0x200
#elif (EMU_SYSTEM == MVS)
#define GFX_MEMORY			memory_region_gfx3
#define GFX_SIZE			memory_length_gfx3
#define MAX_CACHE_BLOCKS	0x400
#endif

enum
{
	CACHE_NOTFOUND = 0,
	CACHE_ZIPFILE,
	CACHE_RAWFILE
};

extern UINT32 (*read_cache)(UINT32 offset);
extern void (*update_cache)(UINT32 offset);
#if (EMU_SYSTEM == MVS)
extern int pcm_cache_enable;
#else
extern UINT8 *block_empty;
extern UINT32 block_offset[MAX_CACHE_BLOCKS];
#endif

void cache_init(void);
int cache_start(void);
void cache_shutdown(void);
void cache_sleep(int flag);

#if (EMU_SYSTEM == MVS)
UINT8 *pcm_cache_read(UINT16 new_block);
void pcm_cache_update(UINT16 block);
#endif

#ifdef STATE_SAVE
UINT8 *cache_alloc_state_buffer(INT32 size);
void cache_free_state_buffer(INT32 size);
#endif

#endif /* USE_CACHE */

#endif /* MEMORY_CACHE_H */
