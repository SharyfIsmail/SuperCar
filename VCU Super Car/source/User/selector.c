/*
 * selector.c
 *
 *  Created on: 6 мар. 2020 г.
 *      Author: User
 */

#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"
#include "newCanLib.h"
#include "selector.h"

void vSlectorTxHandler (void *pvParameters);
void vSlectorRxHandler  (void *pvParameters);
TaskHandle_t xSelectorTxHanlder;
TaskHandle_t xSelectorRxHanlder;

void selectorInit()
{
    if(xTaskCreate(vSlectorTxHandler, "SlectorTxHandle", configMINIMAL_STACK_SIZE, NULL, 1, &xSelectorTxHanlder) != pdTRUE)
    {
           /*Task couldn't be created */
           while(1);
    }
    if(xTaskCreate(vSlectorRxHandler, "SlectorRxHandle", configMINIMAL_STACK_SIZE, NULL, 1, &xSelectorRxHanlder) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
}

void vSlectorTxHandler (void *pvParameters)
{
    for(;;)
    {

    }
}

void vSlectorRxHandler  (void *pvParameters)
{
    for(;;)
    {

    }
}
