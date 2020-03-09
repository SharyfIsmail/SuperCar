/*
 * canReceiver.c
 *
 *  Created on: 6 мар. 2020 г.
 *      Author: User
 */

#include "newCanLib.h"
#include "SemikronTx.h"
#include "selector.h"

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
        canGetData(canREG1, canMESSAGE_BOX17, semicronTxCanFrame.p.data);
        xQueueSendFromISR(xQueueSemikronTx, &semicronTxCanFrame, &xHigherPriorityTaskWoken );
    }

}

void canLowLevelIrqMessageNotification(canBASE_t *node, uint32 messageBox)
{
    static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    selectorTx_t selectorTx ;
    if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX18))
    {
        selectorTx.id = canGetID(canREG1, canMESSAGE_BOX18) >> 18U;
        canGetData(canREG1, canMESSAGE_BOX18, selectorTx.data);
        xQueueSendFromISR(xQueueSelectorTx, &selectorTx, &xHigherPriorityTaskWoken );
    }
}
