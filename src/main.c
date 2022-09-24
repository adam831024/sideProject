/******************************************************************************
 * Copyright (C)
 *
 * NAME:
 *		main.c
 * DESCRIPTION:
 *
*******************************************************************************/
/******************************************************************************
 * Includes
 *******************************************************************************/
/*Standard include*/
#include <stdint.h>
#include "Nano100Series.h"              // Device header
#include "uart.h"

/*Free-RTOS include*/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

/*Application include*/
#include "osUart.h"
#include "osUtility.h"
/******************************************************************************
 * Module Preprocessor Constants
 *******************************************************************************/
#define CONSTANT 5

/******************************************************************************
 * Module Preprocessor Macros
 *******************************************************************************/

/******************************************************************************
 * Module Typedefs
 *******************************************************************************/

/******************************************************************************
 * Module Variable Definitions
 *******************************************************************************/

/******************************************************************************
 * Function Prototypes
 *******************************************************************************/

/******************************************************************************
 * Function Definitions
 *******************************************************************************/
 /******************************************************************************
 * @brief     init HCLK
 * @return                              void
 *******************************************************************************/
static void init_HCLK(void)
{
	SYS_UnlockReg();
	CLK_EnableXtalRC(CLK_PWRCTL_HXT_EN_Msk); 
	CLK_WaitClockReady(CLK_CLKSTATUS_HXT_STB_Msk);
	CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT, CLK_HCLK_CLK_DIVIDER(1));
	SYS_LockReg();
}
/******************************************************************************
 * @brief     main
 * @param[out] argc                     is the link handle.
 * @param[out] argv                     the role type of the local device.
 * @return                              void
 *******************************************************************************/
int main(int argc, char const *argv[])
{
	uint8_t tBuf[] = {0xaa, 0xbb, 0x81, 0x00, 0x01, 0x00};
	init_HCLK();
	SysTick_Config(SystemCoreClock / 1000);
	init_UART0(115200);
	
	uart0Send(tBuf, 6);
	while(1)
	{
		static uint8_t count = 0;
		uart0Send(&count, 1);
		count++;
		delay_ms(1000);
	}
	return 0;
}
/*************** END OF FUNCTIONS *********************************************/
