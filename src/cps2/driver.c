/******************************************************************************

	driver.c

	CPS2 ドライバ

******************************************************************************/

#include "cps2.h"


/******************************************************************************
	グローバル構造体
******************************************************************************/

struct driver_t CPS2_driver[] =
{
#if USE_CACHE
//    name        cache      kludge              flags player & coin chuter
	{ "ssf2",     0xb50000,  CPS2_KLUDGE_SSF2,    1,   0x00, { COIN_NONE } },
	{ "ddtod",    0xc00000,  0,                   0,   0x0e, { COIN_4P4C, COIN_4P1C, COIN_3P3C, COIN_3P1C, COIN_2P1C } },
	{ "ecofghtr", 0x9c0000,  0,                   0,   0x00, { COIN_NONE } },
	{ "ssf2t",    0xf30000,  CPS2_KLUDGE_SSF2T,   1,   0x00, { COIN_NONE } },
	{ "xmcota",   0x1fb0000, CPS2_KLUDGE_XMCOTA,  1|2, 0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "armwar",   0x1300000, 0,                   0,   0x06, { COIN_2P1C, COIN_2P2C, COIN_2P2C, COIN_3P1C, COIN_3P2C, COIN_3P3C, COIN_3P3C } },
	{ "avsp",     0xb00000,  0,                   0,   0x05, { COIN_3P3C, COIN_3P1C, COIN_2P1C } },
	{ "dstlk",    0x13d0000, 0,                   0,   0x00, { COIN_NONE } },
	{ "ringdest", 0x1100000, 0,                   0,   0x06, { COIN_2P1C, COIN_2P2C, COIN_2P2C } },
	{ "cybots",   0x1f40000, 0,                   0,   0x26, { COIN_2P1C, COIN_2P2C, COIN_2P2C } },
	{ "msh",      0x1ff0000, 0,                   1,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "nwarr",    0x1ed0000, 0,                   0,   0x1d, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "sfa",      0x7e0000,  0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "mmancp2u", 0x790000,  0,                   0,   0x00, { COIN_NONE } },
	{ "19xx",     0x970000,  0,                   0,   0x26, { COIN_2P1C, COIN_2P2C, COIN_2P2C } },
	{ "ddsom",    0x16f0000, 0,                   0,   0x01, { COIN_2P1C, COIN_2P2C, COIN_2P2C, COIN_3P1C, COIN_3P2C, COIN_3P3C, COIN_3P3C, COIN_4P1C, COIN_4P2C, COIN_4P2C, COIN_4P4C, COIN_4P4C } },
	{ "megaman2", 0x7f0000,  0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "qndream",  0x700000,  0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "sfa2",     0x12e0000, 0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "spf2t",    0x3b0000,  0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "xmvsf",    0x1fd0000, 0,                   1|2, 0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "batcir",   0xd70000,  0,                   0,   0x26, { COIN_2P1C, COIN_2P2C, COIN_2P2C, COIN_4P1C, COIN_4P2C, COIN_4P2C, COIN_4P4C, COIN_4P4C } },
	{ "csclub",   0x7f0000,  0,                   0,   0x21, { COIN_2P1C, COIN_2P2C, COIN_2P2C } },
	{ "mshvsf",   0x1fd0000, 0,                   1|2, 0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "sgemf",    0x13e0000, 0,                   2,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "vhunt2",   0x1f60000, 0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "vsav",     0x1fe0000, 0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "vsav2",    0x1fb0000, 0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "mvsc",     0x1f90000, 0,                   1|2, 0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "sfa3",     0x1f60000, 0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "gigawing", 0xf90000,  0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "mmatrix",  0x1d70000, CPS2_KLUDGE_MMATRIX, 0,   0x11, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "mpangj",   0x790000,  0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "mpang",    0x790000,  0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "pzloop2",  0xab0000,  CPS2_KLUDGE_PUZLOOP2,0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "choko",    0x630000,  0,                   0,   0x05, { COIN_2P2C, COIN_2P1C, COIN_2P1C } },
	{ "dimahoo",  0xfb0000,  CPS2_KLUDGE_DIMAHOO, 0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "1944",     0x1390000, 0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "progear",  0xfe0000,  0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "hsf2",     0x1d80000, CPS2_KLUDGE_SSF2T,   1,   0x00, { COIN_NONE } },
	{ "jyangoku", 0x170000,  0,                   0,   0x00, { COIN_NONE } },
#else
//    name        kludge              flags player & coin chuter
	{ "ssf2",     CPS2_KLUDGE_SSF2,    1,   0x00, { COIN_NONE } },
	{ "ddtod",    0,                   0,   0x0e, { COIN_4P4C, COIN_4P1C, COIN_3P3C, COIN_3P1C, COIN_2P1C } },
	{ "ecofghtr", 0,                   0,   0x00, { COIN_NONE } },
	{ "ssf2t",    CPS2_KLUDGE_SSF2T,   1,   0x00, { COIN_NONE } },
	{ "xmcota",   CPS2_KLUDGE_XMCOTA,  1|2, 0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "armwar",   0,                   0,   0x06, { COIN_2P1C, COIN_2P2C, COIN_2P2C, COIN_3P1C, COIN_3P2C, COIN_3P3C, COIN_3P3C } },
	{ "avsp",     0,                   0,   0x05, { COIN_3P3C, COIN_3P1C, COIN_2P1C } },
	{ "dstlk",    0,                   0,   0x00, { COIN_NONE } },
	{ "ringdest", 0,                   0,   0x06, { COIN_2P1C, COIN_2P2C, COIN_2P2C } },
	{ "cybots",   0,                   0,   0x26, { COIN_2P1C, COIN_2P2C, COIN_2P2C } },
	{ "msh",      0,                   1,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "nwarr",    0,                   0,   0x1d, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "sfa",      0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "mmancp2u", 0,                   0,   0x00, { COIN_NONE } },
	{ "19xx",     0,                   0,   0x26, { COIN_2P1C, COIN_2P2C, COIN_2P2C } },
	{ "ddsom",    0,                   0,   0x01, { COIN_2P1C, COIN_2P2C, COIN_2P2C, COIN_3P1C, COIN_3P2C, COIN_3P3C, COIN_3P3C, COIN_4P1C, COIN_4P2C, COIN_4P2C, COIN_4P4C, COIN_4P4C } },
	{ "megaman2", 0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "qndream",  0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "sfa2",     0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "spf2t",    0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "xmvsf",    0,                   1|2, 0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "batcir",   0,                   0,   0x26, { COIN_2P1C, COIN_2P2C, COIN_2P2C, COIN_4P1C, COIN_4P2C, COIN_4P2C, COIN_4P4C, COIN_4P4C } },
	{ "csclub",   0,                   0,   0x21, { COIN_2P1C, COIN_2P2C, COIN_2P2C } },
	{ "mshvsf",   0,                   1|2, 0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "sgemf",    0,                   2,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "vhunt2",   0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "vsav",     0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "vsav2",    0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "mvsc",     0,                   1|2, 0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "sfa3",     0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "gigawing", 0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "mmatrix",  CPS2_KLUDGE_MMATRIX, 0,   0x11, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "mpangj",   0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "mpang",    0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "pzloop2",  CPS2_KLUDGE_PUZLOOP2,0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "choko",    0,                   0,   0x05, { COIN_2P2C, COIN_2P1C, COIN_2P1C } },
	{ "dimahoo",  CPS2_KLUDGE_DIMAHOO, 0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "1944",     0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "progear",  0,                   0,   0x05, { COIN_2P2C, COIN_2P2C, COIN_2P1C } },
	{ "hsf2",     CPS2_KLUDGE_SSF2T,   1,   0x00, { COIN_NONE } },
	{ "jyangoku", 0,                   0,   0x00, { COIN_NONE } },
#endif
	{ NULL }
};

