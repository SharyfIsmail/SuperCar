/*
 * sys_main.h
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_SYS_MAIN_H_
#define INCLUDE_USER_SYS_MAIN_H_

#include "FreeRTOS.h"
#include "os_event_groups.h"
#include <stdint.h>


extern EventGroupHandle_t canMessageLostCheckEventGroup;

#define MASK(x) ((uint8_t)( 1 <<(x)))       /*0- Inverter canMessageLost    */
                                            /*1- BMS canMessageLost         */
                                            /*2- Accelerator canMessageLost */
                                            /*3- Brake canMessageLost       */
                                            /*4- Joystick canMessageLost    */
                                            /*5- Dcdc canMessageLost        */
                                            /*6- checks if errors were fixed*/


#endif /* INCLUDE_USER_SYS_MAIN_H_ */
