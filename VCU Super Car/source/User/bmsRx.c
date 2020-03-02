/*
 * BatteryManagementSystemRx.c
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */
#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"
#include "newCanLib.h"

void vBmsRxHandler (void *pvParameters);
void vBmsHeartBeat (void *pvParameters);


TaskHandle_t xBmsRxHandler;
TaskHandle_t xvBmsHeartBeat;

void BmsRxInit(void)
{
    if(xTaskCreate(vBmsRxHandler, "BmsRxHandler", configMINIMAL_STACK_SIZE, NULL, 1, &xBmsRxHandler ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
    if(xTaskCreate(vBmsHeartBeat, "BmsHeartBeat", configMINIMAL_STACK_SIZE, NULL, 1, &xvBmsHeartBeat ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }


}

void vBmsRxHandler (void *pvParameters)
{
    while(1)
    {
        taskYIELD();
    }
}

void vBmsHeartBeat (void *pvParameters)
{
    TickType_t lastWakeTime ;
    canMessage_t canHearBeat =
    {
     .id = 0x01,
     .dlc = 8,
     .ide = (uint8_t)CAN_Id_Extended,
    };
    lastWakeTime = xTaskGetTickCount();
    while(1)
    {
        newCanTransmit(canREG2, canMESSAGE_BOX2, 0xff5f5, canHearBeat.data, canHearBeat.dlc);
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(100U));
    }
}


