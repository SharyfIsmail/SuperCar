/*
 * newCanLib.h
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_NEWCANLIB_H_
#define INCLUDE_USER_NEWCANLIB_H_

#include <stdint.h>
#include "can.h"
/** @struct can_message
 *  @brief CAN message structure definition
 */
/** @typedef can_message_t
 *  @brief can_message Type Definition
 *
 *  This type is used to form or save CAN messages
 */
typedef struct canMessage
{
    uint32_t id ;      /*!< Specifies the standard identifier (this parameter can be a
                           value between 0 to 0x7FF) or  the extended identifier
                           (this parameter can be a value between 0 to 0x1FFFFFFF). */

    uint8_t ide;       /*!< Specifies the type of identifier for the message.
                           This parameter can be a value of @enum can_identifier */

    uint8_t dlc;       /*!< Specifies the length of the frame that will be
                           transmitted or was be received.
                           This parameter can be a value between 0 to 8 */

    uint8_t data[8];  /*!< Contains the data to be transmitted. It ranges from 0
                          to 0xFF. */
}canMessage_t;

typedef enum can_identifier
{
    CAN_Id_Standard,
    CAN_Id_Extended = 4
} can_identifier_t;
typedef enum
{
    SENDING_NODE = 0x0,
    RECEIVEING_NODE = 0x400
}nodType_t;
uint32 newCanTransmit(canBASE_t *node, uint32 messageBox, canMessage_t* ptr);
void boardCanInit(canBASE_t *node);

#endif /* INCLUDE_USER_NEWCANLIB_H_ */
