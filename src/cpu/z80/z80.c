/******************************************************************************

	z80.c

	Z80 CPUインタフェース関数

******************************************************************************/

#include "emumain.h"


/******************************************************************************
	Z80インタフェース関数
******************************************************************************/

/*--------------------------------------------------------
	CPU初期化
--------------------------------------------------------*/

void z80_init(void)
{
	Cz80_Init(&CZ80);
#if (EMU_SYSTEM == CPS1)
	Cz80_Set_Fetch(&CZ80, 0x0000, 0xffff, (UINT32)memory_region_cpu2);
	if (memory_region_user2)
	{
		Cz80_Set_Encrypt_Range(&CZ80, 0x0000, 0x8000, (UINT32)memory_region_user2);
	}
	Cz80_Set_ReadB(&CZ80, z80_read_memory_8);
	Cz80_Set_WriteB(&CZ80, z80_write_memory_8);
#elif (EMU_SYSTEM == CPS2)
	Cz80_Set_Fetch(&CZ80, 0x0000, 0xffff, (UINT32)memory_region_cpu2);
	Cz80_Set_ReadB(&CZ80, &z80_read_memory_8);
	Cz80_Set_WriteB(&CZ80, &z80_write_memory_8);
#elif (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
	Cz80_Set_Fetch(&CZ80, 0x0000, 0xffff, (UINT32)memory_region_cpu2);
	Cz80_Set_ReadB(&CZ80, &z80_read_memory_8);
	Cz80_Set_WriteB(&CZ80, &z80_write_memory_8);
	Cz80_Set_INPort(&CZ80, &neogeo_z80_port_r);
	Cz80_Set_OUTPort(&CZ80, &neogeo_z80_port_w);
#endif
}


/*--------------------------------------------------------
	CPUリセット
--------------------------------------------------------*/

void z80_reset(void)
{
	Cz80_Reset(&CZ80);
}


/*--------------------------------------------------------
	CPU停止
--------------------------------------------------------*/

void z80_exit(void)
{
}


/*--------------------------------------------------------
	CPU実行
--------------------------------------------------------*/

int z80_execute(int cycles)
{
	return Cz80_Exec(&CZ80, cycles);
}


/*--------------------------------------------------------
	割り込み処理
--------------------------------------------------------*/

void z80_set_irq_line(int irqline, int state)
{
	Cz80_Set_IRQ(&CZ80, irqline, state);
}


/*--------------------------------------------------------
	割り込みコールバック関数設定
--------------------------------------------------------*/

void z80_set_irq_callback(int (*callback)(int line))
{
	Cz80_Set_IRQ_Callback(&CZ80, callback);
}


/*--------------------------------------------------------
	レジスタ取得
--------------------------------------------------------*/

UINT32 z80_get_reg(int regnum)
{
	switch (regnum)
	{
	case Z80_PC:   return Cz80_Get_Reg(&CZ80, CZ80_PC);
	case Z80_SP:   return Cz80_Get_Reg(&CZ80, CZ80_SP);
	case Z80_AF:   return Cz80_Get_Reg(&CZ80, CZ80_AF);
	case Z80_BC:   return Cz80_Get_Reg(&CZ80, CZ80_BC);
	case Z80_DE:   return Cz80_Get_Reg(&CZ80, CZ80_DE);
	case Z80_HL:   return Cz80_Get_Reg(&CZ80, CZ80_HL);
	case Z80_IX:   return Cz80_Get_Reg(&CZ80, CZ80_IX);
	case Z80_IY:   return Cz80_Get_Reg(&CZ80, CZ80_IY);
	case Z80_AF2:  return Cz80_Get_Reg(&CZ80, CZ80_AF2);
	case Z80_BC2:  return Cz80_Get_Reg(&CZ80, CZ80_BC2);
	case Z80_DE2:  return Cz80_Get_Reg(&CZ80, CZ80_DE2);
	case Z80_HL2:  return Cz80_Get_Reg(&CZ80, CZ80_HL2);
	case Z80_R:    return Cz80_Get_Reg(&CZ80, CZ80_R);
	case Z80_I:    return Cz80_Get_Reg(&CZ80, CZ80_I);
	case Z80_IM:   return Cz80_Get_Reg(&CZ80, CZ80_IM);
	case Z80_IFF1: return Cz80_Get_Reg(&CZ80, CZ80_IFF1);
	case Z80_IFF2: return Cz80_Get_Reg(&CZ80, CZ80_IFF2);
	case Z80_HALT: return Cz80_Get_Reg(&CZ80, CZ80_HALT);
	case Z80_IRQ_STATE: return Cz80_Get_Reg(&CZ80, CZ80_IRQ);
	default: return 0;
	}
}


