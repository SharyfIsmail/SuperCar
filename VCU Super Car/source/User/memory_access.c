/*
 * memory_access.c
 *
 *  Created on: Oct 8, 2019
 *      Author: Dmitriy Shulpenkov
 */

#include <stdint.h>
#include <string.h>
#include "spi_helper.h"
#include "sys_vim.h"
#include "FreeRTOS.h"
#include "os_task.h"
#include "memory_access.h"



typedef struct {
    uint8_t dataTypes[ MEMORY_DATA_TYPES_COUNT ];
    uint8_t errorCounter;
    uint8_t batteryInfoCounter;
} memoryStartingPointInfo_t;

typedef union {
    struct {
        uint8_t dataUploadedFlag : 2;
        uint8_t starterPointInitializedFlag : 2;
        uint8_t controlDeviceIdFlag : 2;
        uint8_t notRespondingFlag : 2;
    } points;

    uint8_t value;
} memoryControlPoints_t;

static uint8_t newCommand[ CY14_CMD_WITH_ADDRESS_SIZE ] = { 0U };
static spiDAT1_t spiDataConfig = { .WDEL = FALSE, .DFSEL = SPI_FMT_0 };

//static memoryControlPoints_t levelMemoryBoardsAccess[ MEMORY_BOARD_NUMBER ] = { 0U };
//static memoryStartingPointInfo_t memoryStartingPointOfBatteryWingInfo;
//static memoryStartingPointInfo_t memoryStartingPointOfImpedanceTrack;

TaskHandle_t xTaskToNotifyFromSpi;

//static void InitMemoryStartingPointOfBatteryWingStatistics(void);
//static void InitMemoryStartingPointOfImpedanceTrack(void);
//static void ParseMemoryStartingPointInfoToBuffer(uint8_t memoryBoardNumber, uint8 *outputDataBuffer);

/** @fn void memoryAccessInit()
*   @brief Initializes the SPI Driver for getting access to non-volatile memory
*
*   This function initializes the SPI module and enable interrupt for it.
*/
void memoryAccessInit(void)
{
    /** Initializes the SPI Driver */
    spiInit();

#if (SPI1_INTERRUPT_ENABLE == 1)
    /** Enable SPI1 High Level Interrupt */
    vimChannelMap(12U, SPI1_HIGH_LVL_INTERRUPT_VIM_CHANNEL, &spi1HighLevelInterrupt);
    vimEnableInterrupt(SPI1_HIGH_LVL_INTERRUPT_VIM_CHANNEL, SYS_IRQ);

    /** Enable SPI1 Low Level Interrupt */
    vimChannelMap(26U, SPI1_LOW_LVL_INTERRUPT_VIM_CHANNEL, &spi1LowLevelInterrupt);
    vimEnableInterrupt(SPI1_LOW_LVL_INTERRUPT_VIM_CHANNEL, SYS_IRQ);
#endif

#if (SPI2_INTERRUPT_ENABLE == 1)
    /** Enable SPI2 High Level Interrupt */
    vimChannelMap(17U, SPI2_HIGH_LVL_INTERRUPT_VIM_CHANNEL, &spi2HighLevelInterrupt);
    vimEnableInterrupt(SPI2_HIGH_LVL_INTERRUPT_VIM_CHANNEL, SYS_IRQ);

    /** Enable SPI2 Low Level Interrupt */
    vimChannelMap(30U, SPI2_LOW_LVL_INTERRUPT_VIM_CHANNEL, &spi2LowLevelInterrupt);
    vimEnableInterrupt(SPI2_LOW_LVL_INTERRUPT_VIM_CHANNEL, SYS_IRQ);
#endif

#if (SPI3_INTERRUPT_ENABLE == 1)
    /** Enable SPI3 High Level Interrupt */
    vimChannelMap(37U, SPI3_HIGH_LVL_INTERRUPT_VIM_CHANNEL, &spi3HighLevelInterrupt);
    vimEnableInterrupt(SPI3_HIGH_LVL_INTERRUPT_VIM_CHANNEL, SYS_IRQ);

    /** Enable SPI3 Low Level Interrupt */
    vimChannelMap(38U, SPI3_LOW_LVL_INTERRUPT_VIM_CHANNEL, &spi3LowLevelInterrupt);
    vimEnableInterrupt(SPI3_LOW_LVL_INTERRUPT_VIM_CHANNEL, SYS_IRQ);
#endif
}


