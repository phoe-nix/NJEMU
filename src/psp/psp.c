/******************************************************************************

	psp.c

	PSPメイン

******************************************************************************/

#include <fcntl.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <pspimpose_driver.h>

#include "SystemButtons.h"
#include "psp.h"
#include "common/power_driver.h"


#ifdef KERNEL_MODE
PSP_MODULE_INFO(TARGET_STR, PSP_MODULE_KERNEL, VERSION_MAJOR, VERSION_MINOR);
PSP_MAIN_THREAD_ATTR(0);
#else
PSP_MODULE_INFO(TARGET_STR, PSP_MODULE_USER, VERSION_MAJOR, VERSION_MINOR);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif


/******************************************************************************
	グロ�`バル�篳�
******************************************************************************/

volatile int Loop;
volatile int Sleep;
char launchDir[MAX_PATH];
char screenshotDir[MAX_PATH];  // スクリ�`ンショト隠贋PATH
int devkit_version;
int systembuttons_available;
int njemu_debug;


/******************************************************************************
	グロ�`バル�v方
******************************************************************************/


/******************************************************************************
	ロ�`カル�v方
******************************************************************************/

/*--------------------------------------------------------
	Power Callback
--------------------------------------------------------*/

static SceKernelCallbackFunction PowerCallback(int unknown, int pwrflags, void *arg)
{
	int cbid;

	if (pwrflags & PSP_POWER_CB_POWER_SWITCH)
	{
#if defined(LARGE_MEMORY) && ((EMU_SYSTEM == CPS2) || (EMU_SYSTEM == MVS))
		extern int32_t psp2k_mem_left;

		if (psp2k_mem_left < 0x400000)
		{
			char path[MAX_PATH];
			SceUID fd;

			sprintf(path, "%sresume.bin", launchDir);

			if ((fd = open(path, O_WRONLY|O_CREAT, 0777)) >= 0)
			{
				write(fd, (void *)(PSP2K_MEM_TOP + 0x1c00000), 0x400000);
				close(fd);
			}
		}
#endif
		Sleep = 1;
	}
	else if (pwrflags & PSP_POWER_CB_RESUME_COMPLETE)
	{
#if defined(LARGE_MEMORY) && ((EMU_SYSTEM == CPS2) || (EMU_SYSTEM == MVS))
		extern int32_t psp2k_mem_left;

		if (psp2k_mem_left < 0x400000)
		{
			char path[MAX_PATH];
			SceUID fd;

			sprintf(path, "%sresume.bin", launchDir);

			if ((fd = open(path, O_RDONLY, 0777)) >= 0)
			{
				read(fd, (void *)(PSP2K_MEM_TOP + 0x1c00000), 0x400000);
				close(fd);
			}
			remove(path);
		}
#endif
		Sleep = 0;
	}

	cbid = sceKernelCreateCallback("Power Callback", (void *)PowerCallback, NULL);

	scePowerRegisterCallback(0, cbid);

	return 0;
}

/*--------------------------------------------------------
	コ�`ルバックスレッド恬撹
--------------------------------------------------------*/

static int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Power Callback", (void *)PowerCallback, NULL);
	scePowerRegisterCallback(0, cbid);

	sceKernelSleepThreadCB();

	return 0;
}


/*--------------------------------------------------------
	コ�`ルバックスレッド�O協
--------------------------------------------------------*/

static int SetupCallbacks(void)
{
	SceUID thread_id = 0;

	thread_id = sceKernelCreateThread("Update Thread", CallbackThread, 0x12, 0xFA0, 0, NULL);
	if (thread_id >= 0)
	{
		sceKernelStartThread(thread_id, 0, 0);
	}

	Loop  = LOOP_EXIT;
	Sleep = 0;

	return thread_id;
}


/*--------------------------------------------------------
	main()
--------------------------------------------------------*/

#ifdef KERNEL_MODE
static int user_main(SceSize args, void *argp)
#else
int main(int argc, char *argv[])
#endif
{
	SceUID modID;
	char prx_path[MAX_PATH];

	getcwd(launchDir, MAX_PATH - 1);
	strcat(launchDir, "/");

	memset(screenshotDir, 0x00, sizeof(screenshotDir));

#if	(EMU_SYSTEM == CPS1)
	strcat(screenshotDir, "ms0:/PICTURE/CPS1");
#endif
#if	(EMU_SYSTEM == CPS2)
	strcat(screenshotDir, "ms0:/PICTURE/CPS2");
#endif
#if	(EMU_SYSTEM == MVS)
	strcat(screenshotDir, "ms0:/PICTURE/MVS");
#endif
#if	(EMU_SYSTEM == NCDZ)
	strcat(screenshotDir, "ms0:/PICTURE/NCDZ");
#endif

	mkdir(screenshotDir,0777); // スクショ喘フォルダ恬撹

	devkit_version = sceKernelDevkitVersion();
	njemu_debug = 0;

	SetupCallbacks();

	power_driver->setLowestCpuClock(NULL);

	ui_text_init();
	pad_init();

#if VIDEO_32BPP
	video_set_mode(32);
#else
	video_init();
#endif

	sprintf(prx_path, "%sSystemButtons.prx", launchDir);

	if ((modID = pspSdkLoadStartModule(prx_path, PSP_MEMORY_PARTITION_KERNEL)) >= 0)
	{
		initSystemButtons(devkit_version);
		systembuttons_available = 1;
	}
	else
		systembuttons_available = 0;

	file_browser();
	video_exit();

#ifdef KERNEL_MODE
	sceKernelExitThread(0);
#else
	sceKernelExitGame();
#endif

	return 0;
}


/*--------------------------------------------------------
	Kernelモ�`ド main()
--------------------------------------------------------*/

#ifdef KERNEL_MODE
int main(int argc, char *argv[])
{
	SceUID main_thread;

	pspSdkInstallNoPlainModuleCheckPatch();
	pspSdkInstallKernelLoadModulePatch();

#ifdef ADHOC
	pspSdkLoadAdhocModules();
#endif

	main_thread = sceKernelCreateThread(
						"User Mode Thread",
						user_main,
						0x11,
						256 * 1024,
						PSP_THREAD_ATTR_USER,
						NULL
					);

	sceKernelStartThread(main_thread, 0, 0);
	sceKernelWaitThreadEnd(main_thread, NULL);

	sceKernelExitGame();

	return 0;
}
#endif
