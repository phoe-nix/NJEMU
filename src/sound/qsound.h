/*****************************************************************************

	qsound.c

	CAPCOM QSound エミュレータ (CPS1/CPS2)

******************************************************************************/

#ifndef QSOUND_H
#define QSOUND_H

void qsound_sh_start(void);
void qsound_sh_stop(void);
void qsound_sh_reset(void);

WRITE8_HANDLER( qsound_data_h_w );
WRITE8_HANDLER( qsound_data_l_w );
WRITE8_HANDLER( qsound_cmd_w );
READ8_HANDLER( qsound_status_r );

#ifdef SAVE_STATE
STATE_SAVE( qsound );
STATE_LOAD( qsound );
#endif

#endif /* QSOUND_H */
