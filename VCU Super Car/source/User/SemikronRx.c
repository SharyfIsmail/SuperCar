/*
 * SemikronRx.c
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */

#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"
#include "newCanLib.h"
#include "SemikronRx.h"

void vSemicronRxHandler (void *pvParameters);
void vSemicronNmtCommand (void *pvParameters);
void vSemicronSyn(void *pvParameters);
void vSemicronNmtNodeGuarding(void *pvParameters);
TaskHandle_t xSemicronRxHandler;
TaskHandle_t xNMTCommand;

void semikronRxInit(void)
{
    if(xTaskCreate(vSemicronRxHandler, "SemicronRxHandler", configMINIMAL_STACK_SIZE, (void*)CAN_PERIOD_MS_SEMICRON_HANDLER, 1, &xSemicronRxHandler) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
    if(xTaskCreate(vSemicronNmtCommand, "NMT_Command", configMINIMAL_STACK_SIZE, NULL, 1, &xNMTCommand ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
    if(xTaskCreate(vSemicronSyn, "SemicronSync", configMINIMAL_STACK_SIZE, (void*)CAN_PERIOD_MS_SEMICRON_SYN, 1, NULL) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
    if(xTaskCreate(vSemicronNmtNodeGuarding, "NMT_NodeGuarding", configMINIMAL_STACK_SIZE, (void*)CAN_PERIOD_MS_NMT_NODE_GUARDING, 1, NULL) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
}

void vSemicronRxHandler (void *pvParameters)
{
    TickType_t lastWeakTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS( (uint32_t) pvParameters );

    RX_PDO_03LimitationMode_t limitationMode = SYMMETRIC;
    Rx_PDO_03ControlMode_t    controlMode = DISABLED;
    canMessage_t rxPdo_03 =
    {
     .id  = SEMICRON_SEMICRON_HANDLER,
     .dlc = SEMICRON_HANDLER_DLC,
     .ide = (uint8_t)CAN_Id_Standard,
    };
    setRx_PDO_03ControlMode(&rxPdo_03, (uint8_t)controlMode);
    setRx_PDO_03TorqueRefLim(&rxPdo_03, RX_PDO_03_TORQUE_REF_LIM(0));
    setRx_PDO_03SpeedRefLim(&rxPdo_03, RX_PDO_03_SPEED_REF_LIM(0));
    setRX_PDO_03LimitationMode(&rxPdo_03, (uint8_t)limitationMode);
    lastWeakTime = xTaskGetTickCount();

    while(1)
    {
        newCanTransmit(canREG1, canMESSAGE_BOX4, &rxPdo_03);
        vTaskDelayUntil( &lastWeakTime, transmitPeriod);
    }
}

void vSemicronNmtCommand (void *pvParameters)
{
    nmtCommandSpecifier_t nmtCommandSpecifier = RESET_COMMUNICATION;
    canMessage_t semicronNmtCommand =
    {
     .id = SEMICRON_RX_NMT_NMT_COMMAND,
     .dlc = SEMICRON_RX_NMT_NMT_COMMAND_DLC,
     .ide = (uint8_t)CAN_Id_Standard,
    };
    while(1)
    {
        setNmtCommandSpecifier(&semicronNmtCommand, (uint8_t)nmtCommandSpecifier);
        newCanTransmit(canREG1, canMESSAGE_BOX1, &semicronNmtCommand);
        taskYIELD();
    }
}

void vSemicronSyn(void *pvParameters)
{
    TickType_t lastWeakTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS( (uint32_t) pvParameters );

    canMessage_t semicronSync =
    {
     .id = SEMICRON_SYN,
     .dlc = SEMICRON_SYN_DLC,
     .ide = (uint8_t)CAN_Id_Standard,
    };
    lastWeakTime = xTaskGetTickCount();
    while(1)
    {
        newCanTransmit(canREG1, canMESSAGE_BOX2, &semicronSync);

        vTaskDelayUntil( &lastWeakTime, transmitPeriod );
    }
}

void vSemicronNmtNodeGuarding(void *pvParameters)
{
    TickType_t lastWeakTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS( (uint32_t) pvParameters );
    nmtNodeGuardingState_t  nmtNodeGuardingState = INITIALISATION;
    canMessage_t semicronNodeGuarding =
    {
         .id = SEMICRON_RX_NMT_NODE_GUARDING,
         .dlc = SEMICRON_RX_NMT_NODE_GUARDING_DLC,
         .ide =  (uint8_t)CAN_Id_Standard,
    };
    lastWeakTime = xTaskGetTickCount();
    for(;;)
    {
        setNmtNodeGuardingState(&semicronNodeGuarding,(uint8_t) nmtNodeGuardingState);
        ToggleNmtNodeGuardingToggleBit(&semicronNodeGuarding);
        newCanTransmit(canREG1, canMESSAGE_BOX3, &semicronNodeGuarding);

        vTaskDelayUntil( &lastWeakTime, transmitPeriod );
    }
}
