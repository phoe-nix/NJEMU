/******************************************************************************

	ncdz.c

	NEOGEO CDZエミュレーションコア

******************************************************************************/

#ifndef NCDZ_H
#define NCDZ_H

#include "emumain.h"
#include "cpu/m68000/m68000.h"
#include "cpu/z80/z80.h"
#include "sound/sndintrf.h"
#include "sound/2610intf.h"
#include "timer.h"
#include "driver.h"
#include "cdrom.h"
#include "cdda.h"
#include "inptport.h"
#include "memintrf.h"
#include "sprite.h"
#include "vidhrdw.h"

extern int neogeo_boot_bios;
extern int neogeo_region;

void neogeo_main(void);
void neogeo_restart(void);
void neogeo_bios_exit(void);

#endif /* NCDZ_H */
