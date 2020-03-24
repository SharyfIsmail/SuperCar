/*
 * cy14b512q.h
 *
 *  Created on: Oct 8, 2019
 *      Author: Dmitriy Shulpenkov
 */

#ifndef INCLUDE_CY14B512Q_H_
#define INCLUDE_CY14B512Q_H_

/* -------------- INCLUDES -------------- */

#include <stdint.h>
#include <stdbool.h>

/* -------------- DEFINES -------------- */

#define CY14_CMD_WITH_ADDRESS_SIZE      3U
#define CY14_CMD_WITHOUT_ADDRESS_SIZE   1U

#define CY14_DEVICE_ID_SIZE             4U
#define CY14B512Q2A_DEVICE_ID           0x06818818U

#define CY14_OPCODE_RDSR                0b00000101
#define CY14_OPCODE_FAST_RDSR           0b00001001
#define CY14_OPCODE_WRSR                0b00000001
#define CY14_OPCODE_WREN                0b00000110
#define CY14_OPCODE_WRDI                0b00000100

#define CY14_OPCODE_READ                0b00000011
#define CY14_OPCODE_FAST_READ           0b00001011
#define CY14_OPCODE_WRITE               0b00000010

#define CY14_OPCODE_STORE               0b00111100
#define CY14_OPCODE_RECALL              0b01100000
#define CY14_OPCODE_ASENB               0b01011001
#define CY14_OPCODE_ASDISB              0b00011001

#define CY14_OPCODE_SLEEP               0b10111001
#define CY14_OPCODE_WRSN                0b11000010
#define CY14_OPCODE_RDSN                0b11000011
#define CY14_OPCODE_FAST_RDSN           0b11001001
#define CY14_OPCODE_RDID                0b10011111
#define CY14_OPCODE_FAST_RDID           0b10011001


/* -------------- ENUMS -------------- */

/** @typedef cy14_rdy_status_t
*   @brief Ready bit of Status Register Type Definition
*
*   This type is used to indicates the ready status of device to perform a memory access.
*/
typedef enum {
    CY14_READY = 0,
    CY14_IN_PROGRESS = 1
} cy14_rdy_status_t;

/** @typedef cy14_block_protection_t
*   @brief Block protect bits of Status Register Type Definition
*
*   This type is used to indicates which memory segments are write protected.
*/
typedef enum
{
    CY14_MEMORY_SEGMENTS_NOT_PROTECTED          = 0b00,
    CY14_ONE_QUARTER_MEMORY_SEGMENTS_PROTECTED  = 0b01,
    CY14_ONE_HALF_MEMORY_SEGMENTS_PROTECTED     = 0b10,
    CY14_ALL_MEMORY_SEGMENTS_PROTECTED          = 0b11
} cy14_block_protection_t;


/* -------------- STRUCTURES -------------- */

/** Status Register Union
 *
 */
typedef union
{
    struct
    {
        /** Used for enabling the function of Write Protect Pin (WP) */
        uint8_t WPEN: 1;
        /** Set to '1' for locking serial number */
        uint8_t SNL: 1;
        uint8_t reserv: 2;
        /** Block protection is provided using the BP0 and BP1
         *  pins of the Status Register. These bits can be set
         *  using WRSR instruction and probed using the RDSR
         *  instruction. The nvSRAM is divided into four array
         *  segments. One-quarter, one-half, or all of the
         *  memory segments can be protected. Any data within
         *  the protected segment is read only. Table 5 shows
         *  the function of Block Protect bits.
         */
        cy14_block_protection_t BP: 2;
        /** WEN indicates if the device is write enabled.
         * This bit defaults to ‘0’ (disabled) on power-up.
         * WEN = ‘1’ --> Write enabled
         * WEN = ‘0’ --> Write disabled
         */
        uint8_t WEN: 1;
        /** Read only bit indicates the ready status of device
         *  to perform a memory access. This bit is set to ‘1’
         *  by the device while a STORE or Software RECALL
         *  cycle is in progress.
         */
        cy14_rdy_status_t RDY: 1;
    } statusRegister;

    uint8_t statusRegisterValue;
} cy14_status_register_t;


/* -------------- FUNCTION PROTOTYPES -------------- */

inline bool IsCorrectDeviceIdOfMemoryBoard(uint32_t deviceId){
    return (deviceId == CY14B512Q2A_DEVICE_ID);
}

#endif /* INCLUDE_CY14B512Q_H_ */
