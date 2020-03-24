/*
 * memory_access.h
 *
 *  Created on: Oct 8, 2019
 *      Author: Dmitriy Shulpenkov
 */

#ifndef INCLUDE_MEMORY_ACCESS_H_
#define INCLUDE_MEMORY_ACCESS_H_

/* ------------------- INCLUDES ------------------- */

#include <stdint.h>
#include <limits.h>
#include "cy14b512q.h"

/* ------------------- DEFINES ------------------- */

#define MASK(x) ((unsigned char)(1 << (x)))
#define SELECT_MEMORY_BOARD(x) ((x == 0) ? (MASK(2)) : ((x == 1) ? (MASK(3)) : 0))
#define SWITCH_MEMORY_BOARD(x) (x ^= 1U)

#define MEMORY_BOARD_NUMBER                         2U
#define MEMORY_BOARD_INDEX_OF_FIRST                 1U

#define MEMORY_STARTING_POINT_SIZE                  5U

/** Data types saving in memory */
#define MEMORY_DATA_TYPES_COUNT                     3U
#define CELLS_VOLTAGE_DATA_TYPES_CODE               'B'
#define BATTERY_ERROR_DATA_TYPES_CODE               'E'
#define BATTERY_SOC_DATA_TYPES_CODE                 'S'

#define START_MEMORY_BLOCK_ADDRESS                  ((uint16_t) 0)

/* ------------------- TYPES ------------------- */

typedef struct {
    bool isBatteryWingInfoStoragePresence;
    bool isBatteryWingSocStoragePresence;
} memoryBoardPresenceInSystem_t;

typedef enum {
    BATTERY_WING_INFO_STORAGE,
    BATTERY_WING_IMPEDANCE_TRACK_STORAGE
} storageType_t;

typedef enum {
    FIRST_RESPONDING,
    DEVICE_ID_CONTROL,
    STARTER_POINT_INIT,
    DATA_UPLOADING,
    ALL_COMPLETED
} memoryControlPointStage_t;

typedef enum {
    CONTROL_POINT_NOT_PERFORMED,
    CONTROL_POINT_PASSED,
    CONTROL_POINT_FAIL,
    CONTROL_POINT_TIMEOUT
} cotrolPointState_t;

/* -------------- FUNCTION PROTOTYPES -------------- */

void memoryAccessInit(void);
void sendMemoryAccessCommandFromTask(uint8_t, uint8_t, uint16_t*);
void writeDataToMemoryFromTask(uint8_t, uint8_t*, uint8_t);
void readDataFromMemoryFromTask(uint8_t, uint8_t*, const uint8_t);
uint8_t readMemoryStatusRegisterFromTask(uint8_t, cy14_status_register_t *);
uint8_t readMemoryBoardDeviceIdFromTask(uint8_t, uint32_t *);
uint8_t readMemoryFromTask(uint8_t, uint16_t, uint8_t *, const uint8_t);
uint8_t writeDataBufferToMemoryFromTask(uint8_t, uint16_t, uint8_t *, const uint8_t);
uint8_t enableAutoStoreFunctionInMemory(uint8_t);
uint8_t performManualStoreFunction(uint8_t);



#endif /* INCLUDE_MEMORY_ACCESS_H_ */
