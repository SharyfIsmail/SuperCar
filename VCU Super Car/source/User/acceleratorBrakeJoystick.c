/*
 * acceleratorBrakeJoystick.c
 *
 *  Created on: 11 ���. 2020 �.
 *      Author: User
 */

#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"
#include "sys_main.h"
#include "newCanLib.h"
#include "acceleratorBrakeJoystick.h"

#define NUMBER_OF_HeartBEAT   3

typedef struct
{
    TickType_t maxTime[NUMBER_OF_HeartBEAT];
}maxTimeOutTime_t;

TaskHandle_t xAcceleratorBrakeJoystickTxHandler;

QueueHandle_t xQueueAcceleratorBrakeJoystickTx = NULL;

QueueHandle_t xqueueJoystickMode = NULL;
QueueHandle_t xqueueAcceleratorValue = NULL;
QueueHandle_t xqueueBrakeValue = NULL;

maxTimeOutTime_t maxTimeOutTime =
{
 .maxTime =
 {
     1000,  // accelerator
     1000,  // Brake
     1000,  // Joystick
 }
};
void vAcceleratorBrakeJoystickTxHandler(void *pvParameters);

void acceleratorBrakeJoystickInit(void)
{

    if(xTaskCreate(vAcceleratorBrakeJoystickTxHandler, "AcceleratorBrakeJoystickTxHandler", configMINIMAL_STACK_SIZE, NULL, 1, &xAcceleratorBrakeJoystickTxHandler) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
    xQueueAcceleratorBrakeJoystickTx = xQueueCreate(20U, sizeof(acceleratorBrakeJoystick_t));
    xqueueJoystickMode = xQueueCreate(1U, sizeof(uint8_t));
    xqueueAcceleratorValue =  xQueueCreate(1U, sizeof(int));
    xqueueBrakeValue = xQueueCreate(1U, sizeof(int));
}

static void checkLostsOfComponents(TickType_t accelerator, TickType_t brake, TickType_t joystick , TickType_t checkingTime)
{
    if(accelerator <= checkingTime)
        xEventGroupSetBits(canMessageLostCheckEventGroup, MASK(2U));
    else
        xEventGroupClearBits(canMessageLostCheckEventGroup, MASK(2U));


    if(brake <= checkingTime)
        xEventGroupSetBits(canMessageLostCheckEventGroup, MASK(3U));
    else
        xEventGroupClearBits(canMessageLostCheckEventGroup, MASK(3U));


    if(joystick <= checkingTime)
        xEventGroupSetBits(canMessageLostCheckEventGroup, MASK(4U));
     else
        xEventGroupClearBits(canMessageLostCheckEventGroup, MASK(4U));

}
void vAcceleratorBrakeJoystickTxHandler(void *pvParameters)
{
    TickType_t acceleratorTimeOutControl = xTaskGetTickCount() + maxTimeOutTime.maxTime[0];
    TickType_t brakeTimeOutControl = xTaskGetTickCount() + maxTimeOutTime.maxTime[1];
    TickType_t joystickTimeOutControl =  xTaskGetTickCount() + maxTimeOutTime.maxTime[2];
    TickType_t checkingTime = 0U;
    int torqueValue = 0 ;


    acceleratorBrakeJoystick_t acceleratorBrakeJoystick;
    selectorTx_t *selectorTx = &acceleratorBrakeJoystick.p.selectorTx;
    brakeTx_t *brakeTx = &acceleratorBrakeJoystick.p.brakeTx;
    acceleratorTx_t *accelerator = &acceleratorBrakeJoystick.p.acceleratorTx;

    for(;;)
    {
        if(xQueueReceive(xQueueAcceleratorBrakeJoystickTx, &acceleratorBrakeJoystick, pdMS_TO_TICKS(500)))
        {
            if(acceleratorBrakeJoystick.id == SELECTOR_TX)
            {
                acceleratorTimeOutControl = xTaskGetTickCount() + maxTimeOutTime.maxTime[0];
            }
            else if ( acceleratorBrakeJoystick.id == BRAKE_TX)
            {
                brakeTimeOutControl = xTaskGetTickCount() + maxTimeOutTime.maxTime[1];
            }
            else
            {
                joystickTimeOutControl =  xTaskGetTickCount() + maxTimeOutTime.maxTime[2];
                xQueueOverwrite(xqueueAcceleratorValue, &torqueValue);
            }
        }
        /** Update CAN messages timeout value */
        checkingTime = xTaskGetTickCount();
        checkLostsOfComponents(acceleratorTimeOutControl, brakeTimeOutControl, joystickTimeOutControl, checkingTime);

        taskYIELD();
    }
}
