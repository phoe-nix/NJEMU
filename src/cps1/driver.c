/******************************************************************************

	driver.c

	CPS1 ドライバ

******************************************************************************/

#include "cps1.h"


/******************************************************************************
	グローバル構造体
******************************************************************************/

//                 CPSB ID    multiply protection  priority masks         layer enable masks          ctrl unknwn
#define CPS_B_01 0x00,0x0000, 0x00,0x00,0x00,0x00, {0x68,0x6a,0x6c,0x6e}, {0x02,0x04,0x08,0x30,0x30}, 0x66, 0x70
#define CPS_B_02 0x60,0x0002, 0x00,0x00,0x00,0x00, {0x6a,0x68,0x66,0x64}, {0x02,0x04,0x08,0x00,0x00}, 0x6c, 0x62
#define CPS_B_03 0x00,0x0000, 0x00,0x00,0x00,0x00, {0x6e,0x6c,0x6a,0x68}, {0x20,0x10,0x08,0x00,0x00}, 0x70, 0x66
#define CPS_B_04 0x60,0x0004, 0x00,0x00,0x00,0x00, {0x66,0x70,0x68,0x72}, {0x02,0x0c,0x0c,0x00,0x00}, 0x6e, 0x6a
#define CPS_B_05 0x60,0x0005, 0x00,0x00,0x00,0x00, {0x6a,0x6c,0x6e,0x70}, {0x02,0x08,0x20,0x14,0x14}, 0x68, 0x72
#define CPS_B_11 0x72,0x0401, 0x00,0x00,0x00,0x00, {0x68,0x6a,0x6c,0x6e}, {0x08,0x10,0x20,0x00,0x00}, 0x66, 0x70
#define CPS_B_12 0x60,0x0402, 0x00,0x00,0x00,0x00, {0x6a,0x68,0x66,0x64}, {0x02,0x04,0x08,0x00,0x00}, 0x6c, 0x62
#define CPS_B_13 0x6e,0x0403, 0x00,0x00,0x00,0x00, {0x64,0x66,0x68,0x6a}, {0x20,0x02,0x04,0x00,0x00}, 0x62, 0x6c
#define CPS_B_14 0x5e,0x0404, 0x00,0x00,0x00,0x00, {0x54,0x56,0x58,0x5a}, {0x08,0x20,0x10,0x00,0x00}, 0x52, 0x5c
#define CPS_B_15 0x4e,0x0405, 0x00,0x00,0x00,0x00, {0x44,0x46,0x48,0x4a}, {0x04,0x02,0x20,0x00,0x00}, 0x42, 0x4c
#define CPS_B_16 0x40,0x0406, 0x00,0x00,0x00,0x00, {0x4a,0x48,0x46,0x44}, {0x10,0x0a,0x0a,0x00,0x00}, 0x4c, 0x42
#define CPS_B_17 0x48,0x0407, 0x00,0x00,0x00,0x00, {0x52,0x50,0x4e,0x4c}, {0x08,0x10,0x02,0x00,0x00}, 0x54, 0x4a
#define CPS_B_18 0xd0,0x0408, 0x00,0x00,0x00,0x00, {0xda,0xd8,0xd6,0xd4}, {0x10,0x08,0x02,0x00,0x00}, 0xdc, 0xd2
#define NOBATTRY 0x00,0x0000, 0x40,0x42,0x44,0x46, {0x68,0x6a,0x6c,0x6e}, {0x02,0x04,0x08,0x30,0x30}, 0x66, 0x70
#define BATTRY_1 0x72,0x0800, 0x4e,0x4c,0x4a,0x48, {0x66,0x64,0x62,0x60}, {0x20,0x04,0x08,0x12,0x12}, 0x68, 0x70
#define BATTRY_2 0x00,0x0000, 0x5e,0x5c,0x5a,0x58, {0x6e,0x6c,0x6a,0x68}, {0x30,0x08,0x30,0x00,0x00}, 0x60, 0x70
#define BATTRY_3 0x00,0x0000, 0x46,0x44,0x42,0x40, {0x6e,0x6c,0x6a,0x68}, {0x20,0x12,0x12,0x00,0x00}, 0x60, 0x70
#define BATTRY_4 0x00,0x0000, 0x46,0x44,0x42,0x40, {0x66,0x64,0x62,0x60}, {0x20,0x10,0x02,0x00,0x00}, 0x68, 0x70
#define BATTRY_5 0x00,0x0000, 0x4e,0x4c,0x4a,0x48, {0x6e,0x6c,0x6a,0x68}, {0x20,0x06,0x06,0x00,0x00}, 0x60, 0x70
#define BATTRY_6 0x00,0x0000, 0x00,0x00,0x00,0x00, {0x6e,0x6c,0x6a,0x68}, {0x20,0x14,0x14,0x00,0x00}, 0x60, 0x70
#define BATTRY_7 0x00,0x0000, 0x00,0x00,0x00,0x00, {0x00,0x00,0x00,0x00}, {0x14,0x02,0x14,0x00,0x00}, 0x6c, 0x52
#define QSOUND_1 0x00,0x0000, 0x00,0x00,0x00,0x00, {0x64,0x66,0x68,0x6a}, {0x10,0x08,0x04,0x00,0x00}, 0x62, 0x6c
#define QSOUND_2 0x00,0x0000, 0x00,0x00,0x00,0x00, {0x4c,0x4e,0x40,0x42}, {0x16,0x16,0x16,0x00,0x00}, 0x4a, 0x44
#define QSOUND_3 0x4e,0x0c00, 0x00,0x00,0x00,0x00, {0x54,0x56,0x48,0x4a}, {0x04,0x02,0x20,0x00,0x00}, 0x52, 0x4c
#define QSOUND_4 0x6e,0x0c01, 0x00,0x00,0x00,0x00, {0x40,0x42,0x68,0x6a}, {0x04,0x08,0x10,0x00,0x00}, 0x56, 0x6c
#define QSOUND_5 0x5e,0x0c02, 0x00,0x00,0x00,0x00, {0x6c,0x6e,0x70,0x72}, {0x04,0x08,0x10,0x00,0x00}, 0x6a, 0x5c
#define HACK_B_1 0x00,0x0000, 0x00,0x00,0x00,0x00, {0x52,0x50,0x4e,0x4c}, {0xff,0xff,0xff,0x00,0x00}, 0x54, 0x5c
#define HACK_B_2 0x00,0x0000, 0x00,0x00,0x00,0x00, {0x68,0x6a,0x6c,0x6e}, {0x02,0x04,0x08,0x00,0x00}, 0x60, 0x70
#define HACK_B_3 0x00,0x0000, 0x00,0x00,0x00,0x00, {0x52,0x50,0x4e,0x4c}, {0xff,0xff,0xff,0x00,0x00}, 0xc4, 0x4a

