/******************************************************************************

	inptport.c

	MVS »Î¡¶•›©`•»•®•ﬂ•Â•Ï©`•∑•Á•Û

******************************************************************************/

#ifndef MVS_INPTPORT_H
#define MVS_INPTPORT_H

#define MVS_PORT_MAX	6

#define TEST_SWITCH_FLAGS	(PSP_CTRL_SELECT|PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER)

enum
{
	MVS_A = 0,
	MVS_B,
	MVS_C,
	MVS_D,
	MVS_BUTTON_MAX,
};

enum
{
	P1_UP = 0,		// 0
	P1_DOWN,		// 1
	P1_LEFT,		// 2
	P1_RIGHT,		// 3
	P1_BUTTONA,		// 4
	P1_BUTTONB,		// 5
	P1_BUTTONC,		// 6
	P1_BUTTOND,		// 7
	P1_START,		// 8
	P1_COIN,		// 9
	SERV_COIN,		// 10
	TEST_SWITCH,	// 11
	P1_AF_A,		// 12
	P1_AF_B,		// 13
	P1_AF_C,		// 14
	P1_AF_D,		// 15
	P1_AB,			// 16
	P1_AC,			// 17
	P1_AD,			// 18
	P1_BC,			// 19
	P1_BD,			// 20
	P1_CD,			// 21
	P1_ABC,			// 22
	P1_ABD,			// 23
	P1_ACD,			// 24
	P1_BCD,			// 25
	P1_ABCD,		// 26
	SNAPSHOT,		// 27
	SWPLAYER,		// 28
	COMMANDLIST,	// 29
	OTHER1,			// 30
	OTHER2,			// 31
	OTHER3,			// 32
	MAX_INPUTS
};

enum
{
	INPUT_PLAYER1 = 0,
	INPUT_PLAYER2
};

enum
{
	INPUT_AES = 0,
	INPUT_MVS
};


extern int option_controller;
extern int input_map[MAX_INPUTS];
extern int af_interval;
extern int neogeo_dipswitch;
extern int analog_sensitivity;

extern uint8_t neogeo_port_value[MVS_PORT_MAX];
extern int input_analog_value[2];

void check_input_mode(void);

int input_init(void);
void input_shutdown(void);
void input_reset(void);
void setup_autofire(void);
void update_inputport(void);

#ifdef SAVE_STATE
STATE_SAVE( input );
STATE_LOAD( input );
#endif

#endif /* MVS_INPTPORT_H */
