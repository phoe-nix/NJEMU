/******************************************************************************

	threadwrapper.h

******************************************************************************/

#ifndef THREAD_WRAPPER_H
#define THREAD_WRAPPER_H

#include "emumain.h"

platformThread_t platformCreateThread(const char *name, void (*threadFunc)(void *), uint32_t priority, uint32_t stackSize);
void platformStartThread(platformThread_t thread);
void platformWaitThreadEnd(platformThread_t thread);
void platformDeleteThread(platformThread_t thread);
void platformExitThread(uint32_t exitCode);

#endif /* THREAD_WRAPPER_H */