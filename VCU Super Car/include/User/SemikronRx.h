/*
 * SemikronRx.h
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */
#ifndef INCLUDE_USER_SEMIKRONRX_H_
#define INCLUDE_USER_SEMIKRONRX_H_
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "newCanLib.h"


void semikronRxInit(void);

#define CAN_PERIOD_MS_SEMICRON_SYN           ((uint32_t) 100)
#define SEMICRON_SYN                         ((uint32_t) 0x00000080)
#define SEMICRON_SYN_DLC                     ((uint8_t) 0)


#define CAN_PERIOD_MS_NMT_NODE_GUARDING      ((uint32_t) 500)
#define SEMICRON_RX_NMT_NODE_GUARDING ((uint32_t) 0x0000077A)
#define SEMICRON_RX_NMT_NODE_GUARDING_DLC ((uint8_t) 1)


/**
 * @typedef nmtNodeGuardingState_t
 * @brief Inverter state up to controlling mode
 * */
typedef enum
{
    BOOT_UP          = 0x00, /*!< Specifies the  boot up mode. */
    INITIALISATION   = 0x01, /*!< Specifies the mode  wakeup mode NEW SETION. */
    STOPPED          = 0x04, /*!< Specifies the the end of communication mode. */
    OPERATIONAL      = 0x05, /*!< Specifies the working mode after turning on the battery.*/
    PRE_OPERATIONAL  = 0x7F  /*!< Specifies the working mode before turning on the battery.*/
}nmtNodeGuardingState_t;

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
inline void ToggleNmtNodeGuardingToggleBit(canMessage_t* ptr)
{
    setNmtNodeGuardingToggleBit(ptr, getNmtNodeGuardingToggleBit(ptr) ^ 1 );
}


#define SEMICRON_RX_NMT_NMT_COMMAND ((uint32_t) 0x0)
#define SEMICRON_RX_NMT_NMT_COMMAND_DLC ((uint8_t) 2)

/**
 * @typedef nmtNodeGuardingState_t
 * @brief Inverter state up to controlling mode
 * */
typedef enum
{
   START_REMOTE_NODE      = 0x01, /*!< Specifies the working mode after turning on the battery.*/
   STOP_REMOTE_NODE       = 0x02, /*!< Specifies the the end of communication mode. */
   ENTER_PRE_OPERATIONAL  = 0x80, /*!< Specifies the working mode before turning on the battery.*/
   RESET_NODE             = 0x81, /*!< Specifies the the end of communication mode. */
   RESET_COMMUNICATION    = 0x82  /*!< Specifies the mode  wakeup mode NEW SETION. */
}nmtCommandSpecifier_t;

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

#define CAN_PERIOD_MS_SEMICRON_HANDLER         ((uint32_t) 100)
#define SEMICRON_SEMICRON_HANDLER              ((uint32_t) 0x0000047A)
#define SEMICRON_HANDLER_DLC                   ((uint8_t) 6)

#define RX_PDO_03_TORQUE_REF_LIM(phy) ((uint16_t) ((phy + 100) / 0.02))

/*EMD_CMD_Torque Ref Lim  length : 14 start bit 0*/
inline void setRx_PDO_03TorqueRefLim(canMessage_t *ptr, uint16_t value)
{
    ptr->data[0] = (ptr->data[0] & 0x00) | (uint8_t) value;
    ptr->data[1] = (ptr->data[1] & 0xC0) | (uint8_t) (value >> 8);
}
/*EMD_CMD_Torque Ref Lim VAL  length : 2 start bit 14*/
inline void setRx_PDO_03TorqueRefLimVAL(canMessage_t *ptr, uint8_t value)
{
    ptr->data[1] = (ptr->data[1] & 0x3F) | (uint8_t)((value & 0x03) << 6);
}

#define RX_PDO_03_SPEED_REF_LIM(phy) ((uint16_t) (phy + 32768))
/*EMD_CMD_Speed Ref Lim  length : 16 start bit 16*/
inline void setRx_PDO_03SpeedRefLim(canMessage_t *ptr, uint16_t value)
{
    ptr->data[2] = ptr->data[2] | (uint8_t) value;
    ptr->data[3] = ptr->data[3] | (uint8_t) (value >> 8);
}

