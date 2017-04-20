/* This is a template file for pins configuration created by New Kinetis SDK 2.x Project Wizard. Enjoy! */

#include "fsl_device_registers.h"
#include "MKL26Z4.h"

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Initialize all pins used in this example
 */
void BOARD_InitPins(void)
{
	// deactivate WDOG
	SIM->COPC = 0;
}
