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

static void setVcuRawStatus(causingOfError_t cause);
static void invertorLostHandler(void);
static void bmsLostHandler(void);
static void acceleratorLostHandler(void);
static void brakeLostHandler(void);
static void joystickLostHandler(void);
static void DcdcLostHandler(void);
static void LostComponentsBitSet(EventBits_t value);

typedef bool (*identifyLostComponent_t)(EventBits_t value);
typedef void (*LostComponentHandler_t)(void);

static causingOfError_t  causingOfError = EVERYTHING_IS_FINE;
//static VcuRawStatusLostComponents_t LostComponentsStatus = NO_CRASH_lOST_MESSAGE;

static canMessageLost_t canMessageLoses =
{
    .arr = {false}
};
static canMessageLost_t errorLogIsWrote =
{
 .arr = {false}
};
static lostComponentsStatus_t  lostComponentsStatus = NO_CRASH_lOST_MESSAGE;

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
    const EventBits_t numberOfLost = 0x7F;

    for(;;)
    {
        EventBits_t  getLost = xEventGroupWaitBits(canMessageLostCheckEventGroup, numberOfLost, pdFALSE, pdFALSE, portMAX_DELAY);
        xEventGroupClearBits(canMessageLostCheckEventGroup, MASK(6U));
      //  xQueuePeek(xQueueVcuStatus, &vcuStatus, pdMS_TO_TICKS(0));
        for(int i = 0 ; i < COUNT_OF_COMPONENTS; i++)
        {
            canMessageLoses.arr[i]  = identifyLostComponent[i](getLost);
            if(canMessageLoses.arr[i])
            {
                LostComponentHandler[i]();
                errorLogIsWrote.arr[i] = true;
            }
            else
                errorLogIsWrote.arr[i] = false;
        }
        LostComponentsBitSet(getLost);
        if(!(getLostComponenst(getLost)))
        {
            lostComponentsStatus = NO_CRASH_lOST_MESSAGE;
        }/* else not needed */
        xQueueOverwrite(xQueueLostComponentError, &lostComponentsStatus);
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
static void setVcuRawStatus(causingOfError_t cause)
{
    switch(cause)
    {
    case INVERTOR_CANMESSAGE_LOST:
        lostComponentsStatus= CRASH_LEVEL_ERRORSTOP;
        break;

    case BMS_CANMESSAGE_LOST:
        lostComponentsStatus = CRASH_LEVEL_ERRORSTOP;
        break;

    case ACCELERATOR_CANMESSAGE_LOST:
        if(lostComponentsStatus != CRASH_LEVEL_ERRORSTOP)
        {
           lostComponentsStatus =  CRASH_LEVEL_ERRORDRIVE;
        }
        break;

    case BRAKE_CANMESSAGE_LOST:
        if(lostComponentsStatus != CRASH_LEVEL_ERRORSTOP)
        {
           lostComponentsStatus =  CRASH_LEVEL_ERRORDRIVE;
        }
        break;

    case JOYSTICK_CANMESSAGE_LOST:
        if(lostComponentsStatus != CRASH_LEVEL_ERRORSTOP)
        {
           lostComponentsStatus = CRASH_LEVEL_ERRORDRIVE;
        }
        break;

    case DCDC_CANMESSAGE_LOST:
        if(lostComponentsStatus != CRASH_LEVEL_ERRORSTOP)
        {
           lostComponentsStatus = CRASH_LEVEL_ERRORDRIVE;
        }
        break;
    }
}
static void invertorLostHandler()
{
    causingOfError = INVERTOR_CANMESSAGE_LOST;
    if(!errorLogIsWrote.arr[0])
    {
        logError(causingOfError);
        setVcuRawStatus(causingOfError);
    }/* else not needed */
}
static void bmsLostHandler()
{
    causingOfError = BMS_CANMESSAGE_LOST;
    if(!errorLogIsWrote.arr[1])
    {
        logError(causingOfError);
        setVcuRawStatus(causingOfError);
    }/* else not needed */
}
static void acceleratorLostHandler()
{
    causingOfError = ACCELERATOR_CANMESSAGE_LOST;
    if(!errorLogIsWrote.arr[2])
    {
        logError(causingOfError);
        setVcuRawStatus(causingOfError);
    }/* else not needed */
}
static void brakeLostHandler()
{
    causingOfError = BRAKE_CANMESSAGE_LOST;
    if(!errorLogIsWrote.arr[3])
    {
        logError(causingOfError);
        setVcuRawStatus(causingOfError);
    }/* else not needed */
}
static void joystickLostHandler()
{
    causingOfError = JOYSTICK_CANMESSAGE_LOST;
    if(!errorLogIsWrote.arr[4])
    {
        logError(causingOfError);
        setVcuRawStatus(causingOfError);
    }/* else not needed */
}
static void DcdcLostHandler()
{
    causingOfError = DCDC_CANMESSAGE_LOST;
    if(!errorLogIsWrote.arr[5])
    {
        logError(causingOfError);
        setVcuRawStatus(causingOfError);
    }/* else not needed */
}
