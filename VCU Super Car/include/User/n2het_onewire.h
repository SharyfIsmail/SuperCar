/*
 * n2het_onewire.h
 *
 *  Created on: Sep 17, 2019
 *      Author: Dmitriy Shulpenkov
 */

#ifndef INCLUDE_N2HET_ONEWIRE_H_
#define INCLUDE_N2HET_ONEWIRE_H_

/* -------------- INCLUDES -------------- */

#include "het.h"

/* -------------- DEFINES -------------- */

/* Time for a program loop to execute */
#define LOOP_RESOLUTION_PERIOD                          0.8

/* Time slot length for transmitting zero and one */
#define OW_WRITE_ONE_TIME_SLOT                          15
#define OW_WRITE_ZERO_TIME_SLOT                         60

/* List of instruction numbers for controlling the execution of n2het program */
#define SELECT_OPERATION_MODE_INSTRUCTION_NUMBER        0
#define DEVICE_READY_INSTRUCTION_NUMBER                 6
#define DATA_WRITE_INSTRUCTION_NUMBER                   11
#define END_OF_DATA_WRITE_INSTRUCTION_NUMBER            13
#define READ_NUMBER_OF_BITS_INSTRUCTION_NUMBER          28
#define DATA_READ_INSTRUCTION_NUMBER                    38

/* Offset value for instruction number in interrupt */
#define N2HET_INTERRUPT_INSTRUCTION_OFFSET              1

/* Index of the instruction that generates the interrupt of reading the data byte. */
#define N2HET_END_READING_BYTE_INTERRUPT_FLAG           27

/* OneWire ROM function commands */
#define ONEWIRE_CMD_READ_ROM                            0x33
#define ONEWIRE_CMD_MATCH_ROM                           0x55
#define ONEWIRE_CMD_SEARCH_ROM                          0xF0
#define ONEWIRE_CMD_SKIP_ROM                            0xCC

/* Operation mode N2HET program */
#define STANDBY_OPERATION_MODE                          255
#define RESET_OPERATION_MODE                            0
#define READ_OPERATION_MODE                             1
#define WRITE_OPERATION_MODE                            2

/* -------------- STRUCTURES -------------- */

/** @typedef owDeviceCode_t
*   @brief Unique device code type definition
*
*   This type is used to store unique code of devices with which ROM commands work.
*/
typedef struct{
    uint8_t familyCode;       /* Device family code stored on board */
    uint8_t serialNumber[6];  /* Device serial number stored on board */
    uint8_t crc;              /* CRC data */
} owDeviceCode_t;

/* -------------- FUNCTION PROTOTYPES -------------- */

/* OneWire Interface Functions */
void OneWire_Init();
bool OneWire_TryReset();
void OneWire_WriteByte(uint8_t);
bool OneWire_TryReadByte(uint8_t *);
void N2Het_OneWireInterruptHandle(uint32_t);

/* ROM Command Interface Functions */
const owDeviceCode_t * OneWire_ReadROM();
void OneWire_SkipROM();
//void OneWire_MatchROM();
//void OneWire_SearchROM();

#endif /* INCLUDE_N2HET_ONEWIRE_H_ */
