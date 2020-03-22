/*
 * canMessageLostCheck.h
 *
 *  Created on: 12 мар. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_CANMESSAGELOSTCHECK_H_
#define INCLUDE_USER_CANMESSAGELOSTCHECK_H_

#include "FreeRTOS.h"
#include "os_event_groups.h"
#include "os_queue.h"

#include <stdint.h>

#define LOST_PERIOD_CAN_SEND   ((uint32_t) 5000)
enum componentIndex
{
    INVERTOR_INDEX    = 0,
    BMS_INDEX         = 1,
    ACCELERATOR_INDEX = 2,
    BRAKE_INDEX       = 3,
    JOYSTICK_INDEX    = 4,
    DCDC_INDEX        = 5,
    COUNT_OF_COMPONENTS
};

typedef enum
{
    EVERY_COMPONENT_IS_PRESENTED=  0x00,
    INVERTOR_CANMESSAGE_LOST    =  0x01,
    BMS_CANMESSAGE_LOST         =  0x02,
    ACCELERATOR_CANMESSAGE_LOST =  0x03,
    BRAKE_CANMESSAGE_LOST       =  0x04,
    JOYSTICK_CANMESSAGE_LOST    =  0x05,
    DCDC_CANMESSAGE_LOST        =  0x06,
}causingOfError_t;

/*typedef enum
{
    VCU_Status_Init            = 0x00,
    VCU_Status_Parking         = 0x01,
    VCU_Status_Neutral         = 0x10,
    VCU_Status_Forward         = 0x11,
    VCU_Status_Reverse         = 0x12,
    VCU_Status_Charge          = 0x02,
    VCU_Status_Sleep           = 0x03,
    VCU_Status_ErrorDrive      = 0x20,
    VCU_Status_ErrorStop       = 0x21,
    VCU_Status_ErrorBatteryOff = 0x22,
}VcuModeOperation_t;
*/
/*typedef struct
{
    QueueHandle_t arr[COUNT_OF_COMPONENTS];
}canMessage_t;
*/
typedef struct
{
    bool arr[COUNT_OF_COMPONENTS];
}canMessageLost_t;

void canMessageLostCheckInit(void);

/*returns the 1-st bit*/
inline bool getInverterLost(EventBits_t value)
{
    return (bool) (value & 0x01);
}

/*returns the 2-nd bit*/
inline bool getBmsLost(EventBits_t value)
{
    return (bool) ((value & 0x02) >> 1);
}

/*returns the 3-d bit*/
inline bool getAcceleratorLost(EventBits_t value)
{
    return (bool) ((value & 0x04) >> 2);
}

/*returns the 4-th bit*/
inline bool getBrakeLost(EventBits_t value)
{
    return (bool) ((value & 0x08) >> 3);
}

/*returns the 5-th bit*/
inline bool getJoystickLost(EventBits_t value)
{
    return (bool) ((value & 0x10) >> 4);
}

/*returns the 6-th bit*/
inline bool getDcLost(EventBits_t value)
{
    return (bool) ((value & 0x20) >> 5);
}

inline void setLostComponents(canMessage_t* ptr ,EventBits_t value)
{
     ptr->data[0] = (ptr->data[0] & 0x00) | (uint8_t) (value & 0x3F);
}
inline uint8_t getLostComponenst(canMessage_t* ptr)
{
    return (uint8_t) (ptr->data[0] & 0x3F);
}
#endif /* INCLUDE_USER_CANMESSAGELOSTCHECK_H_ */
