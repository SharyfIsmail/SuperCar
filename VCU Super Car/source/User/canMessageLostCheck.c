/*
 * canMessageLostCheck.c
 *
 *  Created on: 12 мар. 2020 г.
 *      Author: User
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "sys_main.h"
#include "newCanLib.h"
#include "canMessageLostCheck.h"
#include "vcuStateManagement.h"
#include "SemikronRx.h"
#include "externalMemoryTask.h"
#include "timeTask.h"
#include "currentErrorViewer.h"

TaskHandle_t xCanMessageLostCheckHandler;
TaskHandle_t xLostComponentSendExternal;

void vCanMessageLostCheckHandler(void *pvParameters);
void vLostComponentWatcher(void *pvParameters);

static void setVcuRawStatus(causingOfError_t cause, VcuStateMangement_t *vcuStatus);
static void invertorLostHandler(VcuStateMangement_t *vcuStatus);
static void bmsLostHandler(VcuStateMangement_t *vcuStatus);
static void acceleratorLostHandler(VcuStateMangement_t *vcuStatus);
static void brakeLostHandler(VcuStateMangement_t *vcuStatus);
static void joystickLostHandler(VcuStateMangement_t *vcuStatus);
static void DcdcLostHandler(VcuStateMangement_t *vcuStatus);
static void LostComponentsBitSet(EventBits_t value);

typedef bool (*identifyLostComponent_t)(EventBits_t value);
typedef void (*LostComponentHandler_t)(VcuStateMangement_t *vcuStatus);

static causingOfError_t  causingOfError = EVERYTHING_IS_FINE;

static canMessageLost_t canMessageLoses =
{
    .arr = {false}
};
static canMessageLost_t errorLogIsWrote =
{
 .arr = {false}
};

const static identifyLostComponent_t  identifyLostComponent[COUNT_OF_COMPONENTS]=
{
     [INVERTOR_INDEX ]    = getInverterLost,
     [BMS_INDEX ]         = getBmsLost,
     [ACCELERATOR_INDEX ] = getAcceleratorLost,
     [BRAKE_INDEX ]       = getBrakeLost,
     [JOYSTICK_INDEX ]    = getJoystickLost,
     [DCDC_INDEX ]        = getDcLost,
};

const static LostComponentHandler_t  LostComponentHandler[COUNT_OF_COMPONENTS]=
{
    [INVERTOR_INDEX ]    = invertorLostHandler,
    [BMS_INDEX ]         = bmsLostHandler,
    [ACCELERATOR_INDEX ] = acceleratorLostHandler,
    [BRAKE_INDEX ]       = brakeLostHandler,
    [JOYSTICK_INDEX ]    = joystickLostHandler,
    [DCDC_INDEX ]        = DcdcLostHandler,
};


void canMessageLostCheckInit(void)
{
    if(xTaskCreate(vCanMessageLostCheckHandler, "CanMessageLostCheckHandler", configMINIMAL_STACK_SIZE, NULL, 1, &xCanMessageLostCheckHandler) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
    if(xTaskCreate(vLostComponentWatcher, "LostComponentWatcher", configMINIMAL_STACK_SIZE, (void *)LOST_COMPONENT_WATCHER_PERIOD, 1, &xLostComponentSendExternal) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
}

void vCanMessageLostCheckHandler(void *pvParameters)
{
    VcuStateMangement_t vcuStatus = VCU_Status_Init;
    const EventBits_t numberOfLost = 0x7F;

    for(;;)
    {
        EventBits_t  getLost = xEventGroupWaitBits(canMessageLostCheckEventGroup, numberOfLost, pdFALSE, pdFALSE, portMAX_DELAY);
        xEventGroupClearBits(canMessageLostCheckEventGroup, MASK(6U));

        xQueuePeek(xQueueVcuStatus, &vcuStatus, pdMS_TO_TICKS(0));

        for(int i = 0 ; i < COUNT_OF_COMPONENTS; i++)
        {
            canMessageLoses.arr[i]  = identifyLostComponent[i](getLost);
            if(canMessageLoses.arr[i])
            {
                LostComponentHandler[i](&vcuStatus);
                errorLogIsWrote.arr[i] = true;
            }
            else
                errorLogIsWrote.arr[i] = false;
        }
        LostComponentsBitSet(getLost);
        if(!(getLostComponenst(getLost)))
        {
            vcuStatus = VCU_Status_Init;
        }/* else not needed */
        xQueueOverwrite(xQueueVcuStatusManagement, &vcuStatus);
    }
}

