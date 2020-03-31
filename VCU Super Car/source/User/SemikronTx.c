/*
 * SemikronTx.c
 *
 *  Created on: 25 ����. 2020 �.
 *      Author: User
 */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "newCanLib.h"
#include "SemikronTx.h"
#include "SemikronRx.h"
#include "sys_main.h"
#include "vcuStateManagement.h"
#include "externalMemoryTask.h"
#include "string.h"

void vSemicronTxHandler (void *pvParameters);
static void logError(causingOfError_t cause);
static void checkErrorsOnInverter(emdTxPdo01_t *emdTxPdo_01);
static uint8_t errorSeek(causingOfError_t *cause);
static void errorOccured(uint8_t errorIndex);

static  semicronErrorWrite_t semicronErrorWrite =
{
 .arr = {false}
};

static uint8_t errorId[SEMICRON_ERROR_COUNT] =
{
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
  0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
  0x15, 0x16, 0x17, 0x18, 0x19, 0x27, 0x2B, 0x2C, 0x30, 0x32,
  0x33, 0x36, 0x37, 0x49, 0x4F, 0x65, 0x67, 0x7F, 0x9F, 0xA1,
  0xE1, 0xE8, 0xFF
};

TaskHandle_t xSemicronTxHandler;
QueueHandle_t xQueueSemikronTx = NULL;
QueueHandle_t xQueueCausingError = NULL;
void semikronTxInit(void)
{
    if(xTaskCreate(vSemicronTxHandler, "SemicronTxHandler", configMINIMAL_STACK_SIZE, NULL, 1, &xSemicronTxHandler ) != pdTRUE)
    {
           /*Task couldn't be created */
           while(1);
    }/* else not needed */


    xQueueSemikronTx = xQueueCreate(20U, sizeof(semicronTxCanFrame_t));
    xQueueCausingError = xQueueCreate(1U, sizeof(clearError_t));
}


void vSemicronTxHandler (void *pvParameters)
{
    semicronTxCanFrame_t  semicronTxCanFrame;

    emdTxPdo01_t *emdTxPdo_01 = &semicronTxCanFrame.p.emdTxPdo01;
    emdTxPdo02_t *emdTxPdo_02 = &semicronTxCanFrame.p.emdTxPdo02;
    emdTxPdo03_t *emdTxPdo_03 = &semicronTxCanFrame.p.emdTxPdo03;
    emdTxPdo04_t *emdTxPdo_04 = &semicronTxCanFrame.p.emdTxPdo04;
    emdTxPdo05_t *emdTxPdo_05 = &semicronTxCanFrame.p.emdTxPdo05;
    while(1)
    {
        if(xQueueReceive(xQueueSemikronTx, &semicronTxCanFrame, pdMS_TO_TICKS(5000)))
        {
            xEventGroupClearBits(canMessageLostCheckEventGroup, MASK(0U));

            if(semicronTxCanFrame.id < EMD_TxPDO_2)
            {
                if(semicronTxCanFrame.id == EMD_TxPDO_1)    // emdTxPdo_01
                {
                    checkErrorsOnInverter(emdTxPdo_01);
                }
                else                                        // emdTxPdo_05
                {

                }
            }

            else if(semicronTxCanFrame.id > EMD_TxPDO_2)
            {
                if(semicronTxCanFrame.id == EMD_TxPDO_3)    // emdTxPdo_03
                {

                }
                else                                        // emdTxPdo_04
                {


                }
            }

            else                                            // emdTxPdo_02
            {

            }
        }
        else
        {
            xEventGroupSetBits(canMessageLostCheckEventGroup, MASK(0U));
        }

        taskYIELD();
    }
}

static void checkErrorsOnInverter(emdTxPdo01_t *emdTxPdo_01)
{
    VcuStateMangement_t vcuStatus;
    causingOfError_t causingOfError = EVERYTHING_IS_FINE;
    causingOfError = (causingOfError_t)getTx_PDO_01_CausingError(emdTxPdo_01);
    if(causingOfError != EVERYTHING_IS_FINE)
    {
        vcuStatus = VCU_CLEAR_ERROR;

        xQueueOverwrite(xQueueVcuStatusManagement, &vcuStatus);

        uint8_t errorIndex = errorId[errorSeek(&causingOfError)];
        if(!semicronErrorWrite.arr[errorIndex])
            logError(causingOfError); /* else not needed */

        errorOccured(errorIndex);
    }/* else not needed */
}

static void logError(causingOfError_t cause)
{
    uint32_t errorTime = 0;
    CommandToExtMemory_t command =
    {
     .type = EXT_MEMROY_WRITE,
     .errorData =
     {
      .time = errorTime,
      .error = cause,
     }
    };
    xQueueSend(xQueueCommandToExtMemory, &command, portMAX_DELAY);
}

static uint8_t errorSeek(causingOfError_t *cause)
{
   for(int i = 0 ; i< SEMICRON_ERROR_COUNT ; i++)
   {
       if(*cause == (causingOfError_t)errorId[i])
       {
           return (uint8_t)i;
       }/* else not needed */
   }
   *cause = UNKNOWN_ERROR;
   return SEMICRON_ERROR_COUNT - 1 ;
}
static void errorOccured(uint8_t errorIndex)
{
    memset(errorId, false, sizeof(errorId));
    errorId[errorIndex] = true;
}
