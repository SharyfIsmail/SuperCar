/*
 * vcuStateManagement.c
 *
 *  Created on: 19 мар. 2020 г.
 *      Author: User
 */

#include "vcuStateManagement.h"
#include "task.h"

#define QUEUE_LOST_COMPONENT_RAWSTATUS_LENGTH   (1U)
#define QUEUE_SEMICRON_RAWSTATUS_LENGTH         (1U)
#define QUEUE_SELECTOR_MODE_LENGTH              (1U)
#define QUEUE_BATTERY_RAWSTATUS_LENGTH          (1U)

#define COMBINED_LENGTH ( QUEUE_LOST_COMPONENT_RAWSTATUS_LENGTH + \
                          QUEUE_SEMICRON_RAWSTATUS_LENGTH + \
                          QUEUE_SELECTOR_MODE_LENGTH + \
                          QUEUE_BATTERY_RAWSTATUS_LENGTH )

TaskHandle_t xVcuStateManagement;

QueueHandle_t xQueueVcuStatusManagement = NULL;

QueueHandle_t xQueueLostComponentRawStatus = NULL;
QueueHandle_t xQueueSemicronRawStatus = NULL;
QueueHandle_t xQueueSelectorMode = NULL;
QueueHandle_t xQueueBatteryRawStatus = NULL;
QueueHandle_t xQueueSemicronStart = NULL;

QueueHandle_t xQueueVcuStatus = NULL;
static QueueSetMemberHandle_t activatedMember;

static QueueSetHandle_t xqueueSetvcuRawStatuses;

static void setCurrentVcuStatus(VcuErrorStatus_t LostComponents, VcuErrorStatus_t Semicron, SelectorMode_t joystick, VcuRawStatusBattery_t battery);
static void setVcuErrorLostComponents(VcuErrorStatus_t *lostComponentError,VcuStateMangement_t currentVcuStatus, lostComponentsStatus_t LostComponentsStatus);
static void setVcuErrorSemicron(VcuErrorStatus_t *semicronError, VcuStateMangement_t currentVcuStatus, SemicronStatus_t SemicronStatus);
static void setSelectorMode(SelectorMode_t *selectorMode,VcuStateMangement_t currentVcuStatus, SelectorStructModeTx_t selectorStructRequest);

static VcuRawStatuses_t vcuRawStatuses = {
                                          .lostComponentsStatus = NO_CRASH_lOST_MESSAGE,
                                          .SemicronStatus = NO_CRASH_SEMICRON,
                                          .selectorModeStruct = {
                                              .selectorMode = SELECTOR_MODE_UNDEFINED,
                                              .selectorInitialization = SELECTOR_INIT_INIT
                                          },
                                          .vcuRawStatusBattery = BATTERY_INIT
                                         };
static VcuStatusStruct_t currentVcuStatusStruct = { VCU_STATUS_INIT,
                                             VCU_NO_ERROR
                                           };

void vVcuStateManagement(void *pvParameters);

