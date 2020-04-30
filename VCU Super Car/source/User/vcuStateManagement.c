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

QueueHandle_t xQueueLostComponentError = NULL;
QueueHandle_t xQueueSemicronError = NULL;
QueueHandle_t xQueueSelectorMode = NULL;
QueueHandle_t xQueueBatteryMode = NULL;
QueueHandle_t xQueueSemicronStart = NULL;

//QueueHandle_t xQueueVcuStatus = NULL;
static QueueSetMemberHandle_t activatedMember;

static QueueSetHandle_t xqueueSetvcuRawStatuses;
static taskStatuses_t taskStatuses = {
                                          .lostComponentsStatus = NO_CRASH_lOST_MESSAGE,
                                          .SemicronStatus = NO_CRASH_SEMICRON,
                                          .selectorModeStruct =
                                          {
                                              .selectorMode = SELECTOR_MODE_UNDEFINED,
                                              .selectorInitialization = SELECTOR_INIT_INIT
                                          },
                                          .bmsModeState =
                                           {
                                            .batteryMode = BATTERY_INIT,
                                            .batteryState = false
                                           }
                                         };
static VcuStatusStruct_t currentVcuStatusStruct = { VCU_STATUS_INIT,
                                             VCU_NO_ERROR
                                           };
static void setCurrentVcuStatus(VcuStatusStruct_t* vcuCurrentStatus, VcuErrorStatus_t LostComponents, VcuErrorStatus_t Semicron, SelectorMode_t selector,
                                batteryMode_t batteryMode, VcuErrorStatus_t bmsError);
static void setVcuErrorLostComponents(VcuErrorStatus_t *lostComponentError, VcuStateMangement_t currentVcuStatus, lostComponentsStatus_t LostComponentsStatus);
static void setVcuErrorSemicron(VcuErrorStatus_t *semicronError, VcuStateMangement_t currentVcuStatus, SemicronStatus_t SemicronStatus);
static void setvcuSelectorMode(SelectorMode_t *selectorMode, VcuStateMangement_t currentVcuStatus, SelectorStructModeTx_t selectorStructRequest);
static void setVcuBmsError(VcuErrorStatus_t *bmsError, bool error);

void vVcuStateManagement(void *pvParameters);

