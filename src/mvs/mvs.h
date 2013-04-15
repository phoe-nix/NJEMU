/******************************************************************************

	mvs.c

	MVSエミュレーションコア

******************************************************************************/

#ifndef MVS_H
#define MVS_H

#include "emumain.h"
#include "cpu/m68000/m68000.h"
#include "cpu/z80/z80.h"
#include "sound/sndintrf.h"
#include "sound/2610intf.h"
#include "memintrf.h"
#include "inptport.h"
#include "dipsw.h"
#include "timer.h"
#include "vidhrdw.h"
#include "sprite.h"
#include "driver.h"
#include "pd4990a.h"
#include "biosmenu.h"

extern int neogeo_bios;
extern int neogeo_region;
extern int neogeo_save_sound_flag;

void neogeo_main(void);

#endif /* MVS_H */
