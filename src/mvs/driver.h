/******************************************************************************

	driver.c

	MVS ドライバ

******************************************************************************/

#ifndef MVS_DRIVER_H
#define MVS_DRIVER_H

#define NEOGEO_MASTER_CLOCK					(24000000)
#define NEOGEO_PIXEL_CLOCK					(NEOGEO_MASTER_CLOCK / 4)
#define NEOGEO_HTOTAL						(0x180)
#define NEOGEO_HBEND						(0x01e)	/* this should really be 29.5 */
#define NEOGEO_HBSTART						(0x15e) /* this should really be 349.5 */
#define NEOGEO_VTOTAL						(0x108)
#define NEOGEO_VBEND						(0x010)
#define NEOGEO_VBSTART						(0x0f0)
#define NEOGEO_VSSTART						(0x000)
#define NEOGEO_VBLANK_RELOAD_HPOS			(0x11f)

/* VBLANK should fire on line 248 */
#define RASTER_COUNTER_START				0x1f0	/* value assumed right after vblank */
#define RASTER_COUNTER_RELOAD				0x0f8	/* value assumed after 0x1ff */
#define RASTER_LINE_RELOAD					(0x200 - RASTER_COUNTER_START)

#define FLAG_BRAZ	0x1000
#define FLAG_PCB	0x2000

/* Neo Geo games */
/* the four digits number is the game ID stored at address 0x0108 of the program ROM */
/* info on prototypes taken from http://www.members.tripod.com/fresa/proto/puzzle.htm */

