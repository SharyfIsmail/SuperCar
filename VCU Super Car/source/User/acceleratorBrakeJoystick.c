/*
 * acceleratorBrakeJoystick.c
 *
 *  Created on: 11 мар. 2020 г.
 *      Author: User
 */

#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"
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
     1000,
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
    TickType_t acceleratorTimeOutControl = 0U;
    TickType_t brakeTimeOutControl = 0U;
    TickType_t joystickTimeOutControl = 0U;
    TickType_t checkingTime = 0U;

    acceleratorBrakeJoystick_t acceleratorBrakeJoystick;
    selectorTx_t *selectorTx = &acceleratorBrakeJoystick.p.selectorTx;
    brakeTx_t *brakeTx = &acceleratorBrakeJoystick.p.brakeTx;
    acceleratorTx_t *accelerator = &acceleratorBrakeJoystick.p.acceleratorTx;

    for(;;)
    {
        xQueueReceive(xQueueAcceleratorBrakeJoystickTx, &acceleratorBrakeJoystick, pdMS_TO_TICKS(5000));

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


        /** Update CAN messages timeout value */
        checkingTime = xTaskGetTickCount();
        if(acceleratorTimeOutControl < checkingTime)
        {

        }
        if(brakeTimeOutControl < checkingTime)
        {

        }
        if(joystickTimeOutControl < checkingTime)
        {

        }

        taskYIELD();
    }
}

