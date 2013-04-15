/******************************************************************************

	input.c

    PSP•≥•Û•»•Ì©`•È»Î¡¶÷∆”˘

******************************************************************************/

#ifndef PSP_INPUT_H
#define PSP_INPUT_H

#define PSP_CTRL_ANY		\
	(PSP_CTRL_SELECT |		\
	 PSP_CTRL_START |		\
	 PSP_CTRL_UP |			\
	 PSP_CTRL_RIGHT |		\
	 PSP_CTRL_DOWN |		\
	 PSP_CTRL_LEFT |		\
	 PSP_CTRL_LTRIGGER |	\
	 PSP_CTRL_RTRIGGER |	\
	 PSP_CTRL_TRIANGLE |	\
	 PSP_CTRL_CIRCLE |		\
	 PSP_CTRL_CROSS |		\
	 PSP_CTRL_SQUARE)

#define PAD_WAIT_INFINITY	-1

void pad_init(void);
UINT32 poll_gamepad(void);
#if (EMU_SYSTEM == MVS)
UINT32 poll_gamepad_fatfursp(void);
UINT32 poll_gamepad_analog(void);
#endif
void pad_update(void);
int pad_pressed(UINT32 code);
int pad_pressed_any(UINT32 disable_code);
void pad_wait_clear(void);
void pad_wait_press(int msec);

#endif /* PSP_INPUT_H */
