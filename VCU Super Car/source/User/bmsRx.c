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
#include "bmsRx.h"
#include "string.h"

void vBmsRxHandler (void *pvParameters);
void vBmsHeartBeat (void *pvParameters);


TaskHandle_t xBmsRxHandler;
void BmsRxInit(void)
{
    if(xTaskCreate(vBmsRxHandler, "BmsRxHandler", configMINIMAL_STACK_SIZE, (void *)CAN_PERIOD_MS_BMS_CONTACTOR_REQUEST, 1, &xBmsRxHandler ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
    if(xTaskCreate(vBmsHeartBeat, "BmsHeartBeat", configMINIMAL_STACK_SIZE, (void *)CAN_PERIOD_MS_BMS_HEARTBEAT, 1, NULL) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
}

void vBmsRxHandler (void *pvParameters)
{
    TickType_t lastWakeTime ;
    TickType_t transmitPeriod = pdMS_TO_TICKS((uint32_t) pvParameters);
    BmsContactorRequest_t bmsContactorRequestState = CONTACTOR_OFF;
    PcuFault_t pcuFault = NO_PCU_FAULT;
    CriticalPcuFault_t criticalPcuFault = NO_CRITICAL_PCU_FAULT;
    canMessage_t bmsContactorRequest =
    {
     .id  = BMS_CONTACTOR_REQUEST,
     .dlc = BMS_CONTACTOR_REQUEST_DLC,
     .ide = (uint8_t)CAN_Id_Extended,
    };

    memset(bmsContactorRequest.data, 0, sizeof(bmsContactorRequest.data));

    lastWakeTime = xTaskGetTickCount();
    while(1)
    {
        setBmsContactorRequest(&bmsContactorRequest,(uint8_t) bmsContactorRequestState);
        setPcuCriticalFault(&bmsContactorRequest,(uint8_t) criticalPcuFault);
        setPcuFault(&bmsContactorRequest,(uint8_t) pcuFault);
        newCanTransmit(canREG1, canMESSAGE_BOX2, &bmsContactorRequest);

        vTaskDelayUntil(&lastWakeTime, transmitPeriod);
    }
}

void vBmsHeartBeat (void *pvParameters)
{
    TickType_t lastWakeTime ;
    TickType_t transmitPeriod = pdMS_TO_TICKS((uint32_t) pvParameters);
    canMessage_t bmsHearBeat =
    {
     .id  = BMS_HEARTBEAT,
     .dlc = BMS_HEARTBEAT_DLC,
     .ide = (uint8_t)CAN_Id_Extended,
    };

    memset(bmsHearBeat.data, 0, sizeof(bmsHearBeat.data));

    lastWakeTime = xTaskGetTickCount();
    while(1)
    {
        newCanTransmit(canREG1, canMESSAGE_BOX1, &bmsHearBeat);

        vTaskDelayUntil(&lastWakeTime, transmitPeriod);
    }
}

