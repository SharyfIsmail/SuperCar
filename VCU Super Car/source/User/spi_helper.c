/*
 * spi_helper.c
 *
 *  Created on: Oct 10, 2019
 *      Author: Dmitriy Shulpenkov
 */

#include "spi_helper.h"

#define MASK(x) ((unsigned int)(1 << (x)))
#define BYTE_MASK   (255UL)

/** @struct spiBuffer
*   @brief
*
*/
static volatile struct spiBuffer
{
    spiDAT1_t spiDataFormat;
    uint8_t txDataLength;
    uint8_t rxDataLength;
    uint8_t *txData;
    uint8_t *rxData;
} spiBuffer[3U];



static void spi1ModuleInit(void);
static void spi2ModuleInit(void);
static void spi3ModuleInit(void);

/** @fn void spiInit(void)
*   @brief Initializes the SPI Driver
*
*   This function initializes the SPI module.
*/
void spiInit(void)
{
#if (SPI1_MODULE_ENABLE == 1)
    /** Initialize SPI1 */
    spi1ModuleInit();
#endif

#if (SPI2_MODULE_ENABLE == 1)
    /** Initialize SPI2 */
    spi2ModuleInit();
#endif

#if (SPI3_MODULE_ENABLE == 1)
    /** Initialize SPI3 */
    spi3ModuleInit();
#endif
}

static void spi1ModuleInit(void)
{
    /** Bring SPI out of reset */
    spiREG1->GCR0 = 0U;
    spiREG1->GCR0 = 1U;

    /** SPI1 master mode and clock configuration */
    spiREG1->GCR1 = (spiREG1->GCR1 & 0xFFFFFFFCU) | ((uint32)((uint32)1U << 1U)  /* CLOKMOD */
                  | 1U);  /* MASTER */

    /** SPI1 enable pin configuration */
    spiREG1->INT0 = (spiREG1->INT0 & 0xFEFFFFFFU)| (uint32)((uint32)0U << 24U);  /* ENABLE HIGHZ */

    /** Delays */
    spiREG1->DELAY = (uint32)((uint32)78U << 24U)  /* C2TDELAY */
                   | (uint32)((uint32)79U << 16U)  /* T2CDELAY */
                   | (uint32)((uint32)0U << 8U)   /* T2EDELAY */
                   | (uint32)((uint32)0U << 0U);  /* C2EDELAY */

    /** Data Format 0 */
    spiREG1->FMT0 = (uint32)((uint32)0U << 24U)  /* wdelay */
                  | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                  | (uint32)((uint32)0U << 22U)  /* parity enable */
                  | (uint32)((uint32)0U << 21U)  /* wait on enable */
                  | (uint32)((uint32)0U << 20U)  /* shift direction */
                  | (uint32)((uint32)1U << 17U)  /* clock polarity */
                  | (uint32)((uint32)0U << 16U)  /* clock phase */
                  | (uint32)((uint32)79U << 8U) /* baudrate prescale */
                  | (uint32)((uint32)8U << 0U);  /* data word length */

    /** Data Format 1 */
    spiREG1->FMT1 = (uint32)((uint32)0U << 24U)  /* wdelay */
                  | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                  | (uint32)((uint32)0U << 22U)  /* parity enable */
                  | (uint32)((uint32)0U << 21U)  /* wait on enable */
                  | (uint32)((uint32)0U << 20U)  /* shift direction */
                  | (uint32)((uint32)1U << 17U)  /* clock polarity */
                  | (uint32)((uint32)0U << 16U)  /* clock phase */
                  | (uint32)((uint32)1U << 8U) /* baudrate prescale */
                  | (uint32)((uint32)8U << 0U);  /* data word length */

    /** Data Format 2 */
    spiREG1->FMT2 = (uint32)((uint32)0U << 24U)  /* wdelay */
                  | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                  | (uint32)((uint32)0U << 22U)  /* parity enable */
                  | (uint32)((uint32)0U << 21U)  /* wait on enable */
                  | (uint32)((uint32)0U << 20U)  /* shift direction */
                  | (uint32)((uint32)1U << 17U)  /* clock polarity */
                  | (uint32)((uint32)0U << 16U)  /* clock phase */
                  | (uint32)((uint32)1U << 8U) /* baudrate prescale */
                  | (uint32)((uint32)16U << 0U);  /* data word length */

    /** Data Format 3 */
    spiREG1->FMT3 = (uint32)((uint32)0U << 24U)  /* wdelay */
                  | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                  | (uint32)((uint32)0U << 22U)  /* parity enable */
                  | (uint32)((uint32)0U << 21U)  /* wait on enable */
                  | (uint32)((uint32)0U << 20U)  /* shift direction */
                  | (uint32)((uint32)1U << 17U)  /* clock polarity */
                  | (uint32)((uint32)0U << 16U)  /* clock phase */
                  | (uint32)((uint32)1U << 8U) /* baudrate prescale */
                  | (uint32)((uint32)16U << 0U);  /* data word length */

    /** Set interrupt levels */
    spiREG1->LVL = (uint32)((uint32)0U << 9U)  /* TXINT */
                 | (uint32)((uint32)0U << 8U)  /* RXINT */
                 | (uint32)((uint32)1U << 6U)  /* OVRNINT */
                 | (uint32)((uint32)1U << 4U)  /* BITERR */
                 | (uint32)((uint32)0U << 3U)  /* DESYNC */
                 | (uint32)((uint32)0U << 2U)  /* PARERR */
                 | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                 | (uint32)((uint32)0U << 0U);  /* DLENERR */

    /** Clear any pending interrupts */
    spiREG1->FLG |= 0xFFFFU;

    /** Enable interrupts */
    spiREG1->INT0 = (spiREG1->INT0 & 0xFFFF0000U)
                  | (uint32)((uint32)0U << 9U)  /* TXINT */
                  | (uint32)((uint32)0U << 8U)  /* RXINT */
                  | (uint32)((uint32)1U << 6U)  /* OVRNINT */
                  | (uint32)((uint32)1U << 4U)  /* BITERR */
                  | (uint32)((uint32)0U << 3U)  /* DESYNC */
                  | (uint32)((uint32)0U << 2U)  /* PARERR */
                  | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                  | (uint32)((uint32)0U << 0U);  /* DLENERR */

    /** Initialize SPI1 Port */

    /** SPI1 Port output values */
    spiREG1->PC3 =    (uint32)((uint32)1U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)0U << 9U)  /* CLK */
                    | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U);  /* SOMI[0] */

    /** SPI1 Port direction */
    spiREG1->PC1  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)1U << 9U)  /* CLK */
                    | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U); /* SOMI[0] */

    /** SPI1 Port open drain enable */
    spiREG1->PC6  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)0U << 9U)  /* CLK */
                    | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U); /* SOMI[0] */

    /** SPI1 Port pullup / pulldown selection */
    spiREG1->PC8  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)1U << 8U)  /* ENA */
                    | (uint32)((uint32)1U << 9U)  /* CLK */
                    | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U); /* SOMI[0] */

    /** SPI1 Port pullup / pulldown enable*/
    spiREG1->PC7  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)0U << 9U)  /* CLK */
                    | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U); /* SOMI[0] */

    /* SPI1 set all pins to functional */
    spiREG1->PC0  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)1U << 9U)  /* CLK */
                    | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)1U << 11U); /* SOMI[0] */

    /** Finally start SPI1 */
    spiREG1->GCR1 = (spiREG1->GCR1 & 0xFEFFFFFFU) | 0x01000000U;
}

