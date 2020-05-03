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
#include "crc8.h"

#define NUMBER_OF_HeartBEAT   2

typedef struct
{
    TickType_t maxTime[NUMBER_OF_HeartBEAT];
}maxTimeOutTime_t;
maxTimeOutTime_t maxTimeOutTime =
{
 .maxTime =
 {
     1000,  // accelerator
     1000,  // Joystick
 }
};

TaskHandle_t xAcceleratorBrakeJoystickTxHandler;
TaskHandle_t xSelectorRxHandler;

QueueHandle_t xQueueABPeadlSelectorTx = NULL;
QueueHandle_t xqueueAcceleratorValue = NULL;

static const VcuStatusStruct_t *currentVcuStatusStruct;
static SelectorStructModeTx_t  selectorMode = {
                                                .selectorMode = SELECTOR_MODE_INIT,
                                                .selectorInitialization = SELECTOR_INIT_INIT
                                              };
//static VcuStatusStruct_t currentVcuStatusStruct = { VCU_STATUS_INIT,
//                                                    VCU_NO_ERROR
 //                                                 };
static BpSwitch_t bPSwitch = BRAKE_CLOSE;
static SpeedTorque_t speedTorqueStruct = {
                                          .realAPposition = 0,
                                          .speedLimit = 0
                                         };
static uint8_t receivedAPposition = 0;


void vAcceleratorBrakeJoystickTxHandler(void *pvParameters);
void vSelectorRxHandler(void *pvParameters);

static void checkLostsOfComponents(TickType_t accelerator, TickType_t selector , TickType_t checkingTime);
static void parseDataToCanSelector(canMessage_t * selectorRx);
static void parseDataFromCanSelector(selectorTx_t *selectorTx);
static void parseDataFromCanPedal(ABPedalTx_t *aBPedalTx);
static void setRealAPPosition(uint8_t receivedAPposition);

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
    currentVcuStatusStruct = getVcuStatusStruct();
    xQueueABPeadlSelectorTx = xQueueCreate(20U, sizeof(ABPeadlSelector_t));
    xqueueAcceleratorValue =  xQueueCreate(1U, sizeof(SpeedTorque_t));
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
                parseDataFromCanPedal(aBPedalTx);
                setRealAPPosition(receivedAPposition);
                xQueueOverwrite(xqueueAcceleratorValue,&speedTorqueStruct);
            }
            else
            {
                selectorTimeOutControl =  xTaskGetTickCount() + maxTimeOutTime.maxTime[1];
                parseDataFromCanSelector(selectorTx);
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
       // if(xQueuePeek(xQueueVcuStatus, &currentVcuStatusStruct, pdMS_TO_TICKS(0)));

        parseDataToCanSelector(&selectorRx);
        newCanTransmit(canREG1, canMESSAGE_BOX8, &selectorRx);
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

static void parseDataToCanSelector(canMessage_t * selectorRx)
{
    increaseVcuSelectorMessageCounter(selectorRx, 1);
    setVcuSelectorCurrentMode(selectorRx,(uint8_t) currentVcuStatusStruct->vcuStateMangement);
    setVcuSelectorRequestedMode(selectorRx, (uint8_t)selectorMode.selectorMode);
    WriteToCanFrameCrc8(selectorRx->data, selectorRx->dlc);
}
static void parseDataFromCanSelector(selectorTx_t *selectorTx)
{
    selectorMode.selectorMode = (SelectorMode_t) getSelectorVcuModeRequest(selectorTx);
    selectorMode.selectorInitialization = (SelectorInitialization_t)getSelectorVcuInit(selectorTx);
}
static void parseDataFromCanPedal(ABPedalTx_t *aBPedalTx )
{
    receivedAPposition = getAPPosition(aBPedalTx);
    bPSwitch = (BpSwitch_t) getBPSwitch(aBPedalTx);
}
static void setRealAPPosition(uint8_t receivedAPposition)
{
    switch(currentVcuStatusStruct->errorStatus)
    {
    case VCU_NO_ERROR:
        if(currentVcuStatusStruct->vcuStateMangement == VCU_STATUS_FORWARD)
        {
           if(receivedAPposition <= 10)
           {
              // speedTorqueStruct.realAPposition = (int8_t)((receivedAPposition * 2) - 20);
              speedTorqueStruct.realAPposition = 0;


           }
           else if(receivedAPposition < 80)
           {
               speedTorqueStruct.realAPposition = (uint8_t)((uint16_t)(receivedAPposition - 10) * 85/100);
           }
           else
           {
               speedTorqueStruct.realAPposition = (receivedAPposition - 80) * 2 + 60;
           }
           speedTorqueStruct.speedLimit = 9000;
        }
        else if (currentVcuStatusStruct->vcuStateMangement == VCU_STATUS_REVERCE)
        {
            if(receivedAPposition <= 10)
            {
               // speedTorqueStruct.realAPposition = 20 - (receivedAPposition * 2);
                speedTorqueStruct.realAPposition = 0;
            }
            else
            {
                speedTorqueStruct.realAPposition = (int8_t)((int16_t)(10 - receivedAPposition) * 66/100);
            }
            speedTorqueStruct.speedLimit = -3000;
        }
        else
        {
            speedTorqueStruct.realAPposition = 0;
            speedTorqueStruct.speedLimit = 0;
        }
        break;

    case VCU_ERROR_WORK:
        if(currentVcuStatusStruct->vcuStateMangement == VCU_STATUS_FORWARD)
        {
            if(receivedAPposition <= 10)
            {
               // speedTorqueStruct.realAPposition = (int8_t)((receivedAPposition * 2) - 20);
                speedTorqueStruct.realAPposition = 0;
            }
            else if (receivedAPposition < 80)
            {
                speedTorqueStruct.realAPposition = (uint8_t)((uint16_t)(receivedAPposition - 10) * 85/100);
            }
            else
            {
                speedTorqueStruct.realAPposition = 60;
            }
            speedTorqueStruct.speedLimit = 9000;
        }
        else if(currentVcuStatusStruct->vcuStateMangement == VCU_STATUS_REVERCE)
        {
            if(receivedAPposition <= 10)
            {
              //  speedTorqueStruct.realAPposition = 20 - (receivedAPposition * 2);
                speedTorqueStruct.realAPposition = 0;
            }
            else
            {
                speedTorqueStruct.realAPposition = (int8_t)((int16_t)(10 - receivedAPposition) * 66/100);
            }
            speedTorqueStruct.speedLimit = 3000;
        }
        else
        {
            speedTorqueStruct.realAPposition = 0;
            speedTorqueStruct.speedLimit = 0;
        }
        break;

    case  VCU_ERROR_STOP :
        speedTorqueStruct.realAPposition = 0;
        speedTorqueStruct.speedLimit = 0;
        break;
    }
}