#define NGH_nam1975		0x0001		// 0001 (c) 1990 SNK
#define NGH_bstars		0x0002		// 0002 (c) 1990 SNK
#define NGH_tpgolf		0x0003		// 0003 (c) 1990 SNK
#define NGH_mahretsu	0x0004		// 0004 (c) 1990 SNK
#define NGH_maglord		0x0005		// 0005 (c) 1990 Alpha Denshi Co.
#define NGH_ridhero		0x0006		// 0006 (c) 1990 SNK
#define NGH_alpham2		0x0007		// 0007 (c) 1991 SNK
// 0008 Sunshine (prototype) 1990 SNK
#define NGH_ncombat		0x0009		// 0009 (c) 1990 Alpha Denshi Co.
#define NGH_cyberlip	0x0010		// 0010 (c) 1990 SNK
#define NGH_superspy	0x0011		// 0011 (c) 1990 SNK
// 0012
// 0013
#define NGH_mutnat		0x00014		// 0014 (c) 1992 SNK
// 0015
#define NGH_kotm		0x0016		// 0016 (c) 1991 SNK
#define NGH_sengoku		0x0017		// 0017 (c) 1991 SNK
#define NGH_burningf	0x0018		// 0018 (c) 1991 SNK
#define NGH_lbowling	0x0019		// 0019 (c) 1990 SNK
#define NGH_gpilots		0x0020		// 0020 (c) 1991 SNK
#define NGH_joyjoy		0x0021		// 0021 (c) 1990 SNK
#define NGH_bjourney	0x0022		// 0022 (c) 1990 Alpha Denshi Co.
#define NGH_quizdais	0x0023		// 0023 (c) 1991 SNK
#define NGH_lresort		0x0024		// 0024 (c) 1992 SNK
#define NGH_eightman	0x0025		// 0025 (c) 1991 SNK / Pallas
// 0026 Fun Fun Brothers (prototype) 1991 Alpha
#define NGH_minasan		0x0027		// 0027 (c) 1990 Monolith Corp.
// 0028 Dunk Star (prototype) Sammy
#define NGH_legendos	0x0029		// 0029 (c) 1991 SNK
#define NGH_2020bb		0x0030		// 0030 (c) 1991 SNK / Pallas
#define NGH_socbrawl	0x0031		// 0031 (c) 1991 SNK
#define NGH_roboarmy	0x0032		// 0032 (c) 1991 SNK
#define NGH_fatfury1	0x0033		// 0033 (c) 1991 SNK
#define NGH_fbfrenzy	0x0034		// 0034 (c) 1992 SNK
// 0035 Mystic Wand (prototype) 1991 Alpha
#define NGH_bakatono	0x0036		// 0036 (c) 1991 Monolith Corp.
#define NGH_crsword		0x0037		// 0037 (c) 1991 Alpha Denshi Co.
#define NGH_trally		0x0038		// 0038 (c) 1991 Alpha Denshi Co.
#define NGH_kotm2		0x0039		// 0039 (c) 1992 SNK
#define NGH_sengoku2	0x0040		// 0040 (c) 1993 SNK
#define NGH_bstars2		0x0041		// 0041 (c) 1992 SNK
#define NGH_quizdai2	0x0042		// 0042 (c) 1992 SNK
#define NGH_3countb		0x0043		// 0043 (c) 1993 SNK
#define NGH_aof			0x0044		// 0044 (c) 1992 SNK
#define NGH_samsho		0x0045		// 0045 (c) 1993 SNK
#define NGH_tophuntr	0x0046		// 0046 (c) 1994 SNK
#define NGH_fatfury2	0x0047		// 0047 (c) 1992 SNK
#define NGH_janshin		0x0048		// 0048 (c) 1994 Aicom
#define NGH_androdun	0x0049		// 0049 (c) 1992 Visco
#define NGH_ncommand	0x0050		// 0050 (c) 1992 Alpha Denshi Co.
#define NGH_viewpoin	0x0051		// 0051 (c) 1992 Sammy
#define NGH_ssideki		0x0052		// 0052 (c) 1992 SNK
#define NGH_wh1			0x0053		// 0053 (c) 1992 Alpha Denshi Co.
// 0054 Crossed Swords 2  (CD only? not confirmed, MVS might exist)
#define NGH_kof94		0x0055		// 0055 (c) 1994 SNK
#define NGH_aof2		0x0056		// 0056 (c) 1994 SNK
#define NGH_wh2			0x0057		// 0057 (c) 1993 ADK
#define NGH_fatfursp	0x0058		// 0058 (c) 1993 SNK
#define NGH_savagere	0x0059		// 0059 (c) 1995 SNK
#define NGH_fightfev	0x0060		// 0060 (c) 1994 Viccom
#define NGH_ssideki2	0x0061		// 0061 (c) 1994 SNK
#define NGH_spinmast	0x0062		// 0062 (c) 1993 Data East Corporation
#define NGH_samsho2		0x0063		// 0063 (c) 1994 SNK
#define NGH_wh2j		0x0064		// 0064 (c) 1994 ADK / SNK
#define NGH_wjammers	0x0065		// 0065 (c) 1994 Data East Corporation
#define NGH_karnovr		0x0066		// 0066 (c) 1994 Data East Corporation
#define NGH_gururin		0x0067		// 0067 (c) 1994 Face
#define NGH_pspikes2	0x0068		// 0068 (c) 1994 Video System Co.
#define NGH_fatfury3	0x0069		// 0069 (c) 1995 SNK
#define NGH_zupapa		0x0070		// 0070 Zupapa - released in 2001, 1994 prototype probably exists
// 0071 Bang Bang Busters (prototype) 1994 Visco
// 0072 Last Odyssey Pinball Fantasia (prototype) 1995 Monolith
#define NGH_panicbom	0x0073		// 0073 (c) 1994 Eighting / Hudson
#define NGH_aodk		0x0074		// 0074 (c) 1994 ADK / SNK
#define NGH_sonicwi2	0x0075		// 0075 (c) 1994 Video System Co.
#define NGH_zedblade	0x0076		// 0076 (c) 1994 NMK
// 0077 The Warlocks of the Fates (prototype) 1995 Astec
#define NGH_galaxyfg	0x0078		// 0078 (c) 1995 Sunsoft
#define NGH_strhoop		0x0079		// 0079 (c) 1994 Data East Corporation
#define NGH_quizkof		0x0080		// 0080 (c) 1995 Saurus
#define NGH_ssideki3	0x0081		// 0081 (c) 1995 SNK
#define NGH_doubledr	0x0082		// 0082 (c) 1995 Technos
#define NGH_pbobblen	0x0083		// 0083 (c) 1994 Taito
#define NGH_kof95		0x0084		// 0084 (c) 1995 SNK
// 0085 Shinsetsu Samurai Spirits Bushidoretsuden / Samurai Shodown RPG (CD only)
#define NGH_tws96		0x0086		// 0086 (c) 1996 Tecmo
#define NGH_samsho3		0x0087		// 0087 (c) 1995 SNK
#define NGH_stakwin		0x0088		// 0088 (c) 1995 Saurus
#define NGH_pulstar		0x0089		// 0089 (c) 1995 Aicom
#define NGH_whp			0x0090		// 0090 (c) 1995 ADK / SNK
// 0091
#define NGH_kabukikl	0x0092		// 0092 (c) 1995 Hudson
#define NGH_neobombe	0x0093		// 0093 (c) 1997 Hudson
#define NGH_gowcaizr	0x0094		// 0094 (c) 1995 Technos
#define NGH_rbff1		0x0095		// 0095 (c) 1995 SNK
#define NGH_aof3		0x0096		// 0096 (c) 1996 SNK
#define NGH_sonicwi3	0x0097		// 0097 (c) 1995 Video System Co.
// 0098 Idol Mahjong - final romance 2 (CD only? not confirmed, MVS might exist)
// 0099 Neo Pool Masters
#define NGH_turfmast	0x0200		// 0200 (c) 1996 Nazca
#define NGH_mslug		0x0201		// 0201 (c) 1996 Nazca
#define NGH_puzzledp	0x0202		// 0202 (c) 1995 Taito (Visco license)
#define NGH_mosyougi	0x0203		// 0203 (c) 1995 ADK / SNK
// 0204 QP (prototype)
// 0205 Neo-Geo CD Special (CD only)
#define NGH_marukodq	0x0206		// 0206 (c) 1995 Takara
#define NGH_neomrdo		0x0207		// 0207 (c) 1996 Visco
#define NGH_sdodgeb		0x0208		// 0208 (c) 1996 Technos
#define NGH_goalx3		0x0209		// 0209 (c) 1995 Visco
// 0210 Karate Ninja Sho (prototype) 1995 Yumekobo
// 0211 Oshidashi Zintrick (CD only? not confirmed, MVS might exist) 1996 SNK/ADK
#define NGH_overtop		0x0212		// 0212 (c) 1996 ADK
#define NGH_neodrift	0x0213		// 0213 (c) 1996 Visco
#define NGH_kof96		0x0214		// 0214 (c) 1996 SNK
#define NGH_ssideki4	0x0215		// 0215 (c) 1996 SNK
#define NGH_kizuna		0x0216		// 0216 (c) 1996 SNK
#define NGH_ninjamas	0x0217		// 0217 (c) 1996 ADK / SNK
#define NGH_ragnagrd	0x0218		// 0218 (c) 1996 Saurus
#define NGH_pgoal		0x0219		// 0219 (c) 1996 Saurus
// 0220 Choutetsu Brikin'ger - iron clad (MVS existance seems to have been confirmed)
#define NGH_magdrop2	0x0221		// 0221 (c) 1996 Data East Corporation
#define NGH_samsho4		0x0222		// 0222 (c) 1996 SNK
#define NGH_rbffspec	0x0223		// 0223 (c) 1996 SNK
#define NGH_twinspri	0x0224		// 0224 (c) 1996 ADK
#define NGH_wakuwak7	0x0225		// 0225 (c) 1996 Sunsoft
// 0226 Pair Pair Wars (prototype) 1996 Sunsoft?
#define NGH_stakwin2	0x0227		// 0227 (c) 1996 Saurus
#define NGH_ghostlop	0x0228		// 0228 GhostLop (prototype) 1996? Data East
// 0229 King of Fighters '96 CD Collection (CD only)
#define NGH_breakers	0x0230		// 0230 (c) 1996 Visco
#define NGH_miexchng	0x0231		// 0231 (c) 1997 Face
#define NGH_kof97		0x0232		// 0232 (c) 1997 SNK
#define NGH_magdrop3	0x0233		// 0233 (c) 1997 Data East Corporation
#define NGH_lastblad	0x0234		// 0234 (c) 1997 SNK
#define NGH_puzzldpr	0x0235		// 0235 (c) 1997 Taito (Visco license)
#define NGH_irrmaze		0x0236		// 0236 (c) 1997 SNK / Saurus
#define NGH_popbounc	0x0237		// 0237 (c) 1997 Video System Co.
#define NGH_shocktro	0x0238		// 0238 (c) 1997 Saurus
#define NGH_blazstar	0x0239		// 0239 (c) 1998 Yumekobo
#define NGH_rbff2		0x0240		// 0240 (c) 1998 SNK
#define NGH_mslug2		0x0241		// 0241 (c) 1998 SNK
#define NGH_kof98		0x0242		// 0242 (c) 1998 SNK
#define NGH_lastbld2	0x0243		// 0243 (c) 1998 SNK
#define NGH_neocup98	0x0244		// 0244 (c) 1998 SNK
#define NGH_breakrev	0x0245		// 0245 (c) 1998 Visco
#define NGH_shocktr2	0x0246		// 0246 (c) 1998 Saurus
#define NGH_flipshot	0x0247		// 0247 (c) 1998 Visco
#define NGH_pbobbl2n	0x0248		// 0248 (c) 1999 Taito (SNK license)
#define NGH_ctomaday	0x0249		// 0249 (c) 1999 Visco
#define NGH_mslugx		0x0250		// 0250 (c) 1999 SNK
#define NGH_kof99		0x0251		// 0251 (c) 1999 SNK
#define NGH_ganryu		0x0252		// 0252 (c) 1999 Visco
#define NGH_garou		0x0253		// 0253 (c) 1999 SNK
#define NGH_s1945p		0x0254		// 0254 (c) 1999 Psikyo
#define NGH_preisle2	0x0255		// 0255 (c) 1999 Yumekobo
#define NGH_mslug3		0x0256		// 0256 (c) 2000 SNK
#define NGH_kof2000		0x0257		// 0257 (c) 2000 SNK
// 0258 SNK vs. Capcom? (prototype)
#define NGH_bangbead	0x0259		// 0259 (c) 2000 Visco
#define NGH_nitd		0x0260		// 0260 (c) 2000 Eleven / Gavaking
#define NGH_sengoku3	0x0261		// 0261 (c) 2001 SNK
#define NGH_kof2001		0x0262		// 0262 (c) 2001 Eolith / SNK
#define NGH_mslug4		0x0263		// 0263 (c) 2002 Mega Enterprise
#define NGH_rotd		0x0264		// 0264 (c) 2002 Evoga
#define NGH_kof2002		0x0265		// 0265 (c) 2002 Eolith / Playmore
#define NGH_matrim		0x0266		// 0266 (c) 2002 Atlus
#define NGH_pnyaa		0x0267		// 0267 (c) 2003 Aiky / Taito
#define	NGH_mslug5		0x0268		// 0268 (c) 2003 Playmore
#define	NGH_svc			0x0269		// 0269 (c) 2003 Playmore / Capcom
#define	NGH_samsho5		0x0270		// 0270 (c) 2003 Playmore
#define	NGH_kof2003		0x0271		// 0271 (c) 2003 Playmore/
#define	NGH_samsh5sp	0x0272		// 0272 (c) 2003 Playmore