void sendMemoryAccessCommandFromTask(uint8_t operationCode, uint8_t memoryBoardNumber, uint16_t *address)
{
    uint8_t dataBlockSize = 0U;
    spiDataConfig.CSNR = SELECT_MEMORY_BOARD(memoryBoardNumber);

    newCommand[0U] = operationCode;
    if( address != NULL )
    {
        spiDataConfig.CS_HOLD = TRUE;
        newCommand[1U] = (uint8_t)(*address >> 8U);
        newCommand[2U] = (uint8_t)*address;
        dataBlockSize = CY14_CMD_WITH_ADDRESS_SIZE;
    }
    else
    {
        if( operationCode == CY14_OPCODE_RDSR || operationCode == CY14_OPCODE_RDID ){
            spiDataConfig.CS_HOLD = TRUE;
        }
        else {
            spiDataConfig.CS_HOLD = FALSE;
        }
        dataBlockSize = CY14_CMD_WITHOUT_ADDRESS_SIZE;
    }

    xTaskToNotifyFromSpi = xTaskGetCurrentTaskHandle();

    if(xTaskToNotifyFromSpi)
    {
#if (SPI1_MODULE_ENABLE == 1)
    spiSendDataBuffer(spiREG1, &spiDataConfig, dataBlockSize, newCommand);
#elif (SPI2_MODULE_ENABLE == 1)
    spiSendDataBuffer(spiREG2, &spiDataConfig, dataBlockSize, newCommand);
#elif (SPI3_MODULE_ENABLE == 1)
    spiSendDataBuffer(spiREG3, &spiDataConfig, dataBlockSize, newCommand);
#endif
    }
}


void writeDataToMemoryFromTask(uint8_t memoryBoardNumber, uint8_t* dataBuffer, uint8_t length)
{
    spiDataConfig.CS_HOLD = FALSE;
    spiDataConfig.CSNR = SELECT_MEMORY_BOARD(memoryBoardNumber);

    xTaskToNotifyFromSpi = xTaskGetCurrentTaskHandle();

    if(xTaskToNotifyFromSpi)
    {
#if (SPI1_MODULE_ENABLE == 1)
    spiSendDataBuffer(spiREG1, &spiDataConfig, length, dataBuffer);
#elif (SPI2_MODULE_ENABLE == 1)
    spiSendDataBuffer(spiREG2, &spiDataConfig, length, dataBuffer);
#elif (SPI3_MODULE_ENABLE == 1)
    spiSendDataBuffer(spiREG3, &spiDataConfig, length, dataBuffer);
#endif
    }
}

void readDataFromMemoryFromTask(uint8_t memoryBoardNumber, uint8_t *dataBuffer, const uint8_t length)
{
    spiDataConfig.CS_HOLD = FALSE;
    spiDataConfig.CSNR = SELECT_MEMORY_BOARD(memoryBoardNumber);

    xTaskToNotifyFromSpi = xTaskGetCurrentTaskHandle();

    if(xTaskToNotifyFromSpi)
    {
#if (SPI1_MODULE_ENABLE == 1)
    spiGetDataBuffer(spiREG1, &spiDataConfig, length, dataBuffer);
#elif (SPI2_MODULE_ENABLE == 1)
    spiGetDataBuffer(spiREG2, &spiDataConfig, length, dataBuffer);
#elif (SPI3_MODULE_ENABLE == 1)
    spiGetDataBuffer(spiREG3, &spiDataConfig, length, dataBuffer);
#endif
    }
}

uint8_t readMemoryStatusRegisterFromTask(uint8_t memoryBoardNumber, cy14_status_register_t *memoryBoard)
{
    uint32_t callingTaskNotificationValue = 0U;

    if( memoryBoard != NULL )
    {
        sendMemoryAccessCommandFromTask( CY14_OPCODE_RDSR, memoryBoardNumber, NULL );

        callingTaskNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 50U ) );

        if( callingTaskNotificationValue )
        {
            readDataFromMemoryFromTask( memoryBoardNumber, &memoryBoard->statusRegisterValue, sizeof(memoryBoard->statusRegisterValue));

            callingTaskNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 50U ) );

            if( callingTaskNotificationValue )
            {
                return TRUE;
            } /* else not needed */
        } /* else not needed */
    } /* else not needed */

    return FALSE;
}

