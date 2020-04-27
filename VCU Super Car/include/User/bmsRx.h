/*
 * bmsRx.h
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_BMSRX_H_
#define INCLUDE_USER_BMSRX_H_

#include <stdint.h>
#include "newCanLib.h"

void BmsRxInit(void);

#define CAN_PERIOD_MS_BMS_CONTACTOR_REQUEST   ((uint32_t) 20)
#define BMS_CONTACTOR_REQUEST                 ((uint32_t) 0x800F3D0)
#define BMS_CONTACTOR_REQUEST_DLC             ((uint8_t) 3)


inline void setVcuBmsMessageCounter(canMessage_t* ptr, uint8_t value)
{
    ptr->data[1] = (ptr->data[1] & 0xF0) | (value & 0x0F);
}
inline uint8_t getVcuBmsMessageCounter(canMessage_t* ptr)
{
    return ptr->data[1] & 0x0F;
}

inline void increaseVcuBmsMessageCounter(canMessage_t* ptr, uint8_t value)
{
    setVcuBmsMessageCounter(ptr, getVcuBmsMessageCounter(ptr) + value);
}

inline void setVcuRequestModeBms(canMessage_t* ptr, uint8_t value)
{
    ptr->data[1] = (ptr->data[1] & 0x0F) | ((value & 0x0F) << 4);
}

inline void setVcuIsolationMeasSwitchOff(canMessage_t* ptr, uint8_t value)
{
    ptr->data[2] =  (ptr->data[2] & 0xFE) | (value & 0x01);
}

#endif /* INCLUDE_USER_BMSRX_H_ */
