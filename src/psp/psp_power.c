#include <stdio.h>
#include <stdlib.h>
#include <psppower.h>
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

power_driver_t power_psp = {
	"psp",
	psp_init,
	psp_free,
	psp_batteryLifePercent,
	psp_IsBatteryCharging,
};