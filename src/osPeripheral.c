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
#include "main.h"
#include "osUart.h"
#include "osUtility.h"
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
static void osPeripheralCallback(osMsg_t* param);
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
  osLcdPrint(0x00, "   Adam Gooooo  "); /*16 words for a line*/
  osLcdPrint(0x40, " Initialize OK  ");
}
/******************************************************************************
 * @brief     peripheral callback function
 * @param[out] param                		
 * @return  void
 *******************************************************************************/
static void osPeripheralCallback(osMsg_t* param)
{
  // uint16_t len = BUILD_UINT16(param->dataLen[0], param->dataLen[1])+4; 
  // uart0Send((uint8_t*)param, len);
  switch(param->eventID)
  {
    case EVENT_BLE_DEV_ADDR:  /*0x12*/
      break;
    case EVENT_BLE_DEV_FW_VER:  /*0x15*/
      break;
    case EVENT_BLE_DEV_ENABLE_ADV:  /*0x1b*/
      break;
    case EVENT_BLE_DEV_RECV_DATA:  /*0x80*/
      break;
    case EVENT_BLE_DEV_STATE:  /*0x81*/
    {
      /*Dev:112233445566*/
      /*rssi:-11,data:aa*/
      switch(param->data[0])
      {
        case 0x02:  /*connected*/
        {
          PA12 = 1;
          vTaskDelay(200);
          PA12 = 0;
          vTaskDelay(200);
          PA12 = 1;
          vTaskDelay(200);
          PA12 = 0;
        }
        break;
        case 0x03:  /*disconnect*/
        {
          PA12 = 1;
          vTaskDelay(200);
          PA12 = 0;
          vTaskDelay(200);
          PA12 = 1;
          vTaskDelay(200);
          PA12 = 0;
        }
        break;
        default:
          break;
      }
    }
    break;
    case EVENT_BLE_DEV_CONN_RSSI:  /*0x89*/
    {
      uint8_t rssi = 0;
      uint8_t rssiStr[2];
      rssi = param->data[0];
      osLcdPrint(0x40, "rssi:-11,data:aa");
      if(param->data[0]<0x30)
      {
        PA12 = 0;
        PA13 = 0;
        PA14 = 1;
      }
      else if(param->data[0]>=0x40 && param->data[0]<0x50) 
      {
        PA12 = 0;
        PA13 = 1;
        PA14 = 0;
      }
      else
      {
        PA12 = 1;
        PA13 = 0;
        PA14 = 0;
      }
      sprintf(rssiStr, "%d", rssi);
      osLcdPrint(0x46, rssiStr);
    }
    break;
    default:
      break;
  }
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
  osLcdInit();

  /*register callback function*/
  osPeripheralRegisterCb(osPeripheralCallback);
}
/*************** END OF FUNCTIONS *********************************************/
