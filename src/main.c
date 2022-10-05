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

/*task arg*/
uint8_t mainTaskArg = MAIN_TASK_ID;
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
 * @param[in] pData            	Data pointer
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
 * @brief     OS main task
 * @param[out] pvParameters             event arg
 * @return                              void
 *******************************************************************************/
static void mainTask(void *pvParameters)
{
	void *pData = NULL;
	uint16_t dataLen = 0;
	while(1)
	{
		xQueueReceive( osMessageQueue, /* The handle of the queue. */
        				&pData, /* Store the buffer’s address in pcReceivedString. */
        				portMAX_DELAY );
		if(pData)
		{
			osMsg_t* pBuf = (osMsg_t*) pData;
			dataLen = BUILD_UINT16(pBuf->dataLen[0], pBuf->dataLen[1])+3;
			uart0Send((uint8_t*)pBuf, dataLen);	/*debug*/
			switch (pBuf->eventID)
			{
				case EVENT_BLE_DEV_ADDR:	/*0x12*/
				{
					uart0Send(&pBuf->eventID, 1);
				}
				break;
				case EVENT_BLE_DEV_FW_VER:	/*0x15*/
				{
					uart0Send(&pBuf->eventID, 1);
				}
				break;
				case EVENT_BLE_DEV_ENABLE_ADV:	/*0x1b*/
				{
					uart0Send(&pBuf->eventID, 1);
				}
				break;
				case EVENT_BLE_DEV_RECV_DATA:	/*0x80*/
				{
					uart0Send(&pBuf->eventID, 1);
				}
				break;
				case EVENT_BLE_DEV_STATE:	/*0x81*/
				{
					uart0Send(&pBuf->eventID, 1);
					PA12 = 1 ;
				}
				break;
				case EVENT_BLE_DEV_CONN_RSSI:	/*0x89*/
				{
					uart0Send(&pBuf->eventID, 1);
					PA12 = 0;
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
 * @brief     main
 * @param[out] argc                     is the link handle.
 * @param[out] argv                     the role type of the local device.
 * @return                              void
 *******************************************************************************/
int main(int argc, char const *argv[])
{
	uint8_t tBuf[] = {0xaa, 0xbb, MAIN_TASK_ID, 0x00, 0x01, 0x00};
	taskENTER_CRITICAL();

	init_HCLK();
	osPeripheralInit();
	// SysTick_Config(SystemCoreClock / 1000);
	init_UART0(115200);
	uart0Send(tBuf, 6);
	osUartInit();
	osMessageInit();
	stateCheckTimerHandle = xTimerCreate("stateCheck" /* The timer name. */,
										1000 / portTICK_PERIOD_MS /*const TickType_t xTimerPeriodInTicks*/,
										pdTRUE /*const UBaseType_t uxAutoReload, pdFALSE for on shot, pdTRUE for period*/,
										NULL /*void * const pvTimerID*/,
										stateCheckTimerCb /*TimerCallbackFunction_t pxCallbackFunction*/);
	xTaskCreate(mainTask, "mainTask", 256, (void *)&mainTaskArg, 2, NULL /*pxCreatedTask*/);
	taskEXIT_CRITICAL();
	xTimerStart(stateCheckTimerHandle, 0);
	vTaskStartScheduler();
	return 0;
}
/*************** END OF FUNCTIONS *********************************************/
