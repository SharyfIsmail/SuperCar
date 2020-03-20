/*
 * SemikronTx.c
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */
#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"
#include "newCanLib.h"
#include "SemikronTx.h"
#include "SemikronRx.h"
#include "sys_main.h"
#include "vcuStateManagement.h"

void vSemicronTxHandler (void *pvParameters);

TaskHandle_t xSemicronTxHandler;
QueueHandle_t xQueueSemikronTx = NULL;
QueueHandle_t xQueueCausingError = NULL;
void semikronTxInit(void)
{
    if(xTaskCreate(vSemicronTxHandler, "SemicronTxHandler", configMINIMAL_STACK_SIZE, NULL, 1, &xSemicronTxHandler ) != pdTRUE)
    {
           /*Task couldn't be created */
           while(1);
    }


    xQueueSemikronTx = xQueueCreate(20U, sizeof(semicronTxCanFrame_t));
    xQueueCausingError = xQueueCreate(1U, sizeof(clearError_t));
}

static void checkErrorsOnInverter(emdTxPdo01_t *emdTxPdo_01)
{
    VcuStateMangement_t vcuStatus;
    if(getTx_PDO_01_CausingError(emdTxPdo_01))
    {
        vcuStatus = VCU_CLEAR_ERROR;
        xQueueOverwrite(xQueueVcuStatusManagement, &vcuStatus);
    }
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
