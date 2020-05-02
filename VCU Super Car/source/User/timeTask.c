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

struct
{
    uint8_t tm_sec;      /* seconds after the minute   - [0,59]  */
    uint8_t tm_min;      /* minutes after the hour     - [0,59]  */
    uint8_t tm_hour;     /* hours after the midnight   - [0,23]  */
    uint8_t tm_mday;     /* day of the month           - [1,31]  */
    uint8_t tm_mon;      /* months since January       - [0,11]  */
    uint8_t tm_year;     /* years since 1900                     */
}currentDate ;
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
static uint8_t getYears(datetime_t realTime);
static uint8_t getMonths(datetime_t realTime);
static uint8_t getDays(datetime_t realTime);
static uint8_t getHours(datetime_t realTime);
static uint8_t getMinutes(datetime_t realTime);
static uint8_t getSeconds(datetime_t realTime);
static void parseTimeToCan(canMessage_t* ptr);
static void setTimeInDate(datetime_t realTime);

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
               //currentDateTest = localtime(&realTime);

               setTimeInDate(realTime);
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
   // realTime = getDifferenceDateAndTime();
    //DS1904_WriteDateTime(realTime);
}
static uint8_t getYears(datetime_t realTime)
{
    return (20 + (realTime/ 31556926));
}
static uint8_t getMonths(datetime_t realTime)
{
    uint8_t tempMonth = realTime/ 2629743;
    while(tempMonth > 12)
    {
        tempMonth = tempMonth - 12;
    }
    return  1 + tempMonth;
}
static uint8_t getDays(datetime_t realTime)
{
    uint8_t _temp_ = 1;
    uint8_t temp = 31;
    uint16_t tempDay = realTime/ 86400 ;
    while(tempDay > 31)
    {
        switch(_temp_)
        {
              case 1:
                  temp = 31;
                  _temp_++;
                  break;
              case 2:
                  temp = 28;
                  _temp_++;
                  break;
              case 3:
                  temp = 31;
                  _temp_++;
                  break;
              case 4:
                  temp = 30;
                  _temp_++;

                  break;
              case 5:
                  temp = 31;
                  _temp_++;

                  break;
              case 6:
                  temp = 30;
                  _temp_++;

                  break;
              case 7:
                  temp = 31;
                  _temp_++;

                  break;
              case 8:
                  temp = 31;
                  _temp_++;

                  break;
              case 9:
                  temp = 30;
                  _temp_++;

                  break;
              case 10:
                  temp = 31;
                  _temp_++;

                  break;
              case 11:
                  temp = 30;
                  _temp_++;

                  break;
              case 12:
                  temp = 31;
                  _temp_ = 1;;
                  break;

        }
        tempDay = tempDay - temp;
    }
    return (uint8_t) tempDay;
}

static uint8_t getHours(datetime_t realTime)
{
    uint16_t tempHour = realTime/ 3600;
     while(tempHour > 23)
     {
         tempHour = tempHour - 24;
     }
     return (uint8_t) tempHour;
}
static uint8_t getMinutes(datetime_t realTime)
{
    uint32_t tempMinute = realTime/ 60;
    while(tempMinute > 59)
    {
        tempMinute = tempMinute - 60;
    }
    return (uint8_t)(1 + tempMinute);
}
static uint8_t getSeconds(datetime_t realTime)
{
    while(realTime > 59)
    {
        realTime = realTime - 60;
    }
    return  realTime;
}
static void parseTimeToCan(canMessage_t* ptr)
{
    ptr->data[0] = currentDate.tm_year;
    ptr->data[1] = currentDate.tm_mon;
    ptr->data[2] = currentDate.tm_mday;
    ptr->data[3] = currentDate.tm_hour;
    ptr->data[4] = currentDate.tm_min;
    ptr->data[5] = currentDate.tm_sec;
//      ptr->data[0] = currentDateTest->tm_yday;
//      ptr->data[1] = currentDateTest->tm_mon;
//      ptr->data[2] = currentDateTest->tm_mday;
//      ptr->data[3] = currentDateTest->tm_hour;
//      ptr->data[4] = currentDateTest->tm_min;
//      ptr->data[5] = currentDateTest->tm_sec;

}
static void setTimeInDate(datetime_t realTime)
{
    currentDate.tm_year = getYears(realTime);
    currentDate.tm_mon =  getMonths(realTime);
    currentDate.tm_mday = getDays(realTime);
    currentDate.tm_hour = getHours(realTime);
    currentDate.tm_min = getMinutes(realTime);
    currentDate.tm_sec = getSeconds(realTime);
}
