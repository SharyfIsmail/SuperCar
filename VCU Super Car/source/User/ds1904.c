/*
 * ds1904.c
 *
 *  Created on: Sep 20, 2019
 *      Author: Dmitriy Shulpenkov
 */

#include "ds1904.h"
#include "n2het_onewire.h"
#include "time.h"

/** @fn void DS1904_WriteDateTime(datetime_t value)
*   @brief Writes time in seconds in DS1904
*   @param[in] value Recordable time in seconds
*
*   This function writes time in seconds in DS1904.
*/
void DS1904_WriteDateTime(datetime_t value)
{
    OneWire_WriteByte(WRITE_CLOCK_CMD);

    OneWire_WriteByte(OSC_ON_DEVICE_CONTROL_BYTE);

    for(uint8_t i = 0, size = sizeof(datetime_t); i < size; i++)
        OneWire_WriteByte((uint8_t)(value >> (i * 8)));

    while(!OneWire_TryReset());
}

/** @fn void DS1904_ReadDateTime(datetime_t *pDateTime)
*   @brief Reads current time in seconds from DS1904
*   @param[out] pDateTime Pointer to time in seconds
*
*   This function reads current time in seconds from DS1904.
*/
void DS1904_ReadDateTime(datetime_t *pDateTime)
{
    OneWire_WriteByte(READ_CLOCK_CMD);

    uint8_t deviceControlByte = 0;
    while(!OneWire_TryReadByte(&deviceControlByte));

    for(uint8_t i = 0, value = 0, size = sizeof(datetime_t); i < size;){
        if(OneWire_TryReadByte(&value)){
            *pDateTime |= ((datetime_t)value << (i * 8));
            i++;
        }
    }

    while(!OneWire_TryReset());
}

/** @fn datetime_t getDifferenceDateAndTime()
*   @brief Get the difference between 00:00:00 1.01.2019 and current calendar time in seconds
*   @return This function will return the time difference in seconds.
*
*   This function calculates the difference between 00:00:00 1.01.2019 and current calendar time in seconds.
*/
datetime_t getDifferenceDateAndTime()
{
    time_t currentDateTime;
    struct tm * startingPointDateTime;

    /* Get current time in seconds */
    time(&currentDateTime);

    /* Set starting point calendar time in seconds - 00:00:00 1.01.2019 */
    startingPointDateTime = localtime(&currentDateTime);
    startingPointDateTime->tm_sec = 0;
    startingPointDateTime->tm_min = 0;
    startingPointDateTime->tm_hour = 0;
    startingPointDateTime->tm_mday = 1;
    startingPointDateTime->tm_mon = 0;
    startingPointDateTime->tm_year = 120; /* 2019 - 1900 */
    startingPointDateTime->tm_wday = 0;
    startingPointDateTime->tm_yday = 0;

    /* Convert structure tm to time_t */
    time_t startingDateTime = mktime(startingPointDateTime);
    /* Calculates the difference between 00:00:00 1.01.2019 and calendar time in seconds. */
    return (datetime_t)difftime(currentDateTime, startingDateTime);
}
