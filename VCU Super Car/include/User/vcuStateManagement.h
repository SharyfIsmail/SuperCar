/*
 * vcuStateManagement.h
 *
 *  Created on: 19 мар. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_VCUSTATEMANAGEMENT_H_
#define INCLUDE_USER_VCUSTATEMANAGEMENT_H_

#include "FreeRTOS.h"
#include "queue.h"


extern QueueHandle_t xQueueVcuStatus;
//extern QueueHandle_t xQueueVcuStatusManagement;
extern QueueHandle_t xQueueLostComponentRawStatus;
extern QueueHandle_t xQueueSemicronRawStatus;
extern QueueHandle_t xQueueJoystickRawStatus;

void vcuStateManagementInit(void);

typedef enum
{
    VCU_Status_Init                          = 0x00,
    VCU_CLEAR_ERROR                          = 0x01,
    VCU_Status_Charge                        = 0x02,
    VCU_Status_Sleep                         = 0x03,
    VCU_BATTERY_OFF                          = 0x04,
    VCU_BATTERY_ON                           = 0x05,
    VCU_BATTERY_TURNING_ON                   = 0x06,

    //VCU_Status_Parking                       = 0x04,
    //VCU_Status_Neutral                       = 0x05,

    VCU_STATUS_DRIVE                         = 0x13,
    VCU_STATUS_STOP                          = 0x14,

    //VCU_Status_Forward                       = 0x06,
    //VCU_Status_Reverse                       = 0x07,

    VCU_Status_ErrorDrive                    = 0x08,
    //VCU_Status_ErrorDrive_FORWARD            = 0x09,
   // VCU_Status_ErrorDrive_REVERSE            = 0x10,

    VCU_Status_ErrorStop                     = 0x11,
    VCU_Status_ErrorBatteryOff               = 0x12,
}VcuStateMangement_t;

typedef enum
{
    NO_CRASH_lOST_MESSAGE,
    CRASH_LEVEL_ERRORDRIVE,
    CRASH_LEVEL_ERRORSTOP
}lostComponentsStatus_t;

typedef enum
{
    NO_CRASH_SEMICRON,
    CLEAR_ERROR_SEMICRON,
    ERROR_NOT_CLEANED
}SemicronStatus_t;

typedef enum
{
    JOYSTICK_NEUTRAL,
    JOYSTICK_FORWARD,
    JOYSTICK_REVERSE,
    JOYSTCK_PARKING
}VcuRawStatusJoystick_t;

typedef enum
{
    BATTERY_OFF,
    TURNING_BATTERY_OFF,
    TURNING_BATTERY_ON,
    BATTERY_ON,
    BATTERY_ERROR
}VcuRawStatusBattery_t;

typedef struct
{
    lostComponentsStatus_t lostComponentsStatus ;
    SemicronStatus_t SemicronStatus ;
    VcuRawStatusJoystick_t vcuRawStatusJoystick;
    VcuRawStatusBattery_t vcuRawStatusBattery   ;
} VcuRawStatuses_t;




#endif /* INCLUDE_USER_VCUSTATEMANAGEMENT_H_ */
