/******************************************************************************

	mp3.c

	PSP MP3スレッド

******************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <mad.h>
#include "common/thread_driver.h"
#include "common/audio_driver.h"

// TODO: remove this import
#include "emumain.h"

#define MP3_SAMPLES			(736 * 2)
#define MP3_BUFFER_SIZE		(MP3_SAMPLES * 4)


#define MP3_get_filesize()								\
	mp3_fsize = lseek(mp3_fd, 0, SEEK_END);	\
	lseek(mp3_fd, 0, SEEK_SET);


/******************************************************************************
	ローカル変数
******************************************************************************/

static char MP3_file[MAX_PATH];

static volatile int mp3_active;
static volatile int mp3_running;
static volatile int mp3_status;
static volatile int mp3_sleep;

static void *mp3_handle;
static void *mp3_thread;

static uint8_t mp3_out[2][MP3_BUFFER_SIZE];
static uint8_t mp3_in[(2 * MP3_BUFFER_SIZE) + MAD_BUFFER_GUARD];

static int mp3_newfile;
static int mp3_fsize;
static int mp3_filepos;
static int mp3_volume;

static uint32_t mp3_frame;
static uint32_t mp3_start_frame;

static int32_t mp3_fd = -1;


/******************************************************************************
	グローバル変数
******************************************************************************/

char mp3_dir[MAX_PATH];

int option_mp3_enable;
int option_mp3_volume;


/******************************************************************************
	ローカル関数
******************************************************************************/

/*--------------------------------------------------------
	MP3サンプルデータの範囲を修正
--------------------------------------------------------*/

INLINE int16_t MP3Limit(mad_fixed_t value)
{
	if (value >=  MAD_F_ONE) return 32767;
	if (value <= -MAD_F_ONE) return -32767;

	return (int16_t)(value >> (MAD_F_FRACBITS - 15));
}


/*--------------------------------------------------------
	スリープのチェック
--------------------------------------------------------*/

static int MP3SleepCheck(void)
{
	if (Sleep)
	{
		if (mp3_fd >= 0) close(mp3_fd);

		mp3_sleep = 1;

		do
		{
			usleep(5000000);
		} while (Sleep);

		mp3_sleep = 0;

		if ((mp3_fd = open(MP3_file, O_RDONLY, 0777)) < 0)
		{
			mp3_fd = -1;
			mp3_status = MP3_STOP;
			ui_popup(TEXT(COULD_NOT_REOPEN_MP3_FILEx), strrchr(MP3_file, '/') + 1);
			return 1;
		}

		lseek(mp3_fd, mp3_filepos, SEEK_SET);
	}
	else if (mp3_status == MP3_STOP)
	{
		return 1;
	}

	return 0;
}


/*--------------------------------------------------------
	MP3オーディオストリームを更新
--------------------------------------------------------*/

