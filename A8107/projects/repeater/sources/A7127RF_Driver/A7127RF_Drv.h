#ifndef _A7127RF_DRIVER_H_
#define _A7127RF_DRIVER_H_

#include <stdint.h>
#include "SPI.h"
#include "A7127RF_Reg.h"

#define R_REGISTER  0x40

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
//#define SPI_CTL_TX_BIT_LEN_Pos                              (8)

//#define SPI_WRITE_TX(spi, u32TxData)                        (spi->FIFOTXBUF = u32TxData)
#define SPI_READ_RX(spi)                                    (spi->FIFORXBUF)

/* Set SPI Datawidth */
#define SPI_DATA_WIDTH(spi, u32DataWidth)                   (spi->CTRL |= ((u32DataWidth - 1) << 8))
//#define SPI_SS_LOW(spi)                                     (spi->SSCR = 0)
//#define SPI_SS_HIGH(spi)                                    (spi->SSCR = 1)

#define GIO1                                                GPIO0->DATA_PIN.P01     //PB3       //wait Tx complete
#define GIO2                                                !GPIO0->DATA_PIN.P13    //PA0   //A7127 RX complete

#define L01_CSN_LOW()                                       SPI_SS_LOW(SPI) //SPI_SET_SS_LOW(SPI1)
#define L01_CSN_HIGH()                                      SPI_SS_HIGH(SPI) //SPI_SET_SS_HIGH(SPI1)

#define A7127_SPI_CSS_PIN                                   (1 << 0)
#define A7127_SPI_MISO_PIN                                  (1 << 1)
#define A7127_SPI_MOSI_PIN                                  (1 << 2)
#define A7127_SPI_SCLK_PIN                                  (1 << 3)

//Jerry add
uint8_t SPI_ExchangeByte(uint8_t input);

uint8_t A7127_ReadReg(uint8_t Addr);
void A7127_WriteReg(uint8_t addr, uint8_t dataByte);
void A7127_ReadMultiReg(uint8_t StartAddr, uint8_t nBytes, uint8_t *pBuff);
void A7127_WriteMultiReg(uint8_t StartAddr, uint8_t *pBuff, uint8_t Length);

void A7127_Reset(void);
void A7127_WriteID(void);
void A7127_Config(void);
void A7127_Cal(void);
void A7127_ReadID(void);

void A7127RF_StrobeCmd(uint8_t cmd);
void A7127RF_SetCHAN(uint8_t ch);
//void A7127_SetFIFOLen(int16_t fifolen);
void A7127RF_SetFIFOLen(uint32_t fifolen);

void GIOSetDataMode(void);
void GIOSetTXMode(void);
void GIOSetRXMode(void);

void EnableAutoACKResend(void);
void DisableAutoACKResend(void);
void EnableCRCFilter(void);
void DisableCRCFilter(void);
void EnableAutoACK(void);
void DisableAutoACK(void);

void A7127RF_SPI_Config(void);
void A7127RF_Iinit(void);
void A7127RF_Enable(void);

#endif /* _A7127RF_DRIVER_H_ */
