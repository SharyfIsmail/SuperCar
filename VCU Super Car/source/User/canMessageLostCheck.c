/*
 * canMessageLostCheck.c
 *
 *  Created on: 12 мар. 2020 г.
 *      Author: User
 */

#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"
#include "sys_main.h"
#include "newCanLib.h"
#include "canMessageLostCheck.h"

TaskHandle_t xCanMessageLostCheckHandler;

/*QueueHandle_t xQueueInvertorLost = NULL;
QueueHandle_t xQueueBmsLost = NULL;
QueueHandle_t xQueueAcceleratorLost = NULL;
QueueHandle_t xQueueBrakeLost = NULL;
QueueHandle_t xQueueJoystickLost = NULL;
QueueHandle_t xQueueDcdcLost = NULL;*/
canMessageLoses_t canMessageLossesQueue ;
void vCanMessageLostCheckHandler(void *pvParameters);

void canMessageLostCheckInit(void)
{
    if(xTaskCreate(vCanMessageLostCheckHandler, "CanMessageLostCheckHandler", configMINIMAL_STACK_SIZE, NULL, 1, &xCanMessageLostCheckHandler) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }
    for(int i = 0 ; i < IP_COUNT_OF_PERIPHS ; i++)
    {
        canMessageLossesQueue.arr[i] =  xQueueCreate(1U, sizeof(bool));
    }
    /*xQueueInvertorLost = xQueueCreate(1U, sizeof(bool));
    xQueueBmsLost = xQueueCreate(1U, sizeof(bool));
    xQueueAcceleratorLost = xQueueCreate(1U, sizeof(bool));
    xQueueBrakeLost = xQueueCreate(1U, sizeof(bool));
    xQueueJoystickLost = xQueueCreate(1U, sizeof(bool));
    xQueueDcdcLost = xQueueCreate(1U, sizeof(bool));*/

}
static void setCausingError(EventBits_t getLost)
{
    canMessageLost_t canMessageLoses =
    {
      .arr = {false}
    };


    if(getInverterLost(getLost))
    {
        canMessageLoses.arr[0]  = true;
        xQueueOverwrite(canMessageLossesQueue.arr[0], &canMessageLoses.arr[0] );
    }

    if(getBmsLost(getLost))
    {
        canMessageLoses.arr[1]  = true;
        xQueueOverwrite(canMessageLossesQueue.arr[1], &canMessageLoses.arr[1] );
    }

    if(getAcceleratorLost(getLost))
    {
        canMessageLoses.arr[2]  = true;
        xQueueOverwrite(canMessageLossesQueue.arr[2], &canMessageLoses.arr[2] );
    }

    if(getBrakeLost(getLost))
    {
        canMessageLoses.arr[3]  = true;
        xQueueOverwrite(canMessageLossesQueue.arr[3], &canMessageLoses.arr[3] );
    }

    if(getJoystickLost(getLost))
    {
        canMessageLoses.arr[4]  = true;
        xQueueOverwrite(canMessageLossesQueue.arr[4], &canMessageLoses.arr[4] );
    }

    if(getDcLost(getLost))
    {
        canMessageLoses.arr[5]  = true;
        xQueueOverwrite(canMessageLossesQueue.arr[5], &canMessageLoses.arr[5] );
    }
}
void vCanMessageLostCheckHandler(void *pvParameters)
{
   // TickType_t lastWeakTime;
  //  TickType_t transmitPeriod = pdMS_TO_TICKS( (uint32_t) pvParameters );

    canMessage_t causingOfLost=
    {
     .id = 0x01,
     .dlc = 8,
     .ide = CAN_Id_Standard
    };
    const EventBits_t numberOfLost = 0x3F;
    uint8_t ErrorToSend = 0;
    EventBits_t  getLost = 0;
    //lastWeakTime = xTaskGetTickCount();

    for(;;)
    {
        getLost = xEventGroupWaitBits(canMessageLostCheckEventGroup, numberOfLost, pdTRUE, pdFALSE, portMAX_DELAY);
        ErrorToSend |= getLost;

        causingOfLost.data[0] = ErrorToSend;

        setCausingError(getLost);

        newCanTransmit(canREG1, canMESSAGE_BOX7, &causingOfLost);
      //  vTaskDelayUntil( &lastWeakTime, pdMS_TO_TICKS(100));
    }
}

