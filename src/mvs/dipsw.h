/******************************************************************************

	dipsw.c

	MVS DIPスイッチ設定

******************************************************************************/

#ifndef MVS_DIP_SWITCH_H
#define MVS_DIP_SWITCH_H

#define MAX_DIPSWITCHS		8

typedef struct
{
	const char *label;
	UINT8 enable;
	UINT8 mask;
	UINT8 value;
	UINT8 value_max;
	const char *values_label[MAX_DIPSWITCHS + 1];
} dipswitch_t;

extern int neogeo_hard_dipsw;

dipswitch_t *load_dipswitch(void);
void save_dipswitch(void);

#endif /* MVS_DIP_SWITCH_H */
