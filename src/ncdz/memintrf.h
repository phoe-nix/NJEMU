/******************************************************************************

	memintrf.c

	Memory interace

******************************************************************************/

#ifndef NCDZ_MEMINTRF_H
#define NCDZ_MEMINTRF_H

extern UINT8 *memory_region_cpu1;
extern UINT8 *memory_region_cpu2;
extern UINT8 *memory_region_gfx1;
extern UINT8 *memory_region_gfx2;
extern UINT8 *memory_region_gfx3;
extern UINT8 *memory_region_sound1;
extern UINT8 *memory_region_user1;
extern UINT8 *memory_region_user2;

extern UINT32 memory_length_cpu1;
extern UINT32 memory_length_cpu2;
extern UINT32 memory_length_gfx1;
extern UINT32 memory_length_gfx2;
extern UINT32 memory_length_gfx3;
extern UINT32 memory_length_sound1;
extern UINT32 memory_length_user1;
extern UINT32 memory_length_user2;

extern UINT8 neogeo_memcard[0x2000];

int memory_init(void);
void memory_shutdown(void);

UINT8  m68000_read_memory_8(UINT32 address);
UINT16 m68000_read_memory_16(UINT32 address);
UINT32 m68000_read_memory_32(UINT32 address);
void m68000_write_memory_8(UINT32 address, UINT8 value);
void m68000_write_memory_16(UINT32 address, UINT16 value);
void m68000_write_memory_32(UINT32 address, UINT32 value);

UINT8 z80_read_memory_8(UINT32 address);
void z80_write_memory_8(UINT32 address, UINT8 data);

STATE_SAVE( memory );
STATE_LOAD( memory );

#endif /* NCDZ_MEMINTRF_H */
