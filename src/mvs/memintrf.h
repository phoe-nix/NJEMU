/******************************************************************************

	memintrf.c

	MVSメモリインタフェース関数

******************************************************************************/

#ifndef MVS_MEMORY_INTERFACE_H
#define MVS_MEMORY_INTERFACE_H

extern uint8_t *memory_region_cpu1;
extern uint8_t *memory_region_cpu2;
extern uint8_t *memory_region_gfx1;
extern uint8_t *memory_region_gfx2;
extern uint8_t *memory_region_gfx3;
extern uint8_t *memory_region_gfx4;
extern uint8_t *memory_region_sound1;
extern uint8_t *memory_region_sound2;
extern uint8_t *memory_region_user1;
#if !RELEASE
extern uint8_t *memory_region_user2;
#endif
extern uint8_t *memory_region_user3;

extern uint32_t memory_length_cpu1;
extern uint32_t memory_length_cpu2;
extern uint32_t memory_length_gfx1;
extern uint32_t memory_length_gfx2;
extern uint32_t memory_length_gfx3;
extern uint32_t memory_length_gfx4;
extern uint32_t memory_length_sound1;
extern uint32_t memory_length_sound2;
extern uint32_t memory_length_user1;
#if !RELEASE
extern uint32_t memory_length_user2;
#endif
extern uint32_t memory_length_user3;

extern uint8_t  neogeo_memcard[0x800];
extern uint8_t  neogeo_ram[0x10000];
extern uint16_t neogeo_sram16[0x8000];

extern int neogeo_machine_mode;
extern int disable_sound;
extern int use_parent_crom;
extern int use_parent_srom;
extern int use_parent_vrom;

#ifdef LARGE_MEMORY
extern uint32_t psp2k_mem_offset;
extern int32_t psp2k_mem_left;
#endif

int memory_init(void);
void memory_shutdown(void);

#if !RELEASE
void neogeo_decode_fix(uint8_t *mem, uint32_t length, uint8_t *usage);
#endif

uint8_t m68000_read_memory_8(uint32_t offset);
uint16_t m68000_read_memory_16(uint32_t offset);
void m68000_write_memory_8(uint32_t offset, uint8_t data);
void m68000_write_memory_16(uint32_t offset, uint16_t data);

uint8_t z80_read_memory_8(uint32_t address);
void z80_write_memory_8(uint32_t address, uint8_t data);

#ifdef SAVE_STATE
STATE_SAVE( memory );
STATE_LOAD( memory );
int reload_bios(void);
#endif

#endif /* MVS_MEMORY_INTERFACE_H */
