/******************************************************************************
 * Copyright (C)
 *
 * NAME:
 *		osUart.h
 * DESCRIPTION:
 *      header file of osUart.c
*******************************************************************************/
#ifndef _OS_UART_H_
#define _OS_UART_H_

/******************************************************************************
 * Includes
 *******************************************************************************/
/*Standard include*/
#include <stdint.h>

/*Application include*/
#include "fifo.h"

/******************************************************************************
 * Preprocessor Constants
 *******************************************************************************/
#define CMD_HEAD_LEN 5
#define MAX_CMD_SIZE 800


/******************************************************************************
 * Configuration Constants
 *******************************************************************************/

/******************************************************************************
 * Macros
 *******************************************************************************/

/******************************************************************************
 * Typedefs
 *******************************************************************************/
typedef enum
{
	PARSER_SUCCESS = 0x00,
	PARSER_ERROR,
	PARSER_WAITING,				/*still have some data in the buffer */
	PARSER_EMPTY,				/*there is no data in buffer*/
	PARSER_FULL,				/*Buffer full, lost some incoming data*/
	PARSER_CRASH,				/*important error*/
	PARSER_TIMEOUT_FLUSH 		/*5s timeout flush the waiting data in buffer*/
} parserState_t;

/******************************************************************************
 * Variables
 *******************************************************************************/

/******************************************************************************
 * Function Prototypes
 *******************************************************************************/
void init_UART0(uint32_t baudRate);
void uart0Send(uint8_t *pData, uint16_t dataLength);
void init_UART1(uint32_t baudRate);
void uart1Send(uint8_t *pData, uint16_t dataLength);
void osUartInit(void);
#endif /*_OS_UART_H_*/

    /*************** END OF FUNCTIONS *********************************************/
