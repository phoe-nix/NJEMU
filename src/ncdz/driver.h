/******************************************************************************

	driver.c

	NEOGEO CDZ driver (based on M.A.M.E.'s neogeo driver)

******************************************************************************/

#ifndef NEOCDZ_DRIVER_H
#define NEOCDZ_DRIVER_H

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

#define NGH_nam1975		0x0001		// 0001 (c) 1990 SNK
#define NGH_bstars		0x0002		// 0002 (c) 1990 SNK
#define NGH_tpgolf		0x0003		// 0003 (c) 1990 SNK
#define NGH_mahretsu	0x0004		// 0004 (c) 1990 SNK
#define NGH_maglord		0x0005		// 0005 (c) 1990 Alpha Denshi Co.
#define NGH_ridhero		0x0006		// 0006 (c) 1990 SNK
#define NGH_alpham2		0x0007		// 0007 (c) 1991 SNK
#define NGH_ncombat		0x0009		// 0009 (c) 1990 Alpha Denshi Co.
#define NGH_cyberlip	0x0010		// 0010 (c) 1990 SNK
#define NGH_superspy	0x0011		// 0011 (c) 1990 SNK
#define NGH_mutnat		0x00014		// 0014 (c) 1992 SNK
#define NGH_sengoku		0x0017		// 0017 (c) 1991 SNK
#define NGH_burningf	0x0018		// 0018 (c) 1991 SNK
#define NGH_lbowling	0x0019		// 0019 (c) 1990 SNK
#define NGH_gpilots		0x0020		// 0020 (c) 1991 SNK
#define NGH_joyjoy		0x0021		// 0021 (c) 1990 SNK
#define NGH_bjourney	0x0022		// 0022 (c) 1990 Alpha Denshi Co.
#define NGH_quizdais	0x0023		// 0023 (c) 1991 SNK
#define NGH_lresort		0x0024		// 0024 (c) 1992 SNK
#define NGH_2020bb		0x0030		// 0030 (c) 1991 SNK / Pallas
#define NGH_socbrawl	0x0031		// 0031 (c) 1991 SNK
#define NGH_roboarmy	0x0032		// 0032 (c) 1991 SNK
#define NGH_fatfury1	0x0033		// 0033 (c) 1991 SNK
#define NGH_fbfrenzy	0x0034		// 0034 (c) 1992 SNK
#define NGH_crsword		0x0037		// 0037 (c) 1991 Alpha Denshi Co.
#define NGH_trally		0x0038		// 0038 (c) 1991 Alpha Denshi Co.
#define NGH_kotm2		0x0039		// 0039 (c) 1992 SNK
#define NGH_sengoku2	0x0040		// 0040 (c) 1993 SNK
#define NGH_bstars2		0x0041		// 0041 (c) 1992 SNK
#define NGH_3countb		0x0043		// 0043 (c) 1993 SNK
#define NGH_aof			0x0044		// 0044 (c) 1992 SNK
#define NGH_samsho		0x0045		// 0045 (c) 1993 SNK
#define NGH_tophuntr	0x0046		// 0046 (c) 1994 SNK
#define NGH_fatfury2	0x0047		// 0047 (c) 1992 SNK
#define NGH_janshin		0x0048		// 0048 (c) 1994 Aicom
#define NGH_ncommand	0x0050		// 0050 (c) 1992 Alpha Denshi Co.
#define NGH_viewpoin	0x0051		// 0051 (c) 1992 Sammy
#define NGH_ssideki		0x0052		// 0052 (c) 1992 SNK
#define NGH_wh1			0x0053		// 0053 (c) 1992 Alpha Denshi Co.
#define NGH_crsword2	0x0054		// 0054 (c) 1993 ADK / SNK
#define NGH_kof94		0x0055		// 0055 (c) 1994 SNK
#define NGH_aof2		0x0056		// 0056 (c) 1994 SNK
#define NGH_wh2			0x0057		// 0057 (c) 1993 ADK
#define NGH_fatfursp	0x0058		// 0058 (c) 1993 SNK
#define NGH_savagere	0x0059		// 0059 (c) 1995 SNK
#define NGH_ssideki2	0x0061		// 0061 (c) 1994 SNK
#define NGH_samsho2		0x0063		// 0063 (c) 1994 SNK
#define NGH_wh2j		0x0064		// 0064 (c) 1994 ADK / SNK
#define NGH_wjammers	0x0065		// 0065 (c) 1994 Data East Corporation
#define NGH_karnovr		0x0066		// 0066 (c) 1994 Data East Corporation
#define NGH_pspikes2	0x0068		// 0068 (c) 1994 Video System Co.
#define NGH_fatfur3c	0x069c		// 0069 (c) 1995 SNK ('c' means CD version ?)
#define NGH_aodk		0x0074		// 0074 (c) 1994 ADK / SNK
#define NGH_sonicwi2	0x0075		// 0075 (c) 1994 Video System Co.
#define NGH_galaxyfg	0x0078		// 0078 (c) 1995 Sunsoft
#define NGH_strhoop		0x0079		// 0079 (c) 1994 Data East Corporation
#define NGH_quizkof		0x0080		// 0080 (c) 1995 Saurus
#define NGH_ssideki3	0x0081		// 0081 (c) 1995 SNK
#define NGH_doubledr	0x0082		// 0082 (c) 1995 Technos
#define NGH_pbobblen	0x0083		// 0083 (c) 1994 Taito
#define NGH_kof95		0x0084		// 0084 (c) 1995 SNK
#define NGH_ssrpg		0x0085		// 0085 (c) 1995 SNK
#define NGH_samsho3		0x0087		// 0087 (c) 1995 SNK
#define NGH_stakwin		0x0088		// 0088 (c) 1995 Saurus
#define NGH_pulstar		0x0089		// 0089 (c) 1995 Aicom
#define NGH_whp			0x0090		// 0090 (c) 1995 ADK / SNK
#define NGH_kabukikl	0x0092		// 0092 (c) 1995 Hudson
#define NGH_gowcaizr	0x0094		// 0094 (c) 1995 Technos
#define NGH_rbff1		0x0095		// 0095 (c) 1995 SNK
#define NGH_aof3		0x0096		// 0096 (c) 1996 SNK
#define NGH_sonicwi3	0x0097		// 0097 (c) 1995 Video System Co.
#define NGH_fromanc2	0x0098		// 0098 (c) 1995 Video System Co.
#define NGH_turfmast	0x0200		// 0200 (c) 1996 Nazca
#define NGH_mslug		0x0201		// 0201 (c) 1996 Nazca
#define NGH_mosyougi	0x0203		// 0203 (c) 1995 ADK / SNK
#define NGH_adkworld	0x0204		// 0204 (c) 1995 ADK / SNK
#define NGH_ngcdsp		0x0205		// 0205 (c) 1995 SNK
#define NGH_zintrick	0x0211		// 0211 (c) 1995 ADK / SNK
#define NGH_overtop		0x0212		// 0212 (c) 1996 ADK
#define NGH_neodrift	0x0213		// 0213 (c) 1996 Visco
#define NGH_kof96		0x0214		// 0214 (c) 1996 SNK
#define NGH_ninjamas	0x0217		// 0217 (c) 1996 ADK / SNK
#define NGH_ragnagrd	0x0218		// 0218 (c) 1996 Saurus
#define NGH_pgoal		0x0219		// 0219 (c) 1996 Saurus
#define NGH_ironclad	0x0220		// 0220 (c) 1996 Saurus
#define NGH_magdrop2	0x0221		// 0221 (c) 1996 Data East Corporation
#define NGH_samsho4		0x0222		// 0222 (c) 1996 SNK
#define NGH_rbffspec	0x0223		// 0223 (c) 1996 SNK
#define NGH_twinspri	0x0224		// 0224 (c) 1996 ADK
#define NGH_kof96ngc	0x0229		// 0229 (c) 1996 SNK
#define NGH_breakers	0x0230		// 0230 (c) 1996 Visco
#define NGH_kof97		0x0232		// 0232 (c) 1997 SNK
#define NGH_lastblad	0x0234		// 0234 (c) 1997 SNK
#define NGH_rbff2		0x0240		// 0240 (c) 1998 SNK
#define NGH_mslug2		0x0241		// 0241 (c) 1998 SNK
#define NGH_kof98		0x0242		// 0242 (c) 1998 SNK
#define NGH_lastbld2	0x0243		// 0243 (c) 1998 SNK
#define NGH_kof99		0x0251		// 0251 (c) 1999 SNK


