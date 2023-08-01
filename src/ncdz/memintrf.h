/******************************************************************************

	memintrf.c

	Memory interace

******************************************************************************/

#ifndef NCDZ_MEMINTRF_H
#define NCDZ_MEMINTRF_H

extern uint8_t *memory_region_cpu1;
extern uint8_t *memory_region_cpu2;
extern uint8_t *memory_region_gfx1;
extern uint8_t *memory_region_gfx2;
extern uint8_t *memory_region_gfx3;
extern uint8_t *memory_region_sound1;
extern uint8_t *memory_region_user1;
extern uint8_t *memory_region_user2;

extern uint32_t memory_length_cpu1;
extern uint32_t memory_length_cpu2;
extern uint32_t memory_length_gfx1;
extern uint32_t memory_length_gfx2;
extern uint32_t memory_length_gfx3;
extern uint32_t memory_length_sound1;
extern uint32_t memory_length_user1;
extern uint32_t memory_length_user2;

extern uint8_t neogeo_memcard[0x2000];

int memory_init(void);
void memory_shutdown(void);

uint8_t  m68000_read_memory_8(uint32_t address);
uint16_t m68000_read_memory_16(uint32_t address);
uint32_t m68000_read_memory_32(uint32_t address);
void m68000_write_memory_8(uint32_t address, uint8_t value);
void m68000_write_memory_16(uint32_t address, uint16_t value);
void m68000_write_memory_32(uint32_t address, uint32_t value);

uint8_t z80_read_memory_8(uint32_t address);
void z80_write_memory_8(uint32_t address, uint8_t data);

STATE_SAVE( memory );
STATE_LOAD( memory );

#endif /* NCDZ_MEMINTRF_H */
