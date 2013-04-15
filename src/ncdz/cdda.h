/******************************************************************************

	cdda.c

	NEOGEO CDZ CDDAエミュレーション

******************************************************************************/

#ifndef NCDZ_CDDA_H
#define NCDZ_CDDA_H

enum
{
	CDDA_STOP = 0,
	CDDA_PLAY,
	CDDA_PAUSE
};

extern int cdda_current_track;
extern int cdda_disabled;
extern volatile int cdda_playing;
extern volatile int cdda_autoloop;
extern volatile int cdda_command_ack;

int  cdda_init(void);
void cdda_shutdown(void);

void cdda_play(int);
void cdda_pause(void);
void cdda_stop(void);
void cdda_resume(void);

void neogeo_cdda_control(void);
void neogeo_cdda_check(void);

#ifdef SAVE_STATE
STATE_SAVE( cdda );
STATE_LOAD( cdda );
#endif

#endif /* NCDZ_CDDA_H */
