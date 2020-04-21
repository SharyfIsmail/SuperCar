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
#define QUEUE_JOYSTICK_RAWSTATUS_LENGTH         (1U)

#define COMBINED_LENGTH ( QUEUE_LOST_COMPONENT_RAWSTATUS_LENGTH + \
                          QUEUE_SEMICRON_RAWSTATUS_LENGTH + \
                          QUEUE_JOYSTICK_RAWSTATUS_LENGTH )

TaskHandle_t xVcuStateManagement;

QueueHandle_t xQueueVcuStatusManagement = NULL;
QueueHandle_t xQueueLostComponentRawStatus = NULL;
QueueHandle_t xQueueSemicronRawStatus = NULL;
QueueHandle_t xQueueJoystickRawStatus = NULL;
//QueueHandle_t xQueue

QueueHandle_t xQueueVcuStatus = NULL;
static QueueSetMemberHandle_t activatedMember;

static QueueSetHandle_t xqueueSetvcuRawStatuses;

static void setCurrentVcuStatus(VcuStateMangement_t LostComponents, VcuStateMangement_t Semicron, VcuStateMangement_t joystick, VcuStateMangement_t battery);
static VcuStateMangement_t setVcuRawStatusLostComponents(VcuStateMangement_t currentStatus, lostComponentsStatus_t LostComponentsStatus);
static VcuStateMangement_t setVcuRawStatusSemicron(VcuStateMangement_t currentStatus, SemicronStatus_t SemicronStatus);

static VcuRawStatuses_t vcuRawStatuses = { NO_CRASH_lOST_MESSAGE,
                                           NO_CRASH_SEMICRON,
                                           JOYSTICK_NEUTRAL,
                                           BATTERY_OFF};
VcuStateMangement_t currentVcuStatus = VCU_Status_Init;

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
    xQueueJoystickRawStatus = xQueueCreate(QUEUE_JOYSTICK_RAWSTATUS_LENGTH, sizeof(VcuStateMangement_t));

    xQueueAddToSet(xQueueLostComponentRawStatus, xqueueSetvcuRawStatuses);
    xQueueAddToSet(xQueueSemicronRawStatus, xqueueSetvcuRawStatuses);
    xQueueAddToSet(xQueueJoystickRawStatus, xqueueSetvcuRawStatuses);

    xQueueVcuStatus = xQueueCreate(1U, sizeof(VcuStateMangement_t));
}

void vVcuStateManagement(void *pvParameters)
{

     VcuStateMangement_t rawVcuStatusLostComponents = VCU_Status_Init;
     VcuStateMangement_t rawVcuStatusSemicron = VCU_Status_Init;
     VcuStateMangement_t rawVcuStatusJoyStick = VCU_Status_Init;
     VcuStateMangement_t rawVcuStatusBattery  =  VCU_Status_Init;
     //VcuRawStatuses_t  VcuRawStatuses ;

    for(;;)
    {
        activatedMember = xQueueSelectFromSet(xqueueSetvcuRawStatuses, pdMS_TO_TICKS(200));

        if(activatedMember == xQueueLostComponentRawStatus)
        {
            xQueueReceive(xQueueLostComponentRawStatus, &vcuRawStatuses.lostComponentsStatus, 0);
            rawVcuStatusLostComponents = setVcuRawStatusLostComponents(currentVcuStatus, vcuRawStatuses.lostComponentsStatus);

        }
        else if (activatedMember == xQueueSemicronRawStatus)
        {
            xQueueReceive(xQueueSemicronRawStatus, &vcuRawStatuses.SemicronStatus, 0);
            rawVcuStatusSemicron =  setVcuRawStatusSemicron(currentVcuStatus, vcuRawStatuses.SemicronStatus);
        }
        else if (activatedMember == xQueueJoystickRawStatus)
        {

        }
        else if (activatedMember == xQueueJoystickRawStatus)
        {

        }
        else
        {
            /* The 200ms block time expired without an RTOS queue
                        being ready to process. */
        }

        setCurrentVcuStatus(rawVcuStatusLostComponents, rawVcuStatusSemicron, rawVcuStatusJoyStick, rawVcuStatusBattery);
        xQueueOverwrite(xQueueVcuStatus, &currentVcuStatus);
    }
}
static VcuStateMangement_t setVcuRawStatusLostComponents(VcuStateMangement_t currentStatus, lostComponentsStatus_t LostComponentsStatus)
{
    switch(LostComponentsStatus)
    {
    case NO_CRASH_lOST_MESSAGE:
        currentStatus = VCU_Status_Init;
        break;
    case CRASH_LEVEL_ERRORDRIVE:
        if(currentStatus == VCU_STATUS_STOP || currentStatus == VCU_Status_Init)
        {
            currentStatus = VCU_Status_ErrorStop;
        }
        else
        {
            currentStatus = VCU_Status_ErrorDrive;
        }
        break;
    case CRASH_LEVEL_ERRORSTOP:
        currentStatus = VCU_Status_ErrorStop;
        break;
    }
    return currentStatus;
}
static VcuStateMangement_t setVcuRawStatusSemicron(VcuStateMangement_t currentStatus, SemicronStatus_t SemicronStatus)
{
    switch(SemicronStatus)
    {
    case NO_CRASH_SEMICRON:
        currentStatus = VCU_Status_Init;
        break;
    case CLEAR_ERROR_SEMICRON:
        if(currentStatus == VCU_STATUS_STOP || currentStatus == VCU_Status_Init)
        {
            currentStatus = VCU_CLEAR_ERROR;
        }
        else
        {
           currentStatus = VCU_Status_ErrorDrive;
        }
        break;
    }
    return currentStatus;
}
//static VcuStateMangement_t setVcuRawStatusJoyStick(VcuStateMangement_t currentVcuStatus, VcuRawStatusLostComponents_t LostComponentsStatus)
//{

