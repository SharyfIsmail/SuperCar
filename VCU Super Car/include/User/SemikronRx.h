/*
 * SemikronRx.h
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_SEMIKRONRX_H_
#define INCLUDE_USER_SEMIKRONRX_H_
#include <stdint.h>
#include "newCanLib.h"

#define CAN_PERIOD_MS_SEMICRON_SYN           ((uint32_t) 100)
#define SEMICRON_SYN                         ((uint32_t) 0x00000080)
#define SEMICRON_SYN_DLC                     ((uint8_t) 0)


void semikronRxInit(void);

#define CAN_PERIOD_MS_NMT_NODE_GUARDING      ((uint32_t) 500)
#define SEMICRON_RX_NMT_NODE_GUARDING ((uint32_t) 0x0000077A)
#define SEMICRON_RX_NMT_NODE_GUARDING_DLC ((uint8_t) 1)

/*EMD_NMT_Node Guarding State length : 7 start bit : 0 */
inline void setNmtNodeGuardingState(canMessage_t* ptr, uint8_t value)
{
    ptr->data[0] =  (ptr->data[0] & 0x80) | ((uint8_t) value & 0x7F);
}

/*EMD_NMT_Node Guarding Toggle Bit length : 1 start bit : 7 */
inline void setNmtNodeGuardingToggleBit(canMessage_t* ptr, uint8_t value)
{
    ptr->data[0] =  (ptr->data[0] & 0x7F) | (((uint8_t) value & 0x01) << 7);
}
/*EMD_NMT_Node Guarding Toggle Bit length : 1 start bit : 7 */
inline uint8_t getNmtNodeGuardingToggleBit(canMessage_t* ptr)
{
    return ((uint8_t)(ptr->data[0] & 0X80) >> 7) ;
}
/*EMD_NMT_Node Guarding Toggle Bit length : 1 start bit : 7 */
inline void ToggleNmtNodeGuardingToggleBit(canMessage_t* ptr, uint8_t value)
{
    setNmtNodeGuardingToggleBit(ptr, getNmtNodeGuardingToggleBit(ptr) ^ 1 );
}

#define SEMICRON_RX_NMT_NMT_COMMAND ((uint32_t) 0x0)
#define SEMICRON_RX_NMT_NMT_COMMAND_DLC ((uint8_t) 2)

/*EMD_NMT_Command Specifier length : 8 start bit : 0 */
inline void setNmtCommandSpecifier(canMessage_t *ptr, uint8_t value)
{
    ptr->data[0] = (uint8_t) value;
}

/*EMD_NMT_Node Id length : 8 start bit : 8 */
inline void setNmtNodeId(canMessage_t *ptr, uint8_t value)
{
    ptr->data[1] = (uint8_t) value;
}
#endif /* INCLUDE_USER_SEMIKRONRX_H_ */
