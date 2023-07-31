#include <stdio.h>
#include <stdlib.h>
#include <pspctrl.h>
#include "common/input_driver.h"
#include "common/input.h"

typedef struct psp_input {
	int32_t channel;
} psp_input_t;

static void *psp_init(void) {
	psp_input_t *psp = (psp_input_t*)calloc(1, sizeof(psp_input_t));

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	
	return psp;
}

static void psp_free(void *data) {
	psp_input_t *psp = (psp_input_t*)data;
	free(psp);
}

static uint32_t basicPoll(SceCtrlData *paddata) {
	uint32_t data = 0;

	sceCtrlPeekBufferPositive(paddata, 1);

	data |= (paddata->Buttons & PSP_CTRL_UP) ? 1 << PLATFORM_PAD_UP : 0;
	data |= (paddata->Buttons & PSP_CTRL_DOWN) ? 1 << PLATFORM_PAD_DOWN : 0;
	data |= (paddata->Buttons & PSP_CTRL_LEFT) ? 1 << PLATFORM_PAD_LEFT : 0;
	data |= (paddata->Buttons & PSP_CTRL_RIGHT) ? 1 << PLATFORM_PAD_RIGHT : 0;

	data |= (paddata->Buttons & PSP_CTRL_CIRCLE) ? 1 << PLATFORM_PAD_B1 : 0;
	data |= (paddata->Buttons & PSP_CTRL_CROSS) ? 1 << PLATFORM_PAD_B2 : 0;
	data |= (paddata->Buttons & PSP_CTRL_SQUARE) ? 1 << PLATFORM_PAD_B3 : 0;
	data |= (paddata->Buttons & PSP_CTRL_TRIANGLE) ? 1 << PLATFORM_PAD_B4 : 0;

	data |= (paddata->Buttons & PSP_CTRL_LTRIGGER) ? 1 << PLATFORM_PAD_L : 0;
	data |= (paddata->Buttons & PSP_CTRL_RTRIGGER) ? 1 << PLATFORM_PAD_R : 0;
	
	data |= (paddata->Buttons & PSP_CTRL_START) ? 1 << PLATFORM_PAD_START : 0;
	data |= (paddata->Buttons & PSP_CTRL_SELECT) ? 1 << PLATFORM_PAD_SELECT : 0;

	return data;
}

static uint32_t psp_poll(void *data) {
	SceCtrlData paddata;
	uint32_t btnsData = 0;

	btnsData = basicPoll(&paddata);

	if (paddata.Ly >= 0xd0) btnsData |= 1 << PLATFORM_PAD_DOWN;
	if (paddata.Ly <= 0x30) btnsData |= 1 << PLATFORM_PAD_UP;
	if (paddata.Lx <= 0x30) btnsData |= 1 << PLATFORM_PAD_LEFT;
	if (paddata.Lx >= 0xd0) btnsData |= 1 << PLATFORM_PAD_RIGHT;

	return btnsData;
}

#if (EMU_SYSTEM == MVS)
static uint32_t psp_pollFatfursp(void *data) {
	SceCtrlData paddata;
	uint32_t btnsData = 0;

	btnsData = basicPoll(&paddata);

	if (!(paddata.Buttons & PSP_CTRL_UP)    && paddata.Ly >= 0xd0) btnsData |= 1 << PLATFORM_PAD_DOWN;
	if (!(paddata.Buttons & PSP_CTRL_DOWN)  && paddata.Ly <= 0x30) btnsData |= 1 << PLATFORM_PAD_UP;
	if (!(paddata.Buttons & PSP_CTRL_RIGHT) && paddata.Lx <= 0x30) btnsData |= 1 << PLATFORM_PAD_LEFT;
	if (!(paddata.Buttons & PSP_CTRL_LEFT)  && paddata.Lx >= 0xd0) btnsData |= 1 << PLATFORM_PAD_RIGHT;

	return btnsData;
}

static uint32_t psp_pollAnalog(void *data) {
	uint32_t btnsData;
	SceCtrlData paddata;

	btnsData = basicPoll(&paddata);

	if (paddata.Ly >= 0xd0) btnsData |= 1 << PLATFORM_PAD_DOWN;
	if (paddata.Ly <= 0x30) btnsData |= 1 << PLATFORM_PAD_UP;
	if (paddata.Lx <= 0x30) btnsData |= 1 << PLATFORM_PAD_LEFT;
	if (paddata.Lx >= 0xd0) btnsData |= 1 << PLATFORM_PAD_RIGHT;

	btnsData  = paddata.Buttons & 0xffff;
	btnsData |= paddata.Lx << 16;
	btnsData |= paddata.Ly << 24;

	return btnsData;
}
#endif


input_driver_t input_psp = {
	"psp",
	psp_init,
	psp_free,
	psp_poll,
#if (EMU_SYSTEM == MVS)
	psp_pollFatfursp,
	psp_pollAnalog,
#endif
};