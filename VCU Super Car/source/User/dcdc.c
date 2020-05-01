/*
 * dcdc.c
 *
 *  Created on: 1 мая 2020 г.
 *      Author: User
 */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "newCanLib.h"
#include "vcuStateManagement.h"
#include "sys_main.h"
#include "dcdc.h"
#include "crc8.h"

typedef enum
{
    DCDC_OFF,
    DCDC_BUCK,
    DCDC_BOOST,
    DCDC_UDCLNKDISCHARGE
}dcdcMode_t;
static dcdcMode_t dcdcMode = DCDC_OFF;
QueueHandle_t xQueueDcdc = NULL;

static const VcuStatusStruct_t *currentVcuStatusStruct;
static void parseDataToCanVcuDcdc(canMessage_t *dcDC);

void vDcDcTxHandler (void *pvParameters);
void vDcDcRxHandler (void *pvParameters);

void dcDcInit(void)
{
    if(xTaskCreate(vDcDcTxHandler, "DcDcTxHandler", configMINIMAL_STACK_SIZE, NULL, 1, NULL ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
    if(xTaskCreate(vDcDcRxHandler, "DcDcRxHandler", configMINIMAL_STACK_SIZE, (void *)CAN_PERIOD_MS_VCU_DCDC, 1, NULL ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
    xQueueDcdc =  xQueueCreate(5U, sizeof(canMessage_t));
    currentVcuStatusStruct = getVcuStatusStruct();

}
void vDcDcTxHandler (void *pvParameters)
{
    canMessage_t Dcdc;
    for(;;)
    {
        if(xQueueReceive(xQueueDcdc, &Dcdc, pdMS_TO_TICKS(2000)))
        {
            xEventGroupClearBits(canMessageLostCheckEventGroup, MASK(5U));
        }
        else
        {
            xEventGroupSetBits(canMessageLostCheckEventGroup, MASK(5U));
        }
    }
}
void vDcDcRxHandler (void *pvParameters)
{
    TickType_t lastWakeTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS((uint32_t) pvParameters);
    canMessage_t vcuDcDc =
    {
     .id  = VCU_DCDC_ID,
     .dlc = VCU_DCDC_DLC,
     .ide = (uint8_t)CAN_Id_Extended,
     .data = {0}
    };
    lastWakeTime = xTaskGetTickCount();
    for(;;)
    {
        if(currentVcuStatusStruct->vcuStateMangement ==  VCU_STATUS_PARKING)
        {
            dcdcMode = DCDC_BUCK;
        }
        else if (currentVcuStatusStruct->vcuStateMangement ==  VCU_STATUS_SLEEP)
        {
            dcdcMode = DCDC_OFF;
        }
        parseDataToCanVcuDcdc(&vcuDcDc);
        newCanTransmit(canREG1, canMESSAGE_BOX10, &vcuDcDc);
        vTaskDelayUntil(&lastWakeTime, transmitPeriod);
    }
}
static void parseDataToCanVcuDcdc(canMessage_t *dcDC)
{
    increaseVcuDcdcMessageCounter(dcDC, 0x01);
    setDcdcMode(dcDC, (uint8_t)dcdcMode);
    setDcdcVoltHigh(dcDC, 360);
    setDcdcCurrentHigh(dcDC, DCDC_CURRENT_HIGH(2));
    setDcdcCurrentLow(dcDC, DCDC_CURRENT_LOW(50));
    setDcdcVoltLOW(dcDC, DCDC_VOLT_LOW(14));
    WriteToCanFrameCrc8(dcDC->data, dcDC->dlc);
}
