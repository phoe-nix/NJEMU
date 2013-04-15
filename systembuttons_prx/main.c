/******************************************************************************

	SystemButtons.prx

******************************************************************************/

#include <pspsdk.h>
#include <pspctrl.h>
#include <pspimpose_driver.h>


PSP_MODULE_INFO("SystemButtons", PSP_MODULE_KERNEL, 0, 0);
PSP_MAIN_THREAD_ATTR(0);


/******************************************************************************
	prototypes
******************************************************************************/

int sceCtrl_driver_C4AAD55F(SceCtrlData *pad_data, int count);

#define sceCtrlPeekBufferPositive371	sceCtrl_driver_C4AAD55F


/******************************************************************************
	local variables
******************************************************************************/

static volatile int thread_active;
static unsigned int system_buttons;
static int main_volume;
static SceUID sysbutton_thread;

static int (*__sceCtrlPeekBufferPositive)(SceCtrlData *pad_data, int count);
static int (*__sceImposeGetParam)(SceImposeParam param);


/******************************************************************************
	functions
******************************************************************************/

static int SystemButtonsThread(SceSize args, void *argp)
{
	SceCtrlData paddata;

	thread_active = 1;

	while (thread_active)
	{
		if (__sceCtrlPeekBufferPositive)
		{
			(*__sceCtrlPeekBufferPositive)(&paddata, 1);
			system_buttons = paddata.Buttons & 0xf70000;

			if (__sceImposeGetParam)
				main_volume = (*__sceImposeGetParam)(PSP_IMPOSE_MAIN_VOLUME);
		}
		sceKernelDelayThread(1000000/60);
	}

	sceKernelExitThread(0);

	return 0;
}


void initSystemButtons(int devkit_version)
{
	if (devkit_version < 0x03070110)
		__sceCtrlPeekBufferPositive = sceCtrlPeekBufferPositive;
	else
		__sceCtrlPeekBufferPositive = sceCtrlPeekBufferPositive371;

	if (devkit_version >= 0x03050210)
		__sceImposeGetParam = sceImposeGetParam;
}


unsigned int readSystemButtons(void)
{
	return system_buttons;
}


unsigned int readHomeButton(void)
{
	return system_buttons & PSP_CTRL_HOME;
}


unsigned int readVolumeButtons(void)
{
	return system_buttons & (PSP_CTRL_VOLUP | PSP_CTRL_VOLDOWN);
}


unsigned int readVolUpButton(void)
{
	return system_buttons & PSP_CTRL_VOLUP;
}


unsigned int readVolDownButton(void)
{
	return system_buttons & PSP_CTRL_VOLDOWN;
}


unsigned int readNoteButton(void)
{
	return system_buttons & PSP_CTRL_NOTE;
}


unsigned int readScreenButton(void)
{
	return system_buttons & PSP_CTRL_SCREEN;
}


unsigned int readHoldSwitch(void)
{
	return system_buttons & PSP_CTRL_HOLD;
}


unsigned int readWLANSwitch(void)
{
	return system_buttons & PSP_CTRL_WLAN_UP;
}


int readMainVolume(void)
{
	return main_volume;
}


int module_start(SceSize args, void *argp)
{
	__sceCtrlPeekBufferPositive = NULL;
	__sceImposeGetParam = NULL;

	system_buttons = 0;
	main_volume = -1;
	thread_active = 0;
	sysbutton_thread = sceKernelCreateThread(
							"System Button Thread",
							SystemButtonsThread,
							0x11,
							0x200,
							0,
							NULL
						);

	if (sysbutton_thread >= 0)
		sceKernelStartThread(sysbutton_thread, 0, 0);

	return 0;
}


int module_stop(void)
{
	if (sysbutton_thread >= 0)
	{
		thread_active = 0;
		sceKernelWaitThreadEnd(sysbutton_thread, NULL);
	}
	return 0;
}
