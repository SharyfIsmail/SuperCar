/*
 * spi_helper.h
 *
 *  Created on: Oct 10, 2019
 *      Author: Dmitriy Shulpenkov
 */

#ifndef INCLUDE_SPI_HELPER_H_
#define INCLUDE_SPI_HELPER_H_


#include "spi.h"


#define SPI1_MODULE_ENABLE                      1
#define SPI2_MODULE_ENABLE                      0
#define SPI3_MODULE_ENABLE                      0

#define SPI1_INTERRUPT_ENABLE                   1
#define SPI2_INTERRUPT_ENABLE                   0
#define SPI3_INTERRUPT_ENABLE                   0

#define SPI1_HIGH_LVL_INTERRUPT_VIM_CHANNEL     12U
#define SPI1_LOW_LVL_INTERRUPT_VIM_CHANNEL      26U
#define SPI2_HIGH_LVL_INTERRUPT_VIM_CHANNEL     17U
#define SPI2_LOW_LVL_INTERRUPT_VIM_CHANNEL      30U
#define SPI3_HIGH_LVL_INTERRUPT_VIM_CHANNEL     37U
#define SPI3_LOW_LVL_INTERRUPT_VIM_CHANNEL      38U

#define SPI_RECEIVE_BUFFER_FULL_IRQ             36U
#define SPI_TRANSMIT_BUFFER_EMPTY_IRQ           40U

#define SPI_TRANSMISSION_WORD_LENGTH_MASK       0x0000001FU


void spiSendDataBuffer(spiBASE_t *spi, spiDAT1_t *dataConfig, uint8_t blockSize, uint8_t * srcBuff);
void spiGetDataBuffer(spiBASE_t *spi, spiDAT1_t *dataConfig, uint8_t blockSize, uint8_t * destBuff);
//uint8_t spiGetLengthOfTransmissionWord(spiBASE_t *spi, SPIDATAFMT_t dataFormat);
void spiGetConfigValue(spiBASE_t *spi, spi_config_reg_t *config_reg);


#if SPI1_INTERRUPT_ENABLE
    void spi1HighLevelInterrupt();
    void spi1LowLevelInterrupt();
#endif

#ifdef SPI2_INTERRUPT_ENABLE
    void spi2HighLevelInterrupt();
    void spi2LowLevelInterrupt();
#endif

#ifdef SPI3_INTERRUPT_ENABLE
    void spi3HighLevelInterrupt();
    void spi3LowLevelInterrupt();
#endif


#endif /* INCLUDE_SPI_HELPER_H_ */
