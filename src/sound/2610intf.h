/***************************************************************************

	2610intf.c

	YM2610 interface

***************************************************************************/

#ifndef _2610INTF_H
#define _2610INTF_H

#include "ym2610.h"

#define YM2610UpdateRequest()

void YM2610_sh_start(void);
void YM2610_sh_stop(void);
void YM2610_sh_reset(void);
void timer_callback_2610(int param);

READ8_HANDLER( YM2610_status_port_A_r );
READ8_HANDLER( YM2610_status_port_B_r );
READ8_HANDLER( YM2610_read_port_r );

WRITE8_HANDLER( YM2610_control_port_A_w );
WRITE8_HANDLER( YM2610_control_port_B_w );
WRITE8_HANDLER( YM2610_data_port_A_w );
WRITE8_HANDLER( YM2610_data_port_B_w );

#endif /* _2610INTF_H */
