/******************************************************************************
 * Copyright (C)
 *
 * NAME:
 *		osUtility.c
 * DESCRIPTION:
 *
*******************************************************************************/
/******************************************************************************
 * Includes
 *******************************************************************************/
/*Standard include*/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "Nano100Series.h" 

/*Free-RTOS include*/

/*Application include*/
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
#if 0
static uint8_t fac_us = 0;
static uint16_t fac_ms = 0;
#endif
/******************************************************************************
 * Function Prototypes
 *******************************************************************************/

/******************************************************************************
 * Function Definitions
 *******************************************************************************/
#if 0
/******************************************************************************
 * @brief     syste tick delay init
 * @return                              void
 *******************************************************************************/
void delayInit(void)
{
    /*systick HCLK，interrupt interval 1ms*/
	/*SystemCoreClock == 12MHz
    run 12 ticks == 1us, so 1 us delay needs 3 times while loop */ 
    fac_us = SystemCoreClock / 4000000; // TODO: not sure
    fac_ms = (uint16_t)fac_us * 1000;
    // printf("fac_us = %d, fac_ms = %d, SystemCoreClock  =%d\r\n",fac_us, fac_ms, SystemCoreClock);
}

/******************************************************************************
 * @brief     system delay us
 * @param[in] us                		microseconds
 * @return                              void
 *******************************************************************************/
void delayUs(uint16_t us)
{
    uint32_t temp;
    SysTick->LOAD = (uint32_t)us * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL = 0x01;
    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && (!(temp & (1 << 16))));
    SysTick->CTRL = 0x00;
    SysTick->VAL = 0x00;
}

/******************************************************************************
 * @brief     system delay ms
 * @param[in] ms                		milliseconds
 * @return                              void
 *******************************************************************************/
void delayMs(uint16_t ms)
{
    uint32_t temp;
    SysTick->LOAD = (uint32_t)ms * fac_ms;
    SysTick->VAL = 0x00;
    SysTick->CTRL = 0x01;
    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && (!(temp & (1 << 16))));
    SysTick->CTRL = 0x00;
    SysTick->VAL = 0x00;
}
#endif
/******************************************************************************
 * DESCRIPTION: malloc
 * @param[in] size
 *      malloc size
 * RETURNS: pointer 
*******************************************************************************/
void *osMalloc(uint16_t size)
{
    void *memory = (void *)pvPortMalloc(size);
    memset(memory, 0, size);
    return memory;
}
/******************************************************************************
 * DESCRIPTION: 
 *      Free malloc
 * @param[in] ptr
 *      The pointer needed to be free
 * RETURNS: void 
*******************************************************************************/
void osFree(void *ptr)
{
    if (ptr)
    {
        vPortFree(ptr);
    }
}
/*************** END OF FUNCTIONS *********************************************/
