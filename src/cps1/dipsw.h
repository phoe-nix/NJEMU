/******************************************************************************

	dipsw.c

	CPS1 DIPスイッチ設定

******************************************************************************/

#ifndef CPS1_DIP_SWITCH_H
#define CPS1_DIP_SWITCH_H

#define MAX_DIPSWITCHS		32

typedef struct
{
	const char *label;
	uint8_t enable;
	uint8_t mask;
	uint8_t value;
	uint8_t value_max;
	const char *values_label[MAX_DIPSWITCHS + 1];
} dipswitch_t;


dipswitch_t *load_dipswitch(int *sx);
void save_dipswitch(void);

#endif /* CPS1_DIP_SWITCH_H */
