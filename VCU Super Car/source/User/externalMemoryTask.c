/*
 * externalMemoryTask.c
 *
 *  Created on: 23 мар. 2020 г.
 *      Author: User
 */
#include "externalMemoryTask.h"

QueueHandle_t xQueueCommandToExtMemory = NULL;

void vExternalMemoryTask(void *pvParameters);

void externalMemoryTaskInit()
{
    if(xTaskCreate(vExternalMemoryTask, "ExternalMemoryTask",    configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdTRUE)
     {
         /*Task couldn't be created */
         while(1);
     }/* else not needed */
   // xQueueCommandToExtMemory = xQueueCreate(10U, )
}


