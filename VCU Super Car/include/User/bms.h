/*
 * bmsRx.h
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_BMS_H_
#define INCLUDE_USER_BMS_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "newCanLib.h"

extern QueueHandle_t xQueueBmsTx;


void BmsInit(void);

/**
 * @typedef batteryMode_t
 * @brief Switching the battery state
 * */
typedef enum
{
    BATTERY_INIT         = 0x00,  /*!< Specifies init mode. */
    BATTERY_HV_ACTIVE    = 0x01,  /*!< Specifies on mode. */
    BATTERY_CHARGING     = 0x02,  /*!< Specifies charging mode. */
    BATTERY_NORMAL_OFF   = 0x03,  /*!< Specifies oFF mode. */
    BATTERY_EMEGENCY_OFF = 0x04   /*!< Specifies oFF mode when error accured. */
}batteryMode_t;

typedef struct
{
    batteryMode_t batteryMode;
    bool batteryState;
    uint8_t contactorStateFromBMS;
    uint16_t voltFromBMS;
}bmsMode_State_t;

#define CAN_PERIOD_MS_VCU_BMS   ((uint32_t) 20)
#define VCU_BMS_ID              ((uint32_t) 0xC51F3D0)
#define VCU_BMS_DLC             ((uint8_t) 3)


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

#define BMS_VCU_01_ID              ((uint32_t) 0xCB1D0F3)
#define BMS_VCU_01_DLC             ((uint8_t) 8)
typedef struct
{
    uint8_t data[8];
}BmsVcu_01_t;

inline uint8_t getBmsVcu01Checksum(BmsVcu_01_t *ptr)
{
    return ptr->data[0];
}
inline uint8_t getBmsVcu01MessageCounter(BmsVcu_01_t *ptr)
{
    return ptr->data[1] & 0x0F;
}
inline uint8_t getBmsVcu01BatteryMode(BmsVcu_01_t *ptr)
{
    return (ptr->data[1] & 0xF0) >> 4;
}
inline uint8_t getBmsVcu01ContactorMode(BmsVcu_01_t *ptr)
{
    return ptr->data[2] & 0x0F;
}
inline uint8_t getBmsVcu01IsolationControlMode(BmsVcu_01_t *ptr)
{
    return (ptr->data[2] & 0x10) >> 4;
}

inline uint8_t getBmsVcu01IBalancinglSysMode(BmsVcu_01_t *ptr)
{
    return (ptr->data[2] & 0x20) >> 5;
}

inline uint8_t getBmsVcu01ICoolingSyslMode(BmsVcu_01_t *ptr)
{
    return (ptr->data[2] & 0x40) >> 6;
}

inline uint8_t getBmsVcu01IErrorPresents(BmsVcu_01_t *ptr)
{
    return (ptr->data[2] & 0x80) >> 7;
}
inline uint16_t getBmsVcu01IsolationMeasurment(BmsVcu_01_t *ptr)
{
    return (uint16_t)(ptr->data[3]) | ((uint16_t)(ptr->data[4]) << 8);
}
#define BMS_VCU_02_ID              ((uint32_t) 0xCB2D0F3)
#define BMS_VCU_02_DLC             ((uint8_t) 8)
typedef struct
{
    uint8_t data[8];
}BmsVcu_02_t;
inline uint8_t getBmsVcu02Checksum(BmsVcu_02_t *ptr)
{
    return ptr->data[0];
}

inline uint8_t getBmsVcu02MessageCounter(BmsVcu_02_t *ptr)
{
    return ptr->data[1] & 0x0F;
}

inline uint16_t getBmsVcu02MaxchargingCurrent(BmsVcu_02_t *ptr)
{
    return (uint16_t)(ptr->data[1] >> 4) | ((uint16_t)(ptr->data[2]) << 4);
}

inline uint16_t getBmsVcu02MaxDischargingCurrent(BmsVcu_02_t *ptr)
{
    return (uint16_t)(ptr->data[3]) | ((uint16_t)(ptr->data[4] & 0x0F) << 8);
}
inline uint16_t getBmsVcu02Current(BmsVcu_02_t *ptr)
{
    return (uint16_t)(ptr->data[5]) | ((uint16_t)(ptr->data[6]) << 8);
}
#define BMS_VCU_03_ID              ((uint32_t) 0xCB3D0F3)
#define BMS_VCU_03_DLC             ((uint8_t) 8)
typedef struct
{
    uint8_t data[8];
}BmsVcu_03_t;

