/******************************************************************************

	inptport.c

	NEOGEO CDZ »Î¡¶•›©`•»•®•ﬂ•Â•Ï©`•∑•Á•Û

******************************************************************************/

#ifndef NCDZ_INPTPORT_H
#define NCDZ_INPTPORT_H

#define NCDZ_PORT_MAX	3

enum
{
	NCDZ_A = 0,
	NCDZ_B,
	NCDZ_C,
	NCDZ_D,
	NCDZ_BUTTON_MAX,
};

enum
{
	P1_UP = 0,	// 0
	P1_DOWN,	// 1
	P1_LEFT,	// 2
	P1_RIGHT,	// 3
	P1_BUTTONA,	// 4
	P1_BUTTONB,	// 5
	P1_BUTTONC,	// 6
	P1_BUTTOND,	// 7
	P1_START,	// 8
	P1_SELECT,	// 9
	P1_AF_A,	// 10
	P1_AF_B,	// 11
	P1_AF_C,	// 12
	P1_AF_D,	// 13
	P1_AB,		// 14
	P1_AC,		// 15
	P1_AD,		// 16
	P1_BC,		// 17
	P1_BD,		// 18
	P1_CD,		// 19
	P1_ABC,		// 20
	P1_ABD,		// 21
	P1_ACD,		// 22
	P1_BCD,		// 23
	P1_ABCD,	// 24
	SNAPSHOT,	// 25
	SWPLAYER,	// 26
	COMMANDLIST,// 27
	MAX_INPUTS
};

enum
{
	INPUT_PLAYER1 = 0,
	INPUT_PLAYER2
};

extern int option_controller;
extern int input_map[MAX_INPUTS];
extern int af_interval;
extern int analog_sensitivity;

extern uint8_t neogeo_port_value[NCDZ_PORT_MAX];

int input_init(void);
void input_shutdown(void);
void input_reset(void);
void setup_autofire(void);
void update_inputport(void);

#ifdef SAVE_STATE
STATE_SAVE( input );
STATE_LOAD( input );
#endif

#endif /* NCDZ_INPTPORT_H */
