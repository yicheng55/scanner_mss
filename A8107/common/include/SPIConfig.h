/* SPIConfig.h: Davicom SPI configure header for ESL system.                   */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: SPIConfig.h                                                     */
/* Purpose      : ESL SPI configure header for ESL system                      */
/* Autor        : Richard Chuang                                                */
/* Email        : richard_chung@davicom.com.tw                                  */
/* Description  :                                                               */
/* Reference    :                                                               */
/* Copyright    : (c) Davicom Semiconductor Inc. All Rights Reserved.           */
/* -----------------------------------------------------------------------------*/
/* Version      : 1.0.0.0                                                       */
/* Update       : 2018-03-01                                                    */
/* Modified     : Richard Chung                                                 */
/* Description	:                                                               */
/* -----------------------------------------------------------------------------*/
/* How to use:                                                                  */
/*                                                                              */

#ifndef _DAVICOM_SPI_CONFIG_H__
#define _DAVICOM_SPI_CONFIG_H__
#include <A8107M.h>
#include <SPI.h>

//------------------------------------------------------------------------------
//--- Declarative Macro ---
//------------------------------------------------------------------------------
/**
  * @brief  Write datum to TX0 register.
  * @param[in]  spi is the base address of SPI module.
  * @param[in]  u32TxData is the datum which user attempt to transfer through SPI bus.
  * @return none
  * \hideinitializer
  */
#define	 SPI_CTL_TX_BIT_LEN_Pos	      (8)
#define  SPI_WRITE_TX(spi, u32TxData) ( spi->FIFOTXBUF = u32TxData )

#define SPI_SS_LOW(spi) 		              (spi->SSCR = 0)
#define SPI_SS_HIGH(spi) 	                (spi->SSCR = 1)

#endif // _DAVICOM_SPI_CONFIG_H__
