/*
 * crc8.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: User
 */

#ifndef INCLUDE_USER_CRC8_H_
#define INCLUDE_USER_CRC8_H_

#include <stdint.h>
#include <stdbool.h>

uint8_t crc8(const uint8_t *pcBlock, uint32_t len);

void WriteToCanFrameCrc8(uint8_t data[], uint8_t dlc);
bool CheckCanFrameCrc8(const uint8_t data[], uint8_t dlc);

#endif /* INCLUDE_USER_CRC8_H_ */
