/*
 * canReceiver.c
 *
 *  Created on: 6 ���. 2020 �.
 *      Author: User
 */

#include "newCanLib.h"
#include "SemikronTx.h"
#include "os_queue.h"

void canHighLevelIrqMessageNotification(canBASE_t *node, uint32 messageBox)
{
    static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    semicronTxCanFrame_t semicronTxCanFrame;

    if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX16))
    {
        semicronTxCanFrame.id = canGetID(canREG1, canMESSAGE_BOX16) >> 18U;
        canGetData(canREG1, canMESSAGE_BOX16, semicronTxCanFrame.p.data);
        xQueueSendFromISR(xQueueSemikronTx, &semicronTxCanFrame, &xHigherPriorityTaskWoken );
    }
    if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX17))
    {
        semicronTxCanFrame.id = canGetID(canREG1, canMESSAGE_BOX17) >> 18U;
        canGetData(canREG1, canMESSAGE_BOX16, semicronTxCanFrame.p.data);
        xQueueSendFromISR(xQueueSemikronTx, &semicronTxCanFrame, &xHigherPriorityTaskWoken );
    }
}

void canLowLevelIrqMessageNotification(canBASE_t *node, uint32 messageBox)
{

}