static void MP3Update(void)
{
	int flip;
	uint8_t *GuardPtr;
	int16_t *OutputPtr, *OutputEnd;
	struct mad_stream Stream;
	struct mad_frame Frame;
	struct mad_synth Synth;
	mad_timer_t Timer;

	mad_stream_init(&Stream);
	mad_frame_init(&Frame);
	mad_synth_init(&Synth);
	mad_timer_reset(&Timer);

	OutputPtr = (int16_t *)mp3_out[0];
	OutputEnd = (int16_t *)(mp3_out[0] + MP3_BUFFER_SIZE);
	GuardPtr = NULL;

	mp3_filepos = 0;
	mp3_frame = 0;
	flip = 0;
	cdda_command_ack = 1;

	while (mp3_active && mp3_status != MP3_STOP)
	{
		if (Stream.buffer == NULL || Stream.error == MAD_ERROR_BUFLEN)
		{
			uint32_t ReadSize, Remaining;
			uint8_t *ReadStart;

			if (Stream.next_frame != NULL)
			{
				Remaining = Stream.bufend - Stream.next_frame;
				ReadStart = mp3_in + Remaining;
				ReadSize  = (2 * MP3_BUFFER_SIZE) - Remaining;
				memmove(mp3_in, Stream.next_frame, Remaining);
			}
			else
			{
				ReadSize  = 2 * MP3_BUFFER_SIZE;
				ReadStart = mp3_in;
				Remaining = 0;
			}

			if (MP3SleepCheck()) break;

			ReadSize = read(mp3_fd, ReadStart, ReadSize);
			mp3_filepos += ReadSize;
			if (mp3_filepos == mp3_fsize)
			{
				if (cdda_autoloop)
				{
					mp3_filepos = 0;
					lseek(mp3_fd, 0, SEEK_SET);
				}
				else
				{
					cdda_playing = CDDA_STOP;
					mp3_status = MP3_STOP;
				}
			}

			if (mp3_filepos == mp3_fsize)
			{
				GuardPtr = ReadStart + ReadSize;
				memset(GuardPtr, 0, MAD_BUFFER_GUARD);
				ReadSize += MAD_BUFFER_GUARD;
			}

			mad_stream_buffer(&Stream, mp3_in, ReadSize + Remaining);

			Stream.error = 0;
		}

		if (mad_frame_decode(&Frame, &Stream))
		{
			if (MAD_RECOVERABLE(Stream.error))
			{
//				if (Stream.error != MAD_ERROR_LOSTSYNC || Stream.this_frame != GuardPtr)
				continue;
			}
			else if (Stream.error == MAD_ERROR_BUFLEN)
			{
				continue;
			}
			else
			{
				ui_popup(TEXT(MP3_DECODE_ERROR));
				mp3_status = MP3_STOP;
				break;
			}
		}

		mp3_frame++;
		mad_timer_add(&Timer, Frame.header.duration);
		mad_synth_frame(&Synth, &Frame);

		if (mp3_status == MP3_PLAY)
		{
			int i;

			for (i = 0; i < Synth.pcm.length; i++)
			{
				if (MAD_NCHANNELS(&Frame.header) == 2)
				{
					*OutputPtr++ = MP3Limit(Synth.pcm.samples[0][i]);
					*OutputPtr++ = MP3Limit(Synth.pcm.samples[1][i]);
				}
				else
				{
					int16_t data = MP3Limit(Synth.pcm.samples[0][i]);

					*OutputPtr++ = data;
					*OutputPtr++ = data;
				}

				if (OutputPtr == OutputEnd)
				{
					audio_driver->outputPannedBlocking(mp3_handle, mp3_volume, mp3_volume, mp3_out[flip]);
					flip ^= 1;
					OutputPtr = (int16_t *)mp3_out[flip];
					OutputEnd = (int16_t *)(mp3_out[flip] + MP3_BUFFER_SIZE);
				}
			}
		}
		else if (mp3_status == MP3_SEEK)
		{
			if (mp3_frame >= mp3_start_frame)
			{
				mp3_start_frame = 0;
				mp3_status = MP3_SLEEP;
				thread_driver->sleepThread(mp3_thread);
			}
		}
	}

	mad_synth_finish(&Synth);
	mad_frame_finish(&Frame);
	mad_stream_finish(&Stream);

	if (mp3_fd >= 0)
	{
		close(mp3_fd);
		mp3_fd = -1;
	}
}


/*--------------------------------------------------------
	MP3スレッド
--------------------------------------------------------*/

static int MP3Thread(int32_t args, void *argp)
{
	while (mp3_active)
	{
		thread_driver->sleepThread(mp3_thread);

		if (mp3_newfile)
		{
			mp3_newfile = 0;

			mp3_running = 1;
			MP3Update();
			mp3_running = 0;
		}
	}

	return 0;
}


/******************************************************************************
	グローバル関数
******************************************************************************/

/*--------------------------------------------------------
	MP3スレッド開始
--------------------------------------------------------*/

int mp3_thread_start(void)
{
	mp3_handle  = NULL;
	mp3_thread  = NULL;
	mp3_active  = 0;
	mp3_status  = MP3_STOP;
	mp3_running = 0;
	mp3_sleep   = 0;

	mp3_newfile = 0;
	mp3_volume  = 0;

	memset(mp3_out[0], 0, MP3_BUFFER_SIZE);
	memset(mp3_out[1], 0, MP3_BUFFER_SIZE);

	mp3_handle = audio_driver->init();

	if (!audio_driver->chReserve(mp3_handle, PSP_AUDIO_NEXT_CHANNEL, MP3_SAMPLES, PSP_AUDIO_FORMAT_STEREO))
	{
		fatalerror(TEXT(COULD_NOT_RESERVE_AUDIO_CHANNEL_FOR_MP3));
		audio_driver->free(mp3_handle);
		mp3_handle = NULL;
		return 0;
	}

	mp3_active = 1;
	mp3_thread = thread_driver->init();
	if (!thread_driver->createThread(mp3_thread, "MP3 thread", MP3Thread, 0x8, 0x40000))
	{
		fatalerror(TEXT(COULD_NOT_START_MP3_THREAD));
		audio_driver->release(mp3_handle);
		audio_driver->free(mp3_handle);
		thread_driver->free(mp3_thread);
		mp3_handle = NULL;
		mp3_thread = NULL;
		return 0;
	}

	thread_driver->startThread(mp3_thread);

	return 1;
}