// The BrezzaSoft games don't have proper ID codes
#define NGH_vliner		(0x0000 | FLAG_BRAZ)
#define NGH_vlinero		(0x0000 | FLAG_BRAZ)
#define NGH_jockeygp	(0x0001 | FLAG_BRAZ)

// JAMMA PCB
#define	NGH_ms5pcb		(0x0268 | FLAG_PCB)	// 0268 (c) 2003 Playmore - JAMMA PCB
#define	NGH_svcpcb		(0x0269 | FLAG_PCB)	// 0269 (c) 2003 Playmore / Capcom - JAMMA PCB
#define	NGH_svcpcba		(0x0269 | FLAG_PCB)	// 0269 (c) 2003 Playmore / Capcom - JAMMA PCB
#define	NGH_kf2k3pcb	(0x0271 | FLAG_PCB)	// 0271 (c) 2003 Playmore - JAMMA PCB


enum
{
	// SNK
	INIT_neogeo = 0,	// 0
	INIT_fatfury2,		// 1
	INIT_kof98,			// 2
	INIT_mslugx,		// 3
	INIT_kof99,			// 4
	INIT_kof99n,		// 5
	INIT_garou,			// 6
	INIT_garouo,		// 7
	INIT_mslug3,		// 8
	INIT_mslug3n,		// 9
	INIT_kof2000,		// 10
	INIT_kof2000n,		// 11
	INIT_zupapa,		// 12
	INIT_sengoku3,		// 13
	INIT_kof2001,		// 14
	INIT_kof2002,		// 15
	INIT_mslug5,		// 16
	INIT_svchaosa,		// 17
	INIT_samsho5,		// 18
	INIT_kof2003,		// 19
	INIT_samsh5sp,		// 20