static void spi2ModuleInit(void)
{
    /** Bring SPI out of reset */
    spiREG2->GCR0 = 0U;
    spiREG2->GCR0 = 1U;

    /** SPI2 master mode and clock configuration */
    spiREG2->GCR1 = (spiREG2->GCR1 & 0xFFFFFFFCU) | ((uint32)((uint32)1U << 1U)  /* CLOKMOD */
                  | 1U);  /* MASTER */

    /** SPI2 enable pin configuration */
    spiREG2->INT0 = (spiREG2->INT0 & 0xFEFFFFFFU)| (uint32)((uint32)0U << 24U);  /* ENABLE HIGHZ */

    /** Delays */
    spiREG2->DELAY = (uint32)((uint32)22U << 24U)  /* C2TDELAY */
                   | (uint32)((uint32)23U << 16U)  /* T2CDELAY */
                   | (uint32)((uint32)0U << 8U)   /* T2EDELAY */
                   | (uint32)((uint32)0U << 0U);  /* C2EDELAY */

    /** Data Format 0 */
    spiREG2->FMT0 = (uint32)((uint32)0U << 24U)  /* wdelay */
                  | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                  | (uint32)((uint32)0U << 22U)  /* parity enable */
                  | (uint32)((uint32)0U << 21U)  /* wait on enable */
                  | (uint32)((uint32)0U << 20U)  /* shift direction */
                  | (uint32)((uint32)1U << 17U)  /* clock polarity */
                  | (uint32)((uint32)0U << 16U)  /* clock phase */
                  | (uint32)((uint32)1U << 8U) /* baudrate prescale */
                  | (uint32)((uint32)8U << 0U);  /* data word length */

    /** Data Format 1 */
    spiREG2->FMT1 = (uint32)((uint32)0U << 24U)  /* wdelay */
                  | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                  | (uint32)((uint32)0U << 22U)  /* parity enable */
                  | (uint32)((uint32)0U << 21U)  /* wait on enable */
                  | (uint32)((uint32)0U << 20U)  /* shift direction */
                  | (uint32)((uint32)1U << 17U)  /* clock polarity */
                  | (uint32)((uint32)0U << 16U)  /* clock phase */
                  | (uint32)((uint32)1U << 8U) /* baudrate prescale */
                  | (uint32)((uint32)8U << 0U);  /* data word length */

    /** Data Format 2 */
    spiREG2->FMT2 = (uint32)((uint32)0U << 24U)  /* wdelay */
                  | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                  | (uint32)((uint32)0U << 22U)  /* parity enable */
                  | (uint32)((uint32)0U << 21U)  /* wait on enable */
                  | (uint32)((uint32)0U << 20U)  /* shift direction */
                  | (uint32)((uint32)1U << 17U)  /* clock polarity */
                  | (uint32)((uint32)0U << 16U)  /* clock phase */
                  | (uint32)((uint32)1U << 8U) /* baudrate prescale */
                  | (uint32)((uint32)16U << 0U);  /* data word length */

    /** Data Format 3 */
    spiREG2->FMT3 = (uint32)((uint32)0U << 24U)  /* wdelay */
                  | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                  | (uint32)((uint32)0U << 22U)  /* parity enable */
                  | (uint32)((uint32)0U << 21U)  /* wait on enable */
                  | (uint32)((uint32)0U << 20U)  /* shift direction */
                  | (uint32)((uint32)1U << 17U)  /* clock polarity */
                  | (uint32)((uint32)0U << 16U)  /* clock phase */
                  | (uint32)((uint32)1U << 8U) /* baudrate prescale */
                  | (uint32)((uint32)16U << 0U);  /* data word length */

    /** Set interrupt levels */
    spiREG2->LVL = (uint32)((uint32)0U << 9U)  /* TXINT */
                 | (uint32)((uint32)0U << 8U)  /* RXINT */
                 | (uint32)((uint32)1U << 6U)  /* OVRNINT */
                 | (uint32)((uint32)1U << 4U)  /* BITERR */
                 | (uint32)((uint32)0U << 3U)  /* DESYNC */
                 | (uint32)((uint32)0U << 2U)  /* PARERR */
                 | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                 | (uint32)((uint32)0U << 0U);  /* DLENERR */

    /** Clear any pending interrupts */
    spiREG2->FLG |= 0xFFFFU;

    /** Enable interrupts */
    spiREG2->INT0 = (spiREG2->INT0 & 0xFFFF0000U)
                  | (uint32)((uint32)0U << 9U)  /* TXINT */
                  | (uint32)((uint32)0U << 8U)  /* RXINT */
                  | (uint32)((uint32)1U << 6U)  /* OVRNINT */
                  | (uint32)((uint32)1U << 4U)  /* BITERR */
                  | (uint32)((uint32)0U << 3U)  /* DESYNC */
                  | (uint32)((uint32)0U << 2U)  /* PARERR */
                  | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                  | (uint32)((uint32)0U << 0U);  /* DLENERR */

    /** Initialize SPI2 Port */

    /** SPI2 Port output values */
    spiREG2->PC3 =    (uint32)((uint32)1U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)0U << 9U)  /* CLK */
                    | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U);  /* SOMI[0] */

    /** SPI2 Port direction */
    spiREG2->PC1  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)1U << 9U)  /* CLK */
                    | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U); /* SOMI[0] */

    /** SPI2 Port open drain enable */
    spiREG2->PC6  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)0U << 9U)  /* CLK */
                    | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U); /* SOMI[0] */

    /** SPI2 Port pullup / pulldown selection */
    spiREG2->PC8  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)1U << 8U)  /* ENA */
                    | (uint32)((uint32)1U << 9U)  /* CLK */
                    | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U); /* SOMI[0] */

    /** SPI2 Port pullup / pulldown enable*/
    spiREG2->PC7  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)0U << 9U)  /* CLK */
                    | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U); /* SOMI[0] */

    /* SPI2 set all pins to functional */
    spiREG2->PC0  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)1U << 9U)  /* CLK */
                    | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)1U << 11U); /* SOMI[0] */

    /** Finally start SPI2 */
    spiREG2->GCR1 = (spiREG2->GCR1 & 0xFEFFFFFFU) | 0x01000000U;
}