/*--------------------------------------------------------
	MP3スレッド停止
--------------------------------------------------------*/

void mp3_thread_stop(void)
{
	if (mp3_thread)
	{
		mp3_active = 0;
		mp3_stop();

		thread_driver->wakeupThread(mp3_thread);
		thread_driver->waitThreadEnd(mp3_thread);
		thread_driver->deleteThread(mp3_thread);
		thread_driver->free(mp3_thread);
		mp3_thread = NULL;

		audio_driver->release(mp3_handle);
		audio_driver->free(mp3_handle);
		mp3_handle = NULL;
	}
}


/*--------------------------------------------------------
	MP3のボリューム設定
--------------------------------------------------------*/

void mp3_set_volume(void)
{
	mp3_volume = PSP_AUDIO_VOLUME_MAX * (option_mp3_volume * 10) / 100;
}


/*--------------------------------------------------------
	MP3再生
--------------------------------------------------------*/

int mp3_play(const char *name)
{
	if (mp3_thread)
	{
		strcpy(MP3_file, name);

		mp3_stop();

		if ((mp3_fd = open(MP3_file, O_RDONLY, 0777)) >= 0)
		{
			MP3_get_filesize();

			mp3_status  = MP3_PLAY;
			mp3_newfile = 1;

			mp3_set_volume();

			thread_driver->wakeupThread(mp3_thread);
			return 0;
		}
	}
	return 1;
}


/*--------------------------------------------------------
	MP3停止
--------------------------------------------------------*/

void mp3_stop(void)
{
	if (mp3_thread)
	{
		mp3_volume = 0;

		if (mp3_status == MP3_PAUSE)
			thread_driver->resumeThread(mp3_thread);

		mp3_status = MP3_STOP;
		while (mp3_running) usleep(1);

		memset(mp3_out[0], 0, MP3_BUFFER_SIZE);
		memset(mp3_out[1], 0, MP3_BUFFER_SIZE);
	}
}


/*--------------------------------------------------------
	MP3ポーズ
--------------------------------------------------------*/

void mp3_pause(int pause)
{
	if (mp3_thread)
	{
		if (mp3_running)
		{
			if (pause)
			{
				if (mp3_status == MP3_PLAY)
				{
					mp3_status = MP3_PAUSE;
					thread_driver->suspendThread(mp3_thread);
					memset(mp3_out[0], 0, MP3_BUFFER_SIZE);
					memset(mp3_out[1], 0, MP3_BUFFER_SIZE);
					audio_driver->outputPannedBlocking(mp3_handle, 0, 0, mp3_out[0]);
				}
			}
			else
			{
				if (mp3_status == MP3_PAUSE)
				{
					mp3_status = MP3_PLAY;
					thread_driver->resumeThread(mp3_thread);
				}
				else if (mp3_status == MP3_SLEEP)
				{
					mp3_status = MP3_PLAY;
					thread_driver->wakeupThread(mp3_thread);
				}
			}
		}
	}
}


/*--------------------------------------------------------
	シーク位置を設定(ステートロード用)
--------------------------------------------------------*/

void mp3_seek_set(const char *name, uint32_t frame)
{
	if (mp3_thread)
	{
		strcpy(MP3_file, name);

		mp3_stop();

		if ((mp3_fd = open(MP3_file, O_RDONLY, 0777)) < 0)
		{
			mp3_fsize       = 0;
			mp3_status      = MP3_STOP;
			mp3_start_frame = 0;
			mp3_newfile     = 0;
		}
		else
		{
			mp3_fsize = lseek(mp3_fd, 0, SEEK_END);
			lseek(mp3_fd, 0, SEEK_SET);

			mp3_status      = MP3_SEEK;
			mp3_start_frame = frame;
			mp3_newfile     = 1;
		}
	}
}


/*--------------------------------------------------------
	シーク開始
--------------------------------------------------------*/

void mp3_seek_start(void)
{
	if (mp3_thread)
	{
		mp3_set_volume();
		thread_driver->wakeupThread(mp3_thread);
	}
}


/*--------------------------------------------------------
	現在のMP3の再生フレームを取得
--------------------------------------------------------*/

uint32_t mp3_get_current_frame(void)
{
	return mp3_status ? mp3_frame : 0;
}


/*--------------------------------------------------------
	現在のMP3ステータスを取得
--------------------------------------------------------*/

int mp3_get_status(void)
{
	return mp3_status;
}