	// SNK以外
	INIT_nitd,			// 21
	INIT_s1945p,		// 22
	INIT_pnyaa,			// 23
	INIT_preisle2,		// 24
	INIT_ganryu,		// 25
	INIT_bangbead,		// 26
	INIT_mslug4,		// 27
	INIT_rotd,			// 28
	INIT_matrim,		// 29

	// JAMMA PCB
	INIT_ms5pcb,		// 30
	INIT_svcpcb,		// 31
	INIT_kf2k3pcb,		// 32

	// BrezzaSoft
	INIT_jockeygp,		// 33
	INIT_vliner,		// 34

#if !RELEASE
	// bootleg
	INIT_garoubl,		// 35
	INIT_kf2k2pls,		// 36
	INIT_kf2k2mp,		// 37
	INIT_kf2k2mp2,		// 38
	INIT_ms5plus,		// 39
	INIT_svcboot,		// 40
	INIT_svcplus,		// 41
	INIT_svcplusa,		// 42
	INIT_svcsplus,		// 43
	INIT_samsho5b,		// 44
	INIT_kf2k3bl,		// 45
	INIT_kf2k3pl,		// 46
	INIT_kf2k3upl,		// 47
	INIT_kog,			// 48
	INIT_kof10th,		// 49
	INIT_kf10thep,		// 50
	INIT_kf2k5uni,		// 51
	INIT_cthd2003,		// 52
	INIT_ct2k3sp,		// 53
	INIT_kof2k4se,		// 54
	INIT_lans2004,		// 55
	INIT_mslug3b6,		// 56
	INIT_ms4plus,		// 57

