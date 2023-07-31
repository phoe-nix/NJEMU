#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <psptypes.h>
#include <time.h>
#include <psprtc.h>
#include "common/ticker_driver.h"

typedef struct psp_ticker {
} psp_ticker_t;

static void *psp_init(void) {
	psp_ticker_t *psp = (psp_ticker_t*)calloc(1, sizeof(psp_ticker_t));
	return psp;
}

static void psp_free(void *data) {
	psp_ticker_t *psp = (psp_ticker_t*)data;
	free(psp);
}

static TICKER psp_ticker(void *data) {
	uint64_t current_ticks;

	sceRtcGetCurrentTick(&current_ticks);
	return current_ticks;
}

ticker_driver_t ticker_psp = {
	"psp",
	psp_init,
	psp_free,
	psp_ticker,
};