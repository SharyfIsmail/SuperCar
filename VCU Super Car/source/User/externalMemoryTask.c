/*
 * externalMemoryTask.c
 *
 *  Created on: 23 мар. 2020 г.
 *      Author: User
 */
#include "externalMemoryTask.h"
#include "memory_access.h"
#include "reg_spi.h"
#include "crc8.h"
#include "newCanLib.h"
#include "string.h"

#define HEADER_SIZE_BYTES  ((uint8_t) 4)
#define ERROR_SIZE_BYTES   ((uint8_t) 6)
#define MAX_ERROR_QUANTITY ((uint16_t) 7999))
#define ERROR_ADDRESS(number) (HEADER_SIZE_BYTES + ((uint16_t)number)*ERROR_SIZE_BYTES)

QueueHandle_t xQueueCommandToExtMemory = NULL;
typedef struct
{
    uint16_t errorQuantity;
    uint8_t rewritringToExternalMemory;
    uint8_t crc8;
}ExtMemoryHeader_t;

void vExternalMemoryTask(void *pvParameters);
static void writeErrorToExtMemory(const ErrorDataToExtMemory_t *errorData );
static void clearExternalMemory();
static void sendErrorFromExtMemory(const ErrorDataToExtMemory_t *errorDataTocan);

static void errorToBytes(const ErrorDataToExtMemory_t *errorData, uint8_t data[]);
static void errorFromByte(const uint8_t data[], ErrorDataToExtMemory_t *errorData);

static bool longMemoryWriting( uint16_t address, uint8_t tx_data[], uint8_t size);
static bool longMemoryReading( uint16_t address, uint8_t rx_data[], uint8_t size);

static void headerToBytes(const ExtMemoryHeader_t *extMemoryHeader, uint8_t header[]);
static void headerFromBytes(const uint8_t data[], ExtMemoryHeader_t *extMemoryHeader);


static ExtMemoryHeader_t extMemoryHeader = {0};

