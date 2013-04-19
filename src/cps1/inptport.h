/******************************************************************************

	inptport.c

	CPS1 入力ポートエミュレーション

******************************************************************************/

#ifndef CPS1_INPUT_PORT_H
#define CPS1_INPUT_PORT_H

#define CPS1_PORT_MAX		4
#define CPS1_BUTTON_MAX		6

enum
{
	P1_RIGHT = 0,	// 0
	P1_LEFT,		// 1
	P1_DOWN,		// 2
	P1_UP,			// 3
	P1_BUTTON1,		// 4
	P1_BUTTON2,		// 5
	P1_BUTTON3,		// 6
	P1_BUTTON4,		// 7
	P1_BUTTON5,		// 8
	P1_BUTTON6,		// 9
	P1_COIN,		// 10
	P1_START,		// 11

	P1_DIAL_L,		// 12
	P1_DIAL_R,		// 13

	SERV_COIN,		// 14
	SERV_SWITCH,	// 15

	P1_AF_1,		// 16
	P1_AF_2,		// 17
	P1_AF_3,		// 18
	P1_AF_4,		// 19
	P1_AF_5,		// 20
	P1_AF_6,		// 21
	
	P1_12,			// 22
	P1_13,			// 23
	P1_14,			// 24
	P1_23,			// 25
	P1_24,			// 26
	P1_34,			// 27
	P1_123,			// 28
	P1_124,			// 29
	P1_134,			// 30
	P1_234,			// 31
	P1_1234,		// 32
	
	SNAPSHOT,		// 33
	SWPLAYER,		// 34
	COMMANDLIST,	// 35
	
	MAX_INPUTS
};

enum
{
	INPUT_PLAYER1 = 0,
	INPUT_PLAYER2,
	INPUT_PLAYER3,
	INPUT_PLAYER4
};

enum
{
	COIN_NONE = 0,	// 2P 2シューター固定 (チェック必要なし)
	COIN_2P1C,		// 2P 1シューター
	COIN_2P2C,		// 2P 2シューター
	COIN_3P1C,		// 3P 1シューター
	COIN_3P2C,		// 3P 2シューター
	COIN_3P3C,		// 3P 3シューター
	COIN_4P1C,		// 4P 1シューター
	COIN_4P2C,		// 4P 2シューター
	COIN_4P4C,		// 4P 4シューター
	COIN_MAX
};

enum
{
	DIP_A = 0,
	DIP_B,
	DIP_C
};


extern int option_controller;
extern UINT16 cps1_port_value[CPS1_PORT_MAX];
extern int cps1_dipswitch[3];

extern int input_map[MAX_INPUTS];
extern int input_max_players;
extern int input_max_buttons;
extern int analog_sensitivity;
extern int af_interval;

int input_init(void);
void input_shutdown(void);
void input_reset(void);
void update_inputport(void);

void setup_autofire(void);

UINT16 forgottn_read_dial0(void);
UINT16 forgottn_read_dial1(void);

#ifdef SAVE_STATE
STATE_SAVE( input );
STATE_LOAD( input );
#endif

#endif /* CPS1_INPUT_PORT_H */
