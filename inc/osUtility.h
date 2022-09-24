/******************************************************************************
 * Copyright (C)
 *
 * NAME:
 *		osUtility.h
 * DESCRIPTION:
 *      header file of osUtility.c
*******************************************************************************/
#ifndef _OS_UTILITY_H_
#define _OS_UTILITY_H_

/******************************************************************************
 * Includes
 *******************************************************************************/
/*Standard include*/
#include <stdint.h>
#include <stdbool.h>

/*Application include*/

/******************************************************************************
 * Preprocessor Constants
 *******************************************************************************/
/**
 * This constant is
 */
#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)
#define BUILD_UINT16(hiByte, loByte) \
          ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))
/******************************************************************************
 * Configuration Constants
 *******************************************************************************/

/******************************************************************************
 * Macros
 *******************************************************************************/

/******************************************************************************
 * Typedefs
 *******************************************************************************/

/******************************************************************************
 * Variables
 *******************************************************************************/

/******************************************************************************
 * Function Prototypes
 *******************************************************************************/
 #if 0
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);
void delayInit(void);
bool osMessageSend(taskType_t src, taskType_t dest, void *pData);
#endif
void *osMalloc(uint16_t size);
void osFree(void *ptr);
#endif /*_OS_UTILITY_H_*/

/*************** END OF FUNCTIONS *********************************************/
