/*
 * vcuStateManagement.c
 *
 *  Created on: 19 мар. 2020 г.
 *      Author: User
 */

#include "vcuStateManagement.h"
#include "os_task.h"

void vVcuStateManagement(void *pvParameters);

TaskHandle_t xVcuStateManagement;

QueueHandle_t xQueueVcuStatusManagement = NULL;
QueueHandle_t xQueueVcuStatus = NULL;
void vcuStateManagementInit(void)
{
    if(xTaskCreate(vVcuStateManagement, "VcuStateManagement", configMINIMAL_STACK_SIZE, NULL, 2, xVcuStateManagement) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
    xQueueVcuStatusManagement = xQueueCreate(1U, sizeof(VcuStateMangement_t));
    xQueueVcuStatus = xQueueCreate(1U, sizeof(VcuStateMangement_t));
}


void vVcuStateManagement(void *pvParameters)
{
    VcuStateMangement_t vcuStateMangement;
    VcuStateMangement_t vcuStatus = VCU_Status_Init

    for(;;)
    {
        if(xQueueReceive(xQueueVcuStatusManagement, &vcuStateMangement, portMAX_DELAY))
        {

        }

        xQueueOverwrite(xQueueVcuStatus, &vcuStatus);
    }
}