#define RASTER_LINES			264

#define GAME_NAME(name)  		(!strcmp(name, game_name))
#define NGH_NUMBER(value)		(neogeo_ngh == value)

#define	EXMEM_OBJ				0x00
#define	EXMEM_PCMA				0x01
#define	EXMEM_Z80				0x04
#define	EXMEM_FIX				0x05
#define	EXMEM_UNKNOWN			0xff

#define	PRG_TYPE				0x00
#define FIX_TYPE				0x01
#define SPR_TYPE				0x02
#define Z80_TYPE				0x03
#define PCM_TYPE				0x04
#define PAT_TYPE				0x05
#define PAL_TYPE				0x06
#define OBJ_TYPE				0x07
#define AXX_TYPE				0x08
#define BACKUP_RAM				0x09
#define UNKNOWN_TYPE			0x0f

#define UPLOAD_IMMIDIATE		0x00
#define UPLOAD_PATTERN			0x01
#define UPLOAD_MEMORY			0x02
#define UPLOAD_FILE				0x03

#define upload_get_type()		m68000_read_memory_8(0x108000 + 0x7eda)
#define upload_get_bank()		m68000_read_memory_8(0x108000 + 0x7edb)
#define upload_get_dst()		m68000_read_memory_32(0x108000 + 0x7ef4)
#define upload_get_src()		m68000_read_memory_32(0x108000 + 0x7ef8)
#define upload_get_length()		m68000_read_memory_32(0x108000 + 0x7efc)

