/******************************************************************************

	coin.c

	コインカウンタ

******************************************************************************/

#ifndef COIN_COUNTER_H
#define COIN_COUNTER_H

void coin_counter_reset(void);
void coin_counter_w(int num, int on);
void coin_lockout_w(int num, int on);

#ifdef SAVE_STATE
STATE_SAVE( coin );
STATE_LOAD( coin );
#endif

#endif /* COIN_COUNTER_H */