static void spi3ModuleInit(void)
{
    /** Bring SPI out of reset */
    spiREG3->GCR0 = 0U;
    spiREG3->GCR0 = 1U;

    /** SPI3 master mode and clock configuration */
    spiREG3->GCR1 = (spiREG3->GCR1 & 0xFFFFFFFCU) | ((uint32)((uint32)1U << 1U)  /* CLOKMOD */
                  | 1U);  /* MASTER */

    /** SPI3 enable pin configuration */
    spiREG3->INT0 = (spiREG3->INT0 & 0xFEFFFFFFU)| (uint32)((uint32)0U << 24U);  /* ENABLE HIGHZ */

    /** Delays */
    spiREG3->DELAY = (uint32)((uint32)22U << 24U)  /* C2TDELAY */
                   | (uint32)((uint32)23U << 16U)  /* T2CDELAY */
                   | (uint32)((uint32)0U << 8U)   /* T2EDELAY */
                   | (uint32)((uint32)0U << 0U);  /* C2EDELAY */

    /** Data Format 0 */
    spiREG3->FMT0 = (uint32)((uint32)0U << 24U)  /* wdelay */
                  | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                  | (uint32)((uint32)0U << 22U)  /* parity enable */
                  | (uint32)((uint32)0U << 21U)  /* wait on enable */
                  | (uint32)((uint32)0U << 20U)  /* shift direction */
                  | (uint32)((uint32)1U << 17U)  /* clock polarity */
                  | (uint32)((uint32)0U << 16U)  /* clock phase */
                  | (uint32)((uint32)1U << 8U) /* baudrate prescale */
                  | (uint32)((uint32)8U << 0U);  /* data word length */

    /** Data Format 1 */
    spiREG3->FMT1 = (uint32)((uint32)0U << 24U)  /* wdelay */
                  | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                  | (uint32)((uint32)0U << 22U)  /* parity enable */
                  | (uint32)((uint32)0U << 21U)  /* wait on enable */
                  | (uint32)((uint32)0U << 20U)  /* shift direction */
                  | (uint32)((uint32)1U << 17U)  /* clock polarity */
                  | (uint32)((uint32)0U << 16U)  /* clock phase */
                  | (uint32)((uint32)1U << 8U) /* baudrate prescale */
                  | (uint32)((uint32)8U << 0U);  /* data word length */

    /** Data Format 2 */
    spiREG3->FMT2 = (uint32)((uint32)0U << 24U)  /* wdelay */
                  | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                  | (uint32)((uint32)0U << 22U)  /* parity enable */
                  | (uint32)((uint32)0U << 21U)  /* wait on enable */
                  | (uint32)((uint32)0U << 20U)  /* shift direction */
                  | (uint32)((uint32)1U << 17U)  /* clock polarity */
                  | (uint32)((uint32)0U << 16U)  /* clock phase */
                  | (uint32)((uint32)1U << 8U) /* baudrate prescale */
                  | (uint32)((uint32)16U << 0U);  /* data word length */

    /** Data Format 3 */
    spiREG3->FMT3 = (uint32)((uint32)0U << 24U)  /* wdelay */
                  | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                  | (uint32)((uint32)0U << 22U)  /* parity enable */
                  | (uint32)((uint32)0U << 21U)  /* wait on enable */
                  | (uint32)((uint32)0U << 20U)  /* shift direction */
                  | (uint32)((uint32)1U << 17U)  /* clock polarity */
                  | (uint32)((uint32)0U << 16U)  /* clock phase */
                  | (uint32)((uint32)1U << 8U) /* baudrate prescale */
                  | (uint32)((uint32)16U << 0U);  /* data word length */

    /** Set interrupt levels */
    spiREG3->LVL = (uint32)((uint32)0U << 9U)  /* TXINT */
                 | (uint32)((uint32)0U << 8U)  /* RXINT */
                 | (uint32)((uint32)1U << 6U)  /* OVRNINT */
                 | (uint32)((uint32)1U << 4U)  /* BITERR */
                 | (uint32)((uint32)0U << 3U)  /* DESYNC */
                 | (uint32)((uint32)0U << 2U)  /* PARERR */
                 | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                 | (uint32)((uint32)0U << 0U);  /* DLENERR */

    /** Clear any pending interrupts */
    spiREG3->FLG |= 0xFFFFU;

    /** Enable interrupts */
    spiREG3->INT0 = (spiREG3->INT0 & 0xFFFF0000U)
                  | (uint32)((uint32)0U << 9U)  /* TXINT */
                  | (uint32)((uint32)0U << 8U)  /* RXINT */
                  | (uint32)((uint32)1U << 6U)  /* OVRNINT */
                  | (uint32)((uint32)1U << 4U)  /* BITERR */
                  | (uint32)((uint32)0U << 3U)  /* DESYNC */
                  | (uint32)((uint32)0U << 2U)  /* PARERR */
                  | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                  | (uint32)((uint32)0U << 0U);  /* DLENERR */

    /** Initialize SPI3 Port */

    /** - SPI3 Port output values */
    spiREG3->PC3 =    (uint32)((uint32)1U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)0U << 9U)  /* CLK */
                    | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U);  /* SOMI[0] */

    /** - SPI3 Port direction */
    spiREG3->PC1  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)1U << 9U)  /* CLK */
                    | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U); /* SOMI[0] */

    /** - SPI3 Port open drain enable */
    spiREG3->PC6  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)0U << 9U)  /* CLK */
                    | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U); /* SOMI[0] */

    /** - SPI3 Port pullup / pulldown selection */
    spiREG3->PC8  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)1U << 8U)  /* ENA */
                    | (uint32)((uint32)1U << 9U)  /* CLK */
                    | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U); /* SOMI[0] */

    /** - SPI3 Port pullup / pulldown enable*/
    spiREG3->PC7  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)0U << 9U)  /* CLK */
                    | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)0U << 11U); /* SOMI[0] */

    /* SPI3 set all pins to functional */
    spiREG3->PC0  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                    | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                    | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                    | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                    | (uint32)((uint32)0U << 8U)  /* ENA */
                    | (uint32)((uint32)1U << 9U)  /* CLK */
                    | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                    | (uint32)((uint32)1U << 11U); /* SOMI[0] */

    /** - Finally start SPI3 */
    spiREG3->GCR1 = (spiREG3->GCR1 & 0xFEFFFFFFU) | 0x01000000U;
}

