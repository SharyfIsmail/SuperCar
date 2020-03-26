/*
 * BatteryManagementSystemTx.c
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */


#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "newCanLib.h"

void vBmsTxHandler (void *pvParameters);



TaskHandle_t xBmsTxHandler;
QueueHandle_t xQueueBmsTx = NULL;

void BmsTxInit(void)
{
    if(xTaskCreate(vBmsTxHandler, "BmsTxHandler", configMINIMAL_STACK_SIZE, NULL, 1, &xBmsTxHandler ) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */

    xQueueBmsTx =  xQueueCreate(20U, sizeof(canMessage_t));
}

void vBmsTxHandler (void *pvParameters)
{
    while(1)
    {
        taskYIELD();
    }
}




