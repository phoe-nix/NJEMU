#include <stdio.h>
#include <stdlib.h>
#include <pspaudio.h>
#include "common/audio_driver.h"

typedef struct psp_audio {
	int32_t channel;
} psp_audio_t;

static void *psp_init(void) {
	psp_audio_t *psp = (psp_audio_t*)calloc(1, sizeof(psp_audio_t));
	return psp;
}

static void psp_free(void *data) {
	psp_audio_t *psp = (psp_audio_t*)data;
	free(psp);
}

static int32_t psp_volumeMax(void *data) {
	return PSP_AUDIO_VOLUME_MAX;
}

static bool psp_chSRCReserve(void *data, int32_t samples, int32_t freqency, int32_t channels) {
	psp_audio_t *psp = (psp_audio_t*)data;
	psp->channel = sceAudioSRCChReserve(samples, freqency, channels);
	return psp->channel >= 0;
}

static bool psp_chReserve(void *data, int32_t samplecount, int32_t channels) {
	psp_audio_t *psp = (psp_audio_t*)data;
	int32_t format = channels == 1 ? PSP_AUDIO_FORMAT_MONO : PSP_AUDIO_FORMAT_STEREO;
	psp->channel = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, samplecount, format);
	return psp->channel >= 0;
}

static void psp_release(void *data) {
	sceAudioSRCChRelease();
}

static void psp_srcOutputBlocking(void *data, int32_t volume, void *buffer) {
	sceAudioSRCOutputBlocking(volume, buffer);
}

static void psp_outputPannedBlocking(void *data, int leftvol, int rightvol, void *buf) {
	psp_audio_t *psp = (psp_audio_t*)data;
	sceAudioOutputPannedBlocking(psp->channel, leftvol, rightvol, buf);
}

audio_driver_t audio_psp = {
	"psp",
	psp_init,
	psp_free,
	psp_volumeMax,
	psp_chSRCReserve,
	psp_chReserve,
	psp_srcOutputBlocking,
	psp_outputPannedBlocking,
	psp_release,
};