void spiEndNotification(spiBASE_t *spi)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    configASSERT( xTaskToNotifyFromSpi != NULL );

    vTaskNotifyGiveFromISR( xTaskToNotifyFromSpi, &xHigherPriorityTaskWoken );

    xTaskToNotifyFromSpi = NULL;

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void externalMemoryTaskInit()
{
    if(xTaskCreate(vExternalMemoryTask, "ExternalMemoryTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdTRUE)
     {
         /*Task couldn't be created */
         while(1);
     }/* else not needed */

    xQueueCommandToExtMemory = xQueueCreate(10U, sizeof(CommandToExtMemory_t));
}

void vExternalMemoryTask(void *pvParameters)
{
    uint8_t headerByte[4] = {0};

    if(!longMemoryReading(0, headerByte, HEADER_SIZE_BYTES))
    {
        vTaskDelay(portMAX_DELAY);
    }/* else not needed */
    headerFromBytes(headerByte, &extMemoryHeader);
    if(crc8(headerByte, 3) != extMemoryHeader.crc8)
    {
        extMemoryHeader.errorQuantity = 0;
    }

    CommandToExtMemory_t command;
    for(;;)
    {
        if(xQueueReceive(xQueueCommandToExtMemory, &command, portMAX_DELAY))
        {
            switch(command.type)
            {
            case EXT_MEMROY_WRITE:
                writeErrorToExtMemory(&command.errorData);
                break;
            case EXT_MEMORY_SEND_BY_CAN:
                sendErrorFromExtMemory(&command.errorData);
                break;
            case CLEAR_EXTERNAL_MEMORY:
                clearExternalMemory();
                break;
            }
        }/* else not needed */
    }
}
static void writeErrorToExtMemory(const ErrorDataToExtMemory_t *errorData)
{
    uint8_t data[6] = {0};
    errorToBytes(errorData, data);
    uint16_t addr = ERROR_ADDRESS(extMemoryHeader.errorQuantity);

    if(!longMemoryWriting(addr, data, ERROR_SIZE_BYTES))
    {
        vTaskDelay(portMAX_DELAY);
    }/* else not needed */

    if(extMemoryHeader.errorQuantity < 7999)
        extMemoryHeader.errorQuantity++;
    else
    {
        extMemoryHeader.errorQuantity = 0;
        extMemoryHeader.rewritringToExternalMemory++;
    }

    uint8_t headerByte[4] = {0};
    headerToBytes(&extMemoryHeader, headerByte);
    if(!longMemoryWriting(0, headerByte, HEADER_SIZE_BYTES))
    {
        vTaskDelay(portMAX_DELAY);
    }/* else not needed */
}
static void sendErrorFromExtMemory(const ErrorDataToExtMemory_t *errorDataTocan)
{
    uint8_t headerByte[4] = {0};

    canMessage_t canMessage =
    {
     .id  = 0x00,
     .dlc = 8,
     .ide = (uint8_t)CAN_Id_Extended,
    };

    if(!longMemoryReading(0, headerByte, HEADER_SIZE_BYTES))
    {
        vTaskDelay(portMAX_DELAY);
    }/* else not needed */
     headerFromBytes(headerByte, &extMemoryHeader);

    if(extMemoryHeader.errorQuantity == 0)
    {
        memset(&canMessage.data, 0, sizeof(canMessage.data));
        newCanTransmit(canREG1, canMESSAGE_BOX4, &canMessage);
        return;
    }/* else not needed */

    if(errorDataTocan->error == 0x00)
    {
        headerToBytes(&extMemoryHeader, canMessage.data);
        newCanTransmit(canREG1, canMESSAGE_BOX4, &canMessage);
    }
    else if (errorDataTocan->error == 0xFF)
    {
        for(uint16_t errorIndex = 1 ; errorIndex < extMemoryHeader.errorQuantity ; errorIndex++)
        {
            uint16_t addr = ERROR_ADDRESS(errorIndex);
            if( !longMemoryReading( addr, canMessage.data, ERROR_SIZE_BYTES ))
            {
                vTaskDelay( portMAX_DELAY );
            }/* else not needed */
            ErrorDataToExtMemory_t errorDataFromExtMemory;
            errorFromByte(canMessage.data, &errorDataFromExtMemory);
            newCanTransmit(canREG1, canMESSAGE_BOX4, &canMessage);
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
    else
    {
        for(uint16_t errorIndex = 1 ; errorIndex < extMemoryHeader.errorQuantity ; errorIndex++)
        {
            uint16_t addr = ERROR_ADDRESS(errorIndex);
            if( !longMemoryReading( addr, canMessage.data, ERROR_SIZE_BYTES ))
            {
                vTaskDelay( portMAX_DELAY );
            }/* else not needed */
            ErrorDataToExtMemory_t errorDataFromExtMemory;
            errorFromByte(canMessage.data, &errorDataFromExtMemory);
            if(errorDataFromExtMemory.error == errorDataTocan->error )
            {
                newCanTransmit(canREG1, canMESSAGE_BOX4, &canMessage);
                vTaskDelay(pdMS_TO_TICKS(5));
            }/* else not needed */

        }
    }
}
static void clearExternalMemory()
{
    extMemoryHeader.errorQuantity = 0;
    extMemoryHeader.crc8 = 0xFF;
    extMemoryHeader.rewritringToExternalMemory = 0;

    uint8_t headerByte[4] = {0};
    headerToBytes(&extMemoryHeader, headerByte);

    if(!longMemoryWriting(0, headerByte, HEADER_SIZE_BYTES))
    {
        vTaskDelay(portMAX_DELAY);
    }/* else not needed */
}

static void errorToBytes(const ErrorDataToExtMemory_t *errorData, uint8_t data[])
{
    data[0] = (uint8_t) errorData->time;
    data[1] = (uint8_t)(errorData->time >> 8);
    data[2] = (uint8_t)(errorData->time >> 16);
    data[3] = (uint8_t)(errorData->time >> 24);
    data[4] = errorData->error;
    data[5] = crc8(data, 5);
}
static void errorFromByte(const uint8_t data[], ErrorDataToExtMemory_t *errorData)
{
    errorData->time = (uint32_t)data[0]         |
                      ((uint32_t)data[1] >> 8)  |
                      ((uint32_t)data[2] >> 16) |
                      ((uint32_t)data[3] >> 24);
    errorData->error   = (causingOfError_t)data[4];
    errorData->crc = data[5];
}
static void headerToBytes(const ExtMemoryHeader_t *extMemoryHeader, uint8_t header[])
{
    header[0] = (uint8_t)extMemoryHeader->errorQuantity;
    header[1] = (uint8_t)(extMemoryHeader->errorQuantity >> 8);
    header[2] = (uint8_t)extMemoryHeader->rewritringToExternalMemory;
    header[3] = crc8(header, 3);
}
static void headerFromBytes(const uint8_t data[], ExtMemoryHeader_t *extMemoryHeader)
{
    extMemoryHeader->errorQuantity              = (uint16_t)data[0] |
                                                  ((uint16_t)data[1] >> 8);
    extMemoryHeader->rewritringToExternalMemory = (uint8_t)data[2];
    extMemoryHeader->crc8                       = (uint8_t)data[3];
}
static bool longMemoryWriting( uint16_t address, uint8_t tx_data[], uint8_t size)
{
    cy14_status_register_t memoryBoard;
    uint8_t reiterationCount = 50;
    bool result = false;

    for(;;)
    {
        uint8_t operationResult = readMemoryStatusRegisterFromTask( 0, &memoryBoard);
        if(operationResult && memoryBoard.statusRegister.RDY == CY14_READY)
        {
            result =  (bool) writeDataBufferToMemoryFromTask( 0, address, tx_data, size);
            if(result)
                break;/* else not needed */
        }/* else not needed */
        reiterationCount--;
        if(reiterationCount == 0)
            break;/* else not needed */
        vTaskDelay(50);
    }
    return result;
}

static bool longMemoryReading( uint16_t address, uint8_t rx_data[], uint8_t size )
{
    cy14_status_register_t memoryBoard;
    uint32_t reiterationCount = 50;
    bool result = false;

    while(1)
    {
        uint8_t operationResult = readMemoryStatusRegisterFromTask( 0, &memoryBoard );
        if( operationResult && memoryBoard.statusRegister.RDY == CY14_READY )
        {
           result = (bool) readMemoryFromTask( 0, address, rx_data, size );
           if( result )
               break;/* else not needed */
        }/* else not needed */
        reiterationCount--;
        if( reiterationCount == 0 )
            break;/* else not needed */
        vTaskDelay( 50 );
    }
    return result;
}
