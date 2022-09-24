/******************************************************************************
 * Copyright (C)
 *
 * NAME:
 *		osUart.c
 * DESCRIPTION:
 *      
*******************************************************************************/
/******************************************************************************
 * Includes
 *******************************************************************************/
/*Standard include*/
#include <stdint.h>
#include "Nano100Series.h"   
#include "uart.h" 

/*Free-RTOS include*/
#include "FreeRTOS.h"
#include "semphr.h"

/*Application include*/
#include "fifo.h"
#include "main.h"
#include "osUart.h"

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
/*timer*/
TimerHandle_t uartTimerHandle = NULL;

/*uart data fifo*/
SemaphoreHandle_t uartSem = NULL;
Fifo_t fifoBuffer;
uint8_t uartData[MAX_CMD_SIZE] = {0};
uint8_t uartFifoBuffer[MAX_CMD_SIZE] = {0};
/******************************************************************************
 * Function Prototypes
 *******************************************************************************/
static parserState_t uartParser(void);
static void uartTimerCb(TimerHandle_t xTimer);
/******************************************************************************
 * Function Definitions
 *******************************************************************************/
/******************************************************************************
 * @brief     UART0 init
 * @param[in] baudrate                  uart baudrate
 * @return                              void
 *******************************************************************************/
void init_UART0(uint32_t baudRate)
{
  SYS_UnlockReg();
  //--- CLK
  CLK_EnableModuleClock(UART0_MODULE);
  CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_UART_CLK_DIVIDER(1));
  //--- PIN
  SYS->PB_L_MFP = (SYS->PB_L_MFP & ~SYS_PB_L_MFP_PB0_MFP_Msk) | SYS_PB_L_MFP_PB0_MFP_UART0_RX;
  SYS->PB_L_MFP = (SYS->PB_L_MFP & ~SYS_PB_L_MFP_PB1_MFP_Msk) | SYS_PB_L_MFP_PB1_MFP_UART0_TX;
  SYS_LockReg();
  //--- OPEN
  UART_Open(UART0, baudRate);
  //--- NVIC
  UART_ENABLE_INT(UART0, UART_IER_RDA_IE_Msk);
  NVIC_EnableIRQ(UART0_IRQn);
}

/******************************************************************************
 * @brief     IRQ, UART0 Rx callback
 * @return                              void
 *******************************************************************************/
void UART0_IRQHandler(void)
{
  static BaseType_t xHigherPriorityTaskWoken;

  /*the same as UART_WRITE(UART0, UART0->RBR)*/
  // UART0->THR = UART0->RBR;

  /*push fifo and give semaphore*/
  FifoPush(&fifoBuffer, UART0->RBR);
  xSemaphoreGiveFromISR(uartSem, &xHigherPriorityTaskWoken);
  if( xHigherPriorityTaskWoken != pdFALSE )
  {
      // We can force a context switch here.  Context switching from an
      // ISR uses port specific syntax.  Check the demo task for your port
      // to find the syntax required.
  }
}
/******************************************************************************
 * @brief     UART0 Tx send
 * @param[in] pData                     data pointer
 * @param[in] dataLength                length of pData
 * @return                              void
 *******************************************************************************/
void uart0Send(uint8_t *pData, uint16_t dataLength)
{
  uint16_t i = 0;
  for (i = 0; i < dataLength; i++)
  {
    UART0->THR = pData[i];
  }
}
/******************************************************************************
 * @brief     UART1 init
 * @param[in] baudrate                  uart baudrate
 * @return                              void
 *******************************************************************************/
void init_UART1(uint32_t baudrate)
{
  SYS_UnlockReg();
  //--- CLK
  CLK_EnableModuleClock(UART1_MODULE);
  CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_UART_CLK_DIVIDER(1));
  //--- PIN
  SYS->PB_L_MFP &= ~SYS_PB_L_MFP_PB4_MFP_Msk;
  SYS->PB_L_MFP |= SYS_PB_L_MFP_PB4_MFP_UART1_RX;
  SYS->PB_L_MFP &= ~SYS_PB_L_MFP_PB5_MFP_Msk;
  SYS->PB_L_MFP |= SYS_PB_L_MFP_PB5_MFP_UART1_TX;
  SYS_LockReg();
  //--- OPEN
  UART_Open(UART1, baudrate);
  //--- NVIC
  UART_ENABLE_INT(UART1, UART_IER_RDA_IE_Msk);
  NVIC_EnableIRQ(UART1_IRQn);
}
/******************************************************************************
 * @brief     IRQ, UART1 Rx callback
 * @return                              void
 *******************************************************************************/
