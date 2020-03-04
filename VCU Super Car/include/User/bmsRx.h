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

#define CAN_PERIOD_MS_BMS_HEARTBEAT           ((uint32_t) 200)
#define BMS_HEARTBEAT                         ((uint32_t) 0x18FF0213)
#define BMS_HEARTBEAT_DLC                     ((uint8_t) 2)

#define CAN_PERIOD_MS_BMS_CONTACTOR_REQUEST   ((uint32_t) 200)
#define BMS_CONTACTOR_REQUEST                 ((uint32_t) 0x18FF0203)
#define BMS_CONTACTOR_REQUEST_DLC             ((uint8_t) 3)


/**
 * @typedef BmsContactorRequest_t
 * @brief Switching the battery
 * */
typedef enum
{
    CONTACTOR_OFF = 0x00, /*!< Specifies off mode. */
    CONTACTOR_ON = 0x01  /*!< Specifies on mode. */
}BmsContactorRequest_t;

/**
 * @typedef PcuFault_t
 * @brief fixing the fault
 * */
typedef enum
{
    NO_PCU_FAULT = 0x0,
    PCU_FAULT = 0x1
}PcuFault_t;

/**
 * @typedef CriticalPcuFault_t
 * @brief fixing the critical fault
 * */
typedef enum
{
    NO_CRITICAL_PCU_FAULT = 0x0,
    CRITICAL_PCU_FAULT = 0x1
}CriticalPcuFault_t;

/*Contactor request length : 1 start bit : 8*/
inline void setBmsContactorRequest(canMessage_t *ptr, uint8_t value)
{
    ptr->data[1] = (ptr->data[1] & 0xFE ) |  ((uint8_t) value & 0x01);
}

/*Pcu Fault length : 1 start bit : 16*/
inline void setPcuFault(canMessage_t *ptr, uint8_t value)
{
    ptr->data[2] = (ptr->data[2] & 0xFE ) |  ((uint8_t) value & 0x01);
}

/*Pcu Critical Fault length : 1 start bit : 20*/
inline void setPcuCriticalFault(canMessage_t *ptr, uint8_t value)
{
    ptr->data[2] = (ptr->data[2] & 0xEF ) |  ((uint8_t) value & 0x10);
}


#endif /* INCLUDE_USER_BMSRX_H_ */
