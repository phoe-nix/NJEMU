/******************************************************************************

	driver.c

	CPS2 ƒhƒ‰ƒCƒo

******************************************************************************/

#ifndef CPS2_DRIVER_H
#define CPS2_DRIVER_H

#define RASTER_LINES	262

/* CPS2 kludge value */
#define CPS2_KLUDGE_NONE		0x00
#define CPS2_KLUDGE_SSF2		0x01
#define CPS2_KLUDGE_SSF2T		0x02
#define CPS2_KLUDGE_XMCOTA		0x04
#define CPS2_KLUDGE_MMATRIX		0x08
#define CPS2_KLUDGE_DIMAHOO		0x10
#define CPS2_KLUDGE_PUZLOOP2	0x20
#define PHOENIX_EDITION			0x8000


enum
{
	MACHINE_cps2 = 0,
	MACHINE_MAX
};

enum
{
	INPTYPE_cps2 = 0,
	INPTYPE_ssf2,
	INPTYPE_ddtod,
	INPTYPE_sgemf,
	INPTYPE_avsp,
	INPTYPE_cybots,
	INPTYPE_19xx,
	INPTYPE_qndream,
	INPTYPE_batcir,
	INPTYPE_pzloop2,
	INPTYPE_daimahoo,
	INPTYPE_MAX
};

enum
{
	INIT_cps2 = 0,
	INIT_pzloop2,
	INIT_MAX
};

enum
{
	SCREEN_NORMAL = 0,
	SCREEN_VERTICAL,
	SCREENTYPE_MAX
};

struct driver_t
{
	const char *name;
#if USE_CACHE
	const uint32_t cache_size;
#endif
	const uint16_t kludge;
	const uint16_t flags;
	const uint8_t inp_eeprom;
	const uint8_t inp_eeprom_value[16];
};

extern struct driver_t CPS2_driver[];
extern struct driver_t *driver;

TIMER_CALLBACK( cps2_raster_interrupt );
TIMER_CALLBACK( cps2_vblank_interrupt );

int cps2_driver_init(void);
void cps2_driver_reset(void);
void cps2_driver_exit(void);

READ16_HANDLER( cps2_inputport0_r );
READ16_HANDLER( cps2_inputport1_r );

READ16_HANDLER( qsound_sharedram1_r );
WRITE16_HANDLER( qsound_sharedram1_w );
WRITE8_HANDLER( qsound_banksw_w );

READ16_HANDLER( cps2_qsound_volume_r );

READ16_HANDLER( cps2_eeprom_port_r );
WRITE16_HANDLER( cps2_eeprom_port_w );

#ifdef SAVE_STATE
STATE_SAVE( driver );
STATE_LOAD( driver );
#endif

void cps2_init_68k(void);

#endif /* CPS2_DRIVER_H */
