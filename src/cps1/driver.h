/******************************************************************************

	driver.c

	CPS1 driver/ドライバ/駁強

******************************************************************************/

#ifndef CPS1_DRIVER_H
#define CPS1_DRIVER_H

#define RASTER_LINES	262

#define CPS1_KLUDGE_FORGOTTN	1
#define CPS1_KLUDGE_GHOULS		2
#define CPS1_KLUDGE_MERCS		3
#define CPS1_KLUDGE_3WONDERS	4
#define CPS1_KLUDGE_PANG3		5
#define CPS1_KLUDGE_SF2CEB		6
#define CPS1_KLUDGE_WOFB		7
#define CPS1_KLUDGE_CAWINGB		8
#define CPS1_KLUDGE_KNIGHTSB	9

enum
{
	MACHINE_cps1 = 0,	// 0
	MACHINE_forgottn,	// 1
	MACHINE_sf2,		// 2
	MACHINE_qsound,		// 3
	MACHINE_pang3,		// 4
#if !RELEASE
	MACHINE_kodb,		// 5
	MACHINE_mercs,		// 6
#endif
	MACHINE_MAX			// MAX
};

enum
{
	INPTYPE_cps1 = 0,
	INPTYPE_forgottn,	// 1
	INPTYPE_ghouls,		// 2
	INPTYPE_ghoulsu,	// 3
	INPTYPE_daimakai,	// 4
	INPTYPE_strider,	// 5
	INPTYPE_stridrua,	// 6
	INPTYPE_dynwar,		// 7
	INPTYPE_willow,		// 8
	INPTYPE_unsquad,	// 9
	INPTYPE_ffight,		// 10
	INPTYPE_1941,		// 11
	INPTYPE_mercs,		// 12
	INPTYPE_mtwins,		// 13
	INPTYPE_msword,		// 14
	INPTYPE_cawing,		// 15
	INPTYPE_nemo,		// 16
	INPTYPE_sf2,		// 17
	INPTYPE_sf2j,		// 18
	INPTYPE_3wonders,	// 19
	INPTYPE_kod,		// 20
	INPTYPE_kodj,		// 21
	INPTYPE_captcomm,	// 22
	INPTYPE_knights,	// 23
	INPTYPE_varth,		// 24
	INPTYPE_cworld2j,	// 25
	INPTYPE_qad,		// 26
	INPTYPE_qadj,		// 27
	INPTYPE_qtono2,		// 28
	INPTYPE_megaman,	// 29
	INPTYPE_rockmanj,	// 30
	INPTYPE_wof,		// 31
	INPTYPE_dino,		// 32
	INPTYPE_punisher,	// 33
	INPTYPE_slammast,	// 34
	INPTYPE_pnickj,		// 35
	INPTYPE_pang3,		// 36
	INPTYPE_sfzch,		// 37
#if !RELEASE
	INPTYPE_knightsh,	// 38
	INPTYPE_wofh,		// 39
	INPTYPE_wof3js,		// 40
	INPTYPE_wofsj,		// 41
	INPTYPE_dinoh,		// 42
#endif
	INPTYPE_MAX			// MAX
};

enum
{
	INIT_cps1 = 0,
	INIT_wof,			// 1
	INIT_dino,			// 2
	INIT_punisher,		// 3
	INIT_slammast,		// 4
	INIT_pang3,			// 5
#if !RELEASE
	INIT_kodb,			// 6
	INIT_sf2m13,		// 7
	INIT_wofh,			// 8
	INIT_wof3js,		// 9
	INIT_wof3sj,		// 10
	INIT_wofsjb,		// 11
	INIT_dinob,			// 12
	INIT_dinoh,			// 13
#endif
	INIT_MAX
};

enum
{
	SCREEN_NORMAL = 0,
	SCREEN_VERTICAL,
	SCREENTYPE_MAX
};

struct tile_t
{
	UINT16 start;
	UINT16 end;
};

struct driver_t
{
	const char *name;
	const UINT16 cpsb_addr;
	const UINT16 cpsb_value;
	const UINT8  mult_factor1;
	const UINT8  mult_factor2;
	const UINT8  mult_result_lo;
	const UINT8  mult_result_hi;
	const UINT8  priority[4];
	const UINT8  layer_enable_mask[5];
	const UINT8  layer_control;
	const UINT8  control_reg;
	const UINT8  kludge;
	const UINT8  has_stars;
	const UINT8  bank_scroll1;
	const UINT8  bank_scroll2;
	const UINT8  bank_scroll3;
	const UINT32 gfx_limit;
	const struct tile_t scroll1;
	const struct tile_t scroll2;
	const struct tile_t scroll3;
};

extern struct driver_t CPS1_driver[];
extern struct driver_t *driver;

WRITE16_HANDLER( cps1_coinctrl_w );
WRITE16_HANDLER( cps1_coinctrl2_w );

READ16_HANDLER( cps1_dsw_a_r );
READ16_HANDLER( cps1_dsw_b_r );
READ16_HANDLER( cps1_dsw_c_r );

READ16_HANDLER( cps1_inputport0_r );
READ16_HANDLER( cps1_inputport1_r );
READ16_HANDLER( cps1_inputport2_r );
READ16_HANDLER( cps1_inputport3_r );

READ16_HANDLER( forgottn_dial_0_r );
READ16_HANDLER( forgottn_dial_1_r );
WRITE16_HANDLER( forgottn_dial_0_reset_w );
WRITE16_HANDLER( forgottn_dial_1_reset_w );

WRITE8_HANDLER( cps1_snd_bankswitch_w );

WRITE8_HANDLER( cps1_oki_pin7_w );

READ8_HANDLER( cps1_sound_fade_timer_r );
WRITE16_HANDLER( cps1_sound_fade_timer_w );

READ8_HANDLER( cps1_sound_command_r );
WRITE16_HANDLER( cps1_sound_command_w );

READ16_HANDLER( cps1_eeprom_port_r );
WRITE16_HANDLER( cps1_eeprom_port_w );

READ16_HANDLER( qsound_rom_r );
READ16_HANDLER( qsound_sharedram1_r );
WRITE16_HANDLER( qsound_sharedram1_w );
READ16_HANDLER( qsound_sharedram2_r );
WRITE16_HANDLER( qsound_sharedram2_w );
WRITE8_HANDLER( qsound_banksw_w );

TIMER_CALLBACK( cps1_vblank_interrupt );
TIMER_CALLBACK( cps1_qsound_interrupt );

void cps1_sound_interrupt(int state);

void pang3_decode(void);
#if !RELEASE
void kodb_init(void);
void sf2m13_init(void);
void wofb_init(void);
void wofh_init(void);
void wof3js_init(void);
void wof3sj_init(void);
void wofsjb_init(void);
void dinob_init(void);
#endif

int cps1_driver_init(void);
void cps1_driver_reset(void);
void cps1_driver_exit(void);

#ifdef SAVE_STATE
STATE_SAVE( driver );
STATE_LOAD( driver );
#endif

/* kabuki.c */
void wof_decode(void);
void dino_decode(void);
void punisher_decode(void);
void slammast_decode(void);

#endif /* CPS1_DRIVER_H */
