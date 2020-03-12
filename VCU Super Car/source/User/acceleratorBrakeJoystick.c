/*
 * acceleratorBrakeJoystick.c
 *
 *  Created on: 11 мар. 2020 г.
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

maxTimeOutTime_t maxTimeOutTime =
{
 .maxTime =
 {
     1000,  // accelerator
     1000,  // Brake
     1000,  //
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

}


void vAcceleratorBrakeJoystickTxHandler(void *pvParameters)
{
    TickType_t acceleratorTimeOutControl = xTaskGetTickCount() + maxTimeOutTime.maxTime[0];
    TickType_t brakeTimeOutControl = xTaskGetTickCount() + maxTimeOutTime.maxTime[1];
    TickType_t joystickTimeOutControl =  xTaskGetTickCount() + maxTimeOutTime.maxTime[2];
    TickType_t checkingTime = 0U;

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
            }

        }
        /** Update CAN messages timeout value */
        checkingTime = xTaskGetTickCount();

        if(acceleratorTimeOutControl <= checkingTime)
        {
            xEventGroupSetBits(canMessageLostCheckEventGroup, MASK(2U));
        }

        if(brakeTimeOutControl <= checkingTime)
        {
            xEventGroupSetBits(canMessageLostCheckEventGroup, MASK(3U));
        }

        if(joystickTimeOutControl <= checkingTime)
        {
            xEventGroupSetBits(canMessageLostCheckEventGroup, MASK(4U));
        }

        taskYIELD();
    }
}

