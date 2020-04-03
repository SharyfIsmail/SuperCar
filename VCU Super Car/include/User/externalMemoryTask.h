/*
 * externalMemoryTask.h
 *
 *  Created on: 23 мар. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_EXTERNALMEMORYTASK_H_
#define INCLUDE_USER_EXTERNALMEMORYTASK_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

extern QueueHandle_t xQueueCommandToExtMemory;

void externalMemoryTaskInit();


typedef enum
{
    EVERYTHING_IS_FINE            =  0x00,

    SHORT_CIRCUIT_L1_TOP          =  0x01,
    SHORT_CIRCUIT_L1_BOT          =  0x02,
    SHORT_CIRCUIT_L2_TOP          =  0x03,
    SHORT_CIRCUIT_L2_BOT          =  0x04,
    SHORT_CIRCUIT_L3_TOP          =  0x05,
    SHORT_CIRCUIT_L3_BOT          =  0x06,
    UNDERVOLTAGE_L1_TOP           =  0x07,
    UNDERVOLTAGE_L1_BOT           =  0x08,
    UNDERVOLTAGE_L2_TOP           =  0x09,
    UNDERVOLTAGE_L2_BOT           =  0x0A,
    UNDERVOLTAGE_L3_TOP           =  0x0B,
    UNDERVOLTAGE_L3_BOT           =  0x0C,
    OVERCURRENT_PHASE_L1_POS      =  0x0D,
    OVERCURRENT_PHASE_L1_NEG      =  0x0E,
    OVERCURRENT_PHASE_L2_POS      =  0x0F,
    OVERCURRENT_PHASE_L2_NEG      =  0x10,
    OVERCURRENT_PHASE_L3_POS      =  0x11,
    OVERCURRENT_PHASE_L3_NEG      =  0x12,
    DC_LINK_OVERVOLTAGE           =  0x13,
    SUPPLY_UNDERVOLTAGE           =  0x14,
    DCB_OVERTEMP_L1_HV            =  0x15,
    DCB_OVERTEMP_L2_HV            =  0x16,
    DCB_OVERTEMP_L3_HV            =  0x17,
    PCB_OVERTEMPERATURE_HV        =  0x18,
    EXTERNAL_TEMP_SESNOR_ERROR    =  0x19,
    HVIL_MISSING                  =  0x27,
    DC_LINK_UNDERVOLTAGE          =  0x2B,
    DC_LINK_OVERVOLATGE           =  0x2C,
    PCB_OVERTEMPERATURE_SW        =  0x30,
    MOTOR_OVERTEMPERATURE         =  0x32,
    MOTOR_OVERSPEED               =  0x33,
    CAN_A_NODE_GUARD_EXPIRED      =  0x36,
    CAN_BUS_A_OFF                 =  0x37,
    WATCHDOG_OCCURRED             =  0x49,
    INITIALIZATION_FAILED         =  0x4F,
    FATAL_ERROR                   =  0x65,
    INVERTER_INIT_FAILED          =  0x67,
    MAX_CURRENT                   =  0x7F,
    JUNCTION_OVERTEMPERATURE      =  0x9F,
    UNDERVOLTAGE_SUPPLY_INPUT     =  0xA1,
    UNEXPCTED_SHUTDOWN            =  0xE1,
    DC_LINK_VOLTAGE_SENSOR_ERROR  =  0xE8,

    INVERTOR_CANMESSAGE_LOST      =  0xEF,
    BMS_CANMESSAGE_LOST           =  0xF0,
    ACCELERATOR_CANMESSAGE_LOST   =  0xF1,
    BRAKE_CANMESSAGE_LOST         =  0xF2,
    JOYSTICK_CANMESSAGE_LOST      =  0xF3,
    DCDC_CANMESSAGE_LOST          =  0xF4,

    UNKNOWN_ERROR                 =  0xFE,

    ALL_ERRORS                    =  0xFF
}causingOfError_t;


typedef struct
{
    uint32_t time;
    causingOfError_t error;
    uint8_t crc;
}ErrorDataToExtMemory_t;
typedef enum
{
    CLEAR_EXTERNAL_MEMORY  = 1,
    EXT_MEMORY_SEND_BY_CAN = 2,
    EXT_MEMROY_WRITE       = 3,
}CommandToExtMemoryEnum_t;
typedef struct
{
    ErrorDataToExtMemory_t errorData;
    CommandToExtMemoryEnum_t type;
}CommandToExtMemory_t;


#endif /* INCLUDE_USER_EXTERNALMEMORYTASK_H_ */