	// bootleg (MAME plus)
	INIT_kof96ep,		// 58
	INIT_kf2k1pls,		// 59
	INIT_kf2k1pa,		// 60
	INIT_cthd2k3a,		// 61
	INIT_kof2002b,		// 62
	INIT_matrimbl,		// 63
	INIT_kf2k2plc,		// 64
	INIT_kf2k4pls,		// 65
	INIT_kof97pla,		// 66
	INIT_fr2ch,			// 67
	INIT_mslug5b,		// 68

	// MAME 0.113 - 0.119
	INIT_ct2k3sa,		// 69
#endif

	MAX_INIT
};

#define RASTER_LINES	264

enum
{
	RASTER = 0,
	NORMAL
};

extern int neogeo_driver_type;
extern int neogeo_raster_enable;
extern UINT16 neogeo_ngh;

extern UINT8 auto_animation_speed;
extern UINT8 auto_animation_disabled;
extern UINT8 auto_animation_counter;

struct cacheinfo_t
{
	const char *name;
	const char *parent;
	const int crom;
	const int srom;
	const int vrom;
};

extern struct cacheinfo_t MVS_cacheinfo[];


void neogeo_driver_init(void);
void neogeo_driver_exit(void);
void neogeo_driver_reset(void);
void neogeo_reset_driver_type(void);

void neogeo_vblank_interrupt(void);
void neogeo_raster_interrupt(int line);

TIMER_CALLBACK( watchdog_callback );
TIMER_CALLBACK( neogeo_sound_write );

READ16_HANDLER( neogeo_controller2_r );
READ16_HANDLER( neogeo_controller3_r );
READ16_HANDLER( neogeo_controller1and4_r );

WRITE16_HANDLER( watchdog_reset_w );

WRITE16_HANDLER( neogeo_bankswitch_w );

READ16_HANDLER( neogeo_timer_r );
WRITE16_HANDLER( neogeo_z80_w );

WRITE16_HANDLER( io_control_w );
WRITE16_HANDLER( system_control_w );

READ16_HANDLER( neogeo_paletteram_r );
WRITE16_HANDLER( neogeo_paletteram_w );

READ16_HANDLER( neogeo_video_register_r );
WRITE16_HANDLER( neogeo_video_register_w );

READ16_HANDLER( neogeo_memcard16_r );
WRITE16_HANDLER( neogeo_memcard16_w );

WRITE16_HANDLER( neogeo_sram16_w );

