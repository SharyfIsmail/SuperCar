/*
 * acceleratorBrakeJoystick.h
 *
 *  Created on: 11 мар. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_ACCELERATORBRAKEJOYSTICK_H_
#define INCLUDE_USER_ACCELERATORBRAKEJOYSTICK_H_


#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "newCanLib.h"

extern QueueHandle_t xQueueABPeadlSelectorTx;
extern QueueHandle_t xqueueSelectorMode;
extern QueueHandle_t xqueueAcceleratorValue;
void acceleratorBrakeJoystickInit(void);

typedef enum
{
    SELECTOR_INIT ,
    SELECTOR_PARKING,
    SELECTOR_NEUTRAL,
    SELECTOR_FORWARD,
    SELECTOR_REVERSE,
    SELECTOR_UNDEFINED
}SelectorMode_t;

#define SELECTOR_TX      ((uint32_t) 0x800D028)
#define SLECETOR_TX_DLC  ((uint8_t)4)

typedef struct
{
    uint8_t data[4];
}selectorTx_t;


/*Checksum length : 8 start bit 0 :*/
inline uint8_t getSelectorVcuChecksum(selectorTx_t *ptr)
{
    return (uint8_t) (ptr->data[0]);
}

/*message Counter length : 4 start bit 8 :*/
inline uint8_t getSlectorVcuMessageCounter(selectorTx_t *ptr)
{
    return (uint8_t)(ptr->data[1] & 0x0F);
}
/*Selector locking state  length : 2 start bit 12*/
inline uint8_t getSelectorVcuLockingState(selectorTx_t *ptr)
{
    return (uint8_t)(ptr->data[1] & 0x30) >> 4 ;
}
/*Selector Init length : 2 start bit 14*/
inline uint8_t getSelectorVcuInit(selectorTx_t *ptr)
{
    return (uint8_t)(ptr->data[1] & 0xC0) >> 6 ;
}
/*Selector postiong length : 4 start bit 16*/
inline uint8_t getSelectorVcuPosition(selectorTx_t *ptr)
{
    return (uint8_t)(ptr->data[2] & 0x0F) ;
}
/*Selector Button state length :4 start bit 20*/
inline uint8_t getSelectorVcuButtonState(selectorTx_t *ptr)
{
    return (uint8_t)(ptr->data[2] & 0xF0)>>4 ;
}
/*Selector Current Mode length :4 start bit 24*/
inline uint8_t getSelectorVcuCurrentMode(selectorTx_t *ptr)
{
    return (uint8_t)(ptr->data[3] & 0x0F) ;
}
/*Selector Current Mode length :4 start bit 24*/
inline uint8_t getSelectorVcuModeRequest(selectorTx_t *ptr)
{
    return (uint8_t)(ptr->data[3] & 0xF0)>>4 ;
}

#define SELECTOR_RX               ((uint32_t) 0xC0028D0)
#define CAN_PERIOD_MS_SELECTOR_RX ((uint32_t) 100)
#define SLECETOR_RX_DLC             ((uint8_t)3)

typedef struct
{
    uint8_t data[3];
}selectorRx_t;

inline void setVcuSelectorMessageCounter(canMessage_t* ptr, uint8_t value)
{
    ptr->data[1] = (ptr->data[1] & 0xF0) | (value & 0x0F);
}

inline uint8_t getVcuSelectorMessageCounter(canMessage_t* ptr)
{
    return ptr->data[1] & 0x0F;
}

inline void increaseVcuSelectorMessageCounter(canMessage_t* ptr, uint8_t value)
{
    setVcuSelectorMessageCounter(ptr, getVcuSelectorMessageCounter(ptr) + value);
}

inline void setVcuSelectorLeverLocking(canMessage_t* ptr, uint8_t value)
{
    ptr->data[1] = (ptr->data[1] & 0xCF) | ((value & 0x03) << 4);
}

inline void setVcuSelectorIlluminationMode(canMessage_t* ptr, uint8_t value)
{
    ptr->data[1] = (ptr->data[1] & 0x3F) | ((value & 0x03) << 6);
}

inline void setVcuSelectorRequestedMode(canMessage_t* ptr, uint8_t value)
{
    ptr->data[2] = (ptr->data[2] & 0xF0) | (value & 0x0F);
}

inline void setVcuSelectorCurrentMode(canMessage_t* ptr, uint8_t value)
{
    ptr->data[2] = (ptr->data[2] & 0x0F) | ((value & 0x0F) << 4);
}

#define AB_PEDAL_TX     ((uint32_t) 0x800D025)
#define AB_PEDAL_TX_DLC ((uint8_t)5)

typedef struct
{
    uint8_t data[5];
}ABPedalTx_t;

/*Checksum length : 8 start bit 0 :*/
inline uint8_t getABPedalTxChecksum(ABPedalTx_t *ptr)
{
    return (uint8_t) (ptr->data[0]);
}

/*message Counter length : 4 start bit 8 :*/
inline uint8_t getABPedalTxMessageCounter(ABPedalTx_t *ptr)
{
    return (uint8_t)(ptr->data[1] & 0x0F);
}

inline uint8_t getBPSwitch(ABPedalTx_t *ptr)
{
    return (uint8_t)(ptr->data[1] & 0x30) >> 4;
}

inline uint8_t getAPKickdown(ABPedalTx_t *ptr)
{
    return (uint8_t)(ptr->data[1] & 0xC0) >> 6;
}

inline uint8_t getAPPosition(ABPedalTx_t *ptr)
{
    return (uint8_t) (ptr->data[2]);
}

inline uint8_t getBPPosition(ABPedalTx_t *ptr)
{
    return (uint8_t) (ptr->data[3]);
}

inline uint8_t getAPZeroSwitch(ABPedalTx_t *ptr)
{
    return (uint8_t) (ptr->data[4] & 0X03);
}
typedef struct
{
  uint32_t id ;
  union
  {
      uint8_t data[8];
      selectorTx_t selectorTx;
      ABPedalTx_t ABPedalTx;
  }p;
}ABPeadlSelector_t;
#endif /* INCLUDE_USER_ACCELERATORBRAKEJOYSTICK_H_ */