/*--------------------------------------------------------
	レジスタ設定
--------------------------------------------------------*/

void z80_set_reg(int regnum, UINT32 val)
{
	switch (regnum)
	{
	case Z80_PC:   Cz80_Set_Reg(&CZ80, CZ80_PC, val); break;
	case Z80_SP:   Cz80_Set_Reg(&CZ80, CZ80_SP, val); break;
	case Z80_AF:   Cz80_Set_Reg(&CZ80, CZ80_AF, val); break;
	case Z80_BC:   Cz80_Set_Reg(&CZ80, CZ80_BC, val); break;
	case Z80_DE:   Cz80_Set_Reg(&CZ80, CZ80_DE, val); break;
	case Z80_HL:   Cz80_Set_Reg(&CZ80, CZ80_HL, val); break;
	case Z80_IX:   Cz80_Set_Reg(&CZ80, CZ80_IX, val); break;
	case Z80_IY:   Cz80_Set_Reg(&CZ80, CZ80_IY, val); break;
	case Z80_AF2:  Cz80_Set_Reg(&CZ80, CZ80_AF2, val); break;
	case Z80_BC2:  Cz80_Set_Reg(&CZ80, CZ80_BC2, val); break;
	case Z80_DE2:  Cz80_Set_Reg(&CZ80, CZ80_DE2, val); break;
	case Z80_HL2:  Cz80_Set_Reg(&CZ80, CZ80_HL2, val); break;
	case Z80_R:    Cz80_Set_Reg(&CZ80, CZ80_R, val); break;
	case Z80_I:    Cz80_Set_Reg(&CZ80, CZ80_I, val); break;
	case Z80_IM:   Cz80_Set_Reg(&CZ80, CZ80_IM, val); break;
	case Z80_IFF1: Cz80_Set_Reg(&CZ80, CZ80_IFF1, val); break;
	case Z80_IFF2: Cz80_Set_Reg(&CZ80, CZ80_IFF2, val); break;
	case Z80_HALT: Cz80_Set_Reg(&CZ80, CZ80_HALT, val); break;
	case Z80_IRQ_STATE: Cz80_Set_Reg(&CZ80, CZ80_IRQ, val); break;
	default: break;
	}
}


/*------------------------------------------------------
	セーブ/ロード ステート
------------------------------------------------------*/

#ifdef SAVE_STATE

STATE_SAVE( z80 )
{
	UINT32 pc = Cz80_Get_Reg(&CZ80, CZ80_PC);

	state_save_word(&CZ80.BC.W, 1);
	state_save_word(&CZ80.DE.W, 1);
	state_save_word(&CZ80.HL.W, 1);
	state_save_word(&CZ80.AF.W, 1);
	state_save_word(&CZ80.IX.W, 1);
	state_save_word(&CZ80.IY.W, 1);
	state_save_word(&CZ80.SP.W, 1);
	state_save_long(&pc, 1);
	state_save_word(&CZ80.BC2.W, 1);
	state_save_word(&CZ80.DE2.W, 1);
	state_save_word(&CZ80.HL2.W, 1);
	state_save_word(&CZ80.AF2.W, 1);
	state_save_word(&CZ80.R.W, 1);
	state_save_word(&CZ80.IFF.W, 1);
	state_save_byte(&CZ80.I, 1);
	state_save_byte(&CZ80.IM, 1);
	state_save_byte(&CZ80.HaltState, 1);
	state_save_long(&CZ80.IRQLine, 1);
	state_save_long(&CZ80.IRQState, 1);
}

STATE_LOAD( z80 )
{
	UINT32 pc;

	state_load_word(&CZ80.BC.W, 1);
	state_load_word(&CZ80.DE.W, 1);
	state_load_word(&CZ80.HL.W, 1);
	state_load_word(&CZ80.AF.W, 1);
	state_load_word(&CZ80.IX.W, 1);
	state_load_word(&CZ80.IY.W, 1);
	state_load_word(&CZ80.SP.W, 1);
	state_load_long(&pc, 1);
	state_load_word(&CZ80.BC2.W, 1);
	state_load_word(&CZ80.DE2.W, 1);
	state_load_word(&CZ80.HL2.W, 1);
	state_load_word(&CZ80.AF2.W, 1);
	state_load_word(&CZ80.R.W, 1);
	state_load_word(&CZ80.IFF.W, 1);
	state_load_byte(&CZ80.I, 1);
	state_load_byte(&CZ80.IM, 1);
	state_load_byte(&CZ80.HaltState, 1);
	state_load_long(&CZ80.IRQLine, 1);
	state_load_long(&CZ80.IRQState, 1);

	Cz80_Set_Reg(&CZ80, CZ80_PC, pc);
}

#endif /* SAVE_STATE */
