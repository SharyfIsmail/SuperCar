/*
 * timeTask.h
 *
 *  Created on: 6 апр. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_TIMETASK_H_
#define INCLUDE_USER_TIMETASK_H_

#include "ds1904.h"
#include "n2het_onewire.h"
#include "externalMemoryTask.h"

typedef struct
{
    uint32_t id;
    uint8_t data[8];
}TimeSet_t;
extern QueueHandle_t xQueueRealTime;
extern QueueHandle_t xQueueRealTimeSet;

void timerTaskInit(void);
void logError(causingOfError_t cause);

#endif /* INCLUDE_USER_TIMETASK_H_ */
