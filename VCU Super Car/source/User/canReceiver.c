/*
 * canReceiver.c
 *
 *  Created on: 6 мар. 2020 г.
 *      Author: User
 */

#include "newCanLib.h"
#include "SemikronTx.h"
#include "acceleratorBrakeJoystick.h"
#include "externalMemoryTask.h"

static void byteToError(const uint8_t data[],CommandToExtMemory_t *memoryCommand);

void canHighLevelIrqMessageNotification(canBASE_t *node, uint32 messageBox)
{
    static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    semicronTxCanFrame_t semicronTxCanFrame;

    if(canIsRxMessageArrived(canREG2, canMESSAGE_BOX9))
    {
        semicronTxCanFrame.id = canGetID(canREG2, canMESSAGE_BOX9) >> 18U;
        canGetData(canREG2, canMESSAGE_BOX9, semicronTxCanFrame.p.data);
        xQueueSendFromISR(xQueueSemikronTx, &semicronTxCanFrame, &xHigherPriorityTaskWoken );
    }/* else not needed */
    else if(canIsRxMessageArrived(canREG2, canMESSAGE_BOX10))
    {
        semicronTxCanFrame.id = canGetID(canREG2, canMESSAGE_BOX10) >> 18U;
        canGetData(canREG2, canMESSAGE_BOX10, semicronTxCanFrame.p.data);
        xQueueSendFromISR(xQueueSemikronTx, &semicronTxCanFrame, &xHigherPriorityTaskWoken );
    }/* else not needed */

}

void canLowLevelIrqMessageNotification(canBASE_t *node, uint32 messageBox)
{
    static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    ABPeadlSelector_t aBPeadlSelector ;
    CommandToExtMemory_t commandToExternalMemory;

    if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX15))
    {
        aBPeadlSelector.id = canGetID(canREG1, canMESSAGE_BOX15) >> 18U;
        canGetData(canREG1, canMESSAGE_BOX15, aBPeadlSelector.p.data);
        xQueueSendFromISR(xQueueABPeadlSelectorTx, &aBPeadlSelector, &xHigherPriorityTaskWoken);
    }/* else not needed */

    else if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX16))
    {
        uint8_t data[8] = {0};
        canGetData(canREG1, canMESSAGE_BOX16, data);
        canGetID(canREG1, canMESSAGE_BOX16) >> 18U;
        byteToError(data, &commandToExternalMemory);
        xQueueSendFromISR(xQueueCommandToExtMemory, &commandToExternalMemory, &xHigherPriorityTaskWoken);

    }/* else not needed */
}

static void byteToError(const uint8_t data[],CommandToExtMemory_t *memoryCommand )
{
    memoryCommand->errorData.error = (causingOfError_t)data[0];
    memoryCommand->type = (CommandToExtMemoryEnum_t) data[1];
}
