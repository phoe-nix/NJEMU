/******************************************************************************

	memintrf.c

	CPS2メモリインタフェース関数

******************************************************************************/

#ifndef CPS2_MEMORY_INTERFACE_H
#define CPS2_MEMORY_INTERFACE_H

extern uint8_t *memory_region_cpu1;
extern uint8_t *memory_region_cpu2;
extern uint8_t *memory_region_gfx1;
extern uint8_t *memory_region_sound1;
extern uint8_t *memory_region_user1;

extern uint32_t memory_length_cpu1;
extern uint32_t memory_length_cpu2;
extern uint32_t memory_length_gfx1;
extern uint32_t memory_length_sound1;
extern uint32_t memory_length_user1;

extern uint32_t gfx_total_elements[3];
extern uint8_t *gfx_pen_usage[3];

extern uint8_t cps1_ram[0x10000];
extern uint8_t cps2_ram[0x4000 + 2];
extern uint16_t cps1_gfxram[0x30000 >> 1];
extern uint16_t cps1_output[0x100 >> 1];

extern int cps2_objram_bank;
extern uint16_t cps2_objram[2][0x2000 >> 1];
extern uint16_t cps2_output[0x10 >> 1];

extern uint8_t *qsound_sharedram1;
extern uint8_t *qsound_sharedram2;

#ifdef LARGE_MEMORY
extern uint32_t psp2k_mem_offset;
extern int32_t psp2k_mem_left;
#endif

int memory_init(void);
void memory_shutdown(void);

uint8_t m68000_read_memory_8(uint32_t offset);
uint16_t m68000_read_memory_16(uint32_t offset);
void m68000_write_memory_8(uint32_t offset, uint8_t data);
void m68000_write_memory_16(uint32_t offset, uint16_t data);

uint8_t z80_read_memory_8(uint32_t offset);
void z80_write_memory_8(uint32_t offset, uint8_t data);

#ifdef SAVE_STATE
STATE_SAVE( memory );
STATE_LOAD( memory );
#endif

#endif /* CPS2_MEMORY_INTERFACE_H */
