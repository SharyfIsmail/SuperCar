/*
 * SemikronTx.h
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_SEMIKRONTX_H_
#define INCLUDE_USER_SEMIKRONTX_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"

#define SEMICRON_ERROR_COUNT ((uint8_t)43)
void semikronTxInit(void);

#define EMD_TxPDO_1 ((uint32_t) 0x000001FA)
#define EMD_TxPDO_1_DLC ((uint8_t)8)

extern QueueHandle_t xQueueSemikronTx ;
extern QueueHandle_t xQueueCausingError;

typedef struct
{
    bool arr[SEMICRON_ERROR_COUNT];
}semicronErrorWrite_t;

typedef struct
{
    uint8_t data[8];
}emdTxPdo01_t;



/* Inverter losses  length : 12 start bit 0*/
inline uint16_t getTx_PDO_01_InverterLosses(emdTxPdo01_t *ptr)
{
    return ((uint16_t)(ptr->data[0]) | ((uint16_t)(ptr->data[1] & 0x0F) << 8)) * 10 ;
}

/* Inverter state  length : 4 start bit 12*/
inline uint8_t getTx_PDO_01_InverterState(emdTxPdo01_t *ptr)
{
    return ((uint8_t) (ptr->data[1] & 0xF0) >> 4);
}

/* Motor losses  length : 12 start bit 16 */
inline uint8_t getTx_PDO_01_MotorLosses(emdTxPdo01_t *ptr)
{
    return ((uint16_t)(ptr->data[2]) | ((uint16_t)(ptr->data[3] & 0x0F) << 8)) * 10 ;
}

/* Message Counter length : 4 start bit 28*/
inline uint8_t getTx_PDO_01_MessageCounter(emdTxPdo01_t *ptr)
{
    return ((uint8_t) (ptr->data[3] & 0xF0) >> 4);
}

/* Last Error length : 10 start bit : 32*/
inline uint16_t getTx_PDO_01_LastError(emdTxPdo01_t *ptr)
{
    return (uint16_t)(ptr->data[4]) | ((uint16_t)(ptr->data[5] & 0x03) << 8) ;
}

/* Causing Error length : 10 start bit : 48*/
inline uint16_t getTx_PDO_01_CausingError(emdTxPdo01_t *ptr)
{
    return (uint16_t)(ptr->data[6]) | ((uint16_t)(ptr->data[7] & 0x03) << 8) ;
}

/* Limitation Mode length : 1 start bit : 60*/
inline uint8_t getTx_PDO_01_LimitationMode(emdTxPdo01_t *ptr)
{
    return ((uint8_t)(ptr->data[7] & 0x10 ) >> 4);
}

/* ASC State  length : 1 start bit : 61*/
inline uint8_t getTx_PDO_01_ASCState(emdTxPdo01_t *ptr)
{
    return ((uint8_t)(ptr->data[7] & 0x20 ) >> 5);
}

/* Discharge State  length : 1 start bit : 62*/
inline uint8_t getTx_PDO_01_DishargeState(emdTxPdo01_t *ptr)
{
    return ((uint8_t)(ptr->data[7] & 0x40 ) >> 6);
}

/* Spo Input  length : 1 start bit : 63*/
inline uint8_t getTx_PDO_01_SpoInput(emdTxPdo01_t *ptr)
{
    return ((uint8_t)(ptr->data[7] & 0x80 ) >> 7);
}

#define EMD_TxPDO_2 ((uint32_t) 0x000002FA)
#define EMD_TxPDO_2_DLC ((uint8_t)8)

typedef struct
{
    uint8_t data[8];
}emdTxPdo02_t;

/* EMD_ACT_Reference Torquelim lenght: 14 start bit: 0*/
inline int16_t getTx_PDO_02_ReferenceTorqueLimit(emdTxPdo02_t *ptr)
{
    return (int16_t)(((uint16_t)(ptr->data[0]) | ((uint16_t)(ptr->data[1] & 0x3F) << 8)) * 0.02 - 100);
}