void UART1_IRQHandler(void)
{
  /*the same as UART_WRITE(UART1, UART1->RBR)*/
  UART1->THR = UART1->RBR;
}
/******************************************************************************
 * @brief     UART1 Tx send
 * @param[in] pData                     data pointer
 * @param[in] dataLength                length of pData
 * @return                              void
 *******************************************************************************/
void uart1Send(uint8_t *pData, uint16_t dataLength)
{
  uint16_t i = 0;
  for (i = 0; i < dataLength; i++)
  {
    UART1->THR = pData[i];
  }
}
/******************************************************************************
 * @brief     uart timer callack after 2 second to flush fifo
 * @param[out] xTimer             		the pointer to the TimerHandle_t.
 * @return                              void
 *******************************************************************************/
static void uartTimerCb(TimerHandle_t xTimer)
{
  if(!IsFifoEmpty(&fifoBuffer))
  {
    uint8_t indBuf[4];
    FifoFlush(&fifoBuffer);
    indBuf[0] = 0x86;
    indBuf[1] = 0x00;
    indBuf[2] = 0x01;
    indBuf[3] = PARSER_TIMEOUT_FLUSH;
    uart0Send(indBuf, 4);
  }
}
/******************************************************************************
 * @brief     Init UartFifo and Semaphore
 * @return                              void
 *******************************************************************************/
void uartBufferInit(void)
{
  FifoInit(&fifoBuffer, uartFifoBuffer, MAX_CMD_SIZE);
	uartSem = xSemaphoreCreateCounting( 65535, 0 );
  uartTimerHandle = xTimerCreate("uartTimer" /* The timer name. */,
										2000 / portTICK_PERIOD_MS /*const TickType_t xTimerPeriodInTicks*/,
										pdFALSE /*const UBaseType_t uxAutoReload, pdFALSE for on shot, pdTRUE for period*/,
										NULL /*void * const pvTimerID*/,
										uartTimerCb /*TimerCallbackFunction_t pxCallbackFunction*/);
}
/******************************************************************************
 * @brief     run with timer check fifo state 
 * @return                              void
 *******************************************************************************/
void uartParserRun(void)
{
  parserState_t status = PARSER_ERROR;
  /*timer start*/
  xTimerStart(uartTimerHandle, 0);
  /*fifo check and push*/
  if(IsFifoEmpty(&fifoBuffer))
  {
      FifoFlush(&fifoBuffer);
  }
  if(IsFifoFull(&fifoBuffer))
  {
      /*buffer full, Incoming Data lost!!*/
      FifoFlush(&fifoBuffer);
      status = PARSER_FULL;
  }
  else
  {
      /*push data into fifo*/
      status = uartParser();
  }
  /*check status*/
  if(PARSER_SUCCESS == status || PARSER_EMPTY == status )
  {
    /*timer stop*/
    xTimerStop( uartTimerHandle, 0 );
  }
  else if(status == PARSER_ERROR || status == PARSER_CRASH || PARSER_FULL == status)
  {
      uint8_t indBuf[4];
      indBuf[0] = 0x86;
      indBuf[1] = 0x00;
      indBuf[2] = 0x01;
      indBuf[3] = status;
      uart0Send(indBuf, 4);
  }
}
/******************************************************************************
 * @brief     UART data parser
 * @param[in] data                		uart data
 * @param[in] dataLength                uart data length
 * @return                              parserState_t
 *******************************************************************************/