/** @fn void spiSendDataBuffer(spiBASE_t *spi, spiDAT1_t *dataConfig, uint8_t blockSize, uint8_t *srcBuff)
*   @brief Transmits Data using interrupt method
*   @param[in] spi          - Spi module base address
*   @param[in] dataConfig   - Spi DAT1 register configuration
*   @param[in] blockSize    - Number of data
*   @param[in] srcBuff      - Pointer to the source data (8 bit)
*
*   This function transmits blockSize number of data from source buffer using interrupt method.
*/
void spiSendDataBuffer(spiBASE_t *spi, spiDAT1_t *dataConfig, uint8_t blockSize, uint8_t *srcBuff)
{
    uint32_t spiModuleIndex;

    if(spi == spiREG1){
        spiModuleIndex = 0U;
    }
    else if(spi==spiREG2){
        spiModuleIndex = 1U;
    }
    else {
        spiModuleIndex = 2U;
    }

    spiBuffer[spiModuleIndex].spiDataFormat = *dataConfig;
    spiBuffer[spiModuleIndex].txDataLength = blockSize;
    spiBuffer[spiModuleIndex].txData = srcBuff;
    spiBuffer[spiModuleIndex].rxDataLength = blockSize;
    spiBuffer[spiModuleIndex].rxData = NULL;

    spi->INT0 |= 0x0300U;
}

