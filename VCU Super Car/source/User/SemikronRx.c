/*
 * SemikronRx.c
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */

#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"
#include "newCanLib.h"

void vSemicronRxHandler (void *pvParameters);
void vSemicronNmtCommand (void *pvParameters);
void vSemicronSyn(void *pvParameters);

TaskHandle_t xSemicronRxHandler;
TaskHandle_t xNMTCommand;

void semikronRxInit(void)
{
    if(xTaskCreate(vSemicronRxHandler, "SemicronRxHandler", configMINIMAL_STACK_SIZE, NULL, 1, &xSemicronRxHandler ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
    if(xTaskCreate(vSemicronNmtCommand, "NMT_Command", configMINIMAL_STACK_SIZE, NULL, 1, &xNMTCommand ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
    if(xTaskCreate(vSemicronSym, "SemicronSync", configMINIMAL_STACK_SIZE, ((uint32_t)CAN_PERIOD_MS_SEMICRON_SYN), 1,NULL ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
}

void vSemicronRxHandler (void *pvParameters)
{
    while(1)
    {
        taskYIELD();
    }
}

void vSemicronNmtCommand (void *pvParameters)
{
    while(1)
    {
        taskYIELD();
    }
}
void vSemicronSyn(void *pvParameters)
{
    TickType_t lastWeakTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS( (uint32_t) pvParameters );

    while(1)
    {
        vTaskDelayUntil( &lastWeakTime, transmitPeriod );
    }
}


