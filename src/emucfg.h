/******************************************************************************

	emucfg.h

	エミュレーション設定

******************************************************************************/

#ifndef EMUCONFIG_H
#define EMUCONFIG_H

#ifndef EMUSYS_VALUE
#define EMUSYS_VALUE
#define CPS1	0
#define CPS2	1
#define MVS		2
#define NCDZ	3
#endif

#define ALIGN16_DATA			__attribute__((aligned(16)))
#define ALIGN_DATA				__attribute__((aligned(4)))
#define MEM_ALIGN				4


/******************************************************************************
	AdHocの設定
******************************************************************************/

#define ADHOC_UPDATE_EVERY_FRAME	0
#define ADHOC_TIMEOUT				(500*1000)	// 500 ms


/******************************************************************************
	その他の設定
******************************************************************************/

#define QSOUND_STREAM_48KHz		0	// 1にすると音質が上がるかもしれない


/******************************************************************************
	CPS1の設定
******************************************************************************/

#if defined(BUILD_CPS1)

#define APPNAME_STR				"CAPCOM CPS1 Emulator"
#define machine_main			cps1_main

#define EMU_SYSTEM				CPS1
#define SYSTEM_NAME				"CPS1"
#define FPS						60.0
#define TICKS_PER_FRAME			16666.666666
#define USE_CACHE				0
#define EEPROM_SIZE				128
#define GULIST_SIZE				48*1024		// 48KB
#define ENABLE_RASTER_OPTION	1			// on


/******************************************************************************
	CPS2の設定
******************************************************************************/

#elif defined(BUILD_CPS2)

#define APPNAME_STR				"CAPCOM CPS2 Emulator"
#define machine_main			cps2_main

#define EMU_SYSTEM				CPS2
#define SYSTEM_NAME				"CPS2"
#define FPS						59.633333
#define TICKS_PER_FRAME			16769.144773
#ifdef LARGE_MEMORY
#define USE_CACHE				0
#else
#define USE_CACHE				1
#define CACHE_VERSION			"V23"
#endif
#define EEPROM_SIZE				128
#define GULIST_SIZE				48*1024		// 48KB
#define ENABLE_RASTER_OPTION	0			// off


/******************************************************************************
	MVSの設定
******************************************************************************/

#elif defined(BUILD_MVS)

#define APPNAME_STR				"NEOGEO Emulator"
#define machine_main			neogeo_main

#define EMU_SYSTEM				MVS
#define SYSTEM_NAME				"NEO·GEO"
#define FPS						59.185606
#define TICKS_PER_FRAME			16896.0
#define USE_CACHE				1
#define CACHE_VERSION			"V23"
#define GULIST_SIZE				300*1024	// 300KB


/******************************************************************************
	NCDZの設定
******************************************************************************/

#elif defined(BUILD_NCDZ)

#define APPNAME_STR				"NEOGEO CDZ Emulator"
#define machine_main			neogeo_main

#define EMU_SYSTEM				NCDZ
#define SYSTEM_NAME				"NEO·GEO CDZ"
#define FPS						59.185606
#define TICKS_PER_FRAME			16896.0
#define USE_CACHE				0
#define GULIST_SIZE				300*1024	// 300KB

#endif

#endif /* EMUCONFIG_H */
