/******************************************************************************
 * Copyright (C)
 *
 * NAME:
 *    osPeripheral.h
 * DESCRIPTION:
 *      header file of osPeripheral.c
*******************************************************************************/
#ifndef _OS_PERIPHERAL_
#define _OS_PERIPHERAL_

/******************************************************************************
 * Includes
 *******************************************************************************/
/*Standard include*/
#include <stdint.h>

/*Application include*/

/******************************************************************************
 * Preprocessor Constants
 *******************************************************************************/
/**
 * This constant is
 */

/******************************************************************************
 * Configuration Constants
 *******************************************************************************/

/******************************************************************************
 * Macros
 *******************************************************************************/

/******************************************************************************
 * Typedefs
 *******************************************************************************/
enum    /*eventID_t*/
{
    EVENT_BLE_DEV_LOCAL_ADDR =  0x12,   /*eventDevLocalAddr_t*/ 
    EVENT_BLE_DEV_RECV_DATA =   0x80,  /*eventDevRecvData_t*/
    EVENT_BLE_DEV_STATE =       0x81,  /*eventDevState_t*/
    EVENT_BLE_DEV_CONN_RSSI =   0x89,  /*eventDevConnRssi_t*/
};

typedef struct 
{
    uint8_t addr[6];
}eventDevLocalAddr_t;


typedef struct 
{
    uint8_t connHandle;
    uint8_t data[1];
}eventDevRecvData_t;

typedef struct
{
    uint8_t param;
}stateInit_t;

typedef enum
{
    BLE_ADV_DISABLE = 0, 
    BLE_ADV_ENABLE,
    BLE_SCAN_DISABLE, 
    BLE_SCAN_ENABLE,
}advScan_t;

typedef struct
{
    advScan_t param;
}stateAdvScan_t;

typedef struct
{
    uint8_t addr[6];
    uint8_t connHandle;
}stateConn_t;

typedef struct
{
    uint8_t connHandle;
    uint8_t disconnReason;
}stateDisconn_t;

typedef enum
{
    INIT_FINISH = 0, 
    BLE_ADV_SCAN_ENABLE_DISABLE,
    DEV_CONNECT, 
    DEV_DISCONNECT,
}devState_t;

typedef struct 
{
    devState_t state;
    union
    {
        stateInit_t stateInit;
        stateAdvScan_t stateAdvScan;
        stateConn_t stateConn;
        stateDisconn_t stateDisconn;
    }stateParam;
    
}eventDevState_t;

typedef struct 
{
    uint8_t addr[6];
    uint8_t rssi;
}eventDevConnRssi_t;

/******************************************************************************
 * Variables
 *******************************************************************************/

/******************************************************************************
 * Function Prototypes
 *******************************************************************************/
void osPeripheralInit(void);
void osLcdPrint(uint8_t position, uint8_t *tBuf);
#endif /*_OS_PERIPHERAL_*/

/*************** END OF FUNCTIONS *********************************************/
