/******************************************************************************
 * Copyright (C)
 *
 * NAME:
 *		main.h
 * DESCRIPTION:
 *      header file of main.c
*******************************************************************************/
#ifndef _MAIN_H_
#define _MAIN_H_

/******************************************************************************
 * Includes
 *******************************************************************************/
/*Standard include*/
#include <stdint.h>

/*Free-RTOS include*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

/*Application include*/

/******************************************************************************
 * Preprocessor Constants
 *******************************************************************************/

/******************************************************************************
 * Configuration Constants
 *******************************************************************************/

/******************************************************************************
 * Macros
 *******************************************************************************/
enum    /*TASK ID, for Task arg*/
{
    MAIN_TASK_ID =      0x11,
    UART_TASK_ID =      0x22
};

enum    /*handlerID_t, for main Task handler*/
{
    OS_PERIPHERAL_HANDLER_ID =     0x01, 
    OS_OTHERS_HANDLER_ID =         0x02,   /*for example*/ 
};

typedef struct 
{
    uint8_t taskID;    /*handlerID_t*/
    uint8_t eventID;    /*eventID_t*/
    uint8_t dataLen[2];
    uint8_t data[1];
}osMsg_t;

/******************************************************************************
 * Typedefs
 *******************************************************************************/
typedef void (*osPeripheralCallback_t)(osMsg_t* param);
typedef void (*osOthersCallback_t)(osMsg_t* param);

/******************************************************************************
 * Variables
 *******************************************************************************/

/******************************************************************************
 * Function Prototypes
 *******************************************************************************/
void osMessageSend(void *pdata);
void osPeripheralRegisterCb(osPeripheralCallback_t cbFunction);
void osOthersRegisterCb(osOthersCallback_t cbFunction);
#endif /*_MAIN_H_*/

/*************** END OF FUNCTIONS *********************************************/