void vLostComponentWatcher(void *pvParameters)
{
    TickType_t lastWeakTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS( (uint32_t) pvParameters );
    lastWeakTime = xTaskGetTickCount();

    for(;;)
    {
        xEventGroupSetBits(canMessageLostCheckEventGroup, MASK(6U));
        vTaskDelayUntil( &lastWeakTime, transmitPeriod);
    }
}
static void LostComponentsBitSet(EventBits_t value)
{
    uint8_t lostComponentsBits = 0;
    setLostComponents(&lostComponentsBits, value);
    xQueueOverwrite(queueLostComponentsError, &lostComponentsBits);
}
static void setVcuRawStatus(causingOfError_t cause, VcuStateMangement_t *vcuStatus)
{
    switch(cause)
    {
    case INVERTOR_CANMESSAGE_LOST:
        if(*vcuStatus == VCU_Status_Parking || *vcuStatus == VCU_Status_Neutral
            || *vcuStatus == VCU_Status_Init)
        {
            *vcuStatus = VCU_Status_ErrorStop;
        }

        else
        {
            *vcuStatus = VCU_Status_ErrorStop;
        }
        break;

    case BMS_CANMESSAGE_LOST:
        if(*vcuStatus == VCU_Status_Parking || *vcuStatus == VCU_Status_Neutral
            || *vcuStatus == VCU_Status_Init)
        {
            *vcuStatus = VCU_Status_ErrorBatteryOff;
        }
        else
        {
            *vcuStatus = VCU_Status_ErrorBatteryOff;
        }
        break;

    case ACCELERATOR_CANMESSAGE_LOST:
        if(*vcuStatus == VCU_Status_Parking || *vcuStatus == VCU_Status_Neutral
            || *vcuStatus == VCU_Status_Init)
        {
            *vcuStatus = VCU_Status_ErrorStop;
        }
        else if(*vcuStatus != VCU_Status_ErrorStop && *vcuStatus != VCU_Status_ErrorBatteryOff)
        {
            *vcuStatus = VCU_Status_ErrorDrive;
        }
        break;

    case BRAKE_CANMESSAGE_LOST:
        if(*vcuStatus == VCU_Status_Parking || *vcuStatus == VCU_Status_Neutral
                || *vcuStatus == VCU_Status_Init)
        {
            *vcuStatus = VCU_Status_ErrorStop;
        }
        else if(*vcuStatus != VCU_Status_ErrorStop && *vcuStatus != VCU_Status_ErrorBatteryOff)
        {
            *vcuStatus = VCU_Status_ErrorDrive;
        }
        break;

    case JOYSTICK_CANMESSAGE_LOST:
        if(*vcuStatus == VCU_Status_Parking || *vcuStatus == VCU_Status_Neutral
                || *vcuStatus == VCU_Status_Init)
        {
            *vcuStatus = VCU_Status_ErrorStop;
        }
        else if(*vcuStatus != VCU_Status_ErrorStop && *vcuStatus != VCU_Status_ErrorBatteryOff)
        {
            *vcuStatus = VCU_Status_ErrorDrive;
        }
        break;

    case DCDC_CANMESSAGE_LOST:
        if(*vcuStatus == VCU_Status_Parking || *vcuStatus == VCU_Status_Neutral
                  || *vcuStatus == VCU_Status_Init)
        {
            *vcuStatus = VCU_Status_ErrorStop;
        }
        else if(*vcuStatus != VCU_Status_ErrorStop && *vcuStatus != VCU_Status_ErrorBatteryOff)
        {
            *vcuStatus = VCU_Status_ErrorDrive;
        }
        break;
    }
}
static void invertorLostHandler(VcuStateMangement_t *vcuStatus)
{
    causingOfError = INVERTOR_CANMESSAGE_LOST;
    if(!errorLogIsWrote.arr[0])
    {
        logError(causingOfError);
        setVcuRawStatus(causingOfError, vcuStatus);
    }/* else not needed */
}
static void bmsLostHandler(VcuStateMangement_t *vcuStatus)
{
    causingOfError = BMS_CANMESSAGE_LOST;
    if(!errorLogIsWrote.arr[1])
    {
        logError(causingOfError);
        setVcuRawStatus(causingOfError, vcuStatus);
    }/* else not needed */
}
static void acceleratorLostHandler(VcuStateMangement_t *vcuStatus)
{
    causingOfError = ACCELERATOR_CANMESSAGE_LOST;
    if(!errorLogIsWrote.arr[2])
    {
        logError(causingOfError);
        setVcuRawStatus(causingOfError, vcuStatus);
    }/* else not needed */
}
static void brakeLostHandler(VcuStateMangement_t *vcuStatus)
{
    causingOfError = BRAKE_CANMESSAGE_LOST;
    if(!errorLogIsWrote.arr[3])
    {
        logError(causingOfError);
        setVcuRawStatus(causingOfError, vcuStatus);
    }/* else not needed */
}
static void joystickLostHandler(VcuStateMangement_t *vcuStatus)
{
    causingOfError = JOYSTICK_CANMESSAGE_LOST;
    if(!errorLogIsWrote.arr[4])
    {
        logError(causingOfError);
        setVcuRawStatus(causingOfError, vcuStatus);
    }/* else not needed */
}
static void DcdcLostHandler(VcuStateMangement_t *vcuStatus)
{
    causingOfError = DCDC_CANMESSAGE_LOST;
    if(!errorLogIsWrote.arr[5])
    {
        logError(causingOfError);
        setVcuRawStatus(causingOfError, vcuStatus);
    }/* else not needed */
}
