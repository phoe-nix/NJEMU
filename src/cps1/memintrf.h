/******************************************************************************

	memintrf.c

	CPS1メモリインタフェース関数

******************************************************************************/

#ifndef CPS1_MEMORY_INTERFACE_H
#define CPS1_MEMORY_INTERFACE_H

extern UINT8 *memory_region_cpu1;
extern UINT8 *memory_region_cpu2;
extern UINT8 *memory_region_gfx1;
extern UINT8 *memory_region_sound1;
extern UINT8 *memory_region_user1;
extern UINT8 *memory_region_user2;

extern UINT32 memory_length_cpu1;
extern UINT32 memory_length_cpu2;
extern UINT32 memory_length_gfx1;
extern UINT32 memory_length_sound1;
extern UINT32 memory_length_user1;
extern UINT32 memory_length_user2;

extern UINT8 cps1_ram[0x10000];
extern UINT16 cps1_gfxram[0x30000 >> 1];
extern UINT16 cps1_output[0x100 >> 1];

extern UINT8 *qsound_sharedram1;
extern UINT8 *qsound_sharedram2;

int memory_init(void);
void memory_shutdown(void);

UINT8 m68000_read_memory_8(UINT32 offset);
UINT16 m68000_read_memory_16(UINT32 offset);
void m68000_write_memory_8(UINT32 offset, UINT8 data);
void m68000_write_memory_16(UINT32 offset, UINT16 data);

extern UINT8 (*z80_read_memory_8)(UINT32 offset);
extern void (*z80_write_memory_8)(UINT32 offset, UINT8 data);

#ifdef SAVE_STATE
STATE_SAVE( memory );
STATE_LOAD( memory );
#endif

#endif /* CPS1_MEMORY_INTERFACE_H */
