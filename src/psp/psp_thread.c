#include <stdio.h>
#include <pspthreadman.h>
#include "common/thread_driver.h"

typedef struct psp_thread {
	SceUID threadId;
} psp_thread_t;

void *psp_init(void) {
	psp_thread_t *psp = (psp_thread_t*)calloc(1, sizeof(psp_thread_t));
	return psp;
}

void psp_free(void *data) {
	psp_thread_t *psp = (psp_thread_t*)data;
	free(psp);
}

bool psp_createThread(void *data, const char *name, void (*threadFunc)(void *), uint32_t priority, uint32_t stackSize) {
	psp_thread_t *psp = (psp_thread_t*)data;
	psp->threadId = sceKernelCreateThread(name, threadFunc, priority, stackSize, 0, NULL);
	return psp->threadId >= 0;
}

void psp_startThread(void *data) {
	psp_thread_t *psp = (psp_thread_t*)data;
	sceKernelStartThread(psp->threadId, 0, NULL);
}

void psp_waitThreadEnd(void *data) {
	psp_thread_t *psp = (psp_thread_t*)data;
	sceKernelWaitThreadEnd(psp->threadId, NULL);
}

void psp_wakeupThread(void *data) {
	psp_thread_t *psp = (psp_thread_t*)data;
	sceKernelWakeupThread(psp->threadId);
}

void psp_deleteThread(void *data) {
	psp_thread_t *psp = (psp_thread_t*)data;
	sceKernelDeleteThread(psp->threadId);
}

void psp_resumeThread(void *data) {
	psp_thread_t *psp = (psp_thread_t*)data;
	sceKernelResumeThread(psp->threadId);
}

void psp_suspendThread(void *data) {
	psp_thread_t *psp = (psp_thread_t*)data;
	sceKernelSuspendThread(psp->threadId);
}

void psp_sleepThread(void *data) {
	sceKernelSleepThread();
}

void psp_exitThread(void *data, int32_t exitCode) {
	sceKernelExitThread(exitCode);
}

thread_driver_t thread_psp = {
	"psp",
	psp_init,
	psp_free,
	psp_createThread,
	psp_startThread,
	psp_waitThreadEnd,
	psp_wakeupThread,
	psp_deleteThread,
	psp_resumeThread,
	psp_suspendThread,
	psp_sleepThread,
	psp_exitThread
};