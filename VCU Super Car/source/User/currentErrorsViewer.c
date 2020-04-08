/*
 * currentErrorsViewer.c
 *
 *  Created on: 7 апр. 2020 г.
 *      Author: User
 */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "newCanLib.h"

#define ERROR_VIEWER_PERIOD   ((uint32_t) 2000)

#define ERROR_ON_SEMICRON     ((uint8_t) 8)// 8 byte
#define ERROR_LOST_COMPONENTS ((uint8_t) 1)// 1 byte
#define ERROR_HET             ((uint8_t) 1)// 1 byte
#define ERROR_EXTERNAL_MEMORY ((uint8_t) 1)// 1 byte

static void parseError(canMessage_t* currentErrors ,const uint64_t semicronBitError, const uint8_t lostComponentsError,
                                                                        const uint8_t hetError);

QueueHandle_t queueCurrentSemicronError = NULL;
QueueHandle_t queueLostComponentsError = NULL;
QueueHandle_t queueHetError = NULL;
QueueHandle_t queueExternalMemoryError = NULL;

void vErrorViewer(void *pvParameters);
static canMessage_t currentErrors =
{
    .id  = 0x01,
    .dlc = 8,
    .ide = (uint8_t)CAN_Id_Standard,
    .data = {0},
};

void currentErrorViewerInit(void)
{
    if(xTaskCreate(vErrorViewer, "ErrorViewer", configMINIMAL_STACK_SIZE, (void*)ERROR_VIEWER_PERIOD, 1, NULL) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
    queueCurrentSemicronError = xQueueCreate(1U, ERROR_ON_SEMICRON);
    queueLostComponentsError = xQueueCreate(1U, ERROR_LOST_COMPONENTS);
    queueHetError = xQueueCreate(1U, ERROR_HET);
    queueExternalMemoryError = xQueueCreate(1U, ERROR_EXTERNAL_MEMORY);

}


void vErrorViewer(void *pvParameters)
{

    TickType_t lastWakeTime ;
    TickType_t transmitPeriod = pdMS_TO_TICKS((uint32_t) pvParameters);
    uint64_t semicronBitError = 0;
    uint8_t lostComponentsError = 0;
    uint8_t hetError = 0 ;
    lastWakeTime = xTaskGetTickCount();
    for(;;)
    {
        xQueuePeek(queueCurrentSemicronError, &semicronBitError, pdMS_TO_TICKS(0));
        xQueuePeek(queueLostComponentsError, &lostComponentsError, pdMS_TO_TICKS(0));
        xQueuePeek(queueHetError, &hetError, pdMS_TO_TICKS(0));
        parseError(&currentErrors, semicronBitError, lostComponentsError, hetError);
        newCanTransmit(canREG1, canMESSAGE_BOX3, &currentErrors);
        vTaskDelayUntil(&lastWakeTime, transmitPeriod);
    }
}

static void parseError(canMessage_t* currentErrors ,const uint64_t semicronBitError, const uint8_t lostComponentsError,
                                                                        const uint8_t hetError)
{
    currentErrors->data[0] = lostComponentsError  |
                     (hetError << 6)      ;
    currentErrors->data[1] = (uint8_t)  semicronBitError;
    currentErrors->data[2] = (uint8_t) (semicronBitError >> 8);
    currentErrors->data[3] = (uint8_t) (semicronBitError >> 16);
    currentErrors->data[4] = (uint8_t) (semicronBitError >> 24);
    currentErrors->data[5] = (uint8_t) (semicronBitError >> 32);
    currentErrors->data[6] = (uint8_t) (semicronBitError >> 40);
}


