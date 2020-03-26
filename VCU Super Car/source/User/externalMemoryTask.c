/*
 * externalMemoryTask.c
 *
 *  Created on: 23 мар. 2020 г.
 *      Author: User
 */
#include "externalMemoryTask.h"
#include "memory_access.h"
//#include <stdint.h>

#define HEADER_SIZE_BYTES  ((uint8_t) 4)
#define ERROR_SIZE_BYTES   ((uint8_t) 5)
#define MAX_ERROR_QUANTITY ((uint16_t) 7999))
#define ERROR_ADDRESS(number) (HEADER_SIZE_BYTES + ((uint16_t)number)*ERROR_SIZE_BYTES)

QueueHandle_t xQueueCommandToExtMemory = NULL;
typedef struct
{
    uint16_t errorQuantity;
    uint8_t clearErrorFlag;
    uint8_t crc8;
}ExtMemoryHeader_t;

void vExternalMemoryTask(void *pvParameters);
static void writeErrorToExtMemory(const ErrorDataToExtMemory_t *errorData );
static void errorToBytes(const ErrorDataToExtMemory_t *errorData, uint8_t data[]);
static void errorFromByte(const uint8_t data[], ErrorDataToExtMemory_t *errorData);
static bool longMemoryWriting( uint16_t address, uint8_t tx_data[], uint8_t size);
static bool longMemoryReading( uint16_t address, uint8_t rx_data[], uint8_t size);
static void headerToBytes(ExtMemoryHeader_t *extMemoryHeader, uint8_t header[]);
static void headerFromBytes(const uint8_t data[], ExtMemoryHeader_t *extMemoryHeader);



static ExtMemoryHeader_t extMemoryHeader = {0};
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
            }
        }
    }
}
static void writeErrorToExtMemory(const ErrorDataToExtMemory_t *errorData)
{
    uint8_t data[5] = {0};
    errorToBytes(errorData, data);
    uint16_t addr = ERROR_ADDRESS(extMemoryHeader.errorQuantity);
    if(!longMemoryWriting(addr, data, ERROR_SIZE_BYTES))
    {
        vTaskDelay(portMAX_DELAY);
    }

}
static void errorToBytes(const ErrorDataToExtMemory_t *errorData, uint8_t data[])
{
    data[0] = (uint8_t) errorData->time;
    data[1] = (uint8_t)(errorData->time >> 8);
    data[2] = (uint8_t)(errorData->time >> 16);
    data[3] = (uint8_t)(errorData->time >> 24);
    data[4] = errorData->error;
}
static void errorFromByte(const uint8_t data[], ErrorDataToExtMemory_t *errorData)
{
    errorData->time = (uint32_t)data[0]         |
                      ((uint32_t)data[1] >> 8)  |
                      ((uint32_t)data[2] >> 16) |
                      ((uint32_t)data[3] >> 24);
    errorData->error   = (causingOfError_t)data[4];
}
static void headerToBytes(ExtMemoryHeader_t *extMemoryHeader, uint8_t header[])
{
    header[0] = (uint8_t)extMemoryHeader->errorQuantity;
    header[1] = (uint8_t)(extMemoryHeader->errorQuantity >> 8);
    header[2] = (uint8_t)extMemoryHeader->clearErrorFlag;
    header[3] = (uint8_t)extMemoryHeader->crc8;
}
static void headerFromBytes(const uint8_t data[], ExtMemoryHeader_t *extMemoryHeader)
{
    extMemoryHeader->errorQuantity = (uint16_t)data[0] |
                                     ((uint16_t)data[1] >> 8);
    extMemoryHeader->clearErrorFlag   = (uint8_t)data[2];
    extMemoryHeader->crc8             = (uint8_t)data[3];

}
static bool longMemoryWriting( uint16_t address, uint8_t tx_data[], uint8_t size )
{
    cy14_status_register_t memoryBoard;
    uint8_t reiterationCount = 50;
    bool result = false;

    for(;;)
    {
        uint8_t operationResult = readMemoryStatusRegisterFromTask( 0, &memoryBoard );
        if(operationResult && memoryBoard.statusRegister.RDY == CY14_READY)
        {
            result =  (bool) writeDataBufferToMemoryFromTask( 0, address, tx_data, size );
            if(result)
                break;
        }
        reiterationCount--;
        if(reiterationCount == 0)
            break;
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
               break;
        }
        reiterationCount--;
        if( reiterationCount == 0 )
            break;
        vTaskDelay( 50 );

    }
    return result;
}
