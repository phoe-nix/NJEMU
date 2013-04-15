/******************************************************************************

	cps2.c

	CPS2エミュレーションコア

******************************************************************************/

#ifndef CPS2_H
#define CPS2_H

#include "emumain.h"
#include "cpu/m68000/m68000.h"
#include "cpu/z80/z80.h"
#include "sound/sndintrf.h"
#include "sound/qsound.h"
#include "timer.h"
#include "driver.h"
#include "eeprom.h"
#include "inptport.h"
#include "memintrf.h"
#include "sprite.h"
#include "vidhrdw.h"

void cps2_main(void);

#endif /* CPS2_H */
