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
#include "currentErrorViewer.h"
#include "newCanLib.h"
#include "time.h"

#define DS1904_FAMILYCODE  ((uint8_t) 0x24)
#define TIME_TASK_PERIOD ((uint32_t) 1000)

static struct tm * currentDateTest;
static canMessage_t dataTime =
{
    .id  = 0x6DA,
    .dlc = 8,
    .ide = (uint8_t)CAN_Id_Standard,
    .data = {0},
};

static bool errorlogIsWrote = false;
QueueHandle_t xQueueRealTime = NULL;
QueueHandle_t xQueueRealTimeSet = NULL;

void vTimeTask(void *pvParameters);
void vTimeTaskSet(void *pvParameters);
static void ReadRealTime(datetime_t *realTime);
static void writeRealTime (datetime_t realTime);
static void parseTimeToCan(canMessage_t* ptr);

void timerTaskInit(void)
{
    OneWire_Init();
    OneWire_TryReset();

    if(xTaskCreate(vTimeTask, "TimeTask", configMINIMAL_STACK_SIZE, (void*)TIME_TASK_PERIOD, 2, NULL) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
    if(xTaskCreate(vTimeTaskSet, "TimeTaskSet", configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */

    xQueueRealTime = xQueueCreate(1U, sizeof(datetime_t));
    xQueueRealTimeSet = xQueueCreate(1U, sizeof(TimeSet_t));
}
void vTimeTaskSet(void *pvParameters)
{
    TimeSet_t timeSet;
    for(;;)
    {
        if(xQueueReceive(xQueueRealTimeSet, &timeSet, portMAX_DELAY))
        {
            uint32_t time = (uint32_t) timeSet.data[3]        |
                            (uint32_t)(timeSet.data[2] << 8 ) |
                            (uint32_t)(timeSet.data[1] << 16) |
                            (uint32_t)(timeSet.data[0] << 24);

            writeRealTime(time);
        }
        taskYIELD();
    }
}
void vTimeTask(void *pvParameters)
{
    TickType_t lastWeakTime;
    TickType_t transmitPeriod = pdMS_TO_TICKS( (uint32_t) pvParameters);
    causingOfError_t causingOfError = EVERYTHING_IS_FINE;
    uint8_t hetIsFine = 0;

    lastWeakTime = xTaskGetTickCount();
    for(;;)
    {

        datetime_t realTime = (datetime_t) 0;
        const owDeviceCode_t *deviceCode = OneWire_ReadROM();
        if(deviceCode->familyCode == DS1904_FAMILYCODE)
        {
               hetIsFine = 0;
               ReadRealTime(&realTime);
               currentDateTest = localtime(&realTime);

             //  setTimeInDate(realTime);
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
            hetIsFine = 1;
        }

        xQueueOverwrite(xQueueRealTime, &realTime);
        xQueueOverwrite(queueHetError, &hetIsFine);
         //setTimeInDate();
        parseTimeToCan(&dataTime);
        newCanTransmit(canREG1, canMESSAGE_BOX7, &dataTime);

        vTaskDelayUntil( &lastWeakTime, transmitPeriod);
    }
}
static void ReadRealTime(datetime_t *realTime)
{
    OneWire_TryReset();
    OneWire_SkipROM();
    DS1904_ReadDateTime(realTime);

}
static void writeRealTime (datetime_t realTime)
{
    OneWire_TryReset();
    OneWire_SkipROM();
    DS1904_WriteDateTime(realTime);
//    realTime = getDifferenceDateAndTime();
//    OneWire_TryReset();
//    OneWire_SkipROM();
//    DS1904_WriteDateTime(realTime);
}
static void parseTimeToCan(canMessage_t* ptr)
{
      ptr->data[0] = currentDateTest->tm_year + 20;
      ptr->data[1] = currentDateTest->tm_mon + 1;
      ptr->data[2] = currentDateTest->tm_mday  ;
      ptr->data[3] = currentDateTest->tm_hour;
      ptr->data[4] = currentDateTest->tm_min;
      ptr->data[5] = currentDateTest->tm_sec;

}