struct driver_t *driver;


/******************************************************************************
	ローカル変数
******************************************************************************/

static UINT32 z80_bank;
static int readpaddle;
static int next_update_first_line;


/******************************************************************************
	ローカル関数
******************************************************************************/

/*--------------------------------------------------------
	Z80 ROMバンク切り替え
--------------------------------------------------------*/

static void z80_set_bank(UINT32 offset)
{
	if (offset != z80_bank)
	{
		z80_bank = offset;
		memcpy(&memory_region_cpu2[0x8000], &memory_region_cpu2[offset], 0x4000);
	}
}


/******************************************************************************
	コールバック関数
******************************************************************************/

/*--------------------------------------------------------
	M68000割り込み
--------------------------------------------------------*/

TIMER_CALLBACK( cps2_raster_interrupt )
{
	int scanline = param & 0xffff;
	int reg = (param >> 16) ? 0x52/2 : 0x50/2;

	cps1_output[reg] = 0;
	m68000_set_irq_line(4, HOLD_LINE);

	if (scanline >= next_update_first_line)
	{
		if (!skip_this_frame())
		{
			cps2_screenrefresh(next_update_first_line, scanline);
		}

		next_update_first_line = scanline + 1;
	}
}

TIMER_CALLBACK( cps2_vblank_interrupt )
{
	cps1_output[0x50/2] = scanline1;
	cps1_output[0x52/2] = scanline2;
	m68000_set_irq_line(2, HOLD_LINE);

	if (!skip_this_frame())
	{
		if (next_update_first_line <= LAST_VISIBLE_LINE)
		{
			cps2_screenrefresh(next_update_first_line, LAST_VISIBLE_LINE);
		}
		blit_finish();
	}

	cps2_objram_latch();

	next_update_first_line = FIRST_VISIBLE_LINE;
}


