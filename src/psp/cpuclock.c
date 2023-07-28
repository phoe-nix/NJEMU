#include <stdio.h>
#include <psppower.h>
#include "psp.h"


/*------------------------------------------------------
	CPU¥¯¥í¥Ã¥¯ÔO¶¨
------------------------------------------------------*/

void set_cpu_clock(int value)
{
	switch (value)
	{
	case PSPCLOCK_266: scePowerSetClockFrequency(266, 266, 133); break;
	case PSPCLOCK_300: scePowerSetClockFrequency(300, 300, 150); break;
	case PSPCLOCK_333: scePowerSetClockFrequency(333, 333, 166); break;
	default: scePowerSetClockFrequency(222, 222, 111); break;
	}
}

void set_lowest_cpu_clock(void)
{
	set_cpu_clock(PSPCLOCK_222);
}