/*
 * vcuStateManagement.c
 *
 *  Created on: 19 мар. 2020 г.
 *      Author: User
 */

#include "vcuStateManagement.h"
#include "task.h"


TaskHandle_t xVcuStateManagement;

QueueHandle_t xQueueVcuStatusManagement = NULL;
QueueHandle_t xQueueVcuStatus = NULL;

static VcuStateMangement_t currentVcuStatus = VCU_Status_Init;
static VcuStateMangement_t rawVcuStatus;

static void setVcuStatus();
void vVcuStateManagement(void *pvParameters);

void vcuStateManagementInit(void)
{
    if(xTaskCreate(vVcuStateManagement, "VcuStateManagement", configMINIMAL_STACK_SIZE, NULL, 2, &xVcuStateManagement) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
    xQueueVcuStatusManagement = xQueueCreate(1U, sizeof(VcuStateMangement_t));
    xQueueVcuStatus = xQueueCreate(1U, sizeof(VcuStateMangement_t));
}

void vVcuStateManagement(void *pvParameters)
{
    for(;;)
    {
        if(xQueueReceive(xQueueVcuStatusManagement, &rawVcuStatus, portMAX_DELAY))
        {
            setVcuStatus();
        }/* else not needed */

        xQueueOverwrite(xQueueVcuStatus, &currentVcuStatus);
    }
}

static void setVcuStatus()
{
    switch(rawVcuStatus)
    {
    case VCU_Status_Init:
        if(currentVcuStatus == VCU_Status_ErrorStop)
        {
            currentVcuStatus = VCU_Status_Init;
        }/* else not needed */
        break;

    case VCU_CLEAR_ERROR:
        if(currentVcuStatus == VCU_Status_Neutral || currentVcuStatus == VCU_Status_Parking)
        {
            currentVcuStatus = VCU_CLEAR_ERROR;
        }
        else
        {
            currentVcuStatus = VCU_Status_ErrorStop;
        }
        break;

    case VCU_Status_Parking:
        if(currentVcuStatus != VCU_Status_ErrorStop )
        {
            currentVcuStatus = VCU_Status_Parking;
        }/* else not needed */
        break;

    case VCU_Status_Neutral:
        if(currentVcuStatus != VCU_Status_Parking && currentVcuStatus != VCU_Status_ErrorStop)
        {
            currentVcuStatus = VCU_Status_Neutral;
        }/* else not needed */
        break;

    case VCU_Status_Forward:
        if(currentVcuStatus != VCU_Status_Parking && currentVcuStatus != VCU_Status_ErrorStop)
        {
            currentVcuStatus = VCU_Status_Forward;
        }/* else not needed */
        break;

    case VCU_Status_Reverse:
        if(currentVcuStatus != VCU_Status_Parking && currentVcuStatus != VCU_Status_ErrorStop)
        {
            currentVcuStatus = VCU_Status_Reverse;
        }
        break;

    case VCU_Status_Charge:
        if(currentVcuStatus == VCU_Status_Parking)
        {
            currentVcuStatus = VCU_Status_Charge;
        }/* else not needed */
        break;

    case VCU_Status_Sleep:
        break;

    case VCU_Status_ErrorDrive:
        if(currentVcuStatus != VCU_Status_ErrorStop && currentVcuStatus != VCU_Status_ErrorBatteryOff)
        {
            currentVcuStatus = VCU_Status_ErrorDrive;
        }/* else not needed */
        break;

    case VCU_Status_ErrorStop:
        if(currentVcuStatus != VCU_Status_ErrorBatteryOff)
        {
            currentVcuStatus = VCU_Status_ErrorStop;
        }/* else not needed */
        break;

    case VCU_Status_ErrorBatteryOff:
        currentVcuStatus = VCU_Status_ErrorBatteryOff;
        break;
    }
}
