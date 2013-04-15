/******************************************************************************

	coin.c

	コインカウンタ

******************************************************************************/

#include "emumain.h"


/******************************************************************************
	ローカル変数
******************************************************************************/

#define COIN_COUNTERS	4

static UINT32 coins[COIN_COUNTERS];
static UINT32 lastcoin[COIN_COUNTERS];
static UINT32 coinlockedout[COIN_COUNTERS];


/******************************************************************************
	グローバル関数
******************************************************************************/

/*--------------------------------------------------------
	コインカウンタリセット
--------------------------------------------------------*/

void coin_counter_reset(void)
{
	memset(coins, 0, sizeof(coins));
	memset(lastcoin, 0, sizeof(lastcoin));
	memset(coinlockedout, 0, sizeof(coinlockedout));
}


/*--------------------------------------------------------
	コインカウンタ更新
--------------------------------------------------------*/

void coin_counter_w(int num, int on)
{
	if (num >= COIN_COUNTERS) return;

	if (on && (lastcoin[num] == 0))
	{
		coins[num]++;
	}
	lastcoin[num] = on;
}


/*--------------------------------------------------------
	コインカウンタロック
--------------------------------------------------------*/

void coin_lockout_w(int num, int on)
{
	if (num >= COIN_COUNTERS) return;

	coinlockedout[num] = on;
}


/*--------------------------------------------------------
	セーブ/ロード ステート
--------------------------------------------------------*/

#ifdef SAVE_STATE

STATE_SAVE( coin )
{
	state_save_long(coins, COIN_COUNTERS);
	state_save_long(lastcoin, COIN_COUNTERS);
	state_save_long(coinlockedout, COIN_COUNTERS);
}

STATE_LOAD( coin )
{
	state_load_long(coins, COIN_COUNTERS);
	state_load_long(lastcoin, COIN_COUNTERS);
	state_load_long(coinlockedout, COIN_COUNTERS);
}

#endif /* SAVE_STATE */
