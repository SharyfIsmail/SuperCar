/*
 * ds1904.h
 *
 *  Created on: Sep 20, 2019
 *      Author: Dmitriy Shulpenkov
 */

#ifndef INCLUDE_DS1904_H_
#define INCLUDE_DS1904_H_

/* -------------- DEFINES -------------- */

/* Clock function commands */
#define READ_CLOCK_CMD              0x66
#define WRITE_CLOCK_CMD             0x99

/* Device control byte value with oscillator states */
#define OSC_ON_DEVICE_CONTROL_BYTE  0x0C
#define OSC_OFF_DEVICE_CONTROL_BYTE 0x00

/* -------------- TYPES -------------- */

typedef unsigned int datetime_t;

/* -------------- DEFINES -------------- */

/* DS1904 Interface Functions */
void DS1904_WriteDateTime(datetime_t);
void DS1904_ReadDateTime(datetime_t *);
datetime_t getDifferenceDateAndTime();

#endif /* INCLUDE_DS1904_H_ */
