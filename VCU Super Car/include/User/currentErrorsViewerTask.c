/*
 * currentErrorsViewerTask.c
 *
 *  Created on: 7 апр. 2020 г.
 *      Author: User
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define ERROR_VIEWER_PERIOD   ((uint32_t) 1000)

#define ERROR_ON_SEMICRON     ((uint8_t) 43)
#define ERROR_LOST_COMPONENTS ((uint8_t) 6)
#define ERROR_HET             ((uint8_t) 1)
#define ERROR_EXTERNAL_MEMORY ((uint8_t) 1)

void vErrorViewer(void *pvParameters);

void currentErrorViewerInit(void)
{
    if(xTaskCreate(vErrorViewer, "ErrorViewer", configMINIMAL_STACK_SIZE, (void*)ERROR_VIEWER_PERIOD, 1, NULL) != pdTRUE)
    {
        /*Task couldn't be created */
        while(1);
    }/* else not needed */
}


void vErrorViewer(void *pvParameters)
{

}