/******************************************************************************
	メモリハンドラ
******************************************************************************/

/*--------------------------------------------------------
	入力ポートリード
--------------------------------------------------------*/

READ16_HANDLER( cps2_inputport0_r )
{
	if (machine_input_type == INPTYPE_pzloop2)
	{
		if (!readpaddle) return cps2_port_value[3];
	}
	return cps2_port_value[0];
}

READ16_HANDLER( cps2_inputport1_r )
{
	return cps2_port_value[1];
}


/*--------------------------------------------------------
	Q-Sound
--------------------------------------------------------*/

READ16_HANDLER( qsound_sharedram1_r )
{
	offset &= 0xfff;
	return qsound_sharedram1[offset] | 0xff00;
}

WRITE16_HANDLER( qsound_sharedram1_w )
{
	if (ACCESSING_LSB)
	{
		offset &= 0xfff;
		qsound_sharedram1[offset] = data;
	}
}

WRITE8_HANDLER( qsound_banksw_w )
{
	/*
		Z80 bank register for music note data. It's odd that it isn't encrypted
		though.
	*/
	UINT32 bankaddress = 0x10000 + ((data & 0x0f) << 14);

	if (bankaddress >= memory_length_cpu2)
		bankaddress = 0x10000;

	z80_set_bank(bankaddress);
}

READ16_HANDLER( cps2_qsound_volume_r )
{
	/* Extra adapter memory (0x660000-0x663fff) available when bit 14 = 0 */
	/* Network adapter (ssf2tb) present when bit 15 = 0 */
	/* Only game known to use both these so far is SSF2TB */

#if 0
	if (strncmp(game_name, "ssf2tb", 6) == 0)
		return 0x2021;
	else
#endif
		return 0xe021;
}


/********************************************************************
*
*  EEPROM
*  ======
*
*   The EEPROM is accessed by a serial protocol using the register
*   0xf1c006
*
********************************************************************/

