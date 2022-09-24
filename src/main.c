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
#include "fifo.h"
#include "main.h"

/******************************************************************************
 * Module Preprocessor Constants
 *******************************************************************************/

/******************************************************************************
 * Module Preprocessor Macros
 *******************************************************************************/

/******************************************************************************
 * Module Typedefs
 *******************************************************************************/

/******************************************************************************
 * Module Variable Definitions
 *******************************************************************************/
/*timer*/
TimerHandle_t stateCheckTimerHandle = NULL;

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
 * @brief     OS event task
 * @param[out] pvParameters             event arg
 * @return                              void
 *******************************************************************************/
static void mainTask(void *pvParameters)
{
	while(1)
	{
		if(!IsFifoEmpty(&fifoBuffer) && pdTRUE== xSemaphoreTake(uartSem, portMAX_DELAY))
		{
			// uint8_t data = FifoPop(&fifoBuffer);
			// uart0Send(&data, 1);
			uartParserRun();
		}
		vTaskDelay(10);
	}
}
/******************************************************************************
 * @brief     state check timer callack every second
 * @param[out] xTimer             		the pointer to the TimerHandle_t.
 * @return                              void
 *******************************************************************************/
static void stateCheckTimerCb(TimerHandle_t xTimer)
{
	/*run every second*/
	static uint8_t count = 0;
	uart0Send(&count, 1);
	count++;
	// osMessageSend(NULL, MAIN_TASK, &event);
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
	taskENTER_CRITICAL();

	init_HCLK();
	SysTick_Config(SystemCoreClock / 1000);
	init_UART0(115200);
	uart0Send(tBuf, 6);
	uartBufferInit();
	stateCheckTimerHandle = xTimerCreate("stateCheck" /* The timer name. */,
										1000 / portTICK_PERIOD_MS /*const TickType_t xTimerPeriodInTicks*/,
										pdTRUE /*const UBaseType_t uxAutoReload, pdFALSE for on shot, pdTRUE for period*/,
										NULL /*void * const pvTimerID*/,
										stateCheckTimerCb /*TimerCallbackFunction_t pxCallbackFunction*/);
	xTaskCreate(mainTask, "mainTask", 256, (void *)123, 3, NULL /*pxCreatedTask*/);
	taskEXIT_CRITICAL();
	xTimerStart(stateCheckTimerHandle, 0);
	vTaskStartScheduler();
	return 0;
}
/*************** END OF FUNCTIONS *********************************************/
