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

#define IP_COUNT_OF_PERIPHS  6

/*typedef enum
{
    EVERY_COMPONENT_IS_PRESENTED=  0x00,
    INVERTOR_CANMESSAGE_LOST    =  0x01,
    BMS_CANMESSAGE_LOST         =  0x02,
    ACCELERATOR_CANMESSAGE_LOST =  0x03,
    BRAKE_CANMESSAGE_LOST       =  0x04,
    JOYSTICK_CANMESSAGE_LOST    =  0x05,
    DCDC_CANMESSAGE_LOST        =  0x06,
};*/

typedef struct
{
    QueueHandle_t arr[IP_COUNT_OF_PERIPHS];
}canMessageLoses_t;

typedef struct
{
    bool arr[IP_COUNT_OF_PERIPHS];
}canMessageLost_t;

void canMessageLostCheckInit(void);

/*returns the 1-st bit*/
inline EventBits_t getInverterLost(EventBits_t value)
{
    return (EventBits_t) (value & 0x01);
}

/*returns the 2-nd bit*/
inline EventBits_t getBmsLost(EventBits_t value)
{
    return (EventBits_t) ((value & 0x02) >> 1);
}

/*returns the 3-d bit*/
inline EventBits_t getAcceleratorLost(EventBits_t value)
{
    return (EventBits_t) ((value & 0x04) >> 2);
}

/*returns the 4-th bit*/
inline EventBits_t getBrakeLost(EventBits_t value)
{
    return (EventBits_t) ((value & 0x08) >> 3);
}

/*returns the 5-th bit*/
inline EventBits_t getJoystickLost(EventBits_t value)
{
    return (EventBits_t) ((value & 0x10) >> 4);
}

/*returns the 6-th bit*/
inline EventBits_t getDcLost(EventBits_t value)
{
    return (EventBits_t) ((value & 0x20) >> 5);
}
#endif /* INCLUDE_USER_CANMESSAGELOSTCHECK_H_ */
