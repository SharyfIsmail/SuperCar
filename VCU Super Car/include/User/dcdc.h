/*
 * dcdc.h
 *
 *  Created on: 1 мая 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_DCDC_H_
#define INCLUDE_USER_DCDC_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "newCanLib.h"

void dcDcInit(void);

extern QueueHandle_t xQueueDcdc;

#define CAN_PERIOD_MS_VCU_DCDC   ((uint32_t) 100)
#define VCU_DCDC_ID              ((uint32_t) 0xC548FD0)
#define VCU_DCDC_DLC             ((uint8_t) 8)

#define DCDC_CURRENT_HIGH(phy) ((uint8_t) (phy * 10))
#define DCDC_CURRENT_LOW(phy)  ((uint16_t) (phy * 10))
#define DCDC_VOLT_LOW(phy)     ((uint16_t) (phy * 10))


inline void setVcuDcdcMessageCounter(canMessage_t* ptr, uint8_t value)
{
    ptr->data[1] = (ptr->data[1] & 0xF0) | (value & 0x0F);
}
inline uint8_t getVcuDcdcMessageCounter(canMessage_t* ptr)
{
    return ptr->data[1] & 0x0F;
}

inline void increaseVcuDcdcMessageCounter(canMessage_t* ptr, uint8_t value)
{
    setVcuDcdcMessageCounter(ptr, getVcuDcdcMessageCounter(ptr) + value);
}
inline void setDcdcMode(canMessage_t* ptr, uint8_t value)
{
    ptr->data[1] = (ptr->data[1] & 0x0F) | ((value & 0x0F) << 4);
}
inline void setDcdcVoltHigh(canMessage_t* ptr, uint16_t value)
{
    ptr->data[2] = (uint8_t)value;
    ptr->data[3] = (uint8_t)(value >> 8);
}

inline void setDcdcCurrentHigh(canMessage_t* ptr, uint8_t value)
{
  ptr->data[4] = value ;
}
inline void setDcdcCurrentLow(canMessage_t* ptr, uint16_t value)
{
    ptr->data[5] = (uint8_t)value;
    ptr->data[6] = (ptr->data[6] & 0xF0) | (uint8_t)(value >> 8);
}
inline void setDcdcVoltLOW(canMessage_t* ptr, uint16_t value)
{
    ptr->data[6] = (ptr->data[6] & 0x0F) | (uint8_t)(value << 4);
    ptr->data[7] = (uint8_t)(value >> 4);
}
#endif /* INCLUDE_USER_DCDC_H_ */
