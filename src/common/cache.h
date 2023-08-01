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
#define MAX_CACHE_BLOCKS	0x800	//支持扩容CROM，原始0x400
#endif

enum
{
	CACHE_NOTFOUND = 0,
	CACHE_ZIPFILE,
	CACHE_RAWFILE
};

extern uint32_t (*read_cache)(uint32_t offset);
extern void (*update_cache)(uint32_t offset);
#if (EMU_SYSTEM == MVS)
extern int pcm_cache_enable;
#else
extern uint8_t *block_empty;
extern uint32_t block_offset[MAX_CACHE_BLOCKS];
#endif

void cache_init(void);
int cache_start(void);
void cache_shutdown(void);
void cache_sleep(int flag);

#if (EMU_SYSTEM == MVS)
uint8_t *pcm_cache_read(uint16_t new_block);
void pcm_cache_update(uint16_t block);
#endif

#ifdef STATE_SAVE
uint8_t *cache_alloc_state_buffer(int32_t size);
void cache_free_state_buffer(int32_t size);
#endif

#endif /* USE_CACHE */

#endif /* MEMORY_CACHE_H */
