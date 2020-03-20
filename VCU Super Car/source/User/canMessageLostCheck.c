/*
 * canMessageLostCheck.c
 *
 *  Created on: 12 ���. 2020 �.
 *      Author: User
 */

#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"
#include "sys_main.h"
#include "newCanLib.h"
#include "canMessageLostCheck.h"
#include "vcuStateManagement.h"
#include "SemikronRx.h"

TaskHandle_t xCanMessageLostCheckHandler;
TaskHandle_t xLostComponentSendExternal;

//canMessageLoses_t ErrorModeOperation;
QueueHandle_t ErrorModeOperationInvertor = NULL;
QueueHandle_t ErrorModeOperationBms = NULL;
QueueHandle_t ErrorModeOperationAccelerator = NULL;
QueueHandle_t ErrorModeOperationBrake = NULL;
QueueHandle_t ErrorModeOperationJoystick = NULL;
QueueHandle_t ErrorModeOperationDcdc = NULL;

void vCanMessageLostCheckHandler(void *pvParameters);
void vLostComponentSendExternal(void *pvParameters);

typedef bool (*identifyLostComponent_t)(EventBits_t value);

typedef void (*LostComponentHandler_t)(VcuStateMangement_t *vcuStatus);
static causingOfError_t  causingOfError = EVERY_COMPONENT_IS_PRESENTED;


static canMessage_t causingOfLost=
{
   .id = 0x01,
   .dlc = 8,
   .ide = CAN_Id_Standard
};

static canMessageLost_t canMessageLoses =
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

static void logError(causingOfError_t cause)
{

}
static void invertorLostHandler(VcuStateMangement_t *vcuStatus)
{
    causingOfError = INVERTOR_CANMESSAGE_LOST;
    logError(causingOfError);
    if(*vcuStatus == VCU_Status_Parking || *vcuStatus == VCU_Status_Neutral || *vcuStatus == VCU_Status_Neutral)
    {
        *vcuStatus = VCU_Status_ErrorStop;
    }

    else
    {
        *vcuStatus = VCU_Status_ErrorStop;
    }
    xQueueOverwrite(ErrorModeOperationInvertor, &vcuStatus);
}
static void bmsLostHandler(VcuStateMangement_t *vcuStatus)
{
    causingOfError = BMS_CANMESSAGE_LOST;
    logError(causingOfError);
    if(*vcuStatus == VCU_Status_Parking || *vcuStatus == VCU_Status_Neutral || *vcuStatus == VCU_Status_Neutral)
    {
        *vcuStatus = VCU_Status_ErrorBatteryOff;
    }
    else
    {
        *vcuStatus = VCU_Status_ErrorBatteryOff;
    }
    xQueueOverwrite(ErrorModeOperationBms, &vcuStatus);

}
static void acceleratorLostHandler(VcuStateMangement_t *vcuStatus)
{
    causingOfError = ACCELERATOR_CANMESSAGE_LOST;
    logError(causingOfError);
    if(*vcuStatus == VCU_Status_Parking || *vcuStatus == VCU_Status_Neutral || *vcuStatus == VCU_Status_Neutral)
    {
        *vcuStatus = VCU_Status_ErrorStop;
    }
    else
    {
        *vcuStatus = VCU_Status_ErrorDrive;
    }
    xQueueOverwrite(ErrorModeOperationAccelerator, &vcuStatus);
}
static void brakeLostHandler(VcuStateMangement_t *vcuStatus)
{
    causingOfError = BRAKE_CANMESSAGE_LOST;
    logError(causingOfError);
    if(*vcuStatus == VCU_Status_Parking || *vcuStatus == VCU_Status_Neutral || *vcuStatus == VCU_Status_Neutral)
    {
        *vcuStatus = VCU_Status_ErrorStop;
    }
    else
    {
        *vcuStatus = VCU_Status_ErrorDrive;
    }
    xQueueOverwrite(ErrorModeOperationBrake, &vcuStatus);

}
static void joystickLostHandler(VcuStateMangement_t *vcuStatus)
{
    causingOfError = JOYSTICK_CANMESSAGE_LOST;
    logError(causingOfError);
    if(*vcuStatus == VCU_Status_Parking || *vcuStatus == VCU_Status_Neutral || *vcuStatus == VCU_Status_Neutral)
    {
        *vcuStatus = VCU_Status_ErrorStop;
    }
    else
    {
        *vcuStatus = VCU_Status_ErrorDrive;
    }
    xQueueOverwrite(ErrorModeOperationJoystick, &vcuStatus);

}
static void DcdcLostHandler(VcuStateMangement_t *vcuStatus)
{
    causingOfError = DCDC_CANMESSAGE_LOST;
    logError(causingOfError);
    if(*vcuStatus == VCU_Status_Parking || *vcuStatus == VCU_Status_Neutral || *vcuStatus == VCU_Status_Neutral)
    {
        *vcuStatus = VCU_Status_ErrorStop;
    }
    else
    {
        *vcuStatus = VCU_Status_ErrorDrive;
    }
    xQueueOverwrite(ErrorModeOperationDcdc, &vcuStatus);

}

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
    }
    if(xTaskCreate(vLostComponentSendExternal, "LostComponentSendExternal", configMINIMAL_STACK_SIZE, (void *)LOST_PERIOD_CAN_SEND, 1, &xLostComponentSendExternal) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }

  /*  for(int i = 0 ; i < COUNT_OF_COMPONENTS ; i++)
    {
        canMessageLossesQueue.arr[i] =  xQueueCreate(1U, sizeof(bool));

    }*/
    ErrorModeOperationInvertor    =  xQueueCreate(1U, sizeof(VcuStateMangement_t));
    ErrorModeOperationBms         =  xQueueCreate(1U, sizeof(VcuStateMangement_t));
    ErrorModeOperationAccelerator =  xQueueCreate(1U, sizeof(VcuStateMangement_t));
    ErrorModeOperationBrake       =  xQueueCreate(1U, sizeof(VcuStateMangement_t));
    ErrorModeOperationJoystick    =  xQueueCreate(1U, sizeof(VcuStateMangement_t));
    ErrorModeOperationDcdc        =  xQueueCreate(1U, sizeof(VcuStateMangement_t));

}

void vCanMessageLostCheckHandler(void *pvParameters)
{
    VcuStateMangement_t vcuStatus;

    const EventBits_t numberOfLost = 0x7F;
    EventBits_t  getLost = 0;

    for(;;)
    {
        getLost = xEventGroupWaitBits(canMessageLostCheckEventGroup, numberOfLost, pdFALSE, pdFALSE, portMAX_DELAY);
        xEventGroupClearBits(canMessageLostCheckEventGroup, MASK(6U));

        xQueuePeek(xQueueVcuStatus, &vcuStatus, pdMS_TO_TICKS(0));

        for(int i = 0 ; i < COUNT_OF_COMPONENTS; i++)
        {
            canMessageLoses.arr[i]  = identifyLostComponent[i](getLost);
            if(canMessageLoses.arr[i])
            {
                LostComponentHandler[i](&vcuStatus);
            }
        }
        setLostComponents(&causingOfLost, getLost);
    }
}

void vLostComponentSendExternal(void *pvParameters)
{
    TickType_t lastWeakTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS( (uint32_t) pvParameters );
    lastWeakTime = xTaskGetTickCount();

    for(;;)
    {
        xEventGroupSetBits(canMessageLostCheckEventGroup, MASK(6U));
        newCanTransmit(canREG1, canMESSAGE_BOX7, &causingOfLost);
        vTaskDelayUntil( &lastWeakTime, transmitPeriod);
    }
}
