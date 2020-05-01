/*
 * vcuAll.h
 *
 *  Created on: 1 мая 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_VCUALL_H_
#define INCLUDE_USER_VCUALL_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "newCanLib.h"

void VcuAllInit(void);
#define CAN_PERIOD_MS_VCU_ALL01   ((uint32_t) 100)
#define VCU_ALL01_ID              ((uint32_t) 0x1851A0D0)
#define VCU_ALL01_DLC             ((uint8_t) 8)
#define VCU_ALL_01_SPEED(phy)     ((uint16_t) (phy / 60))

inline void setVcuAll01MessageCounter(canMessage_t* ptr, uint8_t value)
{
    ptr->data[1] = (ptr->data[1] & 0xF0) | (value & 0x0F);
}
inline uint8_t getVcuAll01MessageCounter(canMessage_t* ptr)
{
    return ptr->data[1] & 0x0F;
}
inline void increaseVcuAll01MessageCounter(canMessage_t* ptr, uint8_t value)
{
    setVcuAll01MessageCounter(ptr, getVcuAll01MessageCounter(ptr) + value);
}

inline void setVcuAll01tMode(canMessage_t* ptr, uint8_t value)
{
    ptr->data[1] = (ptr->data[1] & 0x0F) | ((value & 0x0F) << 4);
}
inline void setVcuAll01Time(canMessage_t* ptr, uint32_t value)
{
    ptr->data[2] = (uint8_t) value;
    ptr->data[3] = (uint8_t) (value >> 8);
    ptr->data[4] = (uint8_t) (value >> 16);
    ptr->data[5] = (uint8_t) (value >> 24);
}
inline void setVcuAll01Error(canMessage_t* ptr, uint8_t value)
{
    ptr->data[6] = (ptr->data[6] & 0xF0) | (value & 0x0F) ;
}

inline void setVcuAll01ContactorMode(canMessage_t* ptr, uint8_t value)
{
    ptr->data[6] = (ptr->data[6] & 0x0F) | ((value & 0x0F) << 4) ;
}
inline void setVcuAll01Speed(canMessage_t* ptr, uint8_t value)
{
    ptr->data[7] = value;
}

#define CAN_PERIOD_MS_VCU_ALL02   ((uint32_t) 1000)
#define VCU_ALL02_ID              ((uint32_t) 0x1852A0D0)
#define VCU_ALL02_DLC             ((uint8_t) 5)

inline void setVcuAll02MessageCounter(canMessage_t* ptr, uint8_t value)
{
    ptr->data[1] = (ptr->data[1] & 0xF0) | (value & 0x0F);
}
inline uint8_t getVcuAll02MessageCounter(canMessage_t* ptr)
{
    return ptr->data[1] & 0x0F;
}
inline void increaseVcuAll02MessageCounter(canMessage_t* ptr, uint8_t value)
{
    setVcuAll02MessageCounter(ptr, getVcuAll02MessageCounter(ptr) + value);
}
inline void setVcuAll02Volt(canMessage_t* ptr, uint16_t value)
{
    ptr->data[1] = (ptr->data[1] & 0x0F) | (uint8_t)(value << 4);
    ptr->data[2] =  (uint8_t)(value >> 4);
}
inline void setVcuAll02Temp01(canMessage_t* ptr, uint8_t value)
{
    ptr->data[3] =  value;
}
inline void setVcuAll02Temp02(canMessage_t* ptr, uint8_t value)
{
    ptr->data[4] =  value;
}
#endif /* INCLUDE_USER_VCUALL_H_ */