// parserState_t uartParser(uint8_t *data, uint16_t dataLength)
static parserState_t uartParser(void)
{
	uint16_t fifoCnt = 0;
	uint16_t i = 0;
RELOOP:
	fifoCnt = FifoCount(&fifoBuffer);
	if (fifoCnt < CMD_HEAD_LEN)
	{
		return PARSER_WAITING;
	}
	if ((0xAA != FifoPrePop(&fifoBuffer, 0)) || (0xBB != FifoPrePop(&fifoBuffer, 1))) /*filter*/
	{
		/*0x12 0xAA 0xBB 0x15 0x00 0x01 0x01 0xAA*/
		/*0xBB 0x12 0x00 0x01 0x01*/
		/*0x12 0x34 0xAA*/
		fifoCnt = FifoCount(&fifoBuffer);
		if (fifoCnt)
		{
			for (i = 0; i < fifoCnt; i++)
			{
				if (0xAA == FifoPrePop(&fifoBuffer, 0))
				{
					if (0xBB == FifoPrePop(&fifoBuffer, 1))
					{
						/*0xAA 0xBB ...*/
						break;
					}
					else if (i == (fifoCnt - 1))
					{
						/*last is 0xAA, keep*/
						break;
					}
					else
					{
						/*0xAA 0x12 ...*/
						uint8_t die = FifoPop(&fifoBuffer);
					}
				}
				else
				{
					/*0x12 0x34 0x56 ...*/
					uint8_t die = FifoPop(&fifoBuffer);
				}
			}
		}
		else
		{
			FifoFlush(&fifoBuffer);
			return PARSER_EMPTY;
		}
	}

	fifoCnt = FifoCount(&fifoBuffer);
	if (fifoCnt >= CMD_HEAD_LEN) /*enough data*/
	{
		/*0xAA 0xBB 0x12 0x34 0x56 ...*/

		uint16_t payLoadLen = ((FifoPrePop(&fifoBuffer, 3) << 8) | FifoPrePop(&fifoBuffer, 4));

		if (fifoCnt >= (payLoadLen + CMD_HEAD_LEN))
		{
			/*0xAA 0xBB 0x12 0x00 0x03 0x11 0x22 0x33 0x44 0x55*/
			for (i = 0; i < (payLoadLen + CMD_HEAD_LEN); i++)
			{
				if (!IsFifoEmpty(&fifoBuffer))
				{
					uartData[i] = FifoPop(&fifoBuffer);
				}
				else
				{
					/*buffer empty*/
					FifoFlush(&fifoBuffer);
					return PARSER_CRASH; /*important error*/
				}
			}
			// TODO:	nfAmzCmdHandler(&uartData[2], payLoadLen + 3); /*0x12 0x00 0x03 0x11 0x22 0x33*/
      uart0Send(&uartData[2], payLoadLen + 3);
			fifoCnt = FifoCount(&fifoBuffer);
			if (fifoCnt)
			{
				/*0x44 0x55*/
				goto RELOOP;
			}
			else
			{
				FifoFlush(&fifoBuffer);
				return PARSER_SUCCESS;
			}
		}
		else
		{
			/*0xAA 0xBB 0x17 0xAA 0xBB 0x15 0x01 0x01*/
			/*0xaa 0xbb 0xaa 0x12 0x01 0x01*/
			for(i=0; i<(fifoCnt-3); i++) /*(fifoCnt-3):  message already started at 0xAA 0xBB, do not need to filter*/
			{
				if ((0xAA == FifoPrePop(&fifoBuffer, i + 2)) && (0xBB == FifoPrePop(&fifoBuffer, i + 3)))
				{
					FifoPop(&fifoBuffer);
					FifoPop(&fifoBuffer);
					goto RELOOP;
				}
			}
			return PARSER_WAITING;
		}
	}
	else if (fifoCnt)
	{
		/*0xAA or 0xAA 0xBB or 0xAA 0xBB 0x12 or 0xAA 0xBB 0x12 0x00*/
		return PARSER_WAITING;
	}
	else
	{
		/*no data in buffer*/
		FifoFlush(&fifoBuffer);
		return PARSER_EMPTY;
	}
}
/*************** END OF FUNCTIONS *********************************************/
