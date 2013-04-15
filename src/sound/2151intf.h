/***************************************************************************

  2151intf.c

  Support interface YM2151(OPM)

***************************************************************************/

#ifndef YM2151INTF_H
#define YM2151INTF_H

#include "ym2151.h"
#include "okim6295.h"

void YM2151_sh_start(int type);
void YM2151_sh_stop(void);
void YM2151_sh_reset(void);

READ8_HANDLER( YM2151_status_port_r );
WRITE8_HANDLER( YM2151_register_port_w );
WRITE8_HANDLER( YM2151_data_port_w );

#ifdef SAVE_STATE
STATE_SAVE( ym2151 );
STATE_LOAD( ym2151 );
#endif

#endif /* YM2151INTF_H */
