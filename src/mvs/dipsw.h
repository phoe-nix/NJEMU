/******************************************************************************

	dipsw.c

	MVS DIP•π•§•√•¡‘O∂®

******************************************************************************/

#ifndef MVS_DIP_SWITCH_H
#define MVS_DIP_SWITCH_H

#define MAX_DIPSWITCHS		8

typedef struct
{
	const char *label;
	uint8_t enable;
	uint8_t mask;
	uint8_t value;
	uint8_t value_max;
	const char *values_label[MAX_DIPSWITCHS + 1];
} dipswitch_t;

extern int neogeo_hard_dipsw;

dipswitch_t *load_dipswitch(void);
void save_dipswitch(void);

#endif /* MVS_DIP_SWITCH_H */
