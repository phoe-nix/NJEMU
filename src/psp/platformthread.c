#include <stdio.h>
#include <pspthreadman.h>
#include "platformthread.h"
#include "common/threadwrapper.h"

platformThread_t platformCreateThread(const char *name, void (*threadFunc)(void *), uint32_t priority, uint32_t stackSize) {
	return sceKernelCreateThread(name, threadFunc, priority, stackSize, 0, NULL);
}

void platformStartThread(platformThread_t thread) {
	sceKernelStartThread(thread, 0, NULL);
}

void platformWaitThreadEnd(platformThread_t thread) {
	sceKernelWaitThreadEnd(thread, NULL);
}

void platformDeleteThread(platformThread_t thread) {
	sceKernelDeleteThread(thread);
}

void platformExitThread(uint32_t exitCode) {
	sceKernelExitThread(exitCode);
}