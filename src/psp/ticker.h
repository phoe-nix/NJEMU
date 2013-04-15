/******************************************************************************

	ticker.c

	時間計測用カウンタ関数 (64bit)

******************************************************************************/

#ifndef TICKER_H
#define TICKER_H

typedef UINT64 TICKER;

//TICKER TICKS_PER_SEC;
#define TICKS_PER_SEC	1000000

//void ticker_init(void);
TICKER ticker(void);

#endif /* TICKER_H */
