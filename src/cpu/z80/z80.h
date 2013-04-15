/******************************************************************************

	z80.c

	Z80 CPUインタフェース関数

******************************************************************************/

#ifndef Z80_H
#define Z80_H

#include "cz80.h"

// MAME互換のレジスタ番号 (一部未対応)
enum
{
	Z80_PC=1, Z80_SP, Z80_AF, Z80_BC, Z80_DE, Z80_HL,
	Z80_IX, Z80_IY,	Z80_AF2, Z80_BC2, Z80_DE2, Z80_HL2,
	Z80_R, Z80_I, Z80_IM, Z80_IFF1, Z80_IFF2, Z80_HALT,
	Z80_NMI_STATE, Z80_IRQ_STATE, Z80_DC0, Z80_DC1, Z80_DC2, Z80_DC3
};

void z80_init(void);
void z80_reset(void);
void z80_exit(void);
int  z80_execute(int cycles);
void z80_set_irq_line(int irqline, int state);
void z80_set_irq_callback(int (*callback)(int line));
UINT32  z80_get_reg(int regnum);
void z80_set_reg(int regnum, UINT32 val);

#ifdef SAVE_STATE
STATE_SAVE( z80 );
STATE_LOAD( z80 );
#endif

#endif /* Z80_H */