/* EMD_ACT_Reference TorqueLim_VAL lenght: 2 start bit: 14*/
inline uint8_t getTx_PDO_02_ReferenceTorqueLimitVAL(emdTxPdo02_t *ptr)
{
    return ((uint8_t)(ptr->data[1] & 0xC0) >> 6);
}

/* EMD_ACT_Max Available Torque lenght : 14 start bit : 16*/
inline uint16_t getTx_PDO_02_MaxAvailableTorque(emdTxPdo02_t *ptr)
{
    return (((uint16_t) ptr->data[2]) | ((uint16_t)ptr->data[3] & 0x3F)<<8) * 0.02;
}

/*EMD_ACT_Max Available Torque VAL lenght : 2 start bit : 30 */
inline uint8_t getTx_PDO_02_MaxAvailableTorqueVAL(emdTxPdo02_t *ptr)
{
    return ((uint8_t)(ptr->data[3] & 0xC0) >> 6);
}

/*EMD_ACT_Cutback Number lenght : 8 start bit : 32 */
inline uint8_t getTx_PDO_02_CutbackNumber (emdTxPdo02_t *ptr)
{
    return (uint8_t)ptr->data[4];
}

/*EMD_ACT_Torque lenght : 14 start bit : 40 */
inline int16_t getTx_PDO_02_Torque (emdTxPdo02_t *ptr)
{
    return (int16_t)(((uint16_t)( ptr->data[5]) | ((uint16_t)(ptr->data[6] & 0x3F) <<8 )) * 0.02 - 100);
}

/* EMD_ACT_Torque VAL lenght: 2 start bit: 54*/
inline uint8_t getTx_PDO_02_TorqueVAL(emdTxPdo02_t *ptr)
{
    return ((uint8_t)(ptr->data[6] & 0xC0) >> 6);
}

/* Message Counter length : 4 start bit 28*/
inline uint8_t getTx_PDO_02_MessageCounter(emdTxPdo01_t *ptr)
{
    return ((uint8_t) ptr->data[7] & 0x0F);
}

#define EMD_TxPDO_3 ((uint32_t) 0x000003FA)
#define EMD_TxPDO_3_DLC ((uint8_t)8)

typedef struct
{
    uint8_t data[8];
}emdTxPdo03_t;

/* EMD_ACT_MotorSpeed length : 16 start bit : 0 */
inline int16_t getTx_PDO_03_MotorSpeed(emdTxPdo03_t *ptr)
{
    return (int16_t)(((uint16_t)ptr->data[0] | ((uint16_t)ptr->data[1] << 8)) - 32768);
}

/*Message Counter length : 4 start bit : 48 */
inline uint8_t getTx_PDO_03_MessageCounter(emdTxPdo03_t *ptr)
{
    return ((uint8_t) ptr->data[6] & 0x0F);
}

/*EMD_ACT_Motor Speed VAL length : 2 start bit : 54 */
inline uint8_t getTx_PDO_03_MotorSpeedVAL(emdTxPdo03_t *ptr)
{
    return ((uint8_t)(ptr->data[6] & 0xC0) >> 6);
}

#define EMD_TxPDO_4 ((uint32_t) 0x000004FA)
#define EMD_TxPDO_4_DLC ((uint8_t)6)

typedef struct
{
    uint8_t data[6];
}emdTxPdo04_t;

/*EMD_ACT_Phase Current length : 14 start bit : 0 */
inline float getTx_PDO_04_PhaseCurrent(emdTxPdo04_t *ptr)
{
    return (float)(((uint16_t)(ptr->data[0]) | ((uint16_t)(ptr->data[1] & 0x3F) << 8)) * 0.1);
}