/** @fn void spiGetDataBuffer(spiBASE_t *spi, spiDAT1_t *dataConfig, uint8_t blockSize, uint8_t *destBuff)
*   @brief Receives Data using interrupt method
*   @param[in] spi           - Spi module base address
*   @param[in] dataConfig    - Spi DAT1 register configuration
*   @param[in] blockSize     - Number of data
*   @param[in] destBuff      - Pointer to the destination data (8 bit)
*
*   This function transmits blockSize number of data from source buffer using interrupt method.
*/
void spiGetDataBuffer(spiBASE_t *spi, spiDAT1_t *dataConfig, uint8_t blockSize, uint8_t *destBuff)
{
    uint32_t spiModuleIndex;

    if(spi == spiREG1){
        spiModuleIndex = 0U;
    }
    else if(spi==spiREG2){
        spiModuleIndex = 1U;
    }
    else {
        spiModuleIndex = 2U;
    }

    spiBuffer[spiModuleIndex].spiDataFormat = *dataConfig;
    spiBuffer[spiModuleIndex].rxDataLength = blockSize;
    spiBuffer[spiModuleIndex].rxData = destBuff;
    spiBuffer[spiModuleIndex].txDataLength = blockSize;
    spiBuffer[spiModuleIndex].txData = NULL;

    spi->INT0 |= 0x0300U;
}

