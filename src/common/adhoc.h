/******************************************************************************

	adhoc.c

	AdHoc 入力ポートデータ送信スレッド

******************************************************************************/

#ifndef ADHOC_THREAD_H
#define ADHOC_THREAD_H

#ifdef ADHOC

typedef struct
{
	UINT32 frame;
	UINT32 buttons;
	UINT8 error;
	UINT8 paused;
	UINT8 loop_flag;
	UINT8 padding;
#if (EMU_SYSTEM == CPS1)
	UINT16 port_value[6];
#elif (EMU_SYSTEM == CPS2)
	UINT16 port_value[4];
#elif (EMU_SYSTEM == MVS)
	UINT16 port_value[6];
#endif
} ADHOC_DATA;


extern int adhoc_enable;
extern int adhoc_server;
extern char adhoc_matching[32];

extern ADHOC_DATA send_data;
extern ADHOC_DATA recv_data;
extern UINT32 adhoc_frame;
extern UINT8 adhoc_paused;
extern volatile int adhoc_update;


int adhoc_start_thread(void);
void adhoc_stop_thread(void);
void adhoc_reset_thread(void);
void adhoc_pause(void);

#endif /* ADHOC */

#endif /* ADHOC_THREAD_H */
