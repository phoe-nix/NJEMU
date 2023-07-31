/******************************************************************************

	psp.c

	PSP�ᥤ��

******************************************************************************/

#ifndef PSP_MAIN_H
#define PSP_MAIN_H

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#include "emucfg.h"

#include <psptypes.h>
#include "include/osd_cpu.h"
#include <pspaudio.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspiofilemgr.h>
#include <pspkernel.h>
#include <psppower.h>
#include <psprtc.h>
#include <pspsdk.h>
#ifdef LARGE_MEMORY
#include <kubridge.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/unistd.h>

#include "common/ticker_driver.h"

#include "psp/ui_text.h"
#include "psp/config.h"
#include "psp/filer.h"
#include "psp/input.h"
#include "psp/ui.h"
#include "psp/ui_draw.h"
#include "psp/ui_menu.h"
#include "psp/video.h"
#include "psp/png.h"
#ifdef ADHOC
#include "psp/adhoc.h"
#endif
#if VIDEO_32BPP
#include "psp/wallpaper.h"
#endif
#include "SystemButtons.h"

#ifdef LARGE_MEMORY
#define PSP2K_MEM_TOP		0xa000000//0xa000000
#define PSP2K_MEM_BOTTOM	0xbffffff//0xbffffff
#define PSP2K_MEM_SIZE		0x2000000//0x2000000
#endif

/******************************************************************************
	PSPの定数
******************************************************************************/

#define REFRESH_RATE		(59.940059)		// (9000000Hz * 1) / (525 * 286)
#define TICKS_PER_FRAME		(16683.333333)

enum
{
	LOOP_EXIT = 0,
	LOOP_BROWSER,
	LOOP_RESTART,
	LOOP_RESET,
	LOOP_EXEC
};

enum
{
	PSPCLOCK_222 = 0,
	PSPCLOCK_266,
	PSPCLOCK_300,
	PSPCLOCK_333,
	PSPCLOCK_MAX
};

extern volatile int Loop;
extern volatile int Sleep;
extern char launchDir[MAX_PATH];
extern int devkit_version;
extern int systembuttons_available;
extern int njemu_debug;

#endif /* PSP_MAIN_H */