/** @fn void spiGetConfigValue(spiBASE_t *spi, spi_config_reg_t *config_reg)
*   @brief Get the initial or current values of the configuration registers
*
*   @param[in] spi          - Spi module base address
*   @param[in] config_reg   - Pointer to the struct to which the initial or current
*                             value of the configuration registers need to be stored
*
*   This function will copy the current value of the configuration registers
*   to the struct pointed by config_reg
*
*/
void spiGetConfigValue(spiBASE_t *spi, spi_config_reg_t *config_reg)
{
    config_reg->CONFIG_GCR1  = spi->GCR1;
    config_reg->CONFIG_INT0  = spi->INT0;
    config_reg->CONFIG_LVL   = spi->LVL;
    config_reg->CONFIG_PC0   = spi->PC0;
    config_reg->CONFIG_PC1   = spi->PC1;
    config_reg->CONFIG_PC6   = spi->PC6;
    config_reg->CONFIG_PC7   = spi->PC7;
    config_reg->CONFIG_PC8   = spi->PC8;
    config_reg->CONFIG_DELAY = spi->DELAY ;
    config_reg->CONFIG_FMT0  = spi->FMT0;
    config_reg->CONFIG_FMT1  = spi->FMT1;
    config_reg->CONFIG_FMT2  = spi->FMT2;
    config_reg->CONFIG_FMT3  = spi->FMT3;
}

/*uint8_t spiGetLengthOfTransmissionWord(spiBASE_t *spi, SPIDATAFMT_t dataFormat)
{
    uint8_t wordLength = 0U;

    if(dataFormat == SPI_FMT_0){
        wordLength = (spi->FMT0 & SPI_TRANSMISSION_WORD_LENGTH_MASK);
    }
    else if(dataFormat == SPI_FMT_1){
        wordLength = (spi->FMT1 & SPI_TRANSMISSION_WORD_LENGTH_MASK);
    }
    else if(dataFormat == SPI_FMT_2){
        wordLength = (spi->FMT2 & SPI_TRANSMISSION_WORD_LENGTH_MASK);
    }
    else {
        wordLength = (spi->FMT3 & SPI_TRANSMISSION_WORD_LENGTH_MASK);
    }

    return wordLength;
}*/


#if (SPI1_INTERRUPT_ENABLE == 1)
    /** @fn void spi1HighLevelInterrupt(void)
    *   @brief Level 0 Interrupt for SPI1
    */
    #pragma CODE_STATE(spi1HighLevelInterrupt, 32)
    #pragma INTERRUPT(spi1HighLevelInterrupt, IRQ)
    void spi1HighLevelInterrupt(void)
    {
        uint32_t vec = spiREG1->INTVECT0;

        if(vec == SPI_RECEIVE_BUFFER_FULL_IRQ) /* Receive Buffer Full Interrupt */
        {
             if(spiBuffer[0U].rxData)
             {
                 uint8_t *destinationBuffer;
                 destinationBuffer = spiBuffer[0U].rxData;

                 *destinationBuffer = (uint8_t)spiREG1->BUF;

                 spiBuffer[0U].rxData++;
                 spiBuffer[0U].rxDataLength--;


                 if(spiBuffer[0U].rxDataLength == 0U)
                 {
                     spiREG1->INT0 &= ~MASK(8);
                     spiEndNotification(spiREG1);
                 }
             }
             else
             {
                 spiREG1->FLG |= MASK(8);
                 spiBuffer[0U].rxDataLength--;


                 if(spiBuffer[0U].rxDataLength == 0U)
                 {
                     spiREG1->INT0 &= ~MASK(8);

                     if(spiBuffer[0U].txDataLength == 0U){
                         spiEndNotification(spiREG1);
                     }
                 }
             }
         }
         else if(vec == SPI_TRANSMIT_BUFFER_EMPTY_IRQ) /* Transmit Buffer Empty Interrupt */
         {
             uint32_t chipSelectHold;
             uint32_t wDelay = (spiBuffer[0U].spiDataFormat.WDEL) ? MASK(26) : 0U;
             SPIDATAFMT_t dataFormat = spiBuffer[0U].spiDataFormat.DFSEL;
             uint8_t chipSelect = spiBuffer[0U].spiDataFormat.CSNR;
             uint8_t txData = *spiBuffer[0U].txData;

             spiBuffer[0U].txDataLength--;

             if(spiBuffer[0U].txDataLength == 0U)
             {
                chipSelectHold = (spiBuffer[0U].spiDataFormat.CS_HOLD) ? MASK(28) : 0U;
             }
             else
             {
                 chipSelectHold = MASK(28);
             }

             if(spiBuffer[0U].txData)
             {
                 /*if(spiGetLengthOfTransmissionWord(spiREG1, dataFormat) == 16U) {
                     spiBuffer[0U].txData++;
                     txData = (txData << 8U) | *spiBuffer[0U].txData;
                     spiBuffer[0U].txData++;
                 }
                 else
                 {
                     spiBuffer[0U].txData++;
                 }*/

                 spiREG1->DAT1 = ((uint32_t)dataFormat << 24U) |
                                 ((uint32_t)chipSelect << 16U) |
                                 (wDelay)           |
                                 (chipSelectHold) |
                                 (uint32_t)txData;

                 spiBuffer[0U].txData++;
             }
             else
             {
                 spiREG1->DAT1 = ((uint32_t)dataFormat << 24U) |
                                 ((uint32_t)chipSelect << 16U) |
                                 (wDelay)           |
                                 (chipSelectHold) |
                                 (uint32_t)0x00000000U;
             }

             if(spiBuffer[0U].txDataLength == 0U)
             {
                 spiREG1->INT0 &= ~MASK(9); /* Disable Tx Interrupt */

                 if(spiBuffer[0U].rxDataLength == 0U){
                     spiEndNotification(spiREG1);
                 }
             }
         }
         else
         {
             /** Clear Interrupt */
             spiREG1->FLG &= ~BYTE_MASK;
         }
    }

    /** @fn void spi1LowLevelInterrupt(void)
    *   @brief Level 1 Interrupt for SPI1
    */
    #pragma CODE_STATE(spi1LowLevelInterrupt, 32)
    #pragma INTERRUPT(spi1LowLevelInterrupt, IRQ)
    void spi1LowLevelInterrupt()
    {
        uint32_t flags = (spiREG1->FLG & BYTE_MASK) & (spiREG1->LVL & BYTE_MASK);
        uint32_t vec = spiREG1->INTVECT1;

        //uint16_t buffer[2] = {0};
        if(vec == 0x26)
        {
            /**spiBuffer[0U].rxData = (uint16_t)spiREG1->BUF;
            spiBuffer[0U].rxData++;
            *spiBuffer[0U].rxData = (uint16_t)spiREG1->BUF;
            spiBuffer[0U].rxDataLength -= 2;*/
            //buffer[0] = (uint16_t)spiREG1->BUF;
            //buffer[1] = (uint16_t)spiREG1->BUF;
        }
        else
        {
            /* Clear Flags and return  */
            spiREG1->FLG = flags;
            spiNotification(spiREG1, flags & BYTE_MASK);
        }
    }
