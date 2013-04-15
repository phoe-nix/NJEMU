/******************************************************************************

	memintrf.c

	CPS2メモリインタフェース関数

******************************************************************************/

#ifndef CPS2_MEMORY_INTERFACE_H
#define CPS2_MEMORY_INTERFACE_H

extern UINT8 *memory_region_cpu1;
extern UINT8 *memory_region_cpu2;
extern UINT8 *memory_region_gfx1;
extern UINT8 *memory_region_sound1;
extern UINT8 *memory_region_user1;

extern UINT32 memory_length_cpu1;
extern UINT32 memory_length_cpu2;
extern UINT32 memory_length_gfx1;
extern UINT32 memory_length_sound1;
extern UINT32 memory_length_user1;

extern UINT32 gfx_total_elements[3];
extern UINT8 *gfx_pen_usage[3];

extern UINT8 cps1_ram[0x10000];
extern UINT8 cps2_ram[0x4000 + 2];
extern UINT16 cps1_gfxram[0x30000 >> 1];
extern UINT16 cps1_output[0x100 >> 1];

extern int cps2_objram_bank;
extern UINT16 cps2_objram[2][0x2000 >> 1];
extern UINT16 cps2_output[0x10 >> 1];

extern UINT8 *qsound_sharedram1;
extern UINT8 *qsound_sharedram2;

#ifdef PSP_SLIM
extern UINT32 psp2k_mem_offset;
extern INT32 psp2k_mem_left;
#endif

int memory_init(void);
void memory_shutdown(void);

UINT8 m68000_read_memory_8(UINT32 offset);
UINT16 m68000_read_memory_16(UINT32 offset);
void m68000_write_memory_8(UINT32 offset, UINT8 data);
void m68000_write_memory_16(UINT32 offset, UINT16 data);

UINT8 z80_read_memory_8(UINT32 offset);
void z80_write_memory_8(UINT32 offset, UINT8 data);

#ifdef SAVE_STATE
STATE_SAVE( memory );
STATE_LOAD( memory );
#endif

#endif /* CPS2_MEMORY_INTERFACE_H */
