/*
 * canMessageLostCheck.h
 *
 *  Created on: 12 мар. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_CANMESSAGELOSTCHECK_H_
#define INCLUDE_USER_CANMESSAGELOSTCHECK_H_

#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"

#include <stdint.h>

#define LOST_COMPONENT_WATCHER_PERIOD   ((uint32_t) 100)
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

inline void setLostComponents(uint8_t *ptr ,EventBits_t value)
{
     *ptr = (*ptr & 0x00) | (uint8_t) (value & 0x3F);
}
inline uint8_t getLostComponenst(EventBits_t value)
{
    return (uint8_t) (value & 0x3F);
}
#endif /* INCLUDE_USER_CANMESSAGELOSTCHECK_H_ */
