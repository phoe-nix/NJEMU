/******************************************************************************

	pd4990a.c

	NEC PD4990A Serial I/O calendar & clock

******************************************************************************/

#ifndef PD4990A_H
#define PD4990A_H

struct pd4990a_s
{
	int seconds;
	int minutes;
	int hours;
	int days;
	int month;
	int year;
	int weekday;
};

extern struct pd4990a_s pd4990a;

void pd4990a_addretrace (void);
void pd4990a_init(void);
READ8_HANDLER( pd4990a_testbit_r );
READ8_HANDLER( pd4990a_databit_r );
WRITE16_HANDLER( pd4990a_control_w );
void pd4990a_increment_day(void);
void pd4990a_increment_month(void);

#ifdef SAVE_STATE
STATE_SAVE( pd4990a );
STATE_LOAD( pd4990a );
#endif

#endif /* PD4990A_H */
