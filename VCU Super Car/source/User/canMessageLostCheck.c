/*
 * canMessageLostCheck.c
 *
 *  Created on: 12 мар. 2020 г.
 *      Author: User
 */

#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"
#include "sys_main.h"
#include "newCanLib.h"
#include "canMessageLostCheck.h"
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

typedef void (*LostComponentHandler_t)(Rx_PDO_03ControlMode_t *controlMode);
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

static Rx_PDO_03ControlMode_t controlMode;


static void logError(causingOfError_t cause)
{

}
static void invertorLostHandler(Rx_PDO_03ControlMode_t *controlMode)
{
    errorModeOperation_t errorModeOperation ;
    causingOfError = INVERTOR_CANMESSAGE_LOST;
    logError(causingOfError);
    if(*controlMode == DISABLED)
    {
        errorModeOperation = VCU_Status_ErrorStop;
    }
    else
    {
        errorModeOperation = VCU_Status_ErrorStop;
    }
    xQueueOverwrite(ErrorModeOperationInvertor, &errorModeOperation);
}
static void bmsLostHandler(Rx_PDO_03ControlMode_t *controlMode)
{
    errorModeOperation_t errorModeOperation ;
    causingOfError = BMS_CANMESSAGE_LOST;
    logError(causingOfError);
    if(*controlMode == DISABLED)
    {
        errorModeOperation = VCU_Status_ErrorBatteryOff;
    }
    else
    {
        errorModeOperation = VCU_Status_ErrorBatteryOff;
    }
    xQueueOverwrite(ErrorModeOperationBms, &errorModeOperation);

}
static void acceleratorLostHandler(Rx_PDO_03ControlMode_t *controlMode)
{
    errorModeOperation_t errorModeOperation ;
    causingOfError = ACCELERATOR_CANMESSAGE_LOST;
    logError(causingOfError);
    if(*controlMode == DISABLED)
    {
        errorModeOperation = VCU_Status_ErrorStop;
    }
    else
    {
        errorModeOperation = VCU_Status_ErrorDrive;
    }
    xQueueOverwrite(ErrorModeOperationAccelerator, &errorModeOperation);
}
static void brakeLostHandler(Rx_PDO_03ControlMode_t *controlMode)
{
    errorModeOperation_t errorModeOperation ;
    causingOfError = BRAKE_CANMESSAGE_LOST;
    logError(causingOfError);
    if(*controlMode == DISABLED)
    {
        errorModeOperation = VCU_Status_ErrorStop;
    }
    else
    {
        errorModeOperation = VCU_Status_ErrorDrive;
    }
    xQueueOverwrite(ErrorModeOperationBrake, &errorModeOperation);

}
static void joystickLostHandler(Rx_PDO_03ControlMode_t *controlMode)
{
    errorModeOperation_t errorModeOperation ;
    causingOfError = JOYSTICK_CANMESSAGE_LOST;
    logError(causingOfError);
    if(*controlMode == DISABLED)
    {
        errorModeOperation = VCU_Status_ErrorStop;
    }
    else
    {
        errorModeOperation = VCU_Status_ErrorDrive;
    }
    xQueueOverwrite(ErrorModeOperationJoystick, &errorModeOperation);

}
static void DcdcLostHandler(Rx_PDO_03ControlMode_t *controlMode)
{
    errorModeOperation_t errorModeOperation ;
    causingOfError = DCDC_CANMESSAGE_LOST;
    logError(causingOfError);
    if(*controlMode == DISABLED)
    {
        errorModeOperation = VCU_Status_ErrorStop;
    }
    else
    {
        errorModeOperation = VCU_Status_ErrorDrive;

    }
    xQueueOverwrite(ErrorModeOperationDcdc, &errorModeOperation);

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
    ErrorModeOperationInvertor    =  xQueueCreate(1U, sizeof(errorModeOperation_t));
    ErrorModeOperationBms         =  xQueueCreate(1U, sizeof(errorModeOperation_t));
    ErrorModeOperationAccelerator =  xQueueCreate(1U, sizeof(errorModeOperation_t));
    ErrorModeOperationBrake       =  xQueueCreate(1U, sizeof(errorModeOperation_t));
    ErrorModeOperationJoystick    =  xQueueCreate(1U, sizeof(errorModeOperation_t));
    ErrorModeOperationDcdc        =  xQueueCreate(1U, sizeof(errorModeOperation_t));

}

void vCanMessageLostCheckHandler(void *pvParameters)
{

    const EventBits_t numberOfLost = 0x3F;
    EventBits_t  getLost = 0;

    for(;;)
    {
        getLost = xEventGroupWaitBits(canMessageLostCheckEventGroup, numberOfLost, pdFALSE, pdFALSE, portMAX_DELAY);
        xQueuePeek(xQueueControllingMode, &controlMode, pdMS_TO_TICKS(0));

        for(int i = 0 ; i < COUNT_OF_COMPONENTS; i++)
        {
            canMessageLoses.arr[i]  = identifyLostComponent[i](getLost);
            if(canMessageLoses.arr[i])
            {
                LostComponentHandler[i](&controlMode);
             //   xQueueOverwrite()
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
        newCanTransmit(canREG1, canMESSAGE_BOX7, &causingOfLost);
        vTaskDelayUntil( &lastWeakTime, transmitPeriod);
    }
}
