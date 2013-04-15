/***************************************************************************

	cps1.c

	CPS1エミュレーションコア

***************************************************************************/

#ifndef CPS1_H
#define CPS1_H

#include "emumain.h"
#include "cpu/m68000/m68000.h"
#include "cpu/z80/z80.h"
#include "sound/sndintrf.h"
#include "sound/2151intf.h"
#include "sound/qsound.h"
#include "timer.h"
#include "dipsw.h"
#include "driver.h"
#include "eeprom.h"
#include "inptport.h"
#include "memintrf.h"
#include "sprite.h"
#include "vidhrdw.h"

void cps1_main(void);

#endif /* CPS1_H */