static void cps2_nvram_read_write(int read_or_write)
{
	char path[MAX_PATH];
	FILE *fp;

	sprintf(path, "%snvram/%s.nv", launchDir, game_name);

	if (read_or_write)
	{
		if ((fp = fopen(path, "wb")) != NULL)
		{
			EEPROM_save(fp);
			fclose(fp);
		}
	}
	else
	{
		if ((fp = fopen(path, "rb")) != NULL)
		{
			EEPROM_load(fp);
			fclose(fp);
		}
	}
}


READ16_HANDLER( cps2_eeprom_port_r )
{
	return (cps2_port_value[2] & 0xfffe) | EEPROM_read_bit();
}


WRITE16_HANDLER( cps2_eeprom_port_w )
{
	if (ACCESSING_MSB)
	{
		/* bit 0 - Unused */
		/* bit 1 - Unused */
		/* bit 2 - Unused */
		/* bit 3 - Unused? */
		/* bit 4 - Eeprom data  */
		/* bit 5 - Eeprom clock */
		/* bit 6 - */
		/* bit 7 - */

		/* EEPROM */
		EEPROM_write_bit(data & 0x1000);
		EEPROM_set_clock_line((data & 0x2000) ? ASSERT_LINE : CLEAR_LINE);
		EEPROM_set_cs_line((data & 0x4000) ? CLEAR_LINE : ASSERT_LINE);
	}

	if (ACCESSING_LSB)
	{
		/* bit 0 - coin counter 1 */
		/* bit 0 - coin counter 2 */
		/* bit 2 - Unused */
		/* bit 3 - Allows access to Z80 address space (Z80 reset) */
		/* bit 4 - lock 1  */
		/* bit 5 - lock 2  */
		/* bit 6 - */
		/* bit 7 - */

		/* Z80 Reset */
		z80_set_reset_line((data & 0x0008) ? CLEAR_LINE : ASSERT_LINE);

		coin_counter_w(0, data & 0x0001);
		if (driver->kludge == CPS2_KLUDGE_PUZLOOP2)	// Puzz Loop 2 uses coin counter 2 input to switch between stick and paddle controls
			readpaddle = data & 0x0002;
		else
			coin_counter_w(1, data & 0x0002);

		if (driver->kludge == CPS2_KLUDGE_MMATRIX)	// Mars Matrix seems to require the coin lockout bit to be reversed
		{
			coin_lockout_w(0, data & 0x0010);
			coin_lockout_w(1, data & 0x0020);
			coin_lockout_w(2, data & 0x0040);
			coin_lockout_w(3, data & 0x0080);
		}
		else
		{
			coin_lockout_w(0, ~data & 0x0010);
			coin_lockout_w(1, ~data & 0x0020);
			coin_lockout_w(2, ~data & 0x0040);
			coin_lockout_w(3, ~data & 0x0080);
		}
	}
}


/******************************************************************************
	ドライバインタフェース
******************************************************************************/

int cps2_driver_init(void)
{
	cps2_init_68k();

	z80_init();
	z80_bank = -1;

	EEPROM_init();

	cps2_nvram_read_write(0);

	if (!strcmp(driver->name, "sfa3"))
		EEPROM_write_data(0x75, 0x04);

	return 1;
}


void cps2_driver_exit(void)
{
#ifdef ADHOC
	if (!adhoc_enable || adhoc_server)
#endif
		cps2_nvram_read_write(1);
}


void cps2_driver_reset(void)
{
	m68000_reset();
	z80_reset();

	coin_counter_reset();

	next_update_first_line = FIRST_VISIBLE_LINE;
}


/******************************************************************************
	セーブ/ロード ステート
******************************************************************************/

#ifdef SAVE_STATE

STATE_SAVE( driver )
{
	state_save_long(&z80_bank, 1);
}

STATE_LOAD( driver )
{
	int bank;

	state_load_long(&bank, 1);

	z80_bank = 0xffffffff;
	z80_set_bank(bank);

	next_update_first_line = FIRST_VISIBLE_LINE;
}

#endif /* SAVE_STATE */
