/*
 * canReceiver.c
 *
 *  Created on: 6 мар. 2020 г.
 *      Author: User
 */

#include "newCanLib.h"
#include "SemikronTx.h"
#include "acceleratorBrakeJoystick.h"

void canHighLevelIrqMessageNotification(canBASE_t *node, uint32 messageBox)
{
    static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    semicronTxCanFrame_t semicronTxCanFrame;

    if(canIsRxMessageArrived(canREG2, canMESSAGE_BOX9))
    {
        semicronTxCanFrame.id = canGetID(canREG2, canMESSAGE_BOX9) >> 18U;
        canGetData(canREG2, canMESSAGE_BOX9, semicronTxCanFrame.p.data);
        xQueueSendFromISR(xQueueSemikronTx, &semicronTxCanFrame, &xHigherPriorityTaskWoken );
    }
    if(canIsRxMessageArrived(canREG2, canMESSAGE_BOX10))
    {
        semicronTxCanFrame.id = canGetID(canREG2, canMESSAGE_BOX10) >> 18U;
        canGetData(canREG2, canMESSAGE_BOX10, semicronTxCanFrame.p.data);
        xQueueSendFromISR(xQueueSemikronTx, &semicronTxCanFrame, &xHigherPriorityTaskWoken );
    }

}

void canLowLevelIrqMessageNotification(canBASE_t *node, uint32 messageBox)
{
    static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    acceleratorBrakeJoystick_t acceleratorBrakeJoystick ;
    if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX15))
    {
        acceleratorBrakeJoystick.id = canGetID(canREG1, canMESSAGE_BOX15) >> 18U;
        canGetData(canREG1, canMESSAGE_BOX15, acceleratorBrakeJoystick.p.data);
        xQueueSendFromISR(xQueueAcceleratorBrakeJoystickTx, &acceleratorBrakeJoystick, &xHigherPriorityTaskWoken );
    }
}