#define with_image()			m68000_read_memory_8(0x108000 + 0x7ddc)
#define spr_disable_w(data)		m68000_write_memory_8(0xff0111, data)
#define fix_disable_w(data)		m68000_write_memory_8(0xff0115, data)
#define video_enable_w(data)	m68000_write_memory_8(0xff0119, data)

enum
{
	NORMAL = 0,
	RASTER,
	RASTER_AOF2
};

typedef struct game_t
{
	const char name[12];
	const uint16_t  ngh_number;
} GAMES;


extern const GAMES games[100];
extern int game_index;
extern const char default_name[16];

extern int neogeo_driver_type;
extern int neogeo_raster_enable;
extern uint16_t neogeo_ngh;

extern uint8_t auto_animation_speed;
extern uint8_t auto_animation_disabled;
extern uint8_t auto_animation_counter;

extern uint16_t raster_line;
extern uint16_t raster_counter;

extern int watchdog_counter;

void neogeo_driver_init(void);
void neogeo_driver_reset(void);
int neogeo_check_game(void);
void neogeo_reset_driver_type(void);

void neogeo_interrupt(void);
extern void (*neogeo_raster_interrupt)(int line);

READ16_HANDLER( neogeo_controller1_r );
READ16_HANDLER( neogeo_controller2_r );
READ16_HANDLER( neogeo_controller3_r );

WRITE16_HANDLER( watchdog_reset_w );

READ16_HANDLER( neogeo_z80_r );
WRITE16_HANDLER( neogeo_z80_w );

//WRITE16_HANDLER( io_control_w );
WRITE16_HANDLER( system_control_w );

READ16_HANDLER( neogeo_paletteram_r );
WRITE16_HANDLER( neogeo_paletteram_w );

READ16_HANDLER( neogeo_video_register_r );
WRITE16_HANDLER( neogeo_video_register_w );

READ16_HANDLER( neogeo_memcard16_r );
WRITE16_HANDLER( neogeo_memcard16_w );

READ16_HANDLER( neogeo_externalmem_r );
WRITE16_HANDLER( neogeo_externalmem_w );

READ16_HANDLER( neogeo_hardcontrol_r );
WRITE16_HANDLER( neogeo_hardcontrol_w );

uint8_t neogeo_z80_port_r(uint16_t port);
void neogeo_z80_port_w(uint16_t port, uint8_t value);

void neogeo_sound_write(int data);
void neogeo_sound_irq(int irq);

#ifdef SAVE_STATE
STATE_SAVE( driver );
STATE_LOAD( driver );
#endif

#endif /* NEOCDZ_DRIVER_H */
