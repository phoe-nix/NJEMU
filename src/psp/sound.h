/******************************************************************************

	sound.c

	PSP サウンドスレッド

******************************************************************************/

#ifndef PSP_SOUND_THREAD_H
#define PSP_SOUND_THREAD_H

#define SOUND_SAMPLES_24000	(400*2)
#define SOUND_SAMPLES_44100	(736*2)
#define SOUND_SAMPLES_48000	(800)

#if (EMU_SYSTEM == CPS2)
#define SOUND_BUFFER_SIZE	((400*2)*2)	// 24KHz固定
#else
#define SOUND_BUFFER_SIZE	((736*2)*2)
#endif


struct sound_t
{
	int stack;
	int channels;
	int frequency;
	int samples;
	void (*update)(INT16 *buffer);
	void (*callback)(INT32 **buffer, int length);
};

#define MAXOUT		(+32767)
#define MINOUT		(-32768)

#define Limit(val, max, min)			\
{										\
	if (val > max) val = max;			\
	else if (val < min) val = min;		\
}

extern struct sound_t *sound;

void sound_thread_init(void);
void sound_thread_exit(void);
void sound_thread_enable(int enable);
void sound_thread_set_volume(void);
int sound_thread_start(void);
void sound_thread_stop(void);

#endif /* PSP_SOUND_THREAD_H */
