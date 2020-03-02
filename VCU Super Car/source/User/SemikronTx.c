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

void vSemicronTxHandler (void *pvParameters);

TaskHandle_t xSemicronTxHandler;
QueueHandle_t xQueueSemikronTx = NULL;


void semikronTxInit(void)
{
    if(xTaskCreate(vSemicronTxHandler, "SemicronTxHandler", configMINIMAL_STACK_SIZE, NULL, 1, &xSemicronTxHandler ) != pdTRUE)
    {
           /*Task couldn't be created */
           while(1);
    }

    xQueueSemikronTx = xQueueCreate(20U, sizeof(12));

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
        if(semicronTxCanFrame.id < emdTxPdo_02)
        {
            if(semicronTxCanFrame.id == emdTxPdo_01)    // emdTxPdo_01
            {

            }
            else                                        // emdTxPdo_05
            {

            }
        }

        else if(semicronTxCanFrame.id > emdTxPdo_02)
        {
            if(semicronTxCanFrame.id == emdTxPdo_03)    // emdTxPdo_03
            {

            }
            else                                        // emdTxPdo_04
            {

            }
        }

        else                                            // emdTxPdo_02
        {

        }
        taskYIELD();
    }
}
