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
	UINT8 enable;
	UINT8 mask;
	UINT8 value;
	UINT8 value_max;
	const char *values_label[MAX_DIPSWITCHS + 1];
} dipswitch_t;


dipswitch_t *load_dipswitch(int *sx);
void save_dipswitch(void);

#endif /* CPS1_DIP_SWITCH_H */