/*EMD_ACT_Phase Current VAL length : 2 start bit : 16 */
inline uint8_t getTx_PDO_04_PhaseCurrentVAL(emdTxPdo04_t *ptr)
{
    return ((uint8_t)(ptr->data[1] & 0xC0) >> 6);
}

/*EMD_ACT_Dc Link Voltage length : 14 start bit : 16*/
inline float getTx_PDO_04_DcLinkVoltage(emdTxPdo04_t *ptr)
{
    return (float)(((uint16_t)(ptr->data[2]) | ((uint16_t)(ptr->data[3] & 0x3F) << 8)) * 0.1);
}

/*EMD_ACT_Dc Link Voltage VAL length : 2 start bit : 30*/
inline uint8_t getTx_PDO_04_DcLinkVoltageVAL(emdTxPdo04_t *ptr)
{
    return ((uint8_t)(ptr->data[3] & 0xC0) >> 6);
}

/*EMD_ACT_Control Mode length : 4 start bit : 32*/
inline uint8_t getTx_PDO_04_ControlMode(emdTxPdo04_t *ptr)
{
    return ((uint8_t) ptr->data[4] & 0x0F);
}

/*EMD_ACT_Message Counter length : 4 start bit : 36*/
inline uint8_t getTx_PDO_04_MessageCounter(emdTxPdo04_t *ptr)
{
    return ((uint8_t) (ptr->data[4] & 0xF0) >> 4);
}

/*EMD_ACT_System Warning length : 8 start bit : 40*/
inline uint8_t getTx_PDO_04_SystemWarning(emdTxPdo04_t *ptr)
{
    return (uint8_t)ptr->data[5];
}

#define EMD_TxPDO_5 ((uint32_t) 0x000001BA)
#define EMD_TxPDO_5_DLC ((uint8_t)4)

typedef struct
{
    uint8_t data[4];
}emdTxPdo05_t;

/*EMD_ACT_Max Junction Temp  length : 8 start bit : 0*/
inline int16_t getTx_PDO_05_MaxJunctionTemp(emdTxPdo05_t *ptr)
{
    return (int16_t)(((uint8_t)ptr->data[0]) - 40 );
}

/*EMD_ACT_Motor Temperature  length : 8 start bit : 8*/
inline int16_t getTx_PDO_05_MotorTemperature(emdTxPdo05_t *ptr)
{
    return (int16_t)(((uint8_t)ptr->data[1]) - 40 );
}

/*EMD_ACT_Control Strategy length : 3 start bit : 16*/
inline uint8_t getTx_PDO_05_ControlStrategy(emdTxPdo05_t *ptr)
{
    return ((uint8_t) ptr->data[2] & 0x07);
}

/*EMD_ACT_Motor Temperature VAL length : 2 start bit : 19*/
inline uint8_t getTx_PDO_05_MotorTemperatureVAL(emdTxPdo05_t *ptr)
{
    return ((uint8_t) (ptr->data[2] & 0x18) >> 3);
}

/*EMD_ACT_Max Junction Temp VAL length : 2 start bit : 21*/
inline uint8_t getTx_PDO_05_MaxJunctionTempVAL(emdTxPdo05_t *ptr)
{
    return ((uint8_t) (ptr->data[2] & 0x60) >> 5);
}

/*EMD_ACT_Message Counter length : 4 start bit : 24*/
inline uint8_t getTx_PDO_05_MessageCounter(emdTxPdo05_t *ptr)
{
    return ((uint8_t)ptr->data[3] & 0x0F);
}

typedef struct
{
    uint32_t id;
    union
    {
        uint8_t data[8];
        emdTxPdo01_t emdTxPdo01;
        emdTxPdo02_t emdTxPdo02;
        emdTxPdo03_t emdTxPdo03;
        emdTxPdo04_t emdTxPdo04;
        emdTxPdo05_t emdTxPdo05;
    }p;
}semicronTxCanFrame_t;
const int16_t* getSpeed();
#endif /* INCLUDE_USER_SEMIKRONTX_H_ */

