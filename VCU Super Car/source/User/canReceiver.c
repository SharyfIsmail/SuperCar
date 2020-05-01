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
#include "bms.h"
#include "dcdc.h"
#include "timeTask.h"

static void byteToError(const uint8_t data[],CommandToExtMemory_t *memoryCommand);

void canHighLevelIrqMessageNotification(canBASE_t *node, uint32 messageBox)
{
    static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    semicronTxCanFrame_t semicronTxCanFrame;
    BmsTxCanFrame_t BmsTxCanFrame;

    if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX15))
    {
        semicronTxCanFrame.id = canGetID(canREG1, canMESSAGE_BOX15) >> 18U;
        canGetData(canREG1, canMESSAGE_BOX15, semicronTxCanFrame.p.data);
        xQueueSendFromISR(xQueueSemikronTx, &semicronTxCanFrame, &xHigherPriorityTaskWoken );
    }/* else not needed */
    else if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX16))
    {
        semicronTxCanFrame.id = canGetID(canREG1, canMESSAGE_BOX16) >> 18U;
        canGetData(canREG1, canMESSAGE_BOX16, semicronTxCanFrame.p.data);
        xQueueSendFromISR(xQueueSemikronTx, &semicronTxCanFrame, &xHigherPriorityTaskWoken );
    }/* else not needed */
    else if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX17))
    {
        BmsTxCanFrame.id = canGetID(canREG1, canMESSAGE_BOX17);
        canGetData(canREG1, canMESSAGE_BOX17, BmsTxCanFrame.p.data);
        xQueueSendFromISR(xQueueBmsTx, &BmsTxCanFrame, &xHigherPriorityTaskWoken );
    }

}

void canLowLevelIrqMessageNotification(canBASE_t *node, uint32 messageBox)
{
    static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    ABPeadlSelector_t aBPeadlSelector ;
    CommandToExtMemory_t commandToExternalMemory;
    TimeSet_t timeSet;
    canMessage_t Dcdc;
    if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX18))
    {
        aBPeadlSelector.id = canGetID(canREG1, canMESSAGE_BOX18);
        canGetData(canREG1, canMESSAGE_BOX18, aBPeadlSelector.p.data);
        xQueueSendFromISR(xQueueABPeadlSelectorTx, &aBPeadlSelector, &xHigherPriorityTaskWoken);
    }/* else not needed */
    else if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX19))
    {
        aBPeadlSelector.id = canGetID(canREG1, canMESSAGE_BOX19);
        canGetData(canREG1, canMESSAGE_BOX19, aBPeadlSelector.p.data);
        xQueueSendFromISR(xQueueABPeadlSelectorTx, &aBPeadlSelector, &xHigherPriorityTaskWoken);
    }/* else not needed */
    else if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX20))
    {
        uint8_t data[8] = {0};
        canGetData(canREG1, canMESSAGE_BOX20, data);
        canGetID(canREG1, canMESSAGE_BOX20) >> 18U;
        byteToError(data, &commandToExternalMemory);
        xQueueSendFromISR(xQueueCommandToExtMemory, &commandToExternalMemory, &xHigherPriorityTaskWoken);

    }/* else not needed */
    else if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX21))
    {

        canGetData(canREG1, canMESSAGE_BOX21, timeSet.data);
        timeSet.id =canGetID(canREG1, canMESSAGE_BOX21) >> 18U;
        xQueueSendFromISR(xQueueRealTimeSet, &timeSet, &xHigherPriorityTaskWoken);

    }/* else not needed */
    else if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX22))
    {
        canGetData(canREG1, canMESSAGE_BOX22, Dcdc.data);
        Dcdc.id =canGetID(canREG1, canMESSAGE_BOX22) >> 18U;
        xQueueSendFromISR(xQueueDcdc, &Dcdc, &xHigherPriorityTaskWoken);
    }
}

static void byteToError(const uint8_t data[],CommandToExtMemory_t *memoryCommand )
{
    memoryCommand->errorData.error = (causingOfError_t)data[0];
    memoryCommand->type = (CommandToExtMemoryEnum_t) data[1];
}
