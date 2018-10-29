/**************************************************************************//**
 * @file        spi.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       SPI Driver source file
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "A8107M.h"

/** @addtogroup A8107M_Driver Standard Driver
    @{
*/
/** @addtogroup SPI_Driver DualTimer Driver
    @{
*/

/**
 *
 * @brief   Initial SPI cotroller to Master mode with 8bits data transfer/receive.
 *
 * @param   *spi                    SPI Pointer
 * @param   mstr                    SPI mode select:
 *                                      0=slave, 1=master
 * @param   spr                     SPI speed opteion (slave mode will ignore this):
 *                                      0=Fsys/4,  1=Fsys/8,   2=Fsys/16,  3=Fsys/32
 *                                      4=Fsys/64, 5=Fsys/128, 6=Fsys/256, 7=Fsys/2 (only support Master mode)
 * @param   type                    SPI transfer mode: 0, 1, 2, 3
 * @return  none
 */
void SPI_Initial(SPI_Type *spi, uint32_t mstr, uint32_t spr, uint32_t type)
{
    spi->CTRL = 0;
    spi->CTRL = ((type & SPI_CTRL_TYPE_Msk) << SPI_CTRL_TYPE_Pos) |
                (0x7ul << SPI_CTRL_BITS_Pos) |
                SPI_CTRL_SPE_Msk |
                SPI_CTRL_FIFOEN_Msk;
    if(mstr)        spi->CTRL |= SPI_CTRL_MSTR_Msk;
    if(spr & 0x1ul) spi->CTRL |= SPI_CTRL_SPR0_Msk;
    if(spr & 0x2ul) spi->CTRL |= SPI_CTRL_SPR1_Msk;
    if(spr & 0x4ul) spi->CTRL |= SPI_CTRL_SPR2_Msk;

    spi->STATUS = 0;        // SSCEN=0
    spi->SSCR = 1;          // SSCR0=1, SCS=1
}

/**
 *
 * @brief   SPI master mode transfer data and receive data throught SPI
 *
 * @param   *spi                    SPI Pointer
 * @param   *txBuf                  8bits buffer for send
 * @param   *rxBuf                  8bits buffer for receive
 * @param   length                  Length for send data
 * @return  none
 */
void SPI_TRx(SPI_Type *spi, uint8_t *txBuf, uint8_t *rxBuf, uint32_t length)
{
    spi->SSCR = 0;  // SCS=0
    while(length)
    {
        spi->FIFOTXBUF = *txBuf; txBuf++;
        while(0 == (spi->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;
        if(0 == (spi->FIFORXSTUS & SPI_FIFORXSTUS_EMPTY_Msk))
        {
            *rxBuf = spi->FIFORXBUF; rxBuf++;
        }
        length--;
    }
    spi->SSCR = 1;  // SCS=1
}

/**
 *
 * @brief   Set TXFIFOINT threshold and Enable/Disable TXFIFOINT.
 *          The TXFIFOINT interrupt will occure when FIFOTXBUF send data >= threshold.
 *
 * @param   *spi                    SPI Pointer
 * @param   threshold               The value range is 1~8. The threshold of TXBUF. When FIFOTXBUF send data >= threshold, TXFIFOINT will occure.
 *                                  For example. The threshold = 2. The TXFIFOINT interrupt will occure when FIFOTXBUF send 2nd data.
 *                                  If user not write data to FIFOTXBUF, TXFIFOINT interrupt will occure when FIFOTXBUF send 3th, 4th, 5th, ...data.
 * @param   int_en                  Enable TXFIFOILINT. 0=Disable, 1=ENable.
 * @return  none
 */
void SPI_SetTxFIFOINT(SPI_Type *spi, uint8_t threshold, uint8_t int_en)
{
    if(threshold < 1) threshold = 1;
    if(threshold > 8) threshold = 8;
    threshold = 8-threshold;
    spi->CTRL |= (threshold & 0x07ul) << SPI_CTRL_TXFIFOIL_Pos;
    spi->INT |= SPI_INT_TXFIFOINTFLAG_Msk;
    (int_en) ? (spi->INT |= SPI_INT_TXFIFOINTEN_Msk) : (spi->INT &= ~SPI_INT_TXFIFOINTEN_Msk);
}

/**
 *
 * @brief   Set RXFIFOINT threshold and Enable/Disable RXFIFOINT.
 *          The RXFIFOINT interrupt will occure when When FIFORXBUF receive data >= threshold.
 *
 * @param   *spi                    SPI Pointer
 * @param   threshold               The value range is 1~8. The threshold of RXBUF. When FIFORXBUF receive data >= threshold, TXFIFOINT will occure.
 *                                  For example. The threshold = 2. The RXFIFOINT interrupt will occure when FIFORXBUF receive 2nd data.
 *                                  If user not read data from FIFORXBUF, RXFIFOINT will occure when FIFORXBUF read 3th, 4th, 5th, ... data.
 * @param   int_en                  Enable RXFIFOILINT. 0=Disable, 1=ENable.
 * @return  none
 */
void SPI_SetRxFIFOINT(SPI_Type *spi, uint8_t threshold, uint8_t int_en)
{
    if(threshold < 1) threshold = 1;
    if(threshold > 8) threshold = 8;
    threshold--;
    spi->CTRL |= (threshold & 0x07ul) << SPI_CTRL_RXFIFOIL_Pos;
    spi->INT |= SPI_INT_RXFIFOINTFLAG_Msk;
    (int_en) ? (spi->INT |= SPI_INT_RXFIFOINTEN_Msk) : (spi->INT &= ~SPI_INT_RXFIFOINTEN_Msk);
}

/**
 *
 * @brief   SPI master mode transfer data and receive data throught SPI
 *
 * @param   *spi                    SPI Pointer
 * @param   *txBuf                  8bits buffer for send
 * @param   *rxBuf                  8bits buffer for receive
 * @param   length                  Length for send data
 * @return  none
 */
uint8_t SPI_GetFIFORem(SPI_Type *spi, uint8_t trx_fifo)
{
    uint8_t rem;
    if(trx_fifo)
    {
        if(spi->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk)
            return 0;
        rem = spi->FIFOTXSTUS & SPI_FIFOTXSTUS_REM_Msk;
        return rem+1;
    }
    else
    {
        if(spi->FIFORXSTUS & SPI_FIFORXSTUS_EMPTY_Msk)
            return 0;
        if(spi->FIFORXSTUS & SPI_FIFORXSTUS_FULL_Msk)
            return 8;
        rem = spi->FIFORXSTUS & SPI_FIFORXSTUS_REM_Msk;
        return rem;
    }
}


/*@}*/ /* end of group SPI_Driver */
/*@}*/ /* end of group A8107M_Driver */