void vcuStateManagementInit(void)
{
    if(xTaskCreate(vVcuStateManagement, "VcuStateManagement", configMINIMAL_STACK_SIZE, NULL, 2, &xVcuStateManagement) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
    xqueueSetvcuRawStatuses = xQueueCreateSet(COMBINED_LENGTH);

    xQueueLostComponentError = xQueueCreate(QUEUE_LOST_COMPONENT_RAWSTATUS_LENGTH, sizeof(lostComponentsStatus_t));
    xQueueSemicronError = xQueueCreate(QUEUE_SEMICRON_RAWSTATUS_LENGTH, sizeof(SemicronStatus_t));
    xQueueSelectorMode = xQueueCreate(QUEUE_SELECTOR_MODE_LENGTH, sizeof(SelectorStructModeTx_t));
    xQueueBatteryMode  = xQueueCreate(QUEUE_BATTERY_RAWSTATUS_LENGTH, sizeof(bmsMode_State_t));

    xQueueAddToSet(xQueueLostComponentError, xqueueSetvcuRawStatuses);
    xQueueAddToSet(xQueueSemicronError, xqueueSetvcuRawStatuses);
    xQueueAddToSet(xQueueSelectorMode, xqueueSetvcuRawStatuses);
    xQueueAddToSet(xQueueBatteryMode, xqueueSetvcuRawStatuses);

    xQueueSemicronStart = xQueueCreate(1U,sizeof(batteryMode_t));
   // xQueueVcuStatus = xQueueCreate(1U, sizeof(VcuStatusStruct_t));
}


void vVcuStateManagement(void *pvParameters)
{
     VcuErrorStatus_t lostComponentError = VCU_ERROR_UNDEFINED;
     VcuErrorStatus_t SemicronError = VCU_ERROR_UNDEFINED;
     SelectorMode_t selectorMode = SELECTOR_MODE_UNDEFINED;
     VcuErrorStatus_t batteryError  =  VCU_ERROR_UNDEFINED;

    for(;;)
    {
        activatedMember = xQueueSelectFromSet(xqueueSetvcuRawStatuses, pdMS_TO_TICKS(200));

        if(activatedMember == xQueueLostComponentError)
        {
            xQueueReceive(xQueueLostComponentError, &taskStatuses.lostComponentsStatus, 0);
            setVcuErrorLostComponents(&lostComponentError,currentVcuStatusStruct.vcuStateMangement ,taskStatuses.lostComponentsStatus);

        }
        else if (activatedMember == xQueueSemicronError)
        {
            xQueueReceive(xQueueSemicronError, &taskStatuses.SemicronStatus, 0);
             setVcuErrorSemicron(&SemicronError, currentVcuStatusStruct.vcuStateMangement, taskStatuses.SemicronStatus);
        }
        else if (activatedMember == xQueueSelectorMode)
        {
            xQueueReceive(xQueueSelectorMode, &taskStatuses.selectorModeStruct, 0);
            setvcuSelectorMode(&selectorMode, currentVcuStatusStruct.vcuStateMangement, taskStatuses.selectorModeStruct);
        }
        else if(activatedMember == xQueueBatteryMode)
        {
            xQueueReceive(xQueueBatteryMode, &taskStatuses.bmsModeState, 0);
          //  batteryError = taskStatuses.bmsModeState.battery;
            setVcuBmsError(&batteryError, taskStatuses.bmsModeState.batteryState);
            xQueueOverwrite(xQueueSemicronStart, &taskStatuses.bmsModeState.batteryMode);
        }
        else
        {
            /* The 200ms block time expired without an RTOS queue
                        being ready to process. */
        }

        setCurrentVcuStatus(&currentVcuStatusStruct, lostComponentError, SemicronError, selectorMode, taskStatuses.bmsModeState.batteryMode, batteryError);
    }
}
static void setVcuBmsError(VcuErrorStatus_t *bmsError, bool error)
{
    if(error)
    {
        *bmsError = VCU_ERROR_STOP;
    }
    else
    {
        *bmsError = VCU_NO_ERROR;
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
        if(currentVcuStatus == VCU_STATUS_REVERCE || currentVcuStatus == VCU_STATUS_FORWARD  ||
           currentVcuStatus == VCU_STATUS_CHARGING )
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
static void setvcuSelectorMode(SelectorMode_t *selectorMode,VcuStateMangement_t currentVcuStatus, SelectorStructModeTx_t selectorStructRequest)
{
    if(selectorStructRequest.selectorInitialization == SELECTOR_INIT_OPERATIONAL)
    {
        switch(selectorStructRequest.selectorMode)
        {
        case SELECTOR_MODE_INIT :
            *selectorMode = SELECTOR_MODE_INIT;
            break;
        case SELECTOR_MODE_PARKING :
            if(currentVcuStatus == VCU_STATUS_NEUTRAL)
                *selectorMode = SELECTOR_MODE_PARKING;
            break;
        case SELECTOR_MODE_NEUTRAL :
            *selectorMode = SELECTOR_MODE_NEUTRAL;
            break;
        case SELECTOR_MODE_FORWARD:
            if(currentVcuStatus == VCU_STATUS_NEUTRAL)
                *selectorMode = SELECTOR_MODE_FORWARD;
            break;
        case SELECTOR_MODE_REVERSE :
            if(currentVcuStatus == VCU_STATUS_NEUTRAL)
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

static void setCurrentVcuStatus(VcuStatusStruct_t* vcuCurrentStatus, VcuErrorStatus_t LostComponents, VcuErrorStatus_t Semicron, SelectorMode_t selector,
                                batteryMode_t batteryMode, VcuErrorStatus_t bmsError)
{
    if(LostComponents == VCU_NO_ERROR && Semicron ==  VCU_NO_ERROR && bmsError != VCU_ERROR_STOP &&(batteryMode == BATTERY_INIT || batteryMode == BATTERY_NORMAL_OFF))
    {
        vcuCurrentStatus->vcuStateMangement = VCU_STATUS_INIT;
        vcuCurrentStatus->errorStatus = VCU_NO_ERROR;
    }
    else if(LostComponents == VCU_NO_ERROR && Semicron == VCU_NO_ERROR &&bmsError != VCU_ERROR_STOP && batteryMode == BATTERY_HV_ACTIVE )
    {
        switch(selector)
        {
        case SELECTOR_MODE_INIT:
            vcuCurrentStatus->vcuStateMangement = VCU_STATUS_PARKING;
            break;
        default :
            vcuCurrentStatus->vcuStateMangement = (VcuStateMangement_t)selector;
            break;
        }

        vcuCurrentStatus->errorStatus = VCU_NO_ERROR;
    }
    else if (LostComponents == VCU_ERROR_WORK && Semicron == VCU_NO_ERROR && bmsError != VCU_ERROR_STOP && batteryMode == BATTERY_HV_ACTIVE)
    {
        vcuCurrentStatus->vcuStateMangement = (VcuStateMangement_t)selector;
        vcuCurrentStatus->errorStatus = VCU_ERROR_WORK;
    }
    else if (LostComponents == VCU_ERROR_STOP || Semicron == VCU_ERROR_STOP  || bmsError == VCU_ERROR_STOP)
    {
        vcuCurrentStatus->vcuStateMangement = VCU_STATUS_SLEEP;
        vcuCurrentStatus->errorStatus = VCU_ERROR_STOP;
    }
}
const VcuStatusStruct_t* getVcuStatusStruct()
{
    return &currentVcuStatusStruct;
}
