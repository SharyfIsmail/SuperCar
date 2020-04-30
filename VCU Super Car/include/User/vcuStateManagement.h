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
#include "bms.h"
#include "acceleratorBrakeJoystick.h"



//extern QueueHandle_t xQueueVcuStatus;
//extern QueueHandle_t xQueueVcuStatusManagement;
extern QueueHandle_t xQueueLostComponentError;
extern QueueHandle_t xQueueSemicronError;
extern QueueHandle_t xQueueSelectorMode;
extern QueueHandle_t xQueueBatteryMode ;

extern QueueHandle_t xQueueSemicronStart;

void vcuStateManagementInit(void);

typedef enum
{
    VCU_STATUS_INIT          = 0x00,
    VCU_STATUS_PARKING       = 0x01,
    VCU_STATUS_NEUTRAL       = 0x02,
    VCU_STATUS_FORWARD       = 0x03,
    VCU_STATUS_REVERCE       = 0x04,
    VCU_STATUS_CHARGING      = 0x05,
    VCU_STATUS_SLEEP         = 0x06,
    VCU_STATUS_UNDEFINED = 0x07
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

typedef struct
{
    lostComponentsStatus_t lostComponentsStatus ;
    SemicronStatus_t SemicronStatus ;
    SelectorStructModeTx_t selectorModeStruct;
    bmsMode_State_t bmsModeState;
}taskStatuses_t;

const VcuStatusStruct_t* getVcuStatusStruct();
#endif /* INCLUDE_USER_VCUSTATEMANAGEMENT_H_ */
