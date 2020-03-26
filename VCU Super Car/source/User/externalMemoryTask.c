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
#define ERROR_SIZE_BYTES   ((uint8_t) 8)
#define MAX_ERROR_QUANTITY ((uint16_t) 7999))
#define ERROR_ADDRESS(number) (HEADER_SIZE_BYTES + ((uint16_t)number)*ERROR_SIZE_BYTES)

QueueHandle_t xQueueCommandToExtMemory = NULL;

void vExternalMemoryTask(void *pvParameters);
static void writeErrorToExtMemory(const ErrorDataToExtMemory_t *errorData );
static void errorToBytes(const ErrorDataToExtMemory_t *errorData, uint8_t data[]);
static bool longMemoryWriting( uint16_t address, uint8_t tx_data[], uint8_t size);
typedef struct
{
    uint16_t errorQuantity;
    uint8_t clearErrorFlag;
    uint8_t crc8;
}ExtMemoryHeader_t;
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

}
static void errorToBytes(const ErrorDataToExtMemory_t *errorData, uint8_t data[])
{
    data[0] = (uint8_t) errorData->time;
    data[1] = (uint8_t)(errorData->time >> 8);
    data[2] = (uint8_t)(errorData->time >> 16);
    data[3] = (uint8_t)(errorData->time >> 24);
    data[4] = errorData->error;
}
static bool longMemoryWriting( uint16_t address, uint8_t tx_data[], uint8_t size )
{
    cy14_status_register_t memoryBoard;
    uint8_t reiterationCount = 200;
    bool result = false;

    for(;;)
    {
        uint8_t operationResult = readMemoryStatusRegisterFromTask( 0, &memoryBoard );
        if(operationResult && memoryBoard.statusRegister.RDY == CY14_READY)
        {
            if(result = writeDataBufferToMemoryFromTask( 0, address, tx_data, size ))
                break;
        }
        reiterationCount--;
        if(reiterationCount == 0)
            break;
        vTaskDelay(50);
    }
    return result;
}