inline uint8_t getBmsVcu03Checksum(BmsVcu_03_t *ptr)
{
    return ptr->data[0];
}

inline uint8_t getBmsVcu03MessageCounter(BmsVcu_03_t *ptr)
{
    return ptr->data[1] & 0x0F;
}

inline uint8_t getBmsVcu03LeftEnergy(BmsVcu_03_t *ptr)
{
    return ptr->data[2];
}

inline uint8_t getBmsVcu03SOUE(BmsVcu_03_t *ptr)
{
    return ptr->data[3];
}

inline uint8_t getBmsVcu03SOC(BmsVcu_03_t *ptr)
{
    return ptr->data[4];
}

inline uint16_t getBmsVcu03leftLives(BmsVcu_03_t *ptr)
{
    return (uint16_t)ptr->data[5] | ((uint16_t)ptr->data[6] << 8);
}
#define BMS_VCU_04_ID              ((uint32_t) 0x18B4D0F3)
#define BMS_VCU_04_DLC             ((uint8_t) 8)
typedef struct
{
    uint8_t data[8];
}BmsVcu_04_t;

inline uint8_t getBmsVcu04Checksum(BmsVcu_04_t *ptr)
{
    return ptr->data[0];
}

inline uint8_t getBmsVcu04MessageCounter(BmsVcu_04_t *ptr)
{
    return ptr->data[1] & 0x0F;
}

inline uint16_t getBmsVcu04Volt(BmsVcu_04_t *ptr)
{
    return (uint16_t) (ptr->data[1] >> 4) | ((uint16_t)(ptr->data[2]) << 4); ;
}

inline uint16_t getBmsVcu04MaxCellVolt(BmsVcu_04_t *ptr)
{
    return (uint16_t) (ptr->data[3]) | ((uint16_t)(ptr->data[4] & 0x03 ) << 8); ;
}

inline uint16_t getBmsVcu04MinCellVolt(BmsVcu_04_t *ptr)
{
    return (uint16_t) (ptr->data[4] >> 2) | ((uint16_t)(ptr->data[5] & 0x3F) << 6); ;
}

inline uint16_t getBmsVcu04MaxChargingVoltage(BmsVcu_04_t *ptr)
{
    return (uint16_t) (ptr->data[5] >> 4) | ((uint16_t)(ptr->data[6]) << 4);
}

inline uint16_t getBmsVcu04MaxDischargingVoltage(BmsVcu_04_t *ptr)
{
    return (uint16_t) (ptr->data[6] >> 6) | (uint16_t)(ptr->data[7] << 6);
}
#define BMS_VCU_05_ID              ((uint32_t) 0x18B5D0F3)
#define BMS_VCU_05_DLC                ((uint8_t) 8)
typedef struct
{
    uint8_t data[8];
}BmsVcu_05_t;

inline uint8_t getBmsVcu05Checksum(BmsVcu_05_t *ptr)
{
    return ptr->data[0];
}

inline uint8_t getBmsVcu05MessageCounter(BmsVcu_05_t *ptr)
{
    return ptr->data[1] & 0x0F;
}

inline uint8_t getBmsVcu05MaxCellTemp(BmsVcu_05_t *ptr)
{
    return ptr->data[5];
}

inline uint8_t getBmsVcu05MinCellTemp(BmsVcu_05_t *ptr)
{
    return ptr->data[6];
}

typedef struct
{
    uint32_t id;
    union
    {
        uint8_t data[8];
        BmsVcu_01_t BmsVcu_01;
        BmsVcu_02_t BmsVcu_02;
        BmsVcu_03_t BmsVcu_03;
        BmsVcu_04_t BmsVcu_04;
        BmsVcu_05_t BmsVcu_05;
    }p;
}BmsTxCanFrame_t;
const bmsMode_State_t* getBatteryModeStateStruct();
#endif /* INCLUDE_USER_BMS_H_ */
