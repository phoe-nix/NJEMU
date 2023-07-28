#include <stdio.h>
#include <stdbool.h>
#include <psppower.h>

bool powerIsBatteryCharging(void) {
	return scePowerIsBatteryCharging();
}

int powerGetBatteryLifePercent(void) {
	return scePowerGetBatteryLifePercent();
}