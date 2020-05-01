/*
 * vcuAll.c
 *
 *  Created on: 1 мая 2020 г.
 *      Author: User
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "newCanLib.h"
#include "vcuStateManagement.h"
#include "bms.h"
#include "timeTask.h"
#include "vcuAll.h"
#include "SemikronTx.h"
#include "crc8.h"
#include "string.h"

static const VcuStatusStruct_t *currentVcuStatusStruct;
static const  bmsMode_State_t *batteryModeStruct;
static const int16_t *speed;
static int16_t motorSpeed = 0;

static uint32_t time = 0;
void vVcuAll01Handler (void *pvParameters);
void vVcuAll02Handler (void *pvParameters);
static void parseDataToCanVcuAll01(canMessage_t *VcuToAll01);
static void parseDataToCanVcuAll02(canMessage_t *VcuToAll02);

void VcuAllInit(void)
{
    if(xTaskCreate(vVcuAll01Handler, "VcuAll01Handler", configMINIMAL_STACK_SIZE, (void *)CAN_PERIOD_MS_VCU_ALL01, 1, NULL) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
    if(xTaskCreate(vVcuAll02Handler, "VcuAll02Handler", configMINIMAL_STACK_SIZE, (void *)CAN_PERIOD_MS_VCU_ALL02, 1, NULL) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */

    currentVcuStatusStruct = getVcuStatusStruct();
    batteryModeStruct = getBatteryModeStateStruct();
    speed = getSpeed();
}

void vVcuAll01Handler (void *pvParameters)
{
    TickType_t lastWakeTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS((uint32_t) pvParameters);
    canMessage_t vcuToAll01 =
    {
     .id  = VCU_ALL01_ID,
     .dlc = VCU_ALL01_DLC,
     .ide = (uint8_t)CAN_Id_Extended,
     .data = {0}
    };

    lastWakeTime = xTaskGetTickCount();
    for(;;)
    {
       // xQueuePeek(xQueueRealTime, &time, pdMS_TO_TICKS(0));
        motorSpeed = *speed;
        if (motorSpeed < 0)
        {
            motorSpeed = motorSpeed * (-1);
        }
        parseDataToCanVcuAll01(&vcuToAll01);
        newCanTransmit(canREG1, canMESSAGE_BOX11, &vcuToAll01);
        vTaskDelayUntil(&lastWakeTime, transmitPeriod);
    }
}

void vVcuAll02Handler (void *pvParameters)
{
    TickType_t lastWakeTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS((uint32_t) pvParameters);
    canMessage_t vcuToAll02 =
    {
     .id  = VCU_ALL02_ID,
     .dlc = VCU_ALL02_DLC,
     .ide = (uint8_t)CAN_Id_Extended,
     .data = {0}
    };
    lastWakeTime = xTaskGetTickCount();
    for(;;)
    {
        parseDataToCanVcuAll02(&vcuToAll02);
        newCanTransmit(canREG1, canMESSAGE_BOX12, &vcuToAll02);
        vTaskDelayUntil(&lastWakeTime, transmitPeriod);
    }
}
static void parseDataToCanVcuAll01(canMessage_t *VcuToAll01)
{
    increaseVcuAll01MessageCounter(VcuToAll01, 0x01);
    setVcuAll01tMode(VcuToAll01,(uint8_t) currentVcuStatusStruct->vcuStateMangement);
    setVcuAll01Time(VcuToAll01, time);
    setVcuAll01Error(VcuToAll01,(uint8_t) currentVcuStatusStruct->errorStatus);
    setVcuAll01ContactorMode(VcuToAll01,(uint8_t) batteryModeStruct->contactorStateFromBMS);
    setVcuAll01Speed(VcuToAll01, (uint8_t)VCU_ALL_01_SPEED(motorSpeed));
    WriteToCanFrameCrc8(VcuToAll01->data, VcuToAll01->dlc);
}
static void parseDataToCanVcuAll02(canMessage_t *VcuToAll02)
{
    increaseVcuAll02MessageCounter(VcuToAll02, 0x01);
    setVcuAll02Volt(VcuToAll02, batteryModeStruct->voltFromBMS);
    setVcuAll02Temp01(VcuToAll02, 0xFF);
    setVcuAll02Temp02(VcuToAll02, 0xFF);
    WriteToCanFrameCrc8(VcuToAll02->data, VcuToAll02->dlc);
}
