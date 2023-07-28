/******************************************************************************

	cpuclock.h

******************************************************************************/

#ifndef CPU_CLOCK_H
#define CPU_CLOCK_H

extern int platform_cpuclock;

void set_cpu_clock(int value);
void set_lowest_cpu_clock(void);
int get_highest_cpu_clock(void);

#endif /* CPU_CLOCK_H */