//}
//static VcuStateMangement_t setVcuRawStatusBattery(VcuStateMangement_t currentVcuStatus, VcuRawStatusLostComponents_t LostComponentsStatus)
//{

//}
static void setCurrentVcuStatus(VcuStateMangement_t LostComponents, VcuStateMangement_t Semicron, VcuStateMangement_t joystick, VcuStateMangement_t battery)
{
    if(LostComponents == VCU_Status_Init && Semicron == VCU_Status_Init &&
       battery == VCU_Status_Init && (joystick == VCU_STATUS_DRIVE || joystick == VCU_STATUS_STOP))
    {
        currentVcuStatus = VCU_Status_Init;
    }
    else if (LostComponents ==  VCU_Status_Init && Semicron == VCU_Status_Init &&
             battery == VCU_BATTERY_ON &&  joystick == VCU_STATUS_DRIVE)
    {
        currentVcuStatus = VCU_STATUS_DRIVE;
    }
    else if (LostComponents == VCU_Status_Init && Semicron == VCU_Status_Init &&
             battery == VCU_BATTERY_ON && joystick == VCU_STATUS_STOP)
    {
    currentVcuStatus = VCU_STATUS_STOP;
    }
    else if (LostComponents == VCU_Status_Init && Semicron == VCU_Status_Init &&
             battery == VCU_Status_Sleep && (joystick == VCU_STATUS_STOP || joystick == VCU_STATUS_DRIVE))
    {
    currentVcuStatus = VCU_Status_Sleep;
    }
//    switch(rawVcuStatus)
//    {
//    case VCU_EVERY_COMPONENT_IS_PRESENTED:
//
//        break;
//    case NO_ERROR_ON_INVERTOR:
//            break;
//    case VCU_Status_Init:
//        if(currentVcuStatus == VCU_Status_ErrorStop)
//        {
//            currentVcuStatus = VCU_Status_Init;
//        }/* else not needed */
//        break;
//
//    case VCU_CLEAR_ERROR:
//        if(currentVcuStatus == VCU_Status_Neutral || currentVcuStatus == VCU_Status_Parking)
//        {
//            currentVcuStatus = VCU_CLEAR_ERROR;
//        }
//        else
//        {
//            currentVcuStatus = VCU_Status_ErrorStop;
//        }
//        break;
//
//    case VCU_Status_Parking:
//        if(currentVcuStatus != VCU_Status_ErrorStop )
//        {
//            currentVcuStatus = VCU_Status_Parking;
//        }/* else not needed */
//        break;
//
//    case VCU_Status_Neutral:
//        if(currentVcuStatus != VCU_Status_Parking && currentVcuStatus != VCU_Status_ErrorStop)
//        {
//            currentVcuStatus = VCU_Status_Neutral;
//        }/* else not needed */
//        break;
//
//    case VCU_Status_Forward:
//        if(currentVcuStatus != VCU_Status_Parking && currentVcuStatus != VCU_Status_ErrorStop)
//        {
//            currentVcuStatus = VCU_Status_Forward;
//        }/* else not needed */
//        break;
//
//    case VCU_Status_Reverse:
//        if(currentVcuStatus != VCU_Status_Parking && currentVcuStatus != VCU_Status_ErrorStop)
//        {
//            currentVcuStatus = VCU_Status_Reverse;
//        }
//        break;
//
//    case VCU_Status_Charge:
//        if(currentVcuStatus == VCU_Status_Parking)
//        {
//            currentVcuStatus = VCU_Status_Charge;
//        }/* else not needed */
//        break;
//
//    case VCU_Status_Sleep:
//        break;
//
//    case VCU_Status_ErrorDrive:
//        if(currentVcuStatus != VCU_Status_ErrorStop && currentVcuStatus != VCU_Status_ErrorBatteryOff)
//        {
//            currentVcuStatus = VCU_Status_ErrorDrive;
//        }/* else not needed */
//        break;
//
//    case VCU_Status_ErrorStop:
//        if(currentVcuStatus != VCU_Status_ErrorBatteryOff)
//        {
//            currentVcuStatus = VCU_Status_ErrorStop;
//        }/* else not needed */
//        break;
//
//    case VCU_Status_ErrorBatteryOff:
//        currentVcuStatus = VCU_Status_ErrorBatteryOff;
//        break;
//    }
}
