/***************************************************************************

  2151intf.c

  Support interface YM2151(OPM) + OKIM6295(ADPCM)

***************************************************************************/

#include "emumain.h"


/*------------------------------------------------------
	Start YM2151 + OKIM6295 emulation
 -----------------------------------------------------*/

void YM2151_sh_start(int type)
{
	YM2151Init(3579545, cps1_sound_interrupt);
	OKIM6295Init(1000000, 1);
}


/*------------------------------------------------------
	Stop YM2151 emulation
 -----------------------------------------------------*/

void YM2151_sh_stop(void)
{
}


/*------------------------------------------------------
	Reset YM2151 emulation
 -----------------------------------------------------*/

void YM2151_sh_reset(void)
{
	YM2151Reset();
	OKIM6295Reset();
}


static int lastreg;

READ8_HANDLER( YM2151_status_port_r )
{
	return YM2151ReadStatus();
}

WRITE8_HANDLER( YM2151_register_port_w )
{
	lastreg = data;
}

WRITE8_HANDLER( YM2151_data_port_w )
{
	YM2151WriteReg(lastreg, data);
}
