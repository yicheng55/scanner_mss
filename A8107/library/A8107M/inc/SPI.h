/**************************************************************************//**
 * @file        SPI.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       SPI Driver Header File
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/

#ifndef __SPI_H__
#define __SPI_H__


/** @addtogroup A8107M_Driver Standard Driver
    @{
*/
/** @addtogroup SPI_Driver UART Driver
    @{
*/

/**
 * @brief   Initial SPI cotroller to Master mode with 8bits data transfer/receive.
 */
extern void SPI_Initial(SPI_Type *spi, uint32_t mstr, uint32_t spr, uint32_t type);

/**
 * @brief   SPI master mode transfer data and receive data throught SPI
 */
extern void SPI_TRx(SPI_Type *spi, uint8_t *txBuf, uint8_t *rxBuf, uint32_t length);

/**
 * @brief   Set TXFIFOINT threshold and Enable/Disable TXFIFOINT.
 *          The TXFIFOINT interrupt will occure when FIFOTXBUF send data >= threshold.
 */
extern void SPI_SetTxFIFOINT(SPI_Type *spi, uint8_t threshold, uint8_t int_en);

/**
 * @brief   Set RXFIFOINT threshold and Enable/Disable RXFIFOINT.
 *          The RXFIFOINT interrupt will occure when When FIFORXBUF receive data >= threshold.
 */
extern void SPI_SetRxFIFOINT(SPI_Type *spi, uint8_t threshold, uint8_t int_en);

extern uint8_t SPI_GetFIFORem(SPI_Type *spi, uint8_t trx_fifo);

/*@}*/ /* end of group SPI_Driver */
/*@}*/ /* end of group A8107M_Driver */

#endif
