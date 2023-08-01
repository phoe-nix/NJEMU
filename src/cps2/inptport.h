/******************************************************************************

	inptport.c

	CPS2 入力ポートエミュレーション

******************************************************************************/

#ifndef CPS2_INPUT_PORT_H
#define CPS2_INPUT_PORT_H


#define CPS2_PORT_MAX		4
#define CPS2_BUTTON_MAX		6

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
	P2_START,		// 12

	P1_DIAL_L,		// 13
	P1_DIAL_R,		// 14

	SERV_COIN,		// 15
	SERV_SWITCH,	// 16

	P1_AF_1,		// 17
	P1_AF_2,		// 18
	P1_AF_3,		// 19
	P1_AF_4,		// 20
	P1_AF_5,		// 21
	P1_AF_6,		// 22
	
	P1_12,			// 23
	P1_13,			// 24
	P1_14,			// 25
	P1_23,			// 26
	P1_24,			// 27
	P1_34,			// 28
	P1_123,			// 29
	P1_124,			// 30
	P1_134,			// 31
	P1_234,			// 32
	P1_1234,		// 33
	P1_456,			// 34
	
	SNAPSHOT,		// 35
	SWPLAYER,		// 36
	COMMANDLIST,	// 37

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


extern int option_controller;
extern uint16_t cps2_port_value[CPS2_PORT_MAX];

extern int input_map[MAX_INPUTS];
extern int input_max_players;
extern int input_max_buttons;
extern int input_coin_chuter;
extern int analog_sensitivity;
extern int af_interval;

int input_init(void);
void input_shutdown(void);
void input_reset(void);
void update_inputport(void);

void setup_autofire(void);

#ifdef SAVE_STATE
STATE_SAVE( input );
STATE_LOAD( input );
#endif

#endif /* CPS2_INPUT_PORT_H */
