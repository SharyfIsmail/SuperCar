/*
 * timeTask.c
 *
 *  Created on: 6 апр. 2020 г.
 *      Author: User
 */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timeTask.h"
#include "externalMemoryTask.h"

#define DS1904_FAMILYCODE  ((uint8_t) 0x24)
#define TIME_TASK_PERIOD ((uint32_t) 500)

void vTimeTask(void *pvParameters);

static void ReadRealTime(datetime_t *realTime);
static bool errorlogIsWrote = false;
QueueHandle_t xQueueRealTime = NULL;

void timerTaskInit(void)
{
    OneWire_Init();
    OneWire_TryReset();

    if(xTaskCreate(vTimeTask, "TimeTask", configMINIMAL_STACK_SIZE, (void*)TIME_TASK_PERIOD, 2, NULL) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
    xQueueRealTime = xQueueCreate(1U, sizeof(datetime_t));
}
void vTimeTask(void *pvParameters)
{
    TickType_t lastWeakTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS( (uint32_t) pvParameters);
    causingOfError_t causingOfError = EVERYTHING_IS_FINE;

    bool hetIsFine = false;
    lastWeakTime = xTaskGetTickCount();
    for(;;)
    {
        datetime_t realTime = (datetime_t) 0;
        const owDeviceCode_t *deviceCode = OneWire_ReadROM();
        if(deviceCode->familyCode == DS1904_FAMILYCODE)
        {
               hetIsFine = true;
               ReadRealTime(&realTime);
               errorlogIsWrote = false;
        }
        else
        {
            realTime = 0xFFFFFFFF;
            causingOfError = N2HET_ERROR;
            if(errorlogIsWrote == false)
            {
                logError(causingOfError);
            }/* else not needed */
            errorlogIsWrote = true;
            hetIsFine = false;
        }

        xQueueOverwrite(xQueueRealTime, &realTime);
        vTaskDelayUntil( &lastWeakTime, transmitPeriod);
    }
}
static void ReadRealTime(datetime_t *realTime)
{
    OneWire_TryReset();
    OneWire_SkipROM();
    DS1904_ReadDateTime(realTime);
}