//                      stars banks  gfx limit  scroll1 limit    scroll2 limit    scroll3 limit
#define GFX_FORGOTTN	1,    0,0,0, 0,        {0xf000,0xffff}, {0x0800,0x1fff}, {0x0800,0x0fff}
#define GFX_GHOULS		0,    0,0,0, 0,        {0x2000,0x3fff}, {0x2000,0x3fff}, {0x1000,0x13ff}
#define GFX_STRIDER		1,    1,0,1, 0,        {0x7000,0x7fff}, {0x2800,0x5fff}, {0x0000,0x0dff}
#define GFX_DYNWAR		0,    0,1,1, 0,        {0x6000,0x7fff}, {0x2000,0x3fff}, {0x0000,0x07ff}
#define GFX_WILLOW		0,    0,1,0, 0x2fffff, {0x7000,0x7fff}, {0x0000,0x1fff}, {0x0a00,0x0dff}
#define GFX_FFIGHT		0,    0,0,0, 0,        {0x4400,0x4bff}, {0x3000,0x3fff}, {0x0980,0x0bff}
#define GFX_1941		0,    0,0,0, 0,        {0x4000,0x47ff}, {0x2400,0x3fff}, {0x0400,0x07ff}
#define GFX_UNSQUAD		0,    0,0,0, 0,        {0x3000,0x3fff}, {0x2000,0x2fff}, {0x0c00,0x0fff}
#define GFX_MERCS		0,    0,0,0, 0x2fffff, {0x0000,0x0bff}, {0x0600,0x1dff}, {0x0780,0x097f}
#define GFX_MSWORD		0,    0,0,0, 0,        {0x4000,0x4fff}, {0x2800,0x37ff}, {0x0e00,0x0fff}
#define GFX_MTWINS		0,    0,0,0, 0,        {0x3000,0x3fff}, {0x2000,0x37ff}, {0x0e00,0x0fff}
#define GFX_NEMO		0,    0,0,0, 0,        {0x4000,0x47ff}, {0x2400,0x33ff}, {0x0d00,0x0fff}
#define GFX_CAWING		0,    0,0,0, 0,        {0x5000,0x57ff}, {0x2c00,0x3fff}, {0x0600,0x09ff}
#define GFX_SF2			0,    2,2,2, 0,        {0x4000,0x4fff}, {0x2800,0x3fff}, {0x0400,0x07ff}
#define GFX_3WONDERS	0,    0,1,0, 0,        {0x5400,0x6fff}, {0x1400,0x7fff}, {0x0e00,0x14ff}
#define GFX_KOD			0,    0,0,0, 0,        {0xc000,0xd7ff}, {0x4800,0x5fff}, {0x1b00,0x1fff}
#define GFX_CAPTCOMM	0,    0,0,0, 0,        {0xc000,0xcfff}, {0x6800,0x7fff}, {0x1400,0x17ff}
#define GFX_KNIGHTS		0,    0,0,0, 0,        {0x8800,0x97ff}, {0x4c00,0x67ff}, {0x1a00,0x1fff}
#define GFX_VARTH		0,    0,0,0, 0,        {0x5800,0x5fff}, {0x1600,0x2bff}, {0x0c00,0x0fff}
#define GFX_CWORLD2J	0,    0,0,0, 0,        {0x7800,0x7fff}, {0x0000,0x37ff}, {0x0e00,0x0eff}
#define GFX_WOF			0,    0,0,0, 0,        {0xd000,0xdfff}, {0x4800,0x67ff}, {0x1c00,0x1fff}
#define GFX_DINO		0,    0,0,0, 0,        {0x0000,0x0fff}, {0x5800,0x6fff}, {0x1c00,0x1fff}
#define GFX_PUNISHER	0,    0,0,0, 0,        {0x0000,0x0fff}, {0x5400,0x6dff}, {0x1b80,0x1fff}
#define GFX_SLAMMAST	0,    0,0,0, 0,        {0x0000,0x0fff}, {0xa000,0xb3ff}, {0x2d00,0x2fff}
#define GFX_SF2HF		0,    2,2,2, 0,        {0x4000,0x4fff}, {0x2800,0x3fff}, {0x0400,0x07ff}
#define GFX_QAD			0,    0,0,0, 0x0fffff, {0x0000,0x07ff}, {0x0400,0x13ff}, {0x0500,0x07ff}
#define GFX_QADJ		0,    0,0,0, 0,        {0x0000,0x07ff}, {0x1000,0x3fff}, {0x0100,0x03ff}
#define GFX_QTONO2		0,    0,0,0, 0x37ffff, {0x0000,0x0fff}, {0x2000,0x6fff}, {0x0200,0x07ff}
#define GFX_MEGAMAN		0,    0,0,0, 0x7effff, {0x0000,0x17ff}, {0xb000,0xcdff}, {0x3400,0x3f7f}
#define GFX_PUNICKJ		0,    0,0,0, 0x1bffff, {0x0000,0x0fff}, {0x0800,0x2fff}, {0x0c00,0x0dff}
#define GFX_PANG3		0,    0,0,0, 0,        {0x0000,0xffff}, {0x0000,0x7fff}, {0x0000,0x1fff}
#define GFX_SFZCH		0,    0,0,0, 0x7effff, {0x0000,0x07ff}, {0xd400,0xf1ff}, {0x3d00,0x3f7f}

	/* name       CPSB    kludge                gfx type */
