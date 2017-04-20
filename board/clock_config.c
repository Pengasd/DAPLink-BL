/* This is a template for clock configuration created by New Kinetis SDK 2.x Project Wizard. Enjoy! */

#include "MKL26Z4.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief configure clock after reset for this demo/example
 */
void BOARD_BootClockRUN(void) {
	/* The user configuration should be placed here */
	// delay to asure processor in FEI (21MHz = 21000 nops for 1ms)
	for(uint16_t count = 0; count < 21000u; count++)
		__asm("nop");

	MCG->C2 = (MCG_C2_RANGE0(0x01) | MCG_C2_HGO0_MASK | MCG_C2_EREFS0_MASK);
	MCG->C1 = (MCG_C1_CLKS(0x02) | MCG_C1_FRDIV(0x03));

	while((MCG->S & MCG_S_OSCINIT0_MASK) == 0)
		__asm("nop");
	while((MCG->S & MCG_S_IREFST_MASK) == MCG_S_IREFST_MASK)
		__asm("nop");
	while((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(0x02))
		__asm("nop");

	MCG->C5 = MCG_C5_PRDIV0(0x02);
	MCG->C6 = MCG_C6_PLLS_MASK;

	while((MCG->S & MCG_S_PLLST_MASK) == 0)
		__asm("nop");
	while((MCG->S & MCG_S_LOCK0_MASK) == 0)
		__asm("nop");

	MCG->C1 &= ~(MCG_C1_CLKS_MASK);

	while((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(0x03))
		__asm("nop");
}

