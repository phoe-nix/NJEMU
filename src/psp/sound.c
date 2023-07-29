/******************************************************************************

	sound.c

	PSP サウンドスレッド

******************************************************************************/

#include <pspaudio.h>
#include "psp.h"
#include "emumain.h"
#include "common/thread_driver.h"

/******************************************************************************
	プロトタイプ
******************************************************************************/

//int sceAudio_38553111(unsigned short samples, unsigned short freq, char unknown);
//int sceAudio_5C37C0AE(void);
//int sceAudio_E0727056(int volume, void *buffer);


/******************************************************************************
	ロ�`カル�篳�
******************************************************************************/

static volatile int sound_active;
static void *sound_thread;
static int sound_volume;
static int sound_enable;
static int16_t ALIGN16_DATA sound_buffer[2][SOUND_BUFFER_SIZE];

static struct sound_t sound_info;


/******************************************************************************
	グロ�`バル�篳�
******************************************************************************/

struct sound_t *sound = &sound_info;


/******************************************************************************
	ロ�`カル�v方
******************************************************************************/

/*--------------------------------------------------------
	サウンド厚仟スレッド
--------------------------------------------------------*/

static int sound_update_thread(int32_t args, void *argp)
{
	int flip = 0;

	while (sound_active)
	{
		if (Sleep)
		{
			do
			{
				usleep(5000000);
			} while (Sleep);
		}

		if (sound_enable)
			(*sound->update)(sound_buffer[flip]);
		else
			memset(sound_buffer[flip], 0, SOUND_BUFFER_SIZE * 2);

		sceAudioSRCOutputBlocking(sound_volume, sound_buffer[flip]);
		flip ^= 1;
	}

	thread_driver->exitThread(sound_thread, 0);

	return 0;
}


/******************************************************************************
	グロ�`バル�v方
******************************************************************************/

/*--------------------------------------------------------
	サウンド兜豚晒
--------------------------------------------------------*/

void sound_thread_init(void)
{
	sound_active = 0;
	sound_thread = NULL;
	sound_volume = 0;
	sound_enable = 0;
}


/*--------------------------------------------------------
	サウンド�K阻
--------------------------------------------------------*/

void sound_thread_exit(void)
{
	sound_thread_stop();
}


/*--------------------------------------------------------
	サウンド嗤��/�o�診个衞罎�
--------------------------------------------------------*/

void sound_thread_enable(int enable)
{
	if (sound_active)
	{
		sound_enable = enable;

		if (sound_enable)
			sound_thread_set_volume();
		else
			sound_volume = 0;
	}
}


/*--------------------------------------------------------
	サウンドのボリュ�`ム�O協
--------------------------------------------------------*/

void sound_thread_set_volume(void)
{
	sound_volume = PSP_AUDIO_VOLUME_MAX * (option_sound_volume * 10) / 100;
}


/*--------------------------------------------------------
	サウンドスレッド�_兵
--------------------------------------------------------*/

int sound_thread_start(void)
{
	sound_active = 0;
	sound_thread = NULL;
	sound_volume = 0;
	sound_enable = 0;

	memset(sound_buffer[0], 0, sizeof(sound_buffer[0]));
	memset(sound_buffer[1], 0, sizeof(sound_buffer[1]));

	sceAudioSRCChRelease();

	if (sceAudioSRCChReserve(sound->samples, sound->frequency, 2) < 0)
	{
		fatalerror(TEXT(COULD_NOT_RESERVE_AUDIO_CHANNEL_FOR_SOUND));
		return 0;
	}

	sound_thread = thread_driver->init();
	if (!thread_driver->createThread(sound_thread, "Sound thread", sound_update_thread, 0x08, sound->stack))
	{
		fatalerror(TEXT(COULD_NOT_START_SOUND_THREAD));
		sceAudioSRCChRelease();
		thread_driver->free(sound_thread);
		sound_thread = NULL;
		return 0;
	}

	sound_active = 1;
	thread_driver->startThread(sound_thread);

	sound_thread_set_volume();

	return 1;
}


/*--------------------------------------------------------
	サウンドスレッド唯峭
--------------------------------------------------------*/

void sound_thread_stop(void)
{
	if (sound_thread)
	{
		sound_volume = 0;
		sound_enable = 0;

		sound_active = 0;
		thread_driver->waitThreadEnd(sound_thread);
		thread_driver->deleteThread(sound_thread);
		thread_driver->free(sound_thread);
		sound_thread = NULL;

		sceAudioSRCChRelease();
	}
}
