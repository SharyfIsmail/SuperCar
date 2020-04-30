/*
 * BatteryManagementSystemRx.c
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */
#include <bms.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "newCanLib.h"
#include "vcuStateManagement.h"
#include "sys_main.h"
#include "crc8.h"
#include "string.h"

static const VcuStatusStruct_t *currentVcuStatusStruct;
static batteryMode_t batteryMode = BATTERY_INIT;
static bmsMode_State_t batteryModeState;
static uint8_t contactorState = 0;
TaskHandle_t xBmsRxHandler;
TaskHandle_t xBmsTxHandler;
QueueHandle_t xQueueBmsTx = NULL;

void vBmsTxHandler (void *pvParameters);
void vBmsRxHandler (void *pvParameters);
static void parseDataToCanVcuBms(canMessage_t *vcuToBms);
static void parseDataFromCanBms_vcu_01(BmsVcu_01_t *BmsVcu_01);

void BmsInit(void)
{
    if(xTaskCreate(vBmsRxHandler, "BmsRxHandler", configMINIMAL_STACK_SIZE, (void *)CAN_PERIOD_MS_VCU_BMS, 1, &xBmsRxHandler ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
    if(xTaskCreate(vBmsTxHandler, "BmsTxHandler", configMINIMAL_STACK_SIZE, NULL, 1, &xBmsTxHandler ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */

    xQueueBmsTx =  xQueueCreate(20U, sizeof(BmsTxCanFrame_t));
    currentVcuStatusStruct = getVcuStatusStruct();
}

void vBmsRxHandler (void *pvParameters)
{
    TickType_t lastWakeTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS((uint32_t) pvParameters);
    canMessage_t vcuToBms =
    {
     .id  = VCU_BMS_ID,
     .dlc = BMS_VCU_DLC,
     .ide = (uint8_t)CAN_Id_Extended,
     .data = {0}
    };
    lastWakeTime = xTaskGetTickCount();
    while(1)
    {
        switch(currentVcuStatusStruct->errorStatus)
        {
        case VCU_ERROR_STOP :
            batteryMode = BATTERY_EMEGENCY_OFF;
         break;
        default :
            if(currentVcuStatusStruct->vcuStateMangement == VCU_STATUS_INIT)
            {
                batteryMode = BATTERY_HV_ACTIVE;
            }
            else if (currentVcuStatusStruct->vcuStateMangement ==  VCU_STATUS_CHARGING)
            {
                batteryMode = BATTERY_CHARGING;
            }
            else if(currentVcuStatusStruct->vcuStateMangement == VCU_STATUS_SLEEP)
            {
                batteryMode = BATTERY_NORMAL_OFF;
            }
            break;
        }
        parseDataToCanVcuBms(&vcuToBms);
        newCanTransmit(canREG1, canMESSAGE_BOX9, &vcuToBms);

        vTaskDelayUntil(&lastWakeTime, transmitPeriod);
    }
}

void vBmsTxHandler (void *pvParameters)
{
    BmsTxCanFrame_t BmsTxCanFrame;
    BmsVcu_01_t *BmsVcu_01 = &BmsTxCanFrame.p.BmsVcu_01;
    BmsVcu_02_t *BmsVcu_02 = &BmsTxCanFrame.p.BmsVcu_02;
    BmsVcu_03_t *BmsVcu_03 = &BmsTxCanFrame.p.BmsVcu_03;
    BmsVcu_04_t *BmsVcu_04 = &BmsTxCanFrame.p.BmsVcu_04;
    BmsVcu_05_t *BmsVcu_05 = &BmsTxCanFrame.p.BmsVcu_05;
    while(1)
    {
        if(xQueueReceive(xQueueBmsTx, &BmsTxCanFrame, pdMS_TO_TICKS(5000)))
        {
            xEventGroupClearBits(canMessageLostCheckEventGroup, MASK(1U));
            if(BmsTxCanFrame.id < BMS_VCU_02_ID)
            {
                if(BmsTxCanFrame.id == BMS_VCU_01_ID)    // BMS_VCU_01_ID
                {
                    parseDataFromCanBms_vcu_01(BmsVcu_01);
                    xQueueSend(xQueueBatteryMode, &batteryModeState, pdMS_TO_TICKS(0));
                }
                else                                     // BMS_VCU_05_ID
                {

                }
            }
            else if(BmsTxCanFrame.id > BMS_VCU_02_ID)
            {
                if(BmsTxCanFrame.id == BMS_VCU_03_ID)     // BMS_VCU_03_ID
                {

                }
                else                                      // BMS_VCU_04_ID
                {

                }
            }
            else                                         // BMS_VCU_02_ID
            {

            }
        }
        else
        {
            xEventGroupSetBits(canMessageLostCheckEventGroup, MASK(1U));
        }
        taskYIELD();
    }
}


static void parseDataToCanVcuBms(canMessage_t *vcuToBms)
{
    setVcuRequestModeBms(vcuToBms, (uint8_t)batteryMode);
    setVcuIsolationMeasSwitchOff(vcuToBms, 0x00);
    increaseVcuBmsMessageCounter(vcuToBms, 0x01);
    WriteToCanFrameCrc8(vcuToBms->data, vcuToBms->dlc);
}
static void parseDataFromCanBms_vcu_01(BmsVcu_01_t *BmsVcu_01)
{
    batteryModeState.batteryMode = (batteryMode_t) getBmsVcu01BatteryMode(BmsVcu_01);
    batteryModeState.batteryState = (bool) getBmsVcu01IErrorPresents(BmsVcu_01);
    contactorState = getBmsVcu01ContactorMode(BmsVcu_01);
}