/*EMD_CMD_Speed Ref Lim VAL  length : 2 start bit 40*/
inline void setRx_PDO_03SpeedRefLimVAL(canMessage_t *ptr, uint8_t value)
{
    ptr->data[5] = (ptr->data[5] & 0xFC) | (uint8_t)(value & 0x03);
}

/**
 * @typedef Rx_PDO_03ControlMode_t
 * @brief Inverter state up to controlling mode
 * */
typedef enum
{
    DISABLED                    = 0x00,/*!< Without mode.*/
    TORQUE_CONTROL_MODE         = 0x01,/*!< Specifies Torque mode.*/
    SPEED_CONTROL_MODE          = 0x02,/*!< Specifies Speed mode.*/
    BRAKE_CHOPPER               = 0x04,/*!< Specifies the  power that cannot be charged to the battery while braking.*/
    POSITION_SENSOR_CALIBRATION = 0x05,/*!< Required when Position sensor calibration was requested over CAN..*/
    TEST_ID_IQ                  = 0x08,
    TEST_CLOSED_LOOP            = 0x09
}Rx_PDO_03ControlMode_t;

/*EMD_CMD_Control Mode  length : 4 start bit 32*/
inline void setRx_PDO_03ControlMode(canMessage_t *ptr, uint8_t value)
{
    ptr->data[4] = (ptr->data[4] & 0xF0) | (uint8_t)(value & 0x0F);
}

/*EMD_MSGCNT_RXPDO3  length : 4 start bit 36*/
inline void setRX_PDO_03MessageCounter(canMessage_t *ptr, uint8_t value)
{
    ptr->data[4] = (ptr->data[4] & 0x0F) | (uint8_t)((value & 0x0F) << 4);
}
inline uint8_t getRx_PDO_03MessageCounter(canMessage_t *ptr)
{
    return (uint8_t) ((ptr->data[4] >> 4) & 0x0F);
}
inline void incRx_PDO_03MessageCounter(canMessage_t *ptr, uint8_t value)
{
    setRX_PDO_03MessageCounter(ptr, getRx_PDO_03MessageCounter(ptr) + value);
}

typedef enum
{
    DO_NOT_CLEAR =  false,
    CLEAR_ERROR  =  true
}clearError_t;

/*EMD_CMD_Clear Error length : 1 start bit 42*/
inline void setRX_PDO_03ClearError(canMessage_t *ptr, uint8_t value)
{
    ptr->data[5] = (ptr->data[5] & 0xFB) | (uint8_t)((value & 0x01) << 2);
}

/*EMD_CMD_Restart Request length : 1 start bit 43*/
inline void setRX_PDO_03RestartRequest(canMessage_t *ptr, uint8_t value)
{
    ptr->data[5] = (ptr->data[5] & 0xF7) | (uint8_t)((value & 0x01) << 3);
}

/*EMD_CMD_Active Dicsharge length : 1 start bit 44*/
inline void setRX_PDO_03ActiveDisharge(canMessage_t *ptr, uint8_t value)
{
    ptr->data[5] = (ptr->data[5] & 0xEF) | (uint8_t)((value & 0x01) << 4);
}

/**
 * @typedef RX_PDO_03LimitationMode_t
 * @brief Inverter state up to controlling mode
 * */
typedef enum
{
    SYMMETRIC  = 0x00, /*!< Specifies symmetric mode.*/
    ASYMMETRIC = 0x01  /*!< Specifies asymmetric mode.*/
}RX_PDO_03LimitationMode_t;

/*EMD_CMD_Limitation Mode : 1 start bit 45*/
inline void setRX_PDO_03LimitationMode(canMessage_t *ptr, uint8_t value)
{
    ptr->data[5] = (ptr->data[5] & 0xDF) | (uint8_t)((value & 0x01) << 5);
}


#endif /* INCLUDE_USER_SEMIKRONRX_H_ */
