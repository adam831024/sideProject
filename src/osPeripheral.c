/******************************************************************************
 * Copyright (C)
 *
 * NAME:
 *		template.c
 * DESCRIPTION:
 *
*******************************************************************************/
/******************************************************************************
 * Includes
 *******************************************************************************/
/*Standard include*/
#include <stdint.h>
#include "Nano100Series.h"              // Device header

/*Free-RTOS include*/
#include "FreeRTOS.h"
#include "timers.h"

/*Application include*/

/******************************************************************************
 * Module Preprocessor Constants
 *******************************************************************************/
 /*LCD*/
#define RS  PC15
#define RW  PC14
#define E   PB15

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
static void osRgbInit(void);
static void osLcdInit(void);
static void osLcdCmd(uint8_t cmd);
static void osLcdPut(uint8_t word);
/******************************************************************************
 * Function Definitions
 *******************************************************************************/
/******************************************************************************
 * @brief     Initialize RGB
 * @param[in] void                		
 * @return  void
 *******************************************************************************/
static void osRgbInit(void)
{
  PA12 = 0;
  PA13 = 0;
  PA14 = 0;
  GPIO_SetMode(PA, BIT12, GPIO_PMD_OUTPUT);
  GPIO_SetMode(PA, BIT13, GPIO_PMD_OUTPUT);
  GPIO_SetMode(PA, BIT14, GPIO_PMD_OUTPUT);
}
/******************************************************************************
 * @brief     LCD Command for initialize
 * @param[in] cmd                		
 * @return  void
 *******************************************************************************/
static void osLcdCmd(uint8_t cmd)
{
  RS=0; RW=0;
  PD->DOUT = cmd;
  vTaskDelay(10);
  E=1; 
  vTaskDelay(10);
  E=0;
  vTaskDelay(100);
}
/******************************************************************************
 * @brief     LCD print a char without confirm position
 * @param[in] word 
 *      a word                		
 * @return  void
 *******************************************************************************/
static void osLcdPut(uint8_t word)
{
  RS=1; RW=0;
  PD->DOUT = word;
  vTaskDelay(10);
  E=1; 
  vTaskDelay(10);
  E=0;
  vTaskDelay(100);
}
/******************************************************************************
 * @brief     LCD print an array
 * @param[in] tBuf      
 *    the string need to print     		
 * @return  void
 *******************************************************************************/
void osLcdPrint(uint8_t position, uint8_t *tBuf)
{
  osLcdCmd(0x80 | position);
  while(*tBuf){ osLcdPut(*tBuf++); }
}
/******************************************************************************
 * @brief     Initialize LCD
 * @param[in] void                		
 * @return  void
 *******************************************************************************/
static void osLcdInit(void)
{
  /*PB->PMD |= 0x40000000; PB15(E), Enable Signal*/
  GPIO_SetMode(PB, BIT15, GPIO_PMD_OUTPUT);
  /*PC->PMD |= 0x50000000; PC14(RW)+PC15(RS)*/
  GPIO_SetMode(PC, BIT14+ BIT15, GPIO_PMD_OUTPUT);
  /*PD->PMD |= 0x00005555; PD0~PD7*/
  GPIO_SetMode(PD, BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7, GPIO_PMD_OUTPUT);

  PD->DMASK |= 0x0000FF00;
  RS=0; RW=0; E=0;
  //--- example
  vTaskDelay(1000);
  osLcdCmd(0x38);     
  osLcdCmd(0x0C);     
  osLcdCmd(0x06);     
  osLcdCmd(0x80);  

  /*clean the LCD*/
  osLcdPrint(0x00, "       adam     ");
  osLcdPrint(0x40, "                ");
}
/******************************************************************************
 * @brief     Initialize RGB and LCD
 * @param[in] void                		
 * @return  void
 *******************************************************************************/
void osPeripheralInit(void)
{
  /*RGB init*/
  osRgbInit();

  /*LCD init*/
  // osLcdInit();
}
/*************** END OF FUNCTIONS *********************************************/