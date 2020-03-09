/*
 * selector.h
 *
 *  Created on: 6 ���. 2020 �.
 *      Author: User
 */

#ifndef INCLUDE_USER_SELECTOR_H_
#define INCLUDE_USER_SELECTOR_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "os_queue.h"

extern QueueHandle_t xQueueSelectorTx;

void selectorInit();


#define SELECTOR_TX ((uint32_t) 0x0000011F)
#define SLECTOR_TX ((uint8_t)7)

typedef struct
{
    uint32_t id ;
    uint8_t data[7];
}selectorTx_t;


/*Checksum length : 8 start bit 0 :*/
inline uint8_t getSelectorChecksum(canMessage_t *ptr)
{
    return (uint8_t) (ptr->data[0]);
}

/*message Counter length : 4 start bit 8 :*/
inline uint8_t getSlectorMessageCounter(canMessage_t *ptr)
{
    return (uint8_t)(ptr->data[1] & 0x0F);
}

/**
 * @typedef lockingState_t
 * @brief fixing the state of lock
 * */
typedef enum
{
    INIT_VALUE      = 0x00,
    UNDEFINED       = 0x01,
    UNLOCKED        = 0x02,
    LOCKED          = 0x03,
    NOT_INSTALLED   = 0x06,
    ERROR_PRESENTED = 0x07
}lockingState_t;

/*Locking state  length : 4 start bit 12 :*/
inline uint8_t getSelectorLockingState(canMessage_t *ptr)
{
    return (uint8_t)((ptr->data[1] & 0xF0)>> 4);
}

/**
 * @typedef selectorLeverInit_t
 * @brief state of Selector
 * */
typedef enum
{
    INIT    = 0x00,/*!< Specifies undo standby state  */
    STANDBY = 0x01 /*!< Specifies that selector is ready to work. */
}selectorLeverInit_t;
/*Selector Lever Initialization length : 4 start bit 16: */
inline uint8_t getSelectorLeverInit(canMessage_t *ptr)
{
    return (uint8_t) (ptr->data[2] & 0x0F );
}


#endif /* INCLUDE_USER_SELECTOR_H_ */
