#include <stdio.h>
#include <stdlib.h>
#include <psppower.h>
#include "psp_power.h"
#include "common/power_driver.h"

typedef struct psp_power {
} psp_power_t;

static void *psp_init(void) {
	psp_power_t *psp = (psp_power_t*)calloc(1, sizeof(psp_power_t));
	return psp;
}

static void psp_free(void *data) {
	psp_power_t *psp = (psp_power_t*)data;
	free(psp);
}

static int32_t psp_batteryLifePercent(void *data) {
	return scePowerGetBatteryLifePercent();
}

static bool psp_IsBatteryCharging(void *data) {
	return scePowerIsBatteryCharging();
}

static void psp_setCpuClock(void *data, int32_t clock) {
	switch (clock) {
	case PSPCLOCK_266: scePowerSetClockFrequency(266, 266, 133); break;
	case PSPCLOCK_300: scePowerSetClockFrequency(300, 300, 150); break;
	case PSPCLOCK_333: scePowerSetClockFrequency(333, 333, 166); break;
	default: scePowerSetClockFrequency(222, 222, 111); break;
	}
}

static void psp_setLowestCpuClock(void *data) {
	psp_setCpuClock(data, PSPCLOCK_222);
}

static int32_t psp_getHighestCpuClock(void *data) {
	return PSPCLOCK_333;
}

power_driver_t power_psp = {
	"psp",
	psp_init,
	psp_free,
	psp_batteryLifePercent,
	psp_IsBatteryCharging,
	psp_setCpuClock,
	psp_setLowestCpuClock,
	psp_getHighestCpuClock,
};