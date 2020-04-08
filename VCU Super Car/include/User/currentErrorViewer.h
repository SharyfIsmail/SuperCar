/*
 * currentErrorViewer.h
 *
 *  Created on: 7 апр. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_CURRENTERRORVIEWER_H_
#define INCLUDE_USER_CURRENTERRORVIEWER_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t queueCurrentSemicronError;
extern QueueHandle_t queueLostComponentsError;
extern QueueHandle_t queueHetError;
extern QueueHandle_t queueExternalMemoryError;

void currentErrorViewerInit(void);

#endif /* INCLUDE_USER_CURRENTERRORVIEWER_H_ */
