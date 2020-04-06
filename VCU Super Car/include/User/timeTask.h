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


extern QueueHandle_t xQueueRealTime;
void timerTaskInit(void);
void ReadRealTime(datetime_t *realTime);

#endif /* INCLUDE_USER_TIMETASK_H_ */
