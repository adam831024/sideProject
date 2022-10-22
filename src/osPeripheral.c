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
#include "osPeripheral.h"
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
  vTaskDelay(50);
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
  vTaskDelay(50);
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
  taskENTER_CRITICAL(); /*to fast lcd str print*/
  // uint16_t len = BUILD_UINT16(param->dataLen[0], param->dataLen[1])+4; 
  // uart0Send((uint8_t*)param, len);
  switch(param->eventID)
  {
    case EVENT_BLE_DEV_LOCAL_ADDR:  /*0x12*/
    {
      eventDevLocalAddr_t *eventVal= (eventDevLocalAddr_t*)param->data;
      uart0Send(eventVal->addr, 6);
    }
    break;
    case EVENT_BLE_DEV_RECV_DATA:  /*0x80*/
    {
      uint8_t dataStr[2];
      eventDevRecvData_t *eventVal= (eventDevRecvData_t*)param->data;
      sprintf(dataStr, "%02x", eventVal->data[0]);
      osLcdPrint(0x4e, dataStr);
      // uart0Send(eventVal->data, len-5);
    }
    break;
    case EVENT_BLE_DEV_STATE:  /*0x81*/
    {
      eventDevState_t *eventVal= (eventDevState_t*)param->data;
      /*Dev:112233445566*/
      /*rssi:-11,data:aa*/
      switch(eventVal->state)
      {
        case INIT_FINISH:
        {
          eventVal->stateParam.stateInit;
        }
        break;
        case BLE_ADV_SCAN_ENABLE_DISABLE:
        {
          PA12 = 1;
          vTaskDelay(200);
          PA12 = 0;
          vTaskDelay(200);
          PA12 = 1;
          vTaskDelay(200);
          PA12 = 0;
          switch (eventVal->stateParam.stateAdvScan.param)
          {
            case BLE_ADV_DISABLE:
            {
              osLcdPrint(0x40, "  BLE Ready ^U^ ");
            }
            break;
            case BLE_ADV_ENABLE:
            {
              osLcdPrint(0x40, "   Advertising  ");
            }
            break;
            case BLE_SCAN_DISABLE:
            {
              osLcdPrint(0x40, "  BLE Ready ^U^ ");
            }
            break;
            case BLE_SCAN_ENABLE:
            {
              osLcdPrint(0x40, "  BLE Scanning   ");
            }
            break;
            default:
              break;
          }
        }
        break;
        case DEV_CONNECT: 
        {
          uint8_t addrStr[12];
          uint8_t i = 0;
          eventVal->stateParam.stateConn;
          uart0Send(eventVal->stateParam.stateConn.addr, 6);
          osLcdPrint(0x00, "Dev:");
          for(i =0 ; i<6;i++)
            sprintf(&addrStr[i*2], "%02x", eventVal->stateParam.stateConn.addr[i]);
          osLcdPrint(0x04, addrStr);
          osLcdPrint(0x40, "rssi:-  ,data:  ");
        }
        break;
        case DEV_DISCONNECT:
        {
          uint8_t disconnReasonStr[2];
          sprintf(disconnReasonStr, "%02x", eventVal->stateParam.stateDisconn.disconnReason);
          osLcdPrint(0x00, "Discon rsn: 0x");
          osLcdPrint(0x0e, disconnReasonStr);
          PA12 = 0;
          PA13 = 0;
          PA14 = 0;
        }
        break;
        default:
          break;
      }
    }
    break;
    case EVENT_BLE_DEV_CONN_RSSI:  /*0x89*/
    {
      eventDevConnRssi_t *eventVal= (eventDevConnRssi_t*)param->data;
      uint8_t rssi = eventVal->rssi;
      uint8_t rssiStr[2];
      sprintf(rssiStr, "%d", rssi);
      osLcdPrint(0x46, rssiStr);
      if(rssi < 55)
      {
        PA12 = 0;
        PA13 = 0;
        PA14 = 1;
      }
      else if(rssi>=55 && rssi<73) 
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
    }
    break;
    default:
      break;
  }
  taskEXIT_CRITICAL(); /*to fast lcd str print*/
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
