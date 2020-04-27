/*
 * BatteryManagementSystemRx.c
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "newCanLib.h"
#include "bmsRx.h"
#include "vcuStateManagement.h"
#include "crc8.h"
#include "string.h"

void vBmsRxHandler (void *pvParameters);

TaskHandle_t xBmsRxHandler;
void BmsRxInit(void)
{
    if(xTaskCreate(vBmsRxHandler, "BmsRxHandler", configMINIMAL_STACK_SIZE, (void *)CAN_PERIOD_MS_BMS_CONTACTOR_REQUEST, 1, &xBmsRxHandler ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
}

void vBmsRxHandler (void *pvParameters)
{
    TickType_t lastWakeTime ;
    TickType_t transmitPeriod = pdMS_TO_TICKS((uint32_t) pvParameters);
    canMessage_t bmsContactorRequest =
    {
     .id  = BMS_CONTACTOR_REQUEST,
     .dlc = BMS_CONTACTOR_REQUEST_DLC,
     .ide = (uint8_t)CAN_Id_Extended,
     .data = {0}
    };
    VcuStatusStruct_t currentVcuStatusStruct = { VCU_Status_Init,
                                                 VCU_ERROR_UNDEFINED
                                               };
    VcuRawStatusBattery_t vcuRawStatusBattery = BATTERY_INIT;
    lastWakeTime = xTaskGetTickCount();
    while(1)
    {
        xQueuePeek(xQueueVcuStatus, &currentVcuStatusStruct, 0);

        if(currentVcuStatusStruct.vcuStateMangement == VCU_Status_Init &&
            currentVcuStatusStruct.errorStatus == VCU_NO_ERROR)
        {
            vcuRawStatusBattery = BATTERY_HV_ACTIVE;
        }
        else if (currentVcuStatusStruct.vcuStateMangement ==  VCU_Status_CHARGING)
        {
            vcuRawStatusBattery = BATTERY_CHARGING;
        }
        else if(currentVcuStatusStruct.vcuStateMangement == VCU_Status_SLEEP)
        {
            vcuRawStatusBattery = BATTERY_NORMAL_OFF;
        }
        setVcuRequestModeBms(&bmsContactorRequest, (uint8_t)vcuRawStatusBattery);
        increaseVcuBmsMessageCounter(&bmsContactorRequest, 1);
        WriteToCanFrameCrc8(bmsContactorRequest.data, bmsContactorRequest.dlc);
        newCanTransmit(canREG1, canMESSAGE_BOX2, &bmsContactorRequest);

        vTaskDelayUntil(&lastWakeTime, transmitPeriod);
    }
}
