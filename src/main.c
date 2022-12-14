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
#include "queue.h"

/*Application include*/
#include "osUart.h"
#include "osUtility.h"
#include "osPeripheral.h"
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
/*Queue osMessage*/
QueueHandle_t osMessageQueue;

/*timer*/
TimerHandle_t stateCheckTimerHandle = NULL;

/*task*/
 TaskHandle_t initTaskHandle = NULL;

/*task arg*/
uint8_t mainTaskArg = MAIN_TASK_ID;

/*callback function*/
osPeripheralCallback_t pOsPeripheralCb = NULL;
osOthersCallback_t pOsOthersCb = NULL;
/******************************************************************************
 * Function Prototypes
 *******************************************************************************/
static void osMessageInit(void );
static void init_HCLK(void);
static void mainTask(void *pvParameters);
static void stateCheckTimerCb(TimerHandle_t xTimer);
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
 * @brief     OS message Queue Sned
 * @param[in] pData            	Data pointer, parser will follow osMsg_t
 * @return                      void
 *******************************************************************************/
void osMessageSend(void *pData)
{
	xQueueSend( osMessageQueue, /* The handle of the queue. */
							&pData, /* The address of the pointer that points to the buffer. */
							portMAX_DELAY );
}
/******************************************************************************
 * @brief     OS message Queue Initialize
 * @return                              void
 *******************************************************************************/
static void osMessageInit(void )
{
	/* Create a queue that can hold a maximum of 10 pointers, in this case character pointers. */
	osMessageQueue = xQueueCreate( 10, sizeof( char * ) );
}
/******************************************************************************
 * @brief     register osPeripheral callback function
 * @return                              void
 *******************************************************************************/
void osPeripheralRegisterCb(osPeripheralCallback_t cbFunction)
{
	pOsPeripheralCb = cbFunction;
}
/******************************************************************************
 * @brief     register osOthers callback function
 * @return                              void
 *******************************************************************************/
void osOthersRegisterCb(osOthersCallback_t cbFunction)
{
	pOsOthersCb = cbFunction;
}
/******************************************************************************
 * @brief     OS main task
 * @param[out] pvParameters             event arg
 * @return                              void
 *******************************************************************************/
static void mainTask(void *pvParameters)
{
	void *pData = NULL;
	while(1)
	{
		xQueueReceive( osMessageQueue, /* The handle of the queue. */
										&pData, /* Store the buffer???s address in pcReceivedString. */
										portMAX_DELAY );
		if(pData)
		{
			osMsg_t *pBuf = (osMsg_t*) pData;
			switch (pBuf->taskID)
			{
				case OS_PERIPHERAL_HANDLER_ID:	/*0x01*/
				{
					if(pOsPeripheralCb != NULL)
					{
						pOsPeripheralCb(pBuf);
					}
				}
				break;
				case OS_OTHERS_HANDLER_ID:	/*0x02*/
				{
					if(pOsOthersCb != NULL)
					{
						pOsOthersCb(pBuf);
					}
				}
				break;
				default:
					break;
			}
			osFree(pData);
		}
		// uart0Send((uint8_t*)(pvParameters), 1);
		// vTaskDelay(1000);
		taskYIELD ();
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
 * @brief     OS main task
 * @param[out] pvParameters             event arg
 * @return                              void
 *******************************************************************************/
static void initTask(void *pvParameters)
{
	taskENTER_CRITICAL();
	
	osPeripheralInit();
	// SysTick_Config(SystemCoreClock / 1000);
	init_UART0(115200);
	osUartInit();
	osMessageInit();
	
	stateCheckTimerHandle = xTimerCreate("stateCheck" /* The timer name. */,
										1000 / portTICK_PERIOD_MS /*const TickType_t xTimerPeriodInTicks*/,
										pdTRUE /*const UBaseType_t uxAutoReload, pdFALSE for on shot, pdTRUE for period*/,
										NULL /*void * const pvTimerID*/,
										stateCheckTimerCb /*TimerCallbackFunction_t pxCallbackFunction*/);
	xTaskCreate(mainTask, "mainTask", 256, (void *)&mainTaskArg, 2, NULL /*pxCreatedTask*/);
	xTimerStart(stateCheckTimerHandle, 0);
	vTaskDelete( initTaskHandle );

	taskEXIT_CRITICAL();
}
/******************************************************************************
 * @brief     main
 * @param[out] argc                     is the link handle.
 * @param[out] argv                     the role type of the local device.
 * @return                              void
 *******************************************************************************/
int main(int argc, char const *argv[])
{
	init_HCLK();
	xTaskCreate(initTask, "initTask", 256, NULL, 2, &initTaskHandle);
	vTaskStartScheduler();
	return 0;
}
/*************** END OF FUNCTIONS *********************************************/