struct driver_t CPS1_driver[] =
{
	{"forgottn",   CPS_B_01, CPS1_KLUDGE_FORGOTTN, GFX_FORGOTTN },
	{"forgottnu",  CPS_B_01, CPS1_KLUDGE_FORGOTTN, GFX_FORGOTTN },
	{"forgottnu1", CPS_B_01, CPS1_KLUDGE_FORGOTTN, GFX_FORGOTTN },
	{"forgottnua", CPS_B_01, CPS1_KLUDGE_FORGOTTN, GFX_FORGOTTN },
	{"forgottnuaa",CPS_B_01, CPS1_KLUDGE_FORGOTTN, GFX_FORGOTTN },
	{"lostwrld",   CPS_B_01, CPS1_KLUDGE_FORGOTTN, GFX_FORGOTTN },
	{"lostwrldo",  CPS_B_01, CPS1_KLUDGE_FORGOTTN, GFX_FORGOTTN },
	{"ghouls",     CPS_B_01, CPS1_KLUDGE_GHOULS,   GFX_GHOULS   },
	{"ghoulsu",    CPS_B_01, CPS1_KLUDGE_GHOULS,   GFX_GHOULS   },
	{"daimakai",   CPS_B_01, CPS1_KLUDGE_GHOULS,   GFX_GHOULS   },
	{"daimakair",  CPS_B_01, CPS1_KLUDGE_GHOULS,   GFX_GHOULS   },
	{"strider",    CPS_B_01, 0,                    GFX_STRIDER  },
	{"striderua",  CPS_B_01, 0,                    GFX_STRIDER  },
	{"striderj",   CPS_B_01, 0,                    GFX_STRIDER  },
	{"striderjr",  CPS_B_01, 0,                    GFX_STRIDER  },
	{"dynwar",     CPS_B_02, 0,                    GFX_DYNWAR   },
	{"dynwara",    CPS_B_02, 0,                    GFX_DYNWAR   },
	{"dynwarj",    CPS_B_02, 0,                    GFX_DYNWAR   },
	{"dynwarjr",   CPS_B_01, 0,                    GFX_DYNWAR   },
	{"willow",     CPS_B_03, 0,                    GFX_WILLOW   },
	{"willowo",    CPS_B_03, 0,                    GFX_WILLOW   },
	{"willowj",    CPS_B_03, 0,                    GFX_WILLOW   },
	{"unsquad",    CPS_B_11, 0,                    GFX_UNSQUAD  },
	{"area88",     CPS_B_11, 0,                    GFX_UNSQUAD  },
	{"area88r",    CPS_B_01, 0,                    GFX_UNSQUAD  },
	{"ffight",     CPS_B_04, 0,                    GFX_FFIGHT   },
	{"ffighta",    CPS_B_04, 0,                    GFX_FFIGHT   },
	{"ffightu",    CPS_B_04, 0,                    GFX_FFIGHT   },
	{"ffightu1",   CPS_B_04, 0,                    GFX_FFIGHT   },
	{"ffightua",   CPS_B_01, 0,                    GFX_FFIGHT   },
	{"ffightub",   CPS_B_05, 0,                    GFX_FFIGHT   },
	{"ffightj",    CPS_B_04, 0,                    GFX_FFIGHT   },
	{"ffightj1",   CPS_B_01, 0,                    GFX_FFIGHT   },
	{"ffightj2",   CPS_B_02, 0,                    GFX_FFIGHT   },
	{"ffightj3",   CPS_B_05, 0,                    GFX_FFIGHT   },
#if !RELEASE
	{"ffightbl",   CPS_B_04, 0,                    GFX_FFIGHT   },
	{"ffightjh",   CPS_B_01, 0,                    GFX_FFIGHT   },
	{"fcrash",     CPS_B_02, 0,                    GFX_FFIGHT   },
#endif
	{"1941",       CPS_B_05, 0,                    GFX_1941     },
	{"1941r1",     CPS_B_05, 0,                    GFX_1941     },
	{"1941u",      CPS_B_05, 0,                    GFX_1941     },
	{"1941j",      CPS_B_05, 0,                    GFX_1941     },
	{"mercs",      CPS_B_12, CPS1_KLUDGE_MERCS,    GFX_MERCS    },
	{"mercsu",     CPS_B_12, CPS1_KLUDGE_MERCS,    GFX_MERCS    },
	{"mercsur1",   CPS_B_12, CPS1_KLUDGE_MERCS,    GFX_MERCS    },
	{"mercsj",     CPS_B_12, CPS1_KLUDGE_MERCS,    GFX_MERCS    },
	{"msword",     CPS_B_13, 0,                    GFX_MSWORD   },
	{"mswordr1",   CPS_B_13, 0,                    GFX_MSWORD   },
	{"mswordu",    CPS_B_13, 0,                    GFX_MSWORD   },
	{"mswordj",    CPS_B_13, 0,                    GFX_MSWORD   },
	{"mtwins",     CPS_B_14, 0,                    GFX_MTWINS   },
	{"chikij",     CPS_B_14, 0,                    GFX_MTWINS   },
	{"nemo",       CPS_B_15, 0,                    GFX_NEMO     },
	{"nemoj",      CPS_B_15, 0,                    GFX_NEMO     },
	{"cawing",     CPS_B_16, 0,                    GFX_CAWING   },
	{"cawingr1",   CPS_B_16, 0,                    GFX_CAWING   },
	{"cawingu",    CPS_B_16, 0,                    GFX_CAWING   },
	{"cawingj",    CPS_B_16, 0,                    GFX_CAWING   },
#if !RELEASE
	{"cawingb",    CPS_B_16, CPS1_KLUDGE_CAWINGB,  GFX_CAWING   },//
#endif
	{"sf2",        CPS_B_11, 0,                    GFX_SF2      },
	{"sf2eb",      CPS_B_17, 0,                    GFX_SF2      },
	{"sf2ed",      CPS_B_05, 0,                    GFX_SF2      },
	{"sf2ee",      CPS_B_18, 0,                    GFX_SF2      },
	{"sf2ua",      CPS_B_17, 0,                    GFX_SF2      },
	{"sf2ub",      CPS_B_17, 0,                    GFX_SF2      },
	{"sf2uc",      CPS_B_12, 0,                    GFX_SF2      },
	{"sf2ud",      CPS_B_05, 0,                    GFX_SF2      },
	{"sf2ue",      CPS_B_18, 0,                    GFX_SF2      },
	{"sf2uf",      CPS_B_15, 0,                    GFX_SF2      },
	{"sf2ug",      CPS_B_11, 0,                    GFX_SF2      },
	{"sf2ui",      CPS_B_14, 0,                    GFX_SF2      },
	{"sf2uk",      CPS_B_17, 0,                    GFX_SF2      },
	{"sf2j",       CPS_B_13, 0,                    GFX_SF2      },
	{"sf2ja",      CPS_B_17, 0,                    GFX_SF2      },
	{"sf2jc",      CPS_B_12, 0,                    GFX_SF2      },
	{"sf2jf",      CPS_B_15, 0,                    GFX_SF2      },
	{"sf2jh",      CPS_B_13, 0,                    GFX_SF2      },
	{"sf2jl",      CPS_B_17, 0,                    GFX_SF2      },
#if !RELEASE
	{"sf2qp1",     CPS_B_17, 0,                    GFX_SF2      },
	{"sf2ebbl",    CPS_B_17, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2thndr",   CPS_B_17, 0,                    GFX_SF2      },
#endif
	{"3wonders",   BATTRY_1, CPS1_KLUDGE_3WONDERS, GFX_3WONDERS },
	{"3wondersr1", BATTRY_1, CPS1_KLUDGE_3WONDERS, GFX_3WONDERS },
	{"3wondersu",  BATTRY_1, CPS1_KLUDGE_3WONDERS, GFX_3WONDERS },
	{"wonder3",    BATTRY_1, CPS1_KLUDGE_3WONDERS, GFX_3WONDERS },
	{"3wondersb",  BATTRY_1, CPS1_KLUDGE_3WONDERS, GFX_3WONDERS },
	{"3wondersh",  CPS_B_02, CPS1_KLUDGE_3WONDERS, GFX_3WONDERS },
	{"kod",        BATTRY_2, 0,                    GFX_KOD      },
	{"kodr1",      BATTRY_2, 0,                    GFX_KOD      },
	{"kodu",       BATTRY_2, 0,                    GFX_KOD      },
	{"kodj",       BATTRY_2, 0,                    GFX_KOD      },
	{"kodja",      BATTRY_2, 0,                    GFX_KOD      },
#if !RELEASE
	{"kodb",       BATTRY_2, 0,                    GFX_KOD      },
	{"kodh",       NOBATTRY, 0,                    GFX_KOD      },
#endif
	{"captcomm",   BATTRY_3, 0,                    GFX_CAPTCOMM },
	{"captcommr1", BATTRY_3, 0,                    GFX_CAPTCOMM },
	{"captcommu",  BATTRY_3, 0,                    GFX_CAPTCOMM },
	{"captcommj",  BATTRY_3, 0,                    GFX_CAPTCOMM },
	{"captcommjr1",BATTRY_3, 0,                    GFX_CAPTCOMM },
	{"captcommb",  BATTRY_3, 0,                    GFX_CAPTCOMM },
	{"knights",    BATTRY_4, 0,                    GFX_KNIGHTS  },
	{"knightsu",   BATTRY_4, 0,                    GFX_KNIGHTS  },
	{"knightsj",   BATTRY_4, 0,                    GFX_KNIGHTS  },
	{"knightsja",  BATTRY_4, 0,                    GFX_KNIGHTS  },
#if !RELEASE
	{"knightsb",   BATTRY_4, 0,                    GFX_KNIGHTS  },
	{"knightsb2",  BATTRY_4, CPS1_KLUDGE_KNIGHTSB, GFX_KNIGHTS  },
	{"knightsh",   NOBATTRY, 0,                    GFX_KNIGHTS  },
	{"knightsi",   BATTRY_4, 0,                    GFX_KNIGHTS  },
	{"knightsr",   BATTRY_4, 0,                    GFX_KNIGHTS  },
#endif
	{"sf2ce",      NOBATTRY, 0,                    GFX_SF2      },
	{"sf2ceea",    NOBATTRY, 0,                    GFX_SF2      },
	{"sf2ceua",    NOBATTRY, 0,                    GFX_SF2      },
	{"sf2ceub",    NOBATTRY, 0,                    GFX_SF2      },
	{"sf2ceuc",    NOBATTRY, 0,                    GFX_SF2      },
	{"sf2ceja",    NOBATTRY, 0,                    GFX_SF2      },
	{"sf2cejb",    NOBATTRY, 0,                    GFX_SF2      },
	{"sf2cejc",    NOBATTRY, 0,                    GFX_SF2      },
#if !RELEASE
	{"sf2bhh",     NOBATTRY, 0,                    GFX_SF2      },
	{"sf2rb",      NOBATTRY, 0,                    GFX_SF2      },
	{"sf2rb2",     NOBATTRY, 0,                    GFX_SF2      },
	{"sf2rb3",     NOBATTRY, 0,                    GFX_SF2      },
	{"sf2red",     NOBATTRY, 0,                    GFX_SF2      },
	{"sf2v004",    NOBATTRY, 0,                    GFX_SF2      },
	{"sf2acc",     NOBATTRY, 0,                    GFX_SF2      },
	{"sf2acca",    NOBATTRY, 0,                    GFX_SF2      },
	{"sf2accp2",   NOBATTRY, 0,                    GFX_SF2      },
	{"sf2amf",     NOBATTRY, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2dkot2",   NOBATTRY, 0,                    GFX_SF2      },
	{"sf2m1",      NOBATTRY, 0,                    GFX_SF2      },
	{"sf2m2",      NOBATTRY, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2m3",      HACK_B_3, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2m4",      HACK_B_1, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2m5",      NOBATTRY, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2m6",      NOBATTRY, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2m7",      NOBATTRY, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2m8",      HACK_B_3, 0,                    GFX_SF2      },
	{"sf2dongb",   NOBATTRY, 0,                    GFX_SF2      },
	{"sf2m9",      NOBATTRY, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2m10",     NOBATTRY, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2m11",     HACK_B_1, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2m12",     HACK_B_1, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2m13",     NOBATTRY, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2m14",     HACK_B_3, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2yyc",     NOBATTRY, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2mdt",     NOBATTRY, 0,   GFX_SF2      },
	{"sf2mdta",    NOBATTRY, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2koryu",   NOBATTRY, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2tlona",   NOBATTRY, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
	{"sf2tlonb",   NOBATTRY, CPS1_KLUDGE_SF2CEB,   GFX_SF2      },
#endif
	{"varth",      CPS_B_04, 0,                    GFX_VARTH    },
	{"varthr1",    CPS_B_04, 0,                    GFX_VARTH    },
	{"varthu",     CPS_B_04, 0,                    GFX_VARTH    },
	{"varthj",     BATTRY_5, 0,                    GFX_VARTH    },
	{"cworld2j",   BATTRY_6, 0,                    GFX_CWORLD2J },
	{"wof",        QSOUND_1, 0,                    GFX_WOF      },
	{"wofr1",      NOBATTRY, 0,                    GFX_WOF      },
	{"wofa",       NOBATTRY, 0,                    GFX_WOF      },
	{"wofu",       QSOUND_1, 0,                    GFX_WOF      },
	{"wofj",       QSOUND_1, 0,                    GFX_WOF      },
#if !RELEASE
	{"wofb",       NOBATTRY, 0,                    GFX_WOF      },
	{"wofchs",     NOBATTRY, 0,                    GFX_WOF      },
	{"wofh",       HACK_B_2, CPS1_KLUDGE_WOFB,     GFX_WOF      },
	{"wofha",      HACK_B_2, CPS1_KLUDGE_WOFB,     GFX_WOF      },
	{"wofhfh",     NOBATTRY, 0,                    GFX_WOF      },
	{"wof3js",     CPS_B_01, 0,                    GFX_WOF      },
	{"wof3sj",     HACK_B_2, CPS1_KLUDGE_WOFB,     GFX_WOF      },
	{"wof3sja",    HACK_B_2, CPS1_KLUDGE_WOFB,     GFX_WOF      },
	{"wofsj",      HACK_B_2, CPS1_KLUDGE_WOFB,     GFX_WOF      },
	{"wofsja",     HACK_B_2, CPS1_KLUDGE_WOFB,     GFX_WOF      },
	{"wofsjb",     CPS_B_01, 0,                    GFX_WOF      },
	{"wofth",      QSOUND_1, 0,                    GFX_WOF      },
	{"wofjjq",     QSOUND_1, 0,                    GFX_WOF      },
	{"woffh",      QSOUND_1, 0,                    GFX_WOF      },
	{"wofzhs",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofbio",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofctj",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofctm",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofgyc",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofgyq",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofhzj",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofjqz",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofsjj",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofyjs",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofzyq",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofzy2",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofzy3",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofsbl",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofsz",      QSOUND_1, 0,                    GFX_WOF      },
	{"wofdy",      QSOUND_1, 0,                    GFX_WOF      },
	{"wofbt",      QSOUND_1, 0,                    GFX_WOF      },
	{"wofhgz",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofdyg",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofyxg",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofzhy",     QSOUND_1, 0,                    GFX_WOF      },
	{"wofmr",      QSOUND_1, 0,                    GFX_WOF      },
	{"wof5boss",   QSOUND_1, 0,                    GFX_WOF      },
#endif
	{"dino",       QSOUND_2, 0,                    GFX_DINO     },
	{"dinou",      QSOUND_2, 0,                    GFX_DINO     },
	{"dinoj",      QSOUND_2, 0,                    GFX_DINO     },
#if !RELEASE
	{"dinob",      QSOUND_2, 0,                    GFX_DINO     },
	{"dinoh",      NOBATTRY, 0,                    GFX_DINO     },
	{"dinoha",     NOBATTRY, 0,                    GFX_DINO     },
	{"dinohb",     QSOUND_2, 0,                    GFX_DINO     },
	{"dinohunt",   NOBATTRY, 0,                    GFX_DINO     },
#endif
	{"punisher",   QSOUND_3, 0,                    GFX_PUNISHER },
	{"punisheru",  QSOUND_3, 0,                    GFX_PUNISHER },
	{"punisherh",  QSOUND_3, 0,                    GFX_PUNISHER },
	{"punisherj",  QSOUND_3, 0,                    GFX_PUNISHER },
#if !RELEASE
	{"punisherbz", NOBATTRY, 0,                    GFX_PUNISHER },
#endif
	{"slammast",   QSOUND_4, 0,                    GFX_SLAMMAST },
	{"slammastu",  QSOUND_4, 0,                    GFX_SLAMMAST },
#if !RELEASE
//	{"slammasa",   QSOUND_4, 0,                    GFX_SLAMMAST },
//	{"slammash",   QSOUND_4, 0,                    GFX_SLAMMAST },
#endif
	{"mbomberj",   QSOUND_4, 0,                    GFX_SLAMMAST },
	{"mbombrd",    QSOUND_5, 0,                    GFX_SLAMMAST },
	{"mbombrdj",   QSOUND_5, 0,                    GFX_SLAMMAST },
	{"sf2hf",      NOBATTRY, 0,                    GFX_SF2HF    },
	{"sf2hfu",     NOBATTRY, 0,                    GFX_SF2HF    },
	{"sf2hfj",     NOBATTRY, 0,                    GFX_SF2HF    },
	{"qad",        BATTRY_7, 0,                    GFX_QAD      },
	{"qadjr",      NOBATTRY, 0,                    GFX_QADJ     },
	{"qtono2j",    NOBATTRY, 0,                    GFX_QTONO2   },
	{"megaman",    NOBATTRY, 0,                    GFX_MEGAMAN  },
	{"megamana",   NOBATTRY, 0,                    GFX_MEGAMAN  },
	{"rockmanj",   NOBATTRY, 0,                    GFX_MEGAMAN  },
	{"pnickj",     NOBATTRY, 0,                    GFX_PUNICKJ  },
	{"pnicku",     NOBATTRY, 0,                    GFX_PUNICKJ  },
	{"pang3",      NOBATTRY, CPS1_KLUDGE_PANG3,    GFX_PANG3    },
	{"pang3r1",    NOBATTRY, CPS1_KLUDGE_PANG3,    GFX_PANG3    },
	{"pang3j",     NOBATTRY, CPS1_KLUDGE_PANG3,    GFX_PANG3    },
	{"pang3b",     NOBATTRY, CPS1_KLUDGE_PANG3,    GFX_PANG3    },
	{"sfzch",      NOBATTRY, 0,                    GFX_SFZCH    },
	{"sfach",      NOBATTRY, 0,                    GFX_SFZCH    },
	{"wofch",      NOBATTRY, 0,                    GFX_WOF      },
	{NULL}
};

struct driver_t *driver;


/******************************************************************************
	ローカル変数
******************************************************************************/

static UINT32 z80_bank;

static int cps1_sound_fade_timer;
static UINT8 sound_data;


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

TIMER_CALLBACK( cps1_vblank_interrupt )
{
	m68000_set_irq_line(2, HOLD_LINE);
	if (!skip_this_frame())
	{
		cps1_screenrefresh();
		blit_finish();
	}
	cps1_objram_latch();
}


/******************************************************************************
	メモリハンドラ
******************************************************************************/

/*--------------------------------------------------------
	コインコントロール
--------------------------------------------------------*/

WRITE16_HANDLER( cps1_coinctrl_w )
{
	if (ACCESSING_MSB)
	{
		coin_counter_w(0,  data & 0x0100);
		coin_counter_w(1,  data & 0x0200);
		coin_lockout_w(0, ~data & 0x0400);
		coin_lockout_w(1, ~data & 0x0800);
	}
#if 0
	if (ACCESSING_LSB)
	{
		/* mercs sets bit 0 */
		set_led_status(0, data & 0x02);
		set_led_status(1, data & 0x04);
		set_led_status(2, data & 0x08);
	}
#endif
}

WRITE16_HANDLER( cps1_coinctrl2_w )
{
	if (ACCESSING_LSB)
	{
		coin_counter_w(2,  data & 0x01);
		coin_lockout_w(2, ~data & 0x02);
		coin_counter_w(3,  data & 0x04);
		coin_lockout_w(3, ~data & 0x08);
	}
}


/*--------------------------------------------------------
	ディップスイッチリード
--------------------------------------------------------*/

READ16_HANDLER( cps1_dsw_a_r )
{
	return cps1_dipswitch[DIP_A] | (cps1_dipswitch[DIP_A] << 8);
}

READ16_HANDLER( cps1_dsw_b_r )
{
	return cps1_dipswitch[DIP_B] | (cps1_dipswitch[DIP_B] << 8);
}

READ16_HANDLER( cps1_dsw_c_r )
{
	return cps1_dipswitch[DIP_C] | (cps1_dipswitch[DIP_C] << 8);
}


/*--------------------------------------------------------
	入力ポートリード
--------------------------------------------------------*/

READ16_HANDLER( cps1_inputport0_r )
{
	return cps1_port_value[0];
}

READ16_HANDLER( cps1_inputport1_r )
{
	return cps1_port_value[1];
}

READ16_HANDLER( cps1_inputport2_r )
{
	return cps1_port_value[2];
}

READ16_HANDLER( cps1_inputport3_r )
{
	return cps1_port_value[3];
}


/*--------------------------------------------------------
	ロストワールド用 ダイアル入力
--------------------------------------------------------*/

static int dial[2];

READ16_HANDLER( forgottn_dial_0_r )
{
	return ((forgottn_read_dial0() - dial[0]) >> (offset << 3)) & 0xff;
}

READ16_HANDLER( forgottn_dial_1_r )
{
	return ((forgottn_read_dial1() - dial[1]) >> (offset << 3)) & 0xff;
}

WRITE16_HANDLER( forgottn_dial_0_reset_w )
{
	dial[0] = forgottn_read_dial0();
}

WRITE16_HANDLER( forgottn_dial_1_reset_w )
{
	dial[1] = forgottn_read_dial1();
}


/*--------------------------------------------------------
	Sound (YM2151 + OKIM6295)
--------------------------------------------------------*/

WRITE8_HANDLER( cps1_snd_bankswitch_w )
{
	int length = memory_length_cpu2 - 0x10000;
	int bankaddress = (data * 0x4000) & (length - 1);

	z80_set_bank(bankaddress + 0x10000);
}

WRITE8_HANDLER( cps1_oki_pin7_w )
{
	OKIM6295_set_pin7_w(0, (data & 1));
}

READ8_HANDLER( cps1_sound_fade_timer_r )
{
	return cps1_sound_fade_timer;
}

WRITE16_HANDLER( cps1_sound_fade_timer_w )
{
	if (ACCESSING_LSB)
		cps1_sound_fade_timer = data & 0xff;
}

READ8_HANDLER( cps1_sound_command_r )
{
	// Z80
	return sound_data;
}

WRITE16_HANDLER( cps1_sound_command_w )
{
#if !RELEASE
	if (machine_init_type == INIT_dinoh
	 || machine_init_type == INIT_dinohunt)
	{
		/* Pass the Sound Code to the Q-Sound Shared Ram */
		qsound_sharedram1[0x0001] = data;
	}
	else
#endif
	{
		// M68000
		if (ACCESSING_LSB) sound_data = data & 0xff;
	}
}

void cps1_sound_interrupt(int state)
{
	z80_set_irq_line(0, state ? ASSERT_LINE : CLEAR_LINE);
}


/*--------------------------------------------------------
	Sound (Q Sound)
--------------------------------------------------------*/

READ16_HANDLER( qsound_rom_r )
{
	if (!memory_region_user1) return 0;

	offset &= 0x7fff;
	return memory_region_user1[offset] | 0xff00;
}

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

READ16_HANDLER( qsound_sharedram2_r )
{
	offset &= 0xfff;
	return qsound_sharedram2[offset] | 0xff00;
}

WRITE16_HANDLER( qsound_sharedram2_w )
{
	if (ACCESSING_LSB)
	{
		offset &= 0xfff;
		qsound_sharedram2[offset] = data;
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


/*--------------------------------------------------------
	EEPROM
--------------------------------------------------------*/

static struct EEPROM_interface qsound_eeprom_interface =
{
	7,		/* address bits */
	8,		/* data bits */
	"0110",	/*  read command */
	"0101",	/* write command */
	"0111"	/* erase command */
};

static struct EEPROM_interface pang3_eeprom_interface =
{
	6,		/* address bits */
	16,		/* data bits */
	"0110",	/*  read command */
	"0101",	/* write command */
	"0111"	/* erase command */
};

static void cps1_nvram_read_write(int read_or_write)
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

READ16_HANDLER( cps1_eeprom_port_r )
{
	return EEPROM_read_bit();
}

WRITE16_HANDLER( cps1_eeprom_port_w )
{
	if (ACCESSING_LSB)
	{
		/*
			bit 0 = data
			bit 6 = clock
			bit 7 = cs
		*/
		EEPROM_write_bit(data & 0x01);
		EEPROM_set_cs_line((data & 0x80) ? CLEAR_LINE : ASSERT_LINE);
		EEPROM_set_clock_line((data & 0x40) ? ASSERT_LINE : CLEAR_LINE);
	}
}


/******************************************************************************
	Pang!3 (Japan) ROM暗号化解除
******************************************************************************/

void pang3_decode(void)
{
	UINT16 *rom = (UINT16 *)memory_region_cpu1;
	int addr, src, dst;

	for (addr = 0x80000; addr < 0x100000; addr += 2)
	{
		/* only the low 8 bits of each word are encrypted */
		src = rom[addr / 2];
		dst = src & 0xff00;
		if ( src & 0x01) dst ^= 0x04;
		if ( src & 0x02) dst ^= 0x21;
		if ( src & 0x04) dst ^= 0x01;
		if (~src & 0x08) dst ^= 0x50;
		if ( src & 0x10) dst ^= 0x40;
		if ( src & 0x20) dst ^= 0x06;
		if ( src & 0x40) dst ^= 0x08;
		if (~src & 0x80) dst ^= 0x88;
		rom[addr / 2] = dst;
	}
}


/******************************************************************************
	bootleg パッチ
******************************************************************************/

#if !RELEASE
void kodb_init(void)
{
	UINT8 *mem8 = memory_region_cpu1;

	// Patch protection? check
	mem8[0x00412] = 0x06;
	mem8[0x00414] = 0x64;
	mem8[0x00415] = 0x71;
	mem8[0x00c44] = 0x1c;
	mem8[0x00c45] = 0x10;
	mem8[0x00c46] = 0x88;
	mem8[0x00c47] = 0xef;
	mem8[0x00c48] = 0x1c;
	mem8[0x00c49] = 0x12;
	mem8[0x034fe] = 0x7c;
	mem8[0x034ff] = 0x22;
	mem8[0x03500] = 0x90;
	mem8[0x03502] = 0x00;
	mem8[0x03503] = 0x00;
	mem8[0x036aa] = 0xfa;
	mem8[0x036ab] = 0x45;
	mem8[0x036ac] = 0x2e;
	mem8[0x036ae] = 0x1a;
	mem8[0x036af] = 0x30;
	mem8[0x04999] = 0x49;
	mem8[0x0499a] = 0x90;
	mem8[0x0499c] = 0x00;
	mem8[0x0499d] = 0x00;
	mem8[0x05e64] = 0x00;
	mem8[0x05e65] = 0x61;
	mem8[0x05e66] = 0x3e;
	mem8[0x05e68] = 0x6d;
	mem8[0x05e69] = 0x08;
	mem8[0x05e92] = 0x00;
	mem8[0x05e93] = 0x00;
	mem8[0x05ea0] = 0x00;
	mem8[0x05ea1] = 0x40;
	mem8[0x0610a] = 0xc2;
	mem8[0x0610b] = 0x38;
	mem8[0x0610c] = 0xc3;
	mem8[0x0610d] = 0x38;
	mem8[0x0610e] = 0xe9;
	mem8[0x0610f] = 0x38;
	mem8[0x06168] = 0xc2;
	mem8[0x06169] = 0x38;
	mem8[0x0616a] = 0xc3;
	mem8[0x0616b] = 0x38;
	mem8[0x0616c] = 0xc0;
	mem8[0x0616d] = 0x38;
	mem8[0x0616e] = 0xc4;
	mem8[0x0616f] = 0x38;
	mem8[0x06188] = 0xc2;
	mem8[0x06189] = 0x38;
	mem8[0x0618a] = 0xc3;
	mem8[0x0618b] = 0x38;
	mem8[0x0618c] = 0xc0;
	mem8[0x0618d] = 0x38;
	mem8[0x0618e] = 0xc4;
	mem8[0x0618f] = 0x38;
	mem8[0x061a0] = 0xc2;
	mem8[0x061a1] = 0x38;
	mem8[0x061a2] = 0xc3;
	mem8[0x061a3] = 0x38;
	mem8[0x061a4] = 0xc0;
	mem8[0x061a5] = 0x38;
	mem8[0x061a6] = 0xc4;
	mem8[0x061a7] = 0x38;
	mem8[0x061b8] = 0xc2;
	mem8[0x061b9] = 0x38;
	mem8[0x061ba] = 0xc3;
	mem8[0x061bb] = 0x38;
	mem8[0x061bc] = 0xc0;
	mem8[0x061bd] = 0x38;
	mem8[0x061be] = 0xc4;
	mem8[0x061bf] = 0x38;
	mem8[0x06232] = 0xc0;
	mem8[0x06233] = 0x38;
	mem8[0x06234] = 0xc3;
	mem8[0x06235] = 0x38;
	mem8[0x06236] = 0xc1;
	mem8[0x06237] = 0x38;
	mem8[0x06238] = 0xc4;
	mem8[0x06239] = 0x38;
	mem8[0x0623a] = 0x6c;
	mem8[0x0623b] = 0x02;
	mem8[0x0623c] = 0xff;
	mem8[0x0623e] = 0xfe;
	mem8[0x0623f] = 0xff;
	mem8[0x06284] = 0xc0;
	mem8[0x06285] = 0x38;
	mem8[0x06286] = 0xc3;
	mem8[0x06287] = 0x38;
	mem8[0x06288] = 0xc1;
	mem8[0x06289] = 0x38;
	mem8[0x0628a] = 0xc4;
	mem8[0x0628b] = 0x38;
	mem8[0x062ce] = 0xc0;
	mem8[0x062cf] = 0x38;
	mem8[0x062d0] = 0xc3;
	mem8[0x062d1] = 0x38;
	mem8[0x062d2] = 0xc1;
	mem8[0x062d3] = 0x38;
	mem8[0x062d4] = 0xc4;
	mem8[0x062d5] = 0x38;
	mem8[0x06318] = 0xc0;
	mem8[0x06319] = 0x38;
	mem8[0x0631a] = 0xc3;
	mem8[0x0631b] = 0x38;
	mem8[0x0631c] = 0xc1;
	mem8[0x0631d] = 0x38;
	mem8[0x0631e] = 0xc4;
	mem8[0x0631f] = 0x38;
	mem8[0x063b0] = 0xc0;
	mem8[0x063b1] = 0x38;
	mem8[0x063b2] = 0xc3;
	mem8[0x063b3] = 0x38;
	mem8[0x063b4] = 0xc1;
	mem8[0x063b5] = 0x38;
	mem8[0x063b6] = 0xd9;
	mem8[0x063b7] = 0x38;
	mem8[0x0640a] = 0xc0;
	mem8[0x0640b] = 0x38;
	mem8[0x0640c] = 0xc3;
	mem8[0x0640d] = 0x38;
	mem8[0x0640e] = 0xc1;
	mem8[0x0640f] = 0x38;
	mem8[0x06410] = 0xd9;
	mem8[0x06411] = 0x38;
	mem8[0x0645c] = 0xc0;
	mem8[0x0645d] = 0x38;
	mem8[0x0645e] = 0xc3;
	mem8[0x0645f] = 0x38;
	mem8[0x06460] = 0xc1;
	mem8[0x06461] = 0x38;
	mem8[0x06462] = 0xd9;
	mem8[0x06463] = 0x38;
	mem8[0x064ae] = 0xc0;
	mem8[0x064af] = 0x38;
	mem8[0x064b0] = 0xc3;
	mem8[0x064b1] = 0x38;
	mem8[0x064b2] = 0xc1;
	mem8[0x064b3] = 0x38;
	mem8[0x064b4] = 0xd9;
	mem8[0x064b5] = 0x38;
	mem8[0x0657e] = 0xc0;
	mem8[0x0657f] = 0x38;
	mem8[0x06580] = 0xc3;
	mem8[0x06581] = 0x38;
	mem8[0x06582] = 0xc1;
	mem8[0x06583] = 0x38;
	mem8[0x06584] = 0xc4;
	mem8[0x06585] = 0x38;
	mem8[0x065d4] = 0xc0;
	mem8[0x065d5] = 0x38;
	mem8[0x065d6] = 0xc3;
	mem8[0x065d7] = 0x38;
	mem8[0x065d8] = 0xc1;
	mem8[0x065d9] = 0x38;
	mem8[0x065da] = 0xc4;
	mem8[0x065db] = 0x38;
	mem8[0x06622] = 0xc0;
	mem8[0x06623] = 0x38;
	mem8[0x06624] = 0xc3;
	mem8[0x06625] = 0x38;
	mem8[0x06626] = 0xc1;
	mem8[0x06627] = 0x38;
	mem8[0x06628] = 0xc4;
	mem8[0x06629] = 0x38;
	mem8[0x06670] = 0xc0;
	mem8[0x06671] = 0x38;
	mem8[0x06672] = 0xc3;
	mem8[0x06673] = 0x38;
	mem8[0x06674] = 0xc1;
	mem8[0x06675] = 0x38;
	mem8[0x06676] = 0xc4;
	mem8[0x06677] = 0x38;
	mem8[0x06708] = 0xc1;
	mem8[0x06709] = 0x38;
	mem8[0x0670a] = 0xc2;
	mem8[0x0670b] = 0x38;
	mem8[0x0670c] = 0xc3;
	mem8[0x0670d] = 0x38;
	mem8[0x06870] = 0xc2;
	mem8[0x06871] = 0x38;
	mem8[0x06872] = 0xc3;
	mem8[0x06873] = 0x38;
	mem8[0x06874] = 0xfc;
	mem8[0x06875] = 0x38;
	mem8[0x09d96] = 0x69;
	mem8[0x09d97] = 0x42;
	mem8[0x09d98] = 0x4;
	mem8[0x09d9a] = 0x69;
	mem8[0x09d9b] = 0x42;
	mem8[0x0e1a6] = 0x00;
	mem8[0x0e1a7] = 0x7a;
	mem8[0x0e1a8] = 0x2d;
	mem8[0x0e1a9] = 0x1a;
	mem8[0x0e1aa] = 0x49;
	mem8[0x0e1ab] = 0x8e;
	mem8[0x69958] = 0x91;
	mem8[0x69959] = 0x42;
	mem8[0x6995a] = 0xe9;
	mem8[0x6995b] = 0x43;
	mem8[0x6995c] = 0x80;
	mem8[0x6995d] = 0x00;
	mem8[0x7a12e] = 0x80;
	mem8[0x7a130] = 0x6c;
	mem8[0x7a131] = 0x01;
	mem8[0x7a134] = 0x80;
	mem8[0x7a136] = 0x6a;
	mem8[0x7a137] = 0x01;
}

void sf2m3_init(void)
{
	UINT8 *mem8 = (UINT8 *)memory_region_cpu1;

	mem8[0x5E8] = 0x8;
	mem8[0x608] = 0x14;
	mem8[0x610] = 0xC;
	mem8[0x618] = 0x10;
	mem8[0x620] = 0x12;
	mem8[0x628] = 0xE;
	mem8[0x630] = 0x16;
	mem8[0x638] = 0x20;
}

void sf2m13_init(void)
{
	UINT16 *rom = (UINT16 *)memory_region_cpu1;

	// Fix scroll
	rom[0x1d22a/2] = 0x0120;
	// Fix bg layer
	rom[0x1d270/2] = 0x0166;
	// Fix gfx
	rom[0x1d470/2] = 0x0170;

	// Fix title gfx
	rom[0x21bec/2] = 0x0083;
	rom[0x21cf8/2] = 0x828e;
	rom[0x21cfa/2] = 0x829e;
	rom[0x21cfc/2] = 0x82ae;
	rom[0x21d06/2] = 0x827f;
	rom[0x21d08/2] = 0x828f;
	rom[0x21d0a/2] = 0x829f;
	rom[0x21d0c/2] = 0x82af;
	rom[0x21d16/2] = 0x827e;
	rom[0x21d18/2] = 0x82a0;
	rom[0x21d1a/2] = 0x822c;
	rom[0x21d1c/2] = 0x823c;
	rom[0x21d2a/2] = 0x822d;
}

void wofb_init(void)
{
	UINT8 *mem8 = memory_region_cpu1;
	// Fix gfx
	mem8[0x506] = 0xE7;
	mem8[0x507] = 0x48;
	mem8[0x508] = 0xFE;
	mem8[0x509] = 0xFF;
	mem8[0x50A] = 0xF8;
	mem8[0x50B] = 0x4B;
	mem8[0x6ABC] = 0x6;
	mem8[0x6ABD] = 0x1;
	// Fix sound
	mem8[0x764E] = 0x71;
	mem8[0x7650] = 0x71;
	mem8[0x7651] = 0x4E;
	// Fix screen transitions
	mem8[0x5D236] = 0xFC;
	mem8[0x5D237] = 0x28;
	mem8[0x5D238] = 0x0;
	mem8[0x5D23B] = 0x1;
}

void wofsjb_init(void)
{
	UINT8 *mem8 = memory_region_cpu1;
	
	// Fix sprites update
	mem8[0x532] = 0xED;
	mem8[0x533] = 0x33;
	mem8[0x534] = 0xD2;
	mem8[0x535] = 0xE3;
	mem8[0x536] = 0x80;
	mem8[0x537] = 0x00;
	mem8[0x538] = 0x00;
	mem8[0x539] = 0x01;

	// Patch Q sound protection? check
	mem8[0x5a1a] = 0x00;
	mem8[0x5a1b] = 0x67;
	mem8[0x5a1c] = 0x56;
	mem8[0x5a1d] = 0x00;
	mem8[0x5a1e] = 0x7c;
	mem8[0x5a1f] = 0x20;
	mem8[0x5a20] = 0xf1;
	mem8[0x5a21] = 0x00;
	mem8[0x5a22] = 0x00;
	mem8[0x5a23] = 0x80;
	mem8[0x5a24] = 0x28;
	mem8[0x5a25] = 0x4a;
	mem8[0x5a26] = 0x1f;
	mem8[0x5a27] = 0x00;
	mem8[0x5a28] = 0x00;
	mem8[0x5a29] = 0x6a;
	mem8[0x5a40] = 0x5c;
	mem8[0x5a41] = 0x11;
	mem8[0x5a42] = 0x01;
	mem8[0x5a43] = 0x00;
	mem8[0x5a44] = 0x5c;
	mem8[0x5a45] = 0x11;
	mem8[0x5a46] = 0x03;
	mem8[0x5a47] = 0x00;
	mem8[0x5a4a] = 0x07;
	mem8[0x5a4b] = 0x00;
	mem8[0x5a4c] = 0x5c;
	mem8[0x5a4d] = 0x11;
	mem8[0x5a4e] = 0x09;
	mem8[0x5a4f] = 0x00;
	mem8[0x5a50] = 0x5c;
	mem8[0x5a51] = 0x31;
	mem8[0x5a52] = 0x0c;
	mem8[0x5a53] = 0x00;
	mem8[0x5a54] = 0x5c;
	mem8[0x5a55] = 0x11;
	mem8[0x5a56] = 0x0f;
	mem8[0x5a57] = 0x00;
	mem8[0x5a58] = 0x5c;
	mem8[0x5a59] = 0x11;
	mem8[0x72a6] = 0x71;
	mem8[0x72a7] = 0x4e;
	mem8[0x72a8] = 0x71;
	mem8[0x72a9] = 0x4e;
	mem8[0x72aa] = 0x71;
	mem8[0x72ab] = 0x4e;
	mem8[0x72ac] = 0x71;
	mem8[0x72ad] = 0x4e;
	mem8[0x72ae] = 0x71;
	mem8[0x72af] = 0x4e;
	mem8[0x72b0] = 0x71;
	mem8[0x72b1] = 0x4e;
	mem8[0x72b2] = 0x39;
	mem8[0x72b3] = 0x30;
	mem8[0x72b4] = 0xf1;
	mem8[0x72b5] = 0x00;
	mem8[0x72b6] = 0xfe;
	mem8[0x72b7] = 0x9f;
	mem8[0x72b8] = 0x00;
	mem8[0x72b9] = 0x0c;

	wof_decode();
}

void wof3js_init(void)
{
	UINT8 *mem8 = memory_region_cpu1;

	// Patch Q sound protection? check
	mem8[0x0554] = 0xb4;
	mem8[0x0555] = 0x54;
	mem8[0x5a50] = 0x5c;
	mem8[0x5a51] = 0x31;
	mem8[0x5a52] = 0x0c;
	mem8[0x5a53] = 0x00;
	mem8[0x5a54] = 0x5c;
	mem8[0x5a55] = 0x11;
	mem8[0x5a56] = 0x0f;
	mem8[0x5a58] = 0x5c;
	mem8[0x5a59] = 0x11;
	mem8[0x5a5a] = 0x11;
	mem8[0x5a5b] = 0x00;

	wof_decode();
}

void wof3sj_init(void)
{
	UINT8 *mem8 = memory_region_cpu1;

	// Disable Sprite Recoding
	mem8[0x5de96] = 0x00;
	mem8[0x5de97] = 0x61;
	mem8[0x5de98] = 0xe6;
	mem8[0x5de99] = 0x01;
	mem8[0x5dece] = 0x6d;
	mem8[0x5decf] = 0x0c;
	mem8[0x5ded0] = 0x00;
	mem8[0x5ded1] = 0x90;
	mem8[0x5ded2] = 0xd2;
	mem8[0x5ded3] = 0xe3;
	mem8[0x5df26] = 0x90;
	mem8[0x5df27] = 0x00;
	mem8[0x5df28] = 0x00;
	mem8[0x5df29] = 0x00;
	mem8[0x5df36] = 0x90;
	mem8[0x5df37] = 0x00;
	mem8[0x5df38] = 0x00;
	mem8[0x5df39] = 0x40;
	// Patch Q sound protection? check
	mem8[0x05a0e] = 0x39;
	mem8[0x05a0f] = 0x30;
	mem8[0x05a10] = 0xf1;
	mem8[0x05a12] = 0xfe;
	mem8[0x05a13] = 0x9f;
	mem8[0x05a14] = 0x00;
	mem8[0x05a15] = 0x0c;
	mem8[0x05a16] = 0x77;
	mem8[0x05a17] = 0x00;
	mem8[0x05a18] = 0xf4;
	mem8[0x05a19] = 0x66;
	mem8[0x05a1a] = 0xf9;
	mem8[0x05a1b] = 0x4e;
	mem8[0x05a1c] = 0x00;
	mem8[0x05a1e] = 0xbe;
	mem8[0x05a1f] = 0x72;
	mem8[0x05a20] = 0x7c;
	mem8[0x05a21] = 0x20;
	mem8[0x05a22] = 0xf1;
	mem8[0x05a23] = 0x00;
	mem8[0x05a24] = 0x00;
	mem8[0x05a25] = 0x80;
	mem8[0x05a26] = 0x7c;
	mem8[0x05a27] = 0x11;
	mem8[0x05a29] = 0x00;
	mem8[0x05a2a] = 0x01;
	mem8[0x05a2c] = 0x7c;
	mem8[0x05a2d] = 0x11;
	mem8[0x05a2e] = 0x10;
	mem8[0x05a2f] = 0x00;
	mem8[0x05a30] = 0x0f;
	mem8[0x05a31] = 0x00;
	mem8[0x05a32] = 0x7c;
	mem8[0x05a33] = 0x11;
	mem8[0x05a34] = 0x00;
	mem8[0x05a35] = 0x00;
	mem8[0x05a36] = 0x07;
	mem8[0x05a38] = 0x7c;
	mem8[0x05a39] = 0x11;
	mem8[0x05a3a] = 0x00;
	mem8[0x05a3b] = 0x00;
	mem8[0x05a3c] = 0x09;
	mem8[0x05a3d] = 0x00;
	mem8[0x05a3e] = 0x7c;
	mem8[0x05a3f] = 0x11;
	mem8[0x05a40] = 0x00;
	mem8[0x05a41] = 0x00;
	mem8[0x05a42] = 0x0c;
	mem8[0x05a44] = 0x41;
	mem8[0x05a48] = 0x7c;
	mem8[0x05a4a] = 0x00;
	mem8[0x05a4c] = 0x11;
	mem8[0x05a4d] = 0x00;
	mem8[0x05a4e] = 0x7c;
	mem8[0x05a4f] = 0x11;
	mem8[0x05a50] = 0x00;
	mem8[0x05a51] = 0x00;
	mem8[0x05a52] = 0x17;
	mem8[0x05a54] = 0x7c;
	mem8[0x05a56] = 0x00;
	mem8[0x05a58] = 0x19;
	mem8[0x05a59] = 0x00;
	mem8[0x05a5a] = 0x7c;
	mem8[0x05a5b] = 0x11;
	mem8[0x05a5c] = 0x00;
	mem8[0x05a5d] = 0x00;
	mem8[0x05a5e] = 0x13;
	mem8[0x05a60] = 0x7c;
	mem8[0x05a62] = 0x00;
	mem8[0x05a64] = 0x15;
	mem8[0x05a65] = 0x00;
	mem8[0x05a66] = 0x7c;
	mem8[0x05a67] = 0x11;
	mem8[0x05a68] = 0x00;
	mem8[0x05a69] = 0x00;
	mem8[0x05a6a] = 0x1f;
	mem8[0x05a6c] = 0xf9;
	mem8[0x05a6d] = 0x4e;
	mem8[0x05a6e] = 0x05;
	mem8[0x05a70] = 0x6a;
	mem8[0x05a71] = 0xee;
	mem8[0x072a6] = 0xf9;
	mem8[0x072a8] = 0x00;
	mem8[0x072a9] = 0x00;
	mem8[0x072aa] = 0x0e;
	mem8[0x072ab] = 0x5a;
	mem8[0x5ee64] = 0xf9;
	mem8[0x5ee65] = 0x4e;
	mem8[0x5ee66] = 0x00;
	mem8[0x5ee68] = 0x20;
	mem8[0x5ee69] = 0x5a;

	wof_decode();
}

void wofh_init(void)
{
	UINT8 *mem8 = memory_region_cpu1;

	// Stage Order
	mem8[0x072a6] = 0x00;
	// Disable Sprite Recoding
	mem8[0x5d858] = 0x00;
	mem8[0x5d859] = 0x61;
	mem8[0x5d85a] = 0xe6;
	mem8[0x5d85b] = 0x01;
	mem8[0x5d890] = 0x6d;
	mem8[0x5d891] = 0x0c;
	mem8[0x5d892] = 0x00;
	mem8[0x5d893] = 0x90;
	mem8[0x5d894] = 0xd2;
	mem8[0x5d895] = 0xe3;
	mem8[0x5d8e8] = 0x90;
	mem8[0x5d8e9] = 0x00;
	mem8[0x5d8ea] = 0x00;
	mem8[0x5d8eb] = 0x00;
	mem8[0x5d8f8] = 0x90;
	mem8[0x5d8f9] = 0x00;
	mem8[0x5d8fa] = 0x00;
	mem8[0x5d8fb] = 0x40;
	// Patch protection? check
	mem8[0xf11ec] = 0x71;
	mem8[0xf11ed] = 0x4e;
	mem8[0xf11ee] = 0x71;
	mem8[0xf11ef] = 0x4e;
	// Patch Q sound protection? check
	mem8[0x05a0e] = 0x39;
	mem8[0x05a0f] = 0x30;
	mem8[0x05a10] = 0xf1;
	mem8[0x05a12] = 0xfe;
	mem8[0x05a13] = 0x9f;
	mem8[0x05a14] = 0x00;
	mem8[0x05a15] = 0x0c;
	mem8[0x05a16] = 0x77;
	mem8[0x05a17] = 0x00;
	mem8[0x05a18] = 0xf4;
	mem8[0x05a19] = 0x66;
	mem8[0x05a1a] = 0xf9;
	mem8[0x05a1b] = 0x4e;
	mem8[0x05a1c] = 0x00;
	mem8[0x05a1e] = 0xb2;
	mem8[0x05a1f] = 0x72;
	mem8[0x05a20] = 0x7c;
	mem8[0x05a21] = 0x20;
	mem8[0x05a22] = 0xf1;
	mem8[0x05a23] = 0x00;
	mem8[0x05a24] = 0x00;
	mem8[0x05a25] = 0x80;
	mem8[0x05a26] = 0x7c;
	mem8[0x05a27] = 0x11;
	mem8[0x05a29] = 0x00;
	mem8[0x05a2a] = 0x01;
	mem8[0x05a2c] = 0x7c;
	mem8[0x05a2d] = 0x11;
	mem8[0x05a2e] = 0x10;
	mem8[0x05a2f] = 0x00;
	mem8[0x05a30] = 0x0f;
	mem8[0x05a31] = 0x00;
	mem8[0x05a32] = 0x7c;
	mem8[0x05a33] = 0x11;
	mem8[0x05a34] = 0x00;
	mem8[0x05a35] = 0x00;
	mem8[0x05a36] = 0x07;
	mem8[0x05a38] = 0x7c;
	mem8[0x05a39] = 0x11;
	mem8[0x05a3a] = 0x00;
	mem8[0x05a3b] = 0x00;
	mem8[0x05a3c] = 0x09;
	mem8[0x05a3d] = 0x00;
	mem8[0x05a3e] = 0x7c;
	mem8[0x05a3f] = 0x11;
	mem8[0x05a40] = 0x00;
	mem8[0x05a41] = 0x00;
	mem8[0x05a42] = 0x0c;
	mem8[0x05a44] = 0x41;
	mem8[0x05a48] = 0x7c;
	mem8[0x05a4a] = 0x00;
	mem8[0x05a4c] = 0x11;
	mem8[0x05a4d] = 0x00;
	mem8[0x05a4e] = 0x7c;
	mem8[0x05a4f] = 0x11;
	mem8[0x05a50] = 0x00;
	mem8[0x05a51] = 0x00;
	mem8[0x05a52] = 0x17;
	mem8[0x05a54] = 0x7c;
	mem8[0x05a56] = 0x00;
	mem8[0x05a58] = 0x19;
	mem8[0x05a59] = 0x00;
	mem8[0x05a5a] = 0x7c;
	mem8[0x05a5b] = 0x11;
	mem8[0x05a5c] = 0x00;
	mem8[0x05a5d] = 0x00;
	mem8[0x05a5e] = 0x13;
	mem8[0x05a60] = 0x7c;
	mem8[0x05a62] = 0x00;
	mem8[0x05a64] = 0x15;
	mem8[0x05a65] = 0x00;
	mem8[0x05a66] = 0x7c;
	mem8[0x05a67] = 0x11;
	mem8[0x05a68] = 0x00;
	mem8[0x05a69] = 0x00;
	mem8[0x05a6a] = 0x1f;
	mem8[0x05a6c] = 0xf9;
	mem8[0x05a6d] = 0x4e;
	mem8[0x05a6e] = 0x05;
	mem8[0x05a70] = 0x2c;
	mem8[0x05a71] = 0xe8;
	mem8[0x0729a] = 0xf9;
	mem8[0x0729c] = 0x00;
	mem8[0x0729d] = 0x00;
	mem8[0x0729e] = 0x0e;
	mem8[0x0729f] = 0x5a;
	mem8[0x5e826] = 0xf9;
	mem8[0x5e827] = 0x4e;
	mem8[0x5e828] = 0x00;
	mem8[0x5e82a] = 0x20;
	mem8[0x5e82b] = 0x5a;

	wof_decode();
}

void dinob_init(void)
{
	UINT8 *mem8 = memory_region_cpu1;

	// Fix draw scroll
	mem8[0x006c2] = 0xc0;
	mem8[0x006c3] = 0xff;

	// Fix sprites update
	mem8[0x554] = 0x3C;
	mem8[0x555] = 0x30;
	mem8[0x556] = 0x00;
	mem8[0x557] = 0x90;
	mem8[0x558] = 0x6D;
	mem8[0x559] = 0x4A;
	mem8[0x55A] = 0xB4;
	mem8[0x55B] = 0x69;
	mem8[0x55C] = 0x00;
	mem8[0x55D] = 0x67;
	mem8[0x55E] = 0x06;
	mem8[0x55F] = 0x00;
	mem8[0x560] = 0x3C;
	mem8[0x561] = 0x30;
	mem8[0x562] = 0x40;
	mem8[0x563] = 0x90;
	mem8[0x564] = 0xC0;
	mem8[0x565] = 0x33;
	mem8[0x566] = 0x80;
	mem8[0x567] = 0x00;
	mem8[0x568] = 0x00;
	mem8[0x569] = 0x01;

	// Fix gfx
	mem8[0x472] = 0xFC;
	mem8[0x473] = 0x33;
	mem8[0x474] = 0x0;
	mem8[0x475] = 0x90;
	mem8[0x476] = 0x80;
	mem8[0x478] = 0x0;
	mem8[0x479] = 0x1;
	mem8[0x47A] = 0xFC;
	mem8[0x47B] = 0x33;
	mem8[0x47C] = 0x80;
	mem8[0x47D] = 0x90;
	mem8[0x47E] = 0x80;
	mem8[0x480] = 0x2;
	mem8[0x481] = 0x1;
	mem8[0x482] = 0xFC;
	mem8[0x483] = 0x33;
	mem8[0x484] = 0xC0;
	mem8[0x485] = 0x90;
	mem8[0x486] = 0x80;
	mem8[0x487] = 0x0;
	mem8[0x488] = 0x4;
	mem8[0x489] = 0x1;
	mem8[0x48A] = 0xFC;
	mem8[0x48B] = 0x33;
	mem8[0x48C] = 0x0;
	mem8[0x48D] = 0x91;
	mem8[0x48E] = 0x80;
	mem8[0x48F] = 0x0;
	mem8[0x490] = 0x6;
	mem8[0x491] = 0x1;
	mem8[0x006cc] = 0x80;
	mem8[0x006cd] = 0x00;
	mem8[0x006ce] = 0x0C;
	mem8[0x006cf] = 0x01;
	mem8[0x006de] = 0x80;
	mem8[0x006df] = 0x00;
	mem8[0x006e0] = 0x10;
	mem8[0x006e1] = 0x01;
	mem8[0x006f0] = 0x80;
	mem8[0x006f1] = 0x00;
	mem8[0x006f2] = 0x14;
	mem8[0x006f3] = 0x01;
	mem8[0x00704] = 0x80;
	mem8[0x00705] = 0x00;
	mem8[0x00706] = 0x0E;
	mem8[0x00707] = 0x01;
	mem8[0x00718] = 0x80;
	mem8[0x00719] = 0x00;
	mem8[0x0071a] = 0x12;
	mem8[0x0071b] = 0x01;
	mem8[0x0072c] = 0x80;
	mem8[0x0072d] = 0x00;
	mem8[0x0072e] = 0x16;
	mem8[0x0072f] = 0x01;
	// Fix screen transitions
	mem8[0x00b28] = 0x00;
	mem8[0x00b29] = 0x70;
	mem8[0x00b2a] = 0x00;
	mem8[0x00b2b] = 0x72;
	mem8[0x00b2c] = 0x3C;
	mem8[0x00b2d] = 0x34;
	mem8[0x00b32] = 0xC1;
	mem8[0x00b33] = 0x20;
	// Fix sound
	mem8[0x00666] = 0xF1;
	mem8[0x00667] = 0x00;
	mem8[0x00668] = 0x02;
	mem8[0x00669] = 0x80;
	mem8[0xaaa6c] = 0xD8;
	mem8[0xaaa6d] = 0x00;

	// Fix change char
	mem8[0x1900da] = 0x18;
	mem8[0x1900f8] = 0x18;

	dino_decode();
}

void dinohunt_init(void)
{
	UINT8 *mem8 = memory_region_cpu1;

    mem8[0xaacf4] = 0x71;
    mem8[0xaacf5] = 0x4e;

    mem8[0x6AA] = 0xD8;
    mem8[0x6AB] = 0x13;
    mem8[0x6AC] = 0xF1;
    mem8[0x6AD] = 0x00;
    mem8[0x6AE] = 0x07;
    mem8[0x6AF] = 0x80;
    mem8[0x6B0] = 0xD8;
    mem8[0x6B1] = 0x13;
    mem8[0x6B2] = 0xF1;
    mem8[0x6B3] = 0x00;
    mem8[0x6B4] = 0x09;
    mem8[0x6B5] = 0x80;

    mem8[0x43A] = 0x71;
    mem8[0x43B] = 0x4E;
    mem8[0x43C] = 0x71;
    mem8[0x43D] = 0x4E;
    mem8[0x43E] = 0x71;
    mem8[0x43F] = 0x4E;
    mem8[0x440] = 0x71;
    mem8[0x441] = 0x4E;

    mem8[0x664] = 0xD8;
    mem8[0x665] = 0x33;
    mem8[0x666] = 0xF1;
    mem8[0x667] = 0x00;
    mem8[0x668] = 0x02;
    mem8[0x669] = 0x80;

    mem8[0x75E] = 0x39;
    mem8[0x75F] = 0x12;
    mem8[0x760] = 0xF1;
    mem8[0x761] = 0x00;
    mem8[0x762] = 0x01;
    mem8[0x763] = 0xC0;

    mem8[0x790] = 0x39;
    mem8[0x791] = 0x12;
    mem8[0x792] = 0xF1;
    mem8[0x793] = 0x00;
    mem8[0x794] = 0x01;
    mem8[0x795] = 0xC0;

    mem8[0x7B4] = 0x79;
    mem8[0x7B5] = 0x1B;
    mem8[0x7B6] = 0xF1;
    mem8[0x7B7] = 0x00;
    mem8[0x7B8] = 0x01;
    mem8[0x7B9] = 0xC0;
    mem8[0x7BA] = 0x18;
    mem8[0x7BB] = 0x00;

	dino_decode();
}

#endif /* RELEASE */


/******************************************************************************
	ドライバインタフェース
******************************************************************************/

int cps1_driver_init(void)
{
	m68000_init();

	z80_init();
	z80_bank = -1;

	if (machine_driver_type == MACHINE_qsound)
	{
		EEPROM_init(&qsound_eeprom_interface);
		cps1_nvram_read_write(0);
	}
	else if (machine_driver_type == MACHINE_pang3)
	{
		EEPROM_init(&pang3_eeprom_interface);
		cps1_nvram_read_write(0);
	}

	return 1;
}


void cps1_driver_exit(void)
{
#ifdef ADHOC
	if (!adhoc_enable || adhoc_server)
#endif
	{
		if (machine_driver_type == MACHINE_qsound
		||	machine_driver_type == MACHINE_pang3)
		{
			cps1_nvram_read_write(1);
		}
	}
}


void cps1_driver_reset(void)
{
	m68000_reset();
	z80_reset();

	coin_counter_reset();

	sound_data = 0x00;
	cps1_sound_fade_timer = 0;
}


/******************************************************************************
	セーブ/ロード ステート
******************************************************************************/

#ifdef SAVE_STATE

STATE_SAVE( driver )
{
	state_save_long(&z80_bank, 1);
	state_save_long(&sound_data, 1);
	state_save_long(&cps1_sound_fade_timer, 1);
}

STATE_LOAD( driver )
{
	int bank;

	state_load_long(&bank, 1);
	state_load_long(&sound_data, 1);
	state_load_long(&cps1_sound_fade_timer, 1);

	z80_bank = 0xffffffff;
	z80_set_bank(bank);

	coin_counter_reset();
}

#endif /* SAVE_STATE */