#endif


#if (SPI2_INTERRUPT_ENABLE == 1)
    /** @fn void spi2HighLevelInterrupt(void)
    *   @brief Level 0 Interrupt for SPI2
    */
    #pragma CODE_STATE(spi2HighLevelInterrupt, 32)
    #pragma INTERRUPT(spi2HighLevelInterrupt, IRQ)
    void spi2HighLevelInterrupt(void)
    {
        uint32_t flags = (spiREG2->FLG & 0x0000FFFFU) & (~spiREG2->LVL & 0x035FU);
        uint32_t vec = spiREG2->INTVECT0;

        switch(vec)
        {
            case 0x24U: /* Receive Buffer Full Interrupt */
                 {
                    uint16_t *destinationBuffer;
                    destinationBuffer = spiBuffer[0U].rxData;

                    *destinationBuffer = (uint16_t)spiREG2->BUF;
                    spiBuffer[0U].rxData++;
                    spiBuffer[0U].rxDataLength--;

                    if(spiBuffer[0U].rxDataLength == 0U)
                    {
                        spiREG2->INT0 = (spiREG2->INT0 & 0x0000FFFFU) & (~(uint32_t)0x0100U);
                        spiEndNotification(spiREG2);
                    }
                    break;
                 }
            case 0x28U: /* Transmit Buffer Empty Interrupt */
                 {
                     volatile uint32_t SpiBuf;
                     uint32_t chipSelectHold = 0x10000000U;
                     uint32_t wDelay = (spiBuffer[0U].spiDataFormat.WDEL) ? 0x04000000U : 0U;
                     SPIDATAFMT_t dataFormat = spiBuffer[0U].spiDataFormat.DFSEL;
                     uint8_t chipSelect = spiBuffer[0U].spiDataFormat.CSNR;
                     uint16_t txData = *spiBuffer[0U].txData;

                     spiBuffer[0U].txDataLength--;

                     if(spiBuffer[0U].txDataLength == 0U)
                     {
                        chipSelectHold = (spiBuffer[0U].spiDataFormat.CS_HOLD) ? 0x10000000U : 0U;
                     }

                     spiREG2->DAT1 = ((uint32_t)dataFormat << 24U) |
                                     ((uint32_t)chipSelect << 16U) |
                                     (wDelay)           |
                                     (chipSelectHold) |
                                     (uint32_t)txData;

                     spiBuffer[0U].txData++;
                     /* Dummy Receive read if no RX Interrupt enabled */
                     /*if(((spiREG2->INT0 & 0x0000FFFFU)& 0x0100U) == 0U)//Rx Interrupt Disable
                     {
                         if((spiREG2->FLG & 0x00000100U) == 0x00000100U)
                         {
                             SpiBuf = spiREG2->BUF;
                         }
                     }*/

                     if(spiBuffer[0U].txDataLength == 0U)
                     {
                         spiREG2->INT0 = (spiREG2->INT0 & 0x0000FFFFU) & (~(uint32)0x0200U); /* Disable Interrupt */
                         if(spiBuffer[0U].rxDataLength != 0)
                         {
                             spiREG2->INT0 |= 0x0100U;
                         }

                         spiEndNotification(spiREG2);
                     }
                     break;
                 }

            default: /* Clear Flags and return  */
                 spiREG2->FLG = flags;
                 spiNotification(spiREG2, flags & 0xFFU);
                 break;
        }
    }


    /** @fn void spi2LowLevelInterrupt(void)
    *   @brief Level 1 Interrupt for SPI2
    */
    #pragma CODE_STATE(spi2LowLevelInterrupt, 32)
    #pragma INTERRUPT(spi2LowLevelInterrupt, IRQ)
    void spi2LowLevelInterrupt()
    {

    }
