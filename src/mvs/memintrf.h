/******************************************************************************

	memintrf.c

	MVSメモリインタフェース関数

******************************************************************************/

#ifndef MVS_MEMORY_INTERFACE_H
#define MVS_MEMORY_INTERFACE_H

extern UINT8 *memory_region_cpu1;
extern UINT8 *memory_region_cpu2;
extern UINT8 *memory_region_gfx1;
extern UINT8 *memory_region_gfx2;
extern UINT8 *memory_region_gfx3;
extern UINT8 *memory_region_gfx4;
extern UINT8 *memory_region_sound1;
extern UINT8 *memory_region_sound2;
extern UINT8 *memory_region_user1;
#if !RELEASE
extern UINT8 *memory_region_user2;
#endif
extern UINT8 *memory_region_user3;

extern UINT32 memory_length_cpu1;
extern UINT32 memory_length_cpu2;
extern UINT32 memory_length_gfx1;
extern UINT32 memory_length_gfx2;
extern UINT32 memory_length_gfx3;
extern UINT32 memory_length_gfx4;
extern UINT32 memory_length_sound1;
extern UINT32 memory_length_sound2;
extern UINT32 memory_length_user1;
#if !RELEASE
extern UINT32 memory_length_user2;
#endif
extern UINT32 memory_length_user3;

extern UINT8  neogeo_memcard[0x800];
extern UINT8  neogeo_ram[0x10000];
extern UINT16 neogeo_sram16[0x8000];

extern int neogeo_machine_mode;
extern int disable_sound;
extern int use_parent_crom;
extern int use_parent_srom;
extern int use_parent_vrom;

#ifdef PSP_SLIM
extern UINT32 psp2k_mem_offset;
extern INT32 psp2k_mem_left;
#endif

int memory_init(void);
void memory_shutdown(void);

#if !RELEASE
void neogeo_decode_fix(UINT8 *mem, UINT32 length, UINT8 *usage);
#endif

UINT8 m68000_read_memory_8(UINT32 offset);
UINT16 m68000_read_memory_16(UINT32 offset);
void m68000_write_memory_8(UINT32 offset, UINT8 data);
void m68000_write_memory_16(UINT32 offset, UINT16 data);

UINT8 z80_read_memory_8(UINT32 address);
void z80_write_memory_8(UINT32 address, UINT8 data);

#ifdef SAVE_STATE
STATE_SAVE( memory );
STATE_LOAD( memory );
int reload_bios(void);
#endif

#endif /* MVS_MEMORY_INTERFACE_H */
