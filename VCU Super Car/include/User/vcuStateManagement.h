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
    VCU_Status_Init      = 0x00,
    VCU_Status_PARKING   = 0x01,
    VCU_Status_NEUTRAL   = 0x02,
    VCU_Status_FORWARD   = 0x03,
    VCU_Status_REVERCE   = 0x04,
    VCU_Status_CHARGING  = 0x05,
    VCU_Status_SLEEP     = 0x06,
    VCU_Status_UNDEFINED = 0x07
}VcuStateMangement_t;

typedef enum
{
    VCU_NO_ERROR        = 0x00,
    VCU_ERROR_WORK      = 0x01,
    VCU_ERROR_STOP      = 0x02,
    VCU_ERROR_UNDEFINED = 0x03
}VcuErrorStatus_t;

typedef struct
{
    VcuStateMangement_t vcuStateMangement;
    VcuErrorStatus_t    errorStatus;

}VcuStatusStruct_t;
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
    JOYSTICK_INIT ,
    JOYSTCK_PARKING,
    JOYSTICK_NEUTRAL,
    JOYSTICK_FORWARD,
    JOYSTICK_REVERSE,
    JOYSTICK_UNDEFINED
}VcuRawStatusJoystick_t;

typedef enum
{
    BATTERY_INIT,
    BATTERY_HV_ACTIVE,
    BATTERY_CHARGING,
    BATTERY_NORMAL_OFF,
    BATTERY_EMEGENCY_OFF
}VcuRawStatusBattery_t;

typedef struct
{
    lostComponentsStatus_t lostComponentsStatus ;
    SemicronStatus_t SemicronStatus ;
    VcuRawStatusJoystick_t vcuRawStatusJoystick;
    VcuRawStatusBattery_t vcuRawStatusBattery   ;
} VcuRawStatuses_t;




#endif /* INCLUDE_USER_VCUSTATEMANAGEMENT_H_ */