#endif


#if (SPI3_INTERRUPT_ENABLE == 1)
    /** @fn void spi3HighLevelInterrupt(void)
    *   @brief Level 0 Interrupt for SPI1
    */
    #pragma CODE_STATE(spi3HighLevelInterrupt, 32)
    #pragma INTERRUPT(spi3HighLevelInterrupt, IRQ)
    void spi3HighLevelInterrupt(void)
    {
        uint32_t flags = (spiREG3->FLG & 0x0000FFFFU) & (~spiREG3->LVL & 0x035FU);
        uint32_t vec = spiREG3->INTVECT0;

        switch(vec)
        {
            case 0x24U: /* Receive Buffer Full Interrupt */
                 {
                    uint16_t *destinationBuffer;
                    destinationBuffer = spiBuffer[0U].rxData;

                    *destinationBuffer = (uint16_t)spiREG3->BUF;
                    spiBuffer[0U].rxData++;
                    spiBuffer[0U].rxDataLength--;

                    if(spiBuffer[0U].rxDataLength == 0U)
                    {
                        spiREG3->INT0 = (spiREG3->INT0 & 0x0000FFFFU) & (~(uint32_t)0x0100U);
                        spiEndNotification(spiREG3);
                    }
                    break;
                 }
            case 0x28U: /* Transmit Buffer Empty Interrupt */
                 {
                     volatile uint32_t SpiBuf;
                     uint32_t chipSelectHold = 0x10000000U;
                     uint32_t wDelay = (spiBuffer[0U].spiDataFormat.WDEL) ? 0x04000000U : 0U;
                     SPIDATAFMT_t dataFormat = spiBuffer[0U].spiDataFormat.DFSEL;
                     uint8_t chipSelect = spiBuffer[0U].spiDataFormat.CSNR;
                     uint16_t txData = *spiBuffer[0U].txData;

                     spiBuffer[0U].txDataLength--;

                     if(spiBuffer[0U].txDataLength == 0U)
                     {
                        chipSelectHold = (spiBuffer[0U].spiDataFormat.CS_HOLD) ? 0x10000000U : 0U;
                     }

                     spiREG3->DAT1 = ((uint32_t)dataFormat << 24U) |
                                     ((uint32_t)chipSelect << 16U) |
                                     (wDelay)           |
                                     (chipSelectHold) |
                                     (uint32_t)txData;

                     spiBuffer[0U].txData++;
                     /* Dummy Receive read if no RX Interrupt enabled */
                     /*if(((spiREG3->INT0 & 0x0000FFFFU)& 0x0100U) == 0U)//Rx Interrupt Disable
                     {
                         if((spiREG3->FLG & 0x00000100U) == 0x00000100U)
                         {
                             SpiBuf = spiREG3->BUF;
                         }
                     }*/

                     if(spiBuffer[0U].txDataLength == 0U)
                     {
                         spiREG3->INT0 = (spiREG3->INT0 & 0x0000FFFFU) & (~(uint32)0x0200U); /* Disable Interrupt */
                         if(spiBuffer[0U].rxDataLength != 0)
                         {
                             spiREG3->INT0 |= 0x0100U;
                         }

                         spiEndNotification(spiREG3);
                     }
                     break;
                 }

            default: /* Clear Flags and return  */
                 spiREG3->FLG = flags;
                 spiNotification(spiREG3, flags & 0xFFU);
                 break;
        }
    }


    /** @fn void spi3LowLevelInterrupt(void)
    *   @brief Level 3 Interrupt for SPI1
    */
    #pragma CODE_STATE(spi3LowLevelInterrupt, 32)
    #pragma INTERRUPT(spi3LowLevelInterrupt, IRQ)
    void spi3LowLevelInterrupt()
    {

    }
#endif
