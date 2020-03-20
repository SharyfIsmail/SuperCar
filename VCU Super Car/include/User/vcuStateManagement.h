/*
 * vcuStateManagement.h
 *
 *  Created on: 19 мар. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_VCUSTATEMANAGEMENT_H_
#define INCLUDE_USER_VCUSTATEMANAGEMENT_H_

#include "FreeRTOS.h"
#include "os_queue.h"


extern QueueHandle_t xQueueVcuStatus;
extern QueueHandle_t xQueueVcuStatusManagement;

void vcuStateManagementInit(void);

typedef enum
{
    VCU_Status_Init            = 0x00,
    VCU_CLEAR_ERROR            = 0x01,
    VCU_Status_Parking         = 0x04,
    VCU_Status_Neutral         = 0x10,
    VCU_Status_Forward         = 0x11,
    VCU_Status_Reverse         = 0x12,
    VCU_Status_Charge          = 0x02,
    VCU_Status_Sleep           = 0x03,
    VCU_Status_ErrorDrive      = 0x20,
    VCU_Status_ErrorStop       = 0x21,
    VCU_Status_ErrorBatteryOff = 0x22,
}VcuStateMangement_t;


#endif /* INCLUDE_USER_VCUSTATEMANAGEMENT_H_ */
