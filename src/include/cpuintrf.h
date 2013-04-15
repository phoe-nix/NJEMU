/******************************************************************************

	cpuintrf.h

	Core CPU interface definitions. (compatible for M.A.M.E.)

******************************************************************************/

#ifndef CPUINTRF_H
#define CPUINTRF_H


/*************************************
 *
 *	Enum listing all the CPUs
 *
 *************************************/

enum
{
	CPU_M68000 = 0,
	CPU_Z80,
	MAX_CPU
};



/*************************************
 *
 *	Interrupt line constants
 *
 *************************************/

#ifndef IRQ_LINE_STATE
#define IRQ_LINE_STATE
#define CLEAR_LINE		0		/* clear (a fired, held or pulsed) line */
#define ASSERT_LINE		1		/* assert an interrupt immediately */
#define HOLD_LINE		2		/* hold interrupt line until acknowledged */
#define PULSE_LINE		3		/* pulse interrupt line for one instruction */
#define IRQ_LINE_NMI	127		/* IRQ line for NMIs */
#endif

#endif /* CPUINTRF_H */