uint8_t readMemoryBoardDeviceIdFromTask(uint8_t memoryBoardNumber, uint32_t *deviceId)
{
    uint32_t callingTaskNotificationValue = 0U;

    if( deviceId != NULL )
    {
        sendMemoryAccessCommandFromTask( CY14_OPCODE_RDID, memoryBoardNumber, NULL );

        callingTaskNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 50U ) );

        if( callingTaskNotificationValue )
        {
            readDataFromMemoryFromTask( memoryBoardNumber, (uint8_t *)deviceId, CY14_DEVICE_ID_SIZE );

            callingTaskNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 50U ) );

            if( callingTaskNotificationValue )
            {
                return TRUE;
            } /* else not needed */
        } /* else not needed */
    } /* else not needed */

    return FALSE;
}

uint8_t readMemoryFromTask(uint8_t memoryBoardNumber, uint16_t address, uint8_t *dataBuffer, const uint8_t length)
{
    uint32_t callingTaskNotificationValue = 0U;

    if( dataBuffer != NULL )
    {
        sendMemoryAccessCommandFromTask( CY14_OPCODE_READ, memoryBoardNumber, &address );

        callingTaskNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 50U ) );

        if( callingTaskNotificationValue )
        {
            readDataFromMemoryFromTask( memoryBoardNumber, dataBuffer, length);

            callingTaskNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 50U ) );

            if( callingTaskNotificationValue )
            {
                return TRUE;
            } /* else not needed */
        } /* else not needed */
    } /* else not needed */

    return FALSE;
}

uint8_t writeDataBufferToMemoryFromTask(uint8_t memoryBoardNumber, uint16_t address, uint8_t *dataBuffer, const uint8_t length)
{
    cy14_status_register_t memoryBoard = { 0U };
    uint32_t callingTaskNotificationValue = 0U;

    if( dataBuffer != NULL )
    {
        sendMemoryAccessCommandFromTask( CY14_OPCODE_WREN, memoryBoardNumber, NULL );

        callingTaskNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 50U ) );

        if( callingTaskNotificationValue )
        {
            readMemoryStatusRegisterFromTask( memoryBoardNumber, &memoryBoard );

            if( memoryBoard.statusRegister.RDY == CY14_READY && memoryBoard.statusRegister.WEN )
            {
                sendMemoryAccessCommandFromTask( CY14_OPCODE_WRITE, memoryBoardNumber, &address );

                callingTaskNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 50U ) );

                if( callingTaskNotificationValue )
                {
                    writeDataToMemoryFromTask( memoryBoardNumber, dataBuffer, length );

                    callingTaskNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 50U ) );

                    if( callingTaskNotificationValue )
                    {
                        return TRUE;
                    } /* else not needed */
                } /* else not needed */
            } /* else not needed */
        }
    } /* else not needed */

    return FALSE;
}

uint8_t enableAutoStoreFunctionInMemory(uint8_t memoryBoardNumber)
{
    cy14_status_register_t memoryBoard;
    uint32_t callingTaskNotificationValue = 0U;
    uint8_t operationResult = 0U;

    /** Check memory board status register */
    operationResult = readMemoryStatusRegisterFromTask( memoryBoardNumber, &memoryBoard );

    if( operationResult && memoryBoard.statusRegister.RDY == CY14_READY )
    {
        sendMemoryAccessCommandFromTask( CY14_OPCODE_WREN, memoryBoardNumber, NULL );

        callingTaskNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 50U ) );

        if( callingTaskNotificationValue )
        {
            sendMemoryAccessCommandFromTask( CY14_OPCODE_ASENB, memoryBoardNumber, NULL );

            callingTaskNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 50U ) );

            if( callingTaskNotificationValue )
            {
                return TRUE;
            } /* else not needed */
        } /* else not needed */
    } /* else not needed */

    return FALSE;
}

uint8_t performManualStoreFunction(uint8_t memoryBoardNumber)
{
    uint32_t callingTaskNotificationValue = 0U;

    sendMemoryAccessCommandFromTask( CY14_OPCODE_WREN, memoryBoardNumber, NULL );

    callingTaskNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 50U ) );

    if( callingTaskNotificationValue )
    {
        sendMemoryAccessCommandFromTask( CY14_OPCODE_STORE, memoryBoardNumber, NULL );

        callingTaskNotificationValue = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 50U ) );

        if( callingTaskNotificationValue )
        {
            return TRUE;
        } /* else not needed */
    } /* else not needed */

    return FALSE;
}
