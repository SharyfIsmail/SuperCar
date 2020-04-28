/*
 * acceleratorBrakeJoystick.c
 *
 *  Created on: 11 мар. 2020 г.
 *      Author: User
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "sys_main.h"
#include "newCanLib.h"
#include "vcuStateManagement.h"
#include "acceleratorBrakeJoystick.h"

#define NUMBER_OF_HeartBEAT   2

typedef struct
{
    TickType_t maxTime[NUMBER_OF_HeartBEAT];
}maxTimeOutTime_t;
maxTimeOutTime_t maxTimeOutTime =
{
 .maxTime =
 {
     500,  // accelerator
     500,  // Joystick
 }
};

TaskHandle_t xAcceleratorBrakeJoystickTxHandler;
TaskHandle_t xSelectorRxHandler;

QueueHandle_t xQueueABPeadlSelectorTx = NULL;
QueueHandle_t xqueueAcceleratorValue = NULL;

static SelectorStructModeTx_t  selectorMode = {.selectorMode = SELECTOR_MODE_INIT,
                                               .selectorInitialization = SELECTOR_INIT_INIT
};

void vAcceleratorBrakeJoystickTxHandler(void *pvParameters);
void vSelectorRxHandler(void *pvParameters);

static void checkLostsOfComponents(TickType_t accelerator, TickType_t selector , TickType_t checkingTime);

void acceleratorBrakeJoystickInit(void)
{
    if(xTaskCreate(vAcceleratorBrakeJoystickTxHandler, "AcceleratorBrakeJoystickTxHandler", configMINIMAL_STACK_SIZE, NULL, 1, &xAcceleratorBrakeJoystickTxHandler) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */

    if(xTaskCreate(vSelectorRxHandler, "SelectorRxHandler", configMINIMAL_STACK_SIZE, (void*)CAN_PERIOD_MS_SELECTOR_RX, 1, &xSelectorRxHandler) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */

    xQueueABPeadlSelectorTx = xQueueCreate(20U, sizeof(ABPeadlSelector_t));
    xqueueAcceleratorValue =  xQueueCreate(1U, sizeof(int));
   // xqueueBrakeValue = xQueueCreate(1U, sizeof(int));
}


void vAcceleratorBrakeJoystickTxHandler(void *pvParameters)
{
    TickType_t acceleratorTimeOutControl = xTaskGetTickCount() + maxTimeOutTime.maxTime[0];
    TickType_t selectorTimeOutControl =  xTaskGetTickCount() + maxTimeOutTime.maxTime[1];
    TickType_t checkingTime = 0U;

    ABPeadlSelector_t aBPeadlSelector;
    selectorTx_t *selectorTx = &aBPeadlSelector.p.selectorTx;
    ABPedalTx_t *aBPedalTx = &aBPeadlSelector.p.ABPedalTx;

    for(;;)
    {
        if(xQueueReceive(xQueueABPeadlSelectorTx, &aBPeadlSelector, pdMS_TO_TICKS(250)))
        {
            if(aBPeadlSelector.id == AB_PEDAL_TX)
            {
                acceleratorTimeOutControl = xTaskGetTickCount() + maxTimeOutTime.maxTime[0];
            }
            else
            {
                selectorTimeOutControl =  xTaskGetTickCount() + maxTimeOutTime.maxTime[1];
                selectorMode.selectorMode = (SelectorMode_t) getSelectorVcuModeRequest(selectorTx);
                selectorMode.selectorInitialization = (SelectorInitialization_t)getSelectorVcuInit(selectorTx);
                xQueueSend(xQueueSelectorMode, &selectorMode, pdMS_TO_TICKS(0));
            }
        }/* else not needed */

        /** Update CAN messages timeout value */
        checkingTime = xTaskGetTickCount();
        checkLostsOfComponents(acceleratorTimeOutControl, selectorTimeOutControl, checkingTime);

        taskYIELD();
    }
}

void vSelectorRxHandler(void *pvParameters)
{
    VcuStatusStruct_t currentVcuStatusStruct = { VCU_STATUS_INIT,
                                                 VCU_NO_ERROR
                                               };
    TickType_t lastWeakTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS( (uint32_t) pvParameters );
    lastWeakTime = xTaskGetTickCount();
    canMessage_t selectorRx =
    {
     .id  = SELECTOR_RX,
     .dlc = SLECETOR_RX_DLC,
     .ide = (uint8_t)CAN_Id_Extended,
     .data = {0}
    };
    setVcuSelectorIlluminationMode(&selectorRx, 0x01);
    setVcuSelectorLeverLocking(&selectorRx, 0x01);
    for(;;)
    {
        if(xQueuePeek(xQueueVcuStatus, &currentVcuStatusStruct, pdMS_TO_TICKS(0)));

        setVcuSelectorCurrentMode(&selectorRx,(uint8_t) currentVcuStatusStruct.vcuStateMangement);
        setVcuSelectorRequestedMode(&selectorRx, (uint8_t)selectorMode.selectorMode);

        vTaskDelayUntil( &lastWeakTime, transmitPeriod);
    }
}
static void checkLostsOfComponents(TickType_t accelerator, TickType_t selector , TickType_t checkingTime)
{
    if(accelerator < checkingTime)
        xEventGroupSetBits(canMessageLostCheckEventGroup, MASK(2U));
    else
        xEventGroupClearBits(canMessageLostCheckEventGroup, MASK(2U));

    if(selector < checkingTime)
        xEventGroupSetBits(canMessageLostCheckEventGroup, MASK(4U));
     else
        xEventGroupClearBits(canMessageLostCheckEventGroup, MASK(4U));

}