UINT8 neogeo_z80_port_r(UINT16 port);
void neogeo_z80_port_w(UINT16 port, UINT8 value);

void neogeo_sound_irq(int irq);


//--------------------------------------------------------------
// protection
//--------------------------------------------------------------

void mslugx_install_protection(void);

READ16_HANDLER( neogeo_secondbank_r );
WRITE16_HANDLER( neogeo_secondbank_w );

READ16_HANDLER( fatfury2_protection_r );
WRITE16_HANDLER( fatfury2_protection_w );

WRITE16_HANDLER( kof98_protection_w );

READ16_HANDLER( kof99_protection_r );
WRITE16_HANDLER( kof99_protection_w );

READ16_HANDLER( garou_protection_r );
WRITE16_HANDLER( garou_protection_w );
WRITE16_HANDLER( garouo_protection_w );

READ16_HANDLER( mslug3_protection_r );
WRITE16_HANDLER( mslug3_protection_w );

READ16_HANDLER( kof2000_protection_r );
WRITE16_HANDLER( kof2000_protection_w );

READ16_HANDLER( pvc_protection_r );
WRITE16_HANDLER( pvc_protection_w );

READ16_HANDLER( brza_sram_r );
WRITE16_HANDLER( brza_sram_w );

READ16_HANDLER( vliner_r );

void mslug4_AES_protection(void);
void rotd_AES_protection(void);
void matrim_AES_protection(void);
void mslug5_AES_protection(void);
void zupapa_AES_protection(void);
void sengoku3_AES_protection(void);
void nitd_AES_protection(void);
void kof2000_AES_protection(void);

#if !RELEASE
WRITE16_HANDLER( kof10th_protection_w );
WRITE16_HANDLER( cthd2003_protection_w );
READ16_HANDLER( ms5plus_protection_r );
WRITE16_HANDLER( ms5plus_protection_w );
WRITE16_HANDLER( kf2k3bl_protection_w);
WRITE16_HANDLER( kf2k3pl_protection_w);
WRITE16_HANDLER( fr2ch_protection_w);
#endif


//--------------------------------------------------------------
// decrypt ROM
//--------------------------------------------------------------

int kof98_decrypt_68k(void);
void kof99_decrypt_68k(void);
void garou_decrypt_68k(void);
void garouo_decrypt_68k(void);
void mslug3_decrypt_68k(void);
void kof2000_decrypt_68k(void);
int kof2002_decrypt_68k(void);
int samsho5_decrypt_68k(void);
int samsh5p_decrypt_68k(void);
int mslug5_decrypt_68k(void);
int svcchaos_px_decrypt(void);
int kf2k3pcb_decrypt_68k(void);
int kof2003_decrypt_68k(void);
int kof2003biosdecode(void);

#if !RELEASE
int kog_px_decrypt(void);
int kof10th_px_decrypt(void);
int kf10thep_px_decrypt(void);
int kf2k5uni_px_decrypt(void);
int kf2k2mp_px_decrypt(void);
int kf2k2mp2_px_decrypt(void);
int kof2k4se_px_decrypt(void);
int lans2004_px_decrypt(void);
int svcboot_px_decrypt(void);
int svcplus_px_decrypt(void);
int svcplusa_px_decrypt(void);
int svcsplus_px_decrypt(void);
int kf2k3bl_px_decrypt(void);
int kf2k3pl_px_decrypt(void);
void kf2k3upl_px_decrypt(void);
int samsho5b_px_decrypt(void);
void cthd2003_sx_decrypt(void);
void cthd2003_mx_decrypt(void);
void kf2k5uni_sx_decrypt(void);
void kf2k5uni_mx_decrypt(void);
void kf10thep_sx_decrypt(void);

void kof96ep_px_decrypt(void);
int cthd2k3a_px_decrypt(void);
int kf2k4pls_px_decrypt(void);
void kf2k1pa_sx_decrypt(void);
void matrimbl_mx_decrypt(void);

void cthd2003_AES_protection(void);

void patch_cthd2003(void);
void patch_kof97pla(void);
void patch_fr2ch(void);
#endif /* RELEASE */


#ifdef SAVE_STATE
STATE_SAVE( driver );
STATE_LOAD( driver );
#endif

#endif /* MVS_DRIVER_H */
