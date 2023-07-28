/******************************************************************************

	adhoc.c

	AdHoc 入力ポートデータ送信スレッド

******************************************************************************/

#ifndef ADHOC_THREAD_H
#define ADHOC_THREAD_H

#ifdef ADHOC

typedef struct
{
	uint32_t frame;
	uint32_t buttons;
	uint8_t error;
	uint8_t paused;
	uint8_t loop_flag;
	uint8_t padding;
#if (EMU_SYSTEM == CPS1)
	uint16_t port_value[6];
#elif (EMU_SYSTEM == CPS2)
	uint16_t port_value[4];
#elif (EMU_SYSTEM == MVS)
	uint16_t port_value[6];
#endif
} ADHOC_DATA;


extern int adhoc_enable;
extern int adhoc_server;
extern char adhoc_matching[32];

extern ADHOC_DATA send_data;
extern ADHOC_DATA recv_data;
extern uint32_t adhoc_frame;
extern uint8_t adhoc_paused;
extern volatile int adhoc_update;


int adhoc_start_thread(void);
void adhoc_stop_thread(void);
void adhoc_reset_thread(void);
void adhoc_pause(void);

#endif /* ADHOC */

#endif /* ADHOC_THREAD_H */