void vcuStateManagementInit(void)
{
    if(xTaskCreate(vVcuStateManagement, "VcuStateManagement", configMINIMAL_STACK_SIZE, NULL, 2, &xVcuStateManagement) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
    xqueueSetvcuRawStatuses = xQueueCreateSet(COMBINED_LENGTH);

    xQueueLostComponentRawStatus = xQueueCreate(QUEUE_LOST_COMPONENT_RAWSTATUS_LENGTH, sizeof(lostComponentsStatus_t));
    xQueueSemicronRawStatus = xQueueCreate(QUEUE_SEMICRON_RAWSTATUS_LENGTH, sizeof(SemicronStatus_t));
    xQueueSelectorMode = xQueueCreate(QUEUE_SELECTOR_MODE_LENGTH, sizeof(SelectorStructModeTx_t));
    xQueueBatteryRawStatus  = xQueueCreate(QUEUE_BATTERY_RAWSTATUS_LENGTH, sizeof(VcuRawStatusBattery_t));

    xQueueAddToSet(xQueueLostComponentRawStatus, xqueueSetvcuRawStatuses);
    xQueueAddToSet(xQueueSemicronRawStatus, xqueueSetvcuRawStatuses);
    xQueueAddToSet(xQueueSelectorMode, xqueueSetvcuRawStatuses);
    xQueueAddToSet(xQueueBatteryRawStatus, xqueueSetvcuRawStatuses);

    xQueueSemicronStart = xQueueCreate(1U,sizeof(VcuRawStatusBattery_t));
    xQueueVcuStatus = xQueueCreate(1U, sizeof(VcuStatusStruct_t));
}

void vVcuStateManagement(void *pvParameters)
{
     VcuErrorStatus_t lostComponentError = VCU_NO_ERROR;
     VcuErrorStatus_t SemicronError = VCU_ERROR_UNDEFINED;
     SelectorMode_t selectorMode = SELECTOR_MODE_UNDEFINED;
     VcuRawStatusBattery_t rawVcuStatusBattery  =  BATTERY_INIT;

    for(;;)
    {
        activatedMember = xQueueSelectFromSet(xqueueSetvcuRawStatuses, pdMS_TO_TICKS(200));

        if(activatedMember == xQueueLostComponentRawStatus)
        {
            xQueueReceive(xQueueLostComponentRawStatus, &vcuRawStatuses.lostComponentsStatus, 0);
            setVcuErrorLostComponents(&lostComponentError,currentVcuStatusStruct.vcuStateMangement ,vcuRawStatuses.lostComponentsStatus);

        }
        else if (activatedMember == xQueueSemicronRawStatus)
        {
            xQueueReceive(xQueueSemicronRawStatus, &vcuRawStatuses.SemicronStatus, 0);
             setVcuErrorSemicron(&SemicronError, currentVcuStatusStruct.vcuStateMangement, vcuRawStatuses.SemicronStatus);
        }
        else if (activatedMember == xQueueSelectorMode)
        {
            xQueueReceive(xQueueSelectorMode, &vcuRawStatuses.selectorModeStruct, 0);
            setSelectorMode(&selectorMode, currentVcuStatusStruct.vcuStateMangement, vcuRawStatuses.selectorModeStruct);
        }
        else if(activatedMember == xQueueBatteryRawStatus)
        {
            xQueueReceive(xQueueBatteryRawStatus, &vcuRawStatuses.vcuRawStatusBattery, 0);
            rawVcuStatusBattery = vcuRawStatuses.vcuRawStatusBattery;
            xQueueOverwrite(xQueueSemicronStart, &vcuRawStatuses.vcuRawStatusBattery);

        }
        else
        {
            /* The 200ms block time expired without an RTOS queue
                        being ready to process. */
        }

      //  setCurrentVcuStatus(rawVcuStatusLostComponents, rawVcuStatusSemicron, currentSelectorStatus, rawVcuStatusBattery);
        xQueueOverwrite(xQueueVcuStatus, &currentVcuStatusStruct);
    }
}
static void setVcuErrorLostComponents(VcuErrorStatus_t *lostComponentError,VcuStateMangement_t currentVcuStatus, lostComponentsStatus_t LostComponentsStatus)
{
    switch(LostComponentsStatus)
    {
    case NO_CRASH_lOST_MESSAGE:
        *lostComponentError = VCU_NO_ERROR;
        break;
    case CRASH_LEVEL_ERRORDRIVE:
        if(currentVcuStatus == VCU_Status_REVERCE || currentVcuStatus == VCU_Status_FORWARD  ||
           currentVcuStatus == VCU_Status_CHARGING )
        {
            *lostComponentError = VCU_ERROR_WORK;
        }
        else
        {
            *lostComponentError = VCU_ERROR_STOP;
        }
        break;
    case CRASH_LEVEL_ERRORSTOP:
        *lostComponentError = VCU_ERROR_STOP;
        break;
    }
}
static void setVcuErrorSemicron(VcuErrorStatus_t *semicronError, VcuStateMangement_t currentVcuStatus, SemicronStatus_t SemicronStatus)
{
    static uint8_t errorCount = 20;
    switch(SemicronStatus)
    {
    case NO_CRASH_SEMICRON:
        *semicronError = VCU_NO_ERROR;
        errorCount = 20;
        break;
    case CLEAR_ERROR_SEMICRON:
        errorCount--;
        if(currentVcuStatus == VCU_STATUS_INIT )
        {
            if(errorCount == 0)
                *semicronError = VCU_ERROR_STOP;
        }
        else
        {
            *semicronError = VCU_ERROR_STOP;
        }
        break;
    }
}
static void setSelectorMode(SelectorMode_t *selectorMode,VcuStateMangement_t currentVcuStatus, SelectorStructModeTx_t selectorStructRequest)
{
    if(selectorStructRequest.selectorInitialization == SELECTOR_INIT_OPERATIONAL)
    {
        switch(selectorStructRequest.selectorMode)
        {
        case SELECTOR_MODE_INIT :
            *selectorMode = SELECTOR_MODE_INIT;
            break;
        case SELECTOR_MODE_PARKING :
            if(currentVcuStatus == VCU_Status_NEUTRAL)
                *selectorMode = SELECTOR_MODE_PARKING;
            break;
        case SELECTOR_MODE_NEUTRAL :
            *selectorMode = SELECTOR_MODE_NEUTRAL;
            break;
        case SELECTOR_MODE_FORWARD:
            if(currentVcuStatus == VCU_Status_NEUTRAL)
                *selectorMode = SELECTOR_MODE_FORWARD;
            break;
        case SELECTOR_MODE_REVERSE :
            if(currentVcuStatus == VCU_Status_NEUTRAL)
                *selectorMode = SELECTOR_MODE_REVERSE;
            break;
          }
    }
    else if(selectorStructRequest.selectorInitialization == SELECTOR_INIT_ERROR)
    {
        *selectorMode = SELECTOR_MODE_UNDEFINED;
    }
    else
    {
        *selectorMode = SELECTOR_MODE_INIT;
    }
}
//static VcuStateMangement_t setVcuRawStatusBattery(VcuStateMangement_t currentVcuStatus, VcuRawStatusLostComponents_t LostComponentsStatus)
//{

//}
static void setCurrentVcuStatus(VcuErrorStatus_t LostComponents, VcuErrorStatus_t Semicron, SelectorMode_t selector, VcuRawStatusBattery_t battery)
{
    /*if(LostComponents == VCU_STATUS_INIT && Semicron == VCU_STATUS_INIT &&
       battery == VCU_STATUS_INIT && (joystick == VCU_Status_REVERCE || joystick == VCU_Status_REVERCE))
    {
        currentVcuStatus = VCU_STATUS_INIT;
    }
    else if (LostComponents ==  VCU_STATUS_INIT && Semicron == VCU_STATUS_INIT &&
             battery == VCU_Status_REVERCE &&  joystick == VCU_Status_REVERCE)
    {
        currentVcuStatus = VCU_Status_REVERCE;
    }
    else if (LostComponents == VCU_STATUS_INIT && Semicron == VCU_STATUS_INIT &&
             battery == VCU_Status_REVERCE && joystick == VCU_Status_REVERCE)
    {
        currentVcuStatus = VCU_Status_REVERCE;
    }
    else if (LostComponents == VCU_STATUS_INIT && Semicron == VCU_STATUS_INIT &&
             battery == VCU_Status_SLEEP && (joystick == VCU_Status_REVERCE || joystick == VCU_Status_REVERCE))
    {
        currentVcuStatus = VCU_Status_REVERCE;
    }*/

}
const VcuStatusStruct_t* getVcuStatusStruct()
{
    return &currentVcuStatusStruct;
}
