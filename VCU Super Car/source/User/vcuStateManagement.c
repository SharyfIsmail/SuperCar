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

static VcuStateMangement_t vcuStateMangement;
static   VcuStateMangement_t vcuStatus;

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

static void setVcuStatus()
{
    if(vcuStateMangement == VCU_CLEAR_ERROR && (vcuStatus == VCU_Status_Parking
            || vcuStatus == VCU_Status_Neutral))
        vcuStatus = VCU_CLEAR_ERROR;

    else
        vcuStatus = VCU_Status_ErrorDrive;

}

void vVcuStateManagement(void *pvParameters)
{
    for(;;)
    {
        if(xQueueReceive(xQueueVcuStatusManagement, &vcuStateMangement, portMAX_DELAY))
        {
         //   setVcuStatus();
        }

        xQueueOverwrite(xQueueVcuStatus, &vcuStatus);
    }
}
