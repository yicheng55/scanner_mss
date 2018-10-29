/**************************************************************************//**
 * @file        A8107M.h
 * @version     V1.00.02
 * $Revision:   0 $
 * $Date:       2017-01-11 $
 * @brief       The Peripherals Struce typedef for A8107M0.
 *
 * @note        V1.00.02: Add Note.
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/
 
#ifndef __A8107M_H__
#define __A8107M_H__

#include "AMICCOM_CM0.h"

extern uint32_t SystemCoreClock;

/*-------------------- FLASH -------------------*/
/** @addtogroup FLASH
    memory mapped structure for FLASH
    @{
*/
typedef struct
{
    __IO    uint32_t  reserved1;                // Offset: 0x000
    __IO    uint32_t  CTRL;                     // Offset: 0x004            Flash control register
    __IO    uint32_t  LENGTH;                   // Offset: 0x008            The length by 32bits for flash action
    __IO    uint32_t  ADDR;                     // Offset: 0x00C            The address for flash action
    __IO    uint32_t  PWD0;                     // Offset: 0x010            Flash password0 register
    __IO    uint32_t  PWD1;                     // Offset: 0x014            Flash password1 register
    __IO    uint32_t  KEY0;                     // Offset: 0x018            Flash key0 register
    __IO    uint32_t  KEY1;                     // Offset: 0x01C            Flash key1 register
            uint8_t   reserved2[0x100-0x01C-4];
    union {
        __IO    uint32_t  DATA32[32];           // Offset: 0x100~0x17F      Flash Write/Program Data buffer by 32bits
        __IO    uint8_t   DATA8[128];           // Offset: 0x100~0x17C      Flash Write/Program Data buffer by BYTE
    };
            uint8_t   reserved3[0x200-0x180];
    __IO    uint32_t  LOCK[2];                  // Offset: 0x200            Flash Lock register
} Flash_Type;

/* FLASH Register Definitions */

#define FLASH_CTRL_WRITE_Pos    0
#define FLASH_CTRL_WRITE_Msk    (0x1ul<<FLASH_CTRL_WRITE_Pos)
#define FLASH_CTRL_PROG_Pos     1
#define FLASH_CTRL_PROG_Msk     (0x1ul<<FLASH_CTRL_PROG_Pos)
#define FLASH_CTRL_PERASE_Pos   2
#define FLASH_CTRL_PERASE_Msk   (0x1ul<<FLASH_CTRL_PERASE_Pos)
#define FLASH_CTRL_SERASE_Pos   3
#define FLASH_CTRL_SERASE_Msk   (0x1ul<<FLASH_CTRL_SERASE_Pos)
#define FLASH_CTRL_MERASE_Pos   4
#define FLASH_CTRL_MERASE_Msk   (0x1ul<<FLASH_CTRL_MERASE_Pos)
#define FLASH_CTRL_F1M_Pos      5
#define FLASH_CTRL_F1M_Msk      (0x1ul<<FLASH_CTRL_F1M_Pos)
#define FLASH_CTRL_F2M_Pos      6
#define FLASH_CTRL_F2M_Msk      (0x1ul<<FLASH_CTRL_F2M_Pos)
#define FLASH_CTRL_CE_Pos       7
#define FLASH_CTRL_CE_Msk       (0x1ul<<FLASH_CTRL_CE_Pos)

#define FLASH_LENGTH_PWE_Pos    7
#define FLASH_LENGTH_PWE_Msk    (0x1ul<<FLASH_LENGTH_PWE_Pos)

/*@}*/ /* end of group FLASH */


/*-------------------- Radio Frequency -------------------*/

/** @addtogroup RADIO
    @{
*/
#define RF_FIFO_SIZE   256

typedef struct
{
    __O     uint32_t  RESET;              /*!< Offset: 0x000 RF_RESET Register  (W) */
    __IO    uint32_t  STROBE;             /*!< Offset: 0x004 Strobe Command Register  (R/W) */
    __IO    uint32_t  MODECTRL;           /*!< Offset: 0x008 Mode Control Register (R/W) */
            uint32_t  RF_RESERVED0[1];    /*!< Offset: 0x00C Reserved */
    __IO    uint32_t  STATUS;             /*!< Offset: 0x010 STATUS Register (R)  */
    __IO    uint32_t  RFINT;              /*!< Offset: 0x014 STATUS Register (W/R)  */
    union
    {
        __IO   uint32_t  SYNC_ID;         /*!< Offset: 0x018 RF ID Register (R/W)  */
        __IO   uint8_t   SYNCB_ID[4];
    };
            uint32_t  RF_RESERVED1[1];    /*!< Offset: 0x01C Reserved */
    __IO    uint32_t  FIFOCTRL;           /*!< Offset: 0x020 FIFO Control Register (R/W)  */
    __IO    uint32_t  DATARATE;           /*!< Offset: 0x024 Data Rate Register (R/W)  */
    __IO    uint32_t  RFGIO;              /*!< Offset: 0x028 RF GIO Register (R/W)  */
            uint32_t  RF_RESERVED2[21];   /*!< Offset: 0x02C - 0x7C Reserved */
    __IO    uint32_t  CALIBRATION;        /*!< Offset: 0x080 Calibration Register (R/W) */
    __IO    uint32_t  IFCTRL;             /*!< Offset: 0x084 IF Control Register (R/W) */
    __IO    uint32_t  VCOC;               /*!< Offset: 0x088 VCO Current Register (R/W) */
    __IO    uint32_t  VCOBAND;            /*!< Offset: 0x08C VCO BAND Register (R/W) */
    __IO    uint32_t  VCODEV;             /*!< Offset: 0x090 VCO Deviation Register (R/W) */
    __IO    uint32_t  VCODEV2;            /*!< Offset: 0x094 VCO Deviation II Register (R/W) */
            uint32_t  RF_RESERVED3[10];   /*!< Offset: 0x098 - 0x0BC Reserved */
    __IO    uint32_t  CHANNEL;            /*!< Offset: 0x0C0 VCO Deviation II Register (R/W) */
    __IO    uint32_t  FREQUENCY;          /*!< Offset: 0x0C4 RF Frequency Register (R/W) */
    __IO    uint32_t  FREQUENCY2;         /*!< Offset: 0x0C8 RF Frequency II Register (R/W) */
    __IO    uint32_t  CHGROUP;            /*!< Offset: 0x0CC Channel Group Register (R/W) */
            uint32_t  RF_RESERVED4[12];   /*!< Offset: 0x0B0 - 0x0FC Reserved */
    __O     uint32_t  TXCTRL;             /*!< Offset: 0x100 TX Control Register (W) */
    __O     uint32_t  TXPOWER;            /*!< Offset: 0x104 TX Power Register (W) */
    __O     uint32_t  TXPOWER2;           /*!< Offset: 0x108 VCO Deviation II Register (W) */
    __O     uint32_t  TXMOD;              /*!< Offset: 0x10C TX Modulation Register (W) */
            uint32_t  RF_RESERVED5[12];   /*!< Offset: 0x110 - 0x13C Reserved */
    __IO    uint32_t  RX;                 /*!< Offset: 0x140 RX Register (R/W) */
    __IO    uint32_t  RXGAIN;             /*!< Offset: 0x144 RX Gain Register (R/W) */
    __O     uint32_t  RXGAIN2;            /*!< Offset: 0x148 RX Gain II Register */
            uint32_t  RF_RESERVED6[1];    /*!< Offset: 0x14C Reserved */
    union
    {
        __IO   uint32_t  RXDEM;           /*!< Offset: 0x150 RX Demodulation Register (R/W) */
        __IO   uint8_t   RXDEMB[4];
    };
    union
    {
        __IO   uint32_t  RXDEM2;          /*!< Offset: 0x154 RX Demodulation II Register (R/W) */
        __IO   uint8_t   RXDEM2B[4];
    };
            uint32_t  RF_RESERVED7[10];   /*!< Offset: 0x158 - 0x17C Reserved */
    __O     uint32_t  CODE;               /*!< Offset: 0x180 CODE Register (W) */
    __O     uint32_t  CODE2;              /*!< Offset: 0x184 CODE II Register (W) */
            uint32_t  RF_RESERVED8[14];   /*!< Offset: 0x188 - 0x1BC Reserved */
    __IO    uint32_t  DELAY;              /*!< Offset: 0x1C0 DELAY Register (R/W) */
    __IO    uint32_t  CLOCKCTRL;          /*!< Offset: 0x1C4 RF Clock Control Register (R/W) */
    __O     uint32_t  CLOCKCTRL2;         /*!< Offset: 0x1C8 RF Clock Control II Register (W) */
    __O     uint32_t  CLOCKGATE;          /*!< Offset: 0x1CC RF Clock Gate Register (W) */
    __O     uint32_t  CPC;                /*!< Offset: 0x1D0 Charge Pump Register (W) */
    __IO    uint32_t  RFTEST;             /*!< Offset: 0x1D4 RF Test Register (R/W) */
    __IO    uint32_t  RFTEST2;            /*!< Offset: 0x1D8 RF Test II Register (R/W) */
            uint32_t  RF_RESERVED9[9];    /*!< Offset: 0x1DC - 0x1FC Reserved */
    __IO    uint32_t  WORT;               /*!< Offset: 0x200 WOR & WOT Register (R/W) */
    __O     uint32_t  WORT2;              /*!< Offset: 0x204 WOR & WOT Register (R/W) */
            uint32_t  RF_RESERVED10[14];  /*!< Offset: 0x208 - 0x23C Reserved */
    __IO    uint32_t  ADC9;               /*!< Offset: 0x240 9-bit ADC Register (R/W) */
    __IO    uint32_t  THRESHOLD;          /*!< Offset: 0x244 9-bit ADC Threshold Register (R/W) */
            uint32_t  RF_RESERVED11[46];  /*!< Offset: 0x248 - 0x2FC Reserved */
    __IO    uint32_t  BLECTRL;            /*!< Offset: 0x300 BLE Control Register (R/W) */
    __IO    uint32_t  BLEHEADER;          /*!< Offset: 0x304 BLE Header Register (R/W) */
    __O     uint32_t  BLECHIDX;           /*!< Offset: 0x308 BLE Channel Index Register (W) */
    __IO    uint32_t  BLETIME;            /*!< Offset: 0x30C BLE LL Time Control Register (R/W) */
    __O     uint32_t  BLECRCINIT;         /*!< Offset: 0x310 BLE CRC INIT Register (W) */
    __O     uint32_t  BLECRCINIR;         /*!< Offset: 0x314 BLE CRC INIR Register (W) */
    __IO    uint32_t  BLECRC;             /*!< Offset: 0x318 BLE CRC-32 Register (R/W) */
    __IO    uint32_t  BLETXPKT;           /*!< Offset: 0x31C BLE TX Packet Number Register (R/W) */
    __IO    uint32_t  BLERXPKT;           /*!< Offset: 0x320 BLE RX Packet Number Register (R/W) */
            uint32_t  RF_RESERVED12[55];  /*!< Offset: 0x324 - 0x3FC Reserved */
    __O     uint8_t   TX_DATA[RF_FIFO_SIZE];    /*!< Offset: 0x400 - 0x4FF TX DATA Register (W) */
    __I     uint8_t   RX_DATA[RF_FIFO_SIZE];    /*!< Offset: 0x500 - 0x5FF RX DATA Register (R) */
} RADIO_Type;

/* RF Strobe command */
#define CMD_SLEEP       0x80
#define CMD_IDLE        0x90
#define CMD_STBY        0xA0
#define CMD_PLL         0xB0
#define CMD_RX          0xC0
#define CMD_TX          0xD0

/* RF Reset command */
#define RF_RST          0x80
#define TXPOINT_RST     0x40
#define RXPOINT_RST     0x20
#define TXDATA_RST      0x10
#define BFC_RST         0x04

/*@}*/ /* end of group RADIO */

/*-------------------- Sleep Timer -------------------*/
/** @addtogroup SLPTIMER
    @{
*/

typedef struct
{
    __IO    uint32_t  INTERVAL;         // Offset: 0x000    SLPTIMER INTERVAL Register  (R/W) */
    union
    {
        __IO   uint32_t  CONTROL;       // Offset: 0x004    SLPTIMER CONTROL Register  (R/W) */
                                        //  [0]     - CE            Start SLPTIMER counting.
                                        //                              0=stop count.
                                        //                              1=start counting.
                                        //  [1]     - CKS           Clock source select
                                        //  [5]     - IF            Interrupt Flag. Write 1 to clean.
                                        //                              read 0=interrupt not occure
                                        //                              read 1=interrupt occured
                                        //                              write 0=no effect
                                        //                              write 1=clean IF
                                        //  [6]     - IE            Interrupt Enable.
                                        //                              0=disable interrupt
                                        //                              1=enable interrupt
                                        //  [7]     - ON            Turn on SLPTIMER
                                        //                              0=turn off SLPTIMER
                                        //                              1=turn on SLPTIMER
                                        //  [31]    - RST           SLPTIMER Reset
        __IO   uint8_t   CONTROLB[4];   // Offset: 0x004        SLPTIMER CONTROL Register access by BYTE (R/W) */
    };
} SLPTIMER_Type;

#define SLPTIMER_INTERVAL_Pos       0
#define SLPTIMER_INTERVAL_Msk       (0xFFFFul<<SLPTIMER_INTERVAL_Pos)

#define SLPTIMER_CONTROL_CE_Pos     0
#define SLPTIMER_CONTROL_CE_Msk     (0x1ul<<SLPTIMER_CONTROL_CE_Pos)

#define SLPTIMER_CONTROL_CKS_Pos    1
#define SLPTIMER_CONTROL_CKS_Msk    (0x3ul<<SLPTIMER_CONTROL_CKS_Pos)

#define SLPTIMER_CONTROL_IF_Pos     5
#define SLPTIMER_CONTROL_IF_Msk     (0x1ul<<SLPTIMER_CONTROL_IF_Pos)

#define SLPTIMER_CONTROL_IE_Pos     6
#define SLPTIMER_CONTROL_IE_Msk     (0x1ul<<SLPTIMER_CONTROL_IE_Pos)

#define SLPTIMER_CONTROL_ON_Pos     7
#define SLPTIMER_CONTROL_ON_Msk     (0x1ul<<SLPTIMER_CONTROL_ON_Pos)

#define SLPTIMER_CONTROL_RST_Pos    31
#define SLPTIMER_CONTROL_RST_Msk    (0x1ul<<SLPTIMER_CONTROL_RST_Pos)

/*@}*/ /* end of group SLPTIMER */

/*-------------------- AES & CCM -------------------*/

/** @addtogroup AESCCM
    @{
*/

typedef struct
{
    union
    {
        __IO   uint32_t  CONTROL;         /*!< Offset: 0x000 AES CCM CONTROL Register  (R/W) */
        __IO   uint8_t   CONTROLB[4];
    };
    union
    {
        __IO   uint32_t  NONCE[2];        /*!< Offset: 0x004 NONCE Register  (R/W) */
        __IO   uint8_t   NONCEB[8];
    };
    union
    {
        __IO   uint32_t  AESKEY[4];       /*!< Offset: 0x00C AES KEY Register  (R/W) */
        __IO   uint8_t   AESKEYB[16];
    };
    union
    {
        __IO   uint32_t  AESDATA[4];      /*!< Offset: 0x01C AES DATA Register  (R/W) */
        __IO   uint8_t   AESDATAB[16];
    };
    __IO    uint32_t  TXMIC;              /*!< Offset: 0x02C TX MIC Register  (R/W) */
    __IO    uint32_t  RXMIC;              /*!< Offset: 0x030 RX MIC Register  (R/W) */
            uint32_t  RESERVED0[51];      /*!< Offset: 0x034 - 0x0FC Reserved */
    __IO    uint8_t   RXCCM_DATA[256];    /*!< Offset: 0x100 - 0x1FF RX CCM FIFO Register  (R/W) */
    __IO    uint8_t   RXCCM_DATA2[256];   /*!< Offset: 0x200 - 0x2FF RX CCM FIFO II Register  (R/W) */
    __IO    uint8_t   TXCCM_DATA[256];    /*!< Offset: 0x300 - 0x3FF TX CCM FIFO Register  (R/W) */
    __IO    uint8_t   TXCCM_DATA2[256];   /*!< Offset: 0x400 - 0x4FF TX CCM FIFO II Register  (R/W) */
} AESCCM_Type;

/*@}*/ /* end of group AESCCM */

/*-------------------- POWER CONTROL -------------------*/

/** @addtogroup POWER
    @{
*/

typedef struct
{
    union
    {
        __IO    uint32_t  BATTERY;        /*!< Offset: 0x000 Battery Detect Register  (R/W) */
        struct
        {
            __IO    uint32_t  BDS    :   1;
            __IO    uint32_t  BVT    :   3;
            __IO    uint32_t  BDF    :   1;
        } BatteryItem;
    };
    __IO    uint32_t  FALSHCTRL;          /*!< Offset: 0x004 (R/W) */
    __IO    uint32_t  POWERCTRL;          /*!< Offset: 0x008 (R/W) */
    union
    {
        __IO    uint32_t  POWERCTRL2;     /*!< Offset: 0x00C (R/W) */
        __IO    uint8_t   POWERCTRL2B[4];
    };
    __IO    uint32_t  POWERCTRL3;         /*!< Offset: 0x010 (R/W) */
    __IO    uint32_t  DCDCCTRL;           /*!< Offset: 0x014 (R/W) */
    __IO    uint32_t  DCDCCTRL2;          /*!< Offset: 0x018 (R/W) */
    __IO    uint32_t  DCDCCTRL3;          /*!< Offset: 0x01C (R/W) */
    union
    {
        __IO    uint32_t  PowerManagement;        /*!< Offset: 0x020 (R/W) */
        struct
        {
            __IO    uint32_t  PMM      :   1;
            __IO    uint32_t  STOP     :   1;
            __IO    uint32_t  SWB      :   1;
            __IO    uint32_t  ClkSel   :   3;
        } PowerManagementItem;
    };
            uint8_t   reserved1[0x040-0x020-4];
    __IO    uint32_t  RCCTRL;            /*!< Offset: 0x040 (R/W) */
    __IO    uint32_t  RCCTRL2;           /*!< Offset: 0x044 (R/W) */
    __IO    uint32_t  RCTARGET;          /*!< Offset: 0x048 RC Target Register  (R/W) */
    __IO    uint32_t  IO_HDV;            /*!< Offset: 0x04C (R/W) */
} POWER_Type;

#define POWER_BATTERY_BDS_Pos       0
#define POWER_BATTERY_BDS_Msk       (0x1ul<<POWER_BATTERY_BDS_Pos)

#define POWER_BATTERY_BVT_Pos       1
#define POWER_BATTERY_BVT_Msk       (0x7ul<<POWER_BATTERY_BVT_Pos)

#define POWER_BATTERY_BDF_Pos       4
#define POWER_BATTERY_BDF_Msk       (0x1ul<<POWER_BATTERY_BDF_Pos)

#define POWER_RCCTRL_ENCAL_Pos      0
#define POWER_RCCTRL_ENCAL_Msk      (0x1ul<<POWER_RCCTRL_ENCAL_Pos)

#define POWER_RCCTRL_MAN_Pos        1
#define POWER_RCCTRL_MAN_Msk        (0x1ul<<POWER_RCCTRL_MAN_Pos)

#define POWER_RCCTRL_TMRE_Pos       2
#define POWER_RCCTRL_TMRE_Msk       (0x1ul<<POWER_RCCTRL_TMRE_Pos)

#define POWER_RCCTRL_RCTS_Pos       3
#define POWER_RCCTRL_RCTS_Msk       (0x1ul<<POWER_RCCTRL_RCTS_Pos)

#define POWER_RCCTRL_IOSEL_Pos      4
#define POWER_RCCTRL_IOSEL_Msk      (0x1ul<<POWER_RCCTRL_IOSEL_Pos)

#define POWER_RCCTRL_MCALS_Pos      5
#define POWER_RCCTRL_MCALS_Msk      (0x1ul<<POWER_RCCTRL_MCALS_Pos)

/*@}*/ /* end of group POWER */

/*-------------------- 12-BIT ADC -------------------*/

/** @addtogroup ADC12B
    @{
*/
typedef struct
{
    __IO    uint32_t  Control;            /*!< Offset: 0x000 12-BIT ADC Control Register (R/W) */
    union
    {
        __I    uint32_t  Value;           /*!< Offset: 0x004 Value(R) */
        struct
        {
            __I  uint16_t  ADC;
            __I  uint16_t  MVADC;
        } ValueItem;
    };
    __IO    uint32_t  IOSEL;              /*!< Offset: 0x008 ADC IO SELECT Register (R/W) */
} ADC12B_Type;

#define ADC12B_CONTROL_ADCE_Pos     0
#define ADC12B_CONTROL_ADCE_Msk     (0x1ul<<ADC12B_CONTROL_ADCE_Pos)

#define ADC12B_CONTROL_MVS_Pos      1
#define ADC12B_CONTROL_MVS_Msk      (0x7ul<<ADC12B_CONTROL_MVS_Pos)

#define ADC12B_CONTROL_MODE_Pos     4
#define ADC12B_CONTROL_MODE_Msk     (0x1ul<<ADC12B_CONTROL_MODE_Pos)

#define ADC12B_CONTROL_CKS_Pos      5
#define ADC12B_CONTROL_CKS_Msk      (0x3ul<<ADC12B_CONTROL_CKS_Pos)

#define ADC12B_CONTROL_BUPS_Pos     7
#define ADC12B_CONTROL_BUPS_Msk     (0x1ul<<ADC12B_CONTROL_BUPS_Pos)

#define ADC12B_CONTROL_DTMP_Pos     8
#define ADC12B_CONTROL_DTMP_Msk     (0x1ul<<ADC12B_CONTROL_DTMP_Pos)

#define ADC12B_CONTROL_ENADC_Pos    9
#define ADC12B_CONTROL_ENADC_Msk    (0x1ul<<ADC12B_CONTROL_ENADC_Pos)

#define ADC12B_CONTROL_ADCYC_Pos    10
#define ADC12B_CONTROL_ADCYC_Msk    (0x1ul<<ADC12B_CONTROL_ADCYC_Pos)

#define ADC12B_CONTROL_ADIVL_Pos    11
#define ADC12B_CONTROL_ADIVL_Msk    (0x1ul<<ADC12B_CONTROL_ADIVL_Pos)

#define ADC12B_CONTROL_VADS_Pos     12
#define ADC12B_CONTROL_VADS_Msk     (0x7ul<<ADC12B_CONTROL_VADS_Pos)

#define ADC12B_CONTROL_ADCIE_Pos    15
#define ADC12B_CONTROL_ADCIE_Msk    (0x1ul<<ADC12B_CONTROL_ADCIE_Pos)

#define ADC12B_CONTROL_ADCIF_Pos    16
#define ADC12B_CONTROL_ADCIF_Msk    (0x1ul<<ADC12B_CONTROL_ADCIF_Pos)

#define ADC12B_CONTROL_ADC12RN_Pos  31
#define ADC12B_CONTROL_ADC12RN_Msk  (0x1ul<<ADC12B_CONTROL_ADC12RN_Pos)

#define ADC12B_VALUE_ADC_Pos        0
#define ADC12B_VALUE_ADC_Msk        (0xFFFul<<ADC12B_VALUE_ADC_Pos)

#define ADC12B_VALUE_MVADC_Pos      16
#define ADC12B_VALUE_MVADC_Msk      (0xFFF<<ADC12B_VALUE_MVADC_Pos)

#define ADC12B_IOSEL_EN_Pos         0
#define ADC12B_IOSEL_EN_Msk         (0x1ul<<ADC12B_IOSEL_EN_Pos)

#define ADC12B_IOSEL_CH_Pos         1
#define ADC12B_IOSEL_CH_Msk         (0x7ul<<ADC12B_IOSEL_CH_Pos)
/*@}*/ /* end of group ADC12B */

/*-------------------- SPI -------------------*/
/** @addtogroup SPI
    @{
*/
typedef struct
{
    __IO    uint32_t    CTRL;           // Offset: 0x000
    __IO    uint32_t    STATUS;         // Offset: 0x004
    __IO    uint32_t    INT;            // Offset: 0x008
    __IO    uint32_t    SSCR;           // Offset: 0x00C
    __I     uint32_t    FIFOTXSTUS;     // Offset: 0x010
    __I     uint32_t    FIFORXSTUS;     // Offset: 0x014
    __O     uint32_t    FIFOTXBUF;      // Offset: 0x018
    __I     uint32_t    FIFORXBUF;      // Offset: 0x01C
} SPI_Type;

#define SPI_CTRL_SPR0_Pos           0
#define SPI_CTRL_SPR0_Msk           (0x1ul<<SPI_CTRL_SPR0_Pos)

#define SPI_CTRL_SPR1_Pos           1
#define SPI_CTRL_SPR1_Msk           (0x1ul<<SPI_CTRL_SPR1_Pos)

#define SPI_CTRL_TYPE_Pos           2
#define SPI_CTRL_TYPE_Msk           (0x3ul<<SPI_CTRL_TYPE_Pos)

#define SPI_CTRL_CPHA_Pos           2
#define SPI_CTRL_CPHA_Msk           (0x1ul<<SPI_CTRL_CPHA_Pos)

#define SPI_CTRL_CPOL_Pos           3
#define SPI_CTRL_CPOL_Msk           (0x1ul<<SPI_CTRL_CPOL_Pos)

#define SPI_CTRL_MSTR_Pos           4
#define SPI_CTRL_MSTR_Msk           (0x1ul<<SPI_CTRL_MSTR_Pos)

#define SPI_CTRL_SPR2_Pos           5
#define SPI_CTRL_SPR2_Msk           (0x1ul<<SPI_CTRL_SPR2_Pos)

#define SPI_CTRL_SPE_Pos            6
#define SPI_CTRL_SPE_Msk            (0x1ul<<SPI_CTRL_SPE_Pos)

#define SPI_CTRL_SPIE_Pos           7
#define SPI_CTRL_SPIE_Msk           (0x1ul<<SPI_CTRL_SPIE_Pos)

#define SPI_CTRL_BITS_Pos           8
#define SPI_CTRL_BITS_Msk           (0xFul<<SPI_CTRL_BITS_Pos)

#define SPI_CTRL_FIFOEN_Pos         12
#define SPI_CTRL_FIFOEN_Msk         (0x1ul<<SPI_CTRL_FIFOEN_Pos)

#define SPI_CTRL_SFIFORST_Pos       15
#define SPI_CTRL_SFIFORST_Msk       (0x1ul<<SPI_CTRL_SFIFORST_Pos)

#define SPI_CTRL_TXFIFOIL_Pos       16
#define SPI_CTRL_TXFIFOIL_Msk       (0x7ul<<SPI_CTRL_TXFIFOIL_Pos)

#define SPI_CTRL_RXFIFOIL_Pos       20
#define SPI_CTRL_RXFIFOIL_Msk       (0x7ul<<SPI_CTRL_RXFIFOIL_Pos)

#define SPI_STATUS_SSCEN_Pos        0
#define SPI_STATUS_SSCEN_Msk        (0x1ul<<SPI_STATUS_SSCEN_Pos)

#define SPI_STATUS_MODF_Pos         4
#define SPI_STATUS_MODF_Msk         (0x1ul<<SPI_STATUS_MODF_Pos)

#define SPI_STATUS_SS_Pos           5
#define SPI_STATUS_SS_Msk           (0x1ul<<SPI_STATUS_SS_Pos)

#define SPI_STATUS_WCOL_Pos         6
#define SPI_STATUS_WCOL_Msk         (0x1ul<<SPI_STATUS_WCOL_Pos)

#define SPI_STATUS_SPIF_Pos         7
#define SPI_STATUS_SPIF_Msk         (0x1ul<<SPI_STATUS_SPIF_Pos)

#define SPI_INT_TXFIFOINTEN_Pos     0
#define SPI_INT_TXFIFOINTEN_Msk     (0x1ul<<SPI_INT_TXFIFOINTEN_Pos)

#define SPI_INT_RXFIFOINTEN_Pos     1
#define SPI_INT_RXFIFOINTEN_Msk     (0x1ul<<SPI_INT_RXFIFOINTEN_Pos)

#define SPI_INT_TXFIFOINTFLAG_Pos   4
#define SPI_INT_TXFIFOINTFLAG_Msk   (0x1ul<<SPI_INT_TXFIFOINTFLAG_Pos)

#define SPI_INT_RXFIFOINTFLAG_Pos   5
#define SPI_INT_RXFIFOINTFLAG_Msk   (0x1ul<<SPI_INT_RXFIFOINTFLAG_Pos)

#define SPI_SSCR_Pos                0
#define SPI_SSCR_Msk                (0xFFul<<SPI_SSCR_Pos)

#define SPI_FIFOTXSTUS_REM_Pos      0
#define SPI_FIFOTXSTUS_REM_Msk      (0x7ul<<SPI_FIFOTXSTUS_REM_Pos)

#define SPI_FIFOTXSTUS_EMPTY_Pos    3
#define SPI_FIFOTXSTUS_EMPTY_Msk    (0x1ul<<SPI_FIFOTXSTUS_EMPTY_Pos)

#define SPI_FIFOTXSTUS_FULL_Pos     4
#define SPI_FIFOTXSTUS_FULL_Msk     (0x1ul<<SPI_FIFOTXSTUS_FULL_Pos)

#define SPI_FIFORXSTUS_REM_Pos      0
#define SPI_FIFORXSTUS_REM_Msk      (0x7ul<<SPI_FIFORXSTUS_REM_Pos)

#define SPI_FIFORXSTUS_EMPTY_Pos    3
#define SPI_FIFORXSTUS_EMPTY_Msk    (0x1ul<<SPI_FIFORXSTUS_EMPTY_Pos)

#define SPI_FIFORXSTUS_FULL_Pos     4
#define SPI_FIFORXSTUS_FULL_Msk     (0x1ul<<SPI_FIFORXSTUS_FULL_Pos)

#define SPI_FIFOTXBUF_Pos           0
#define SPI_FIFOTXBUF_Msk           (0xFFFFul<<SPI_FIFOTXBUF_Pos)

#define SPI_FIFORXBUF_Pos           0
#define SPI_FIFORXBUF_Msk           (0xFFFFul<<SPI_FIFORXBUF_Pos)

/*@}*/ /* end of group SPI */

/*-------------------- RTC -------------------*/
/** @addtogroup RTC
    @{
*/
typedef struct
{
    __IO    uint32_t    COUNT;          // Offset: 0x000
    __IO    uint32_t    ALARM;          // Offset: 0x004
    __IO    uint32_t    CTRL;           // Offset: 0x008
    __IO    uint32_t    FLAG;           // Offset: 0x00C
    __IO    uint32_t    MANUALCOUNT;    // Offset: 0x010
} RTC_Type;

#define RTC_COUNT_TMS_Pos           0
#define RTC_COUNT_TMS_Msk           (0x7Ful<<RTC_COUNT_TMS_Pos)

#define RTC_COUNT_SEC_Pos           8
#define RTC_COUNT_SEC_Msk           (0x3Ful<<RTC_COUNT_SEC_Pos)

#define RTC_COUNT_MIN_Pos           16
#define RTC_COUNT_MIN_Msk           (0x3Ful<<RTC_COUNT_MIN_Pos)

#define RTC_COUNT_HOUR_Pos          24
#define RTC_COUNT_HOUR_Msk          (0x1Ful<<RTC_COUNT_HOUR_Pos)

#define RTC_COUNT_WEEK_Pos          29
#define RTC_COUNT_WEEK_Msk          (0x3ul<<RTC_COUNT_WEEK_Pos)

#define RTC_ALARM_MIN_Pos           0
#define RTC_ALARM_MIN_Msk           (0x3Ful<<RTC_ALARM_MIN_Pos)

#define RTC_ALARM_HOUR_Pos          8
#define RTC_ALARM_HOUR_Msk          (0x1Ful<<RTC_ALARM_HOUR_Pos)

#define RTC_CTRL_EN_Pos             0
#define RTC_CTRL_EN_Msk             (0x1ul<<RTC_CTRL_EN_Pos)

#define RTC_CTRL_ALARMIE_Pos        1
#define RTC_CTRL_ALARMIE_Msk        (0x1ul<<RTC_CTRL_ALARMIE_Pos)

#define RTC_CTRL_SECIE_Pos          2
#define RTC_CTRL_SECIE_Msk          (0x1ul<<RTC_CTRL_SECIE_Pos)

#define RTC_CTRL_HSECIE_Pos         3
#define RTC_CTRL_HSECIE_Msk         (0x1ul<<RTC_CTRL_HSECIE_Pos)

#define RTC_CTRL_TMSIE_Pos         4
#define RTC_CTRL_TMSIE_Msk         (0x1ul<<RTC_CTRL_TMSIE_Pos)

#define RTC_CTRL_MANUALCOUNTIE_Pos  5
#define RTC_CTRL_MANUALCOUNTIE_Msk  (0x1ul<<RTC_CTRL_MANUALCOUNTIE_Pos)

#define RTC_CTRL_ON_Pos             6
#define RTC_CTRL_ON_Msk             (0x1ul<<RTC_CTRL_ON_Pos)

#define RTC_CTRL_CLKSEL_Pos         7
#define RTC_CTRL_CLKSEL_Msk         (0x1ul<<RTC_CTRL_CLKSEL_Pos)

#define RTC_CTRL_MANUALCOUNTEN_Pos  8
#define RTC_CTRL_MANUALCOUNTEN_Msk  (0x1ul<<RTC_CTRL_MANUALCOUNTEN_Pos)

#define RTC_FLAG_RTC_Pos            0
#define RTC_FLAG_RTC_Msk            (0x1ul<<RTC_FLAG_RTC_Pos)

#define RTC_FLAG_ALARM_Pos          1
#define RTC_FLAG_ALARM_Msk          (0x1ul<<RTC_FLAG_ALARM_Pos)

#define RTC_FLAG_SEC_Pos            2
#define RTC_FLAG_SEC_Msk            (0x1ul<<RTC_FLAG_SEC_Pos)

#define RTC_FLAG_HSEC_Pos           3
#define RTC_FLAG_HSEC_Msk           (0x1ul<<RTC_FLAG_HSEC_Pos)

#define RTC_FLAG_TMS_Pos           4
#define RTC_FLAG_TMS_Msk           (0x1ul<<RTC_FLAG_TMS_Pos)

#define RTC_FLAG_MANUALCOUNT_Pos    5
#define RTC_FLAG_MANUALCOUNT_Msk    (0x1ul<<RTC_FLAG_MANUALCOUNT_Pos)

#define RTC_MANUALCOUNT_COUNT_Pos   0
#define RTC_MANUALCOUNT_COUNT_Msk   (0xFFul<<RTC_MANUALCOUNT_COUNT_Pos)

/*@}*/ /* end of group RTC */

/*-------------------- MPU LCD -------------------*/
/** @addtogroup MPULCD
    @{
*/
typedef struct
{
    __IO    uint32_t  CONFIG;             // Offset: 0x000  LCD Control/Config register
    __IO    uint32_t  WIDTH;              // Offset: 0x004  Set Display Panel width
    __IO    uint32_t  HEIGHT;             // Offset: 0x008  Set Display Panel height
    __IO    uint32_t  BLANKMODE;          // Offset: 0x00C
    __IO    uint32_t  CYCLE;              // Offset: 0x010
    __IO    uint32_t  CMDDAT;             // Offset: 0x014
    __IO    uint32_t  OFFSET;             // Offset: 0x018  SRAM offset (0x0000 / 0x1000)
} MPULCD_Type;

#define MPULCD_CONFIG_A0_Pos        0
#define MPULCD_CONFIG_A0_Msk        (0x1ul<<MPULCD_CONFIG_A0_Pos)

#define MPULCD_CONFIG_RNW_Pos       1
#define MPULCD_CONFIG_RNW_Msk       (0x1ul<<MPULCD_CONFIG_RNW_Pos)

#define MPULCD_CONFIG_BUSY_Pos      2
#define MPULCD_CONFIG_BUSY_Msk      (0x1ul<<MPULCD_CONFIG_BUSY_Pos)

#define MPULCD_CONFIG_ENABLE_Pos    3
#define MPULCD_CONFIG_ENABLE_Msk    (0x1ul<<MPULCD_CONFIG_ENABLE_Pos)

#define MPULCD_CONFIG_CMD_Pos       4
#define MPULCD_CONFIG_CMD_Msk       (0x1ul<<MPULCD_CONFIG_CMD_Pos)

#define MPULCD_CONFIG_TEEN_Pos      5
#define MPULCD_CONFIG_TEEN_Msk      (0x1ul<<MPULCD_CONFIG_TEEN_Pos)

#define MPULCD_CONFIG_TEPOL_Pos     6
#define MPULCD_CONFIG_TEPOL_Msk     (0x1ul<<MPULCD_CONFIG_TEPOL_Pos)

#define MPULCD_CONFIG_CARRY_Pos     7
#define MPULCD_CONFIG_CARRY_Msk     (0x1ul<<MPULCD_CONFIG_CARRY_Pos)

#define MPULCD_CONFIG_CONTINUE_Pos  8
#define MPULCD_CONFIG_CONTINUE_Msk  (0x1ul<<MPULCD_CONFIG_CONTINUE_Pos)

#define MPULCD_CONFIG_8BITS_Pos     9
#define MPULCD_CONFIG_8BITS_Msk     (0x1ul<<MPULCD_CONFIG_8BITS_Pos)

#define MPULCD_CONFIG_16BITS_Pos    10
#define MPULCD_CONFIG_16BITS_Msk    (0x1ul<<MPULCD_CONFIG_16BITS_Pos)

#define MPULCD_CONFIG_SPI_Pos       11
#define MPULCD_CONFIG_SPI_Msk       (0x1ul<<MPULCD_CONFIG_SPI_Pos)

#define MPULCD_CONFIG_IOEN_Pos      12
#define MPULCD_CONFIG_IOEN_Msk      (0x1ul<<MPULCD_CONFIG_IOEN_Pos)

#define MPULCD_CONFIG_INT_Pos       13
#define MPULCD_CONFIG_INT_Msk       (0x1ul<<MPULCD_CONFIG_INT_Pos)

#define MPULCD_CONFIG_RGB_Pos       14
#define MPULCD_CONFIG_RGB_Msk       (0x3ul<<MPULCD_COFNFIG_RGB_Pos)

#define MPULCD_WIDTH_Pos            0
#define MPULCD_WIDTH_Msk            (0xFFul<<MPULCD_WIDTH_Pos)

#define MPULCD_HEIGHT_Pos           0
#define MPULCD_HEIGHT_Msk           (0xFFul<<MPULCD_HEIGHT_Pos)

#define MPULCD_BLANKMODE_Pos        0
#define MPULCD_BLANKMODE_Msk        (0xFul<<MPULCD_BLANKMODE_Pos)

#define MPULCD_CYCLE_Pos            0
#define MPULCD_CYCLE_Msk            (0x3Ful<<MPULCD_CYCLE_Pos)

#define MPULCD_CMDDAT_Pos           0
#define MPULCD_CMDDAT_Msk           (0xFFFFul<<MPULCD_CMDDAT_Pos)

#define MPULCD_OFFSET_Pos           0
#define MPULCD_OFFSET_Msk           (0x3FFFul<<MPULCD_OFFSET_Pos)

/*@}*/ /* end of group MPULCD */

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/** @addtogroup PERIPHERAL_MEM_MAP Peripheral Memory Map
    @{
*/
#define FLASH_BASE              0x4001F100
#define POWER_BASE              0x50000000
#define RADIO_BASE              0x50001000
#define SPI_BASE                0x50002000
#define RTC_BASE                0x50005000
#define SLPTIMER_BASE           0x50006000
#define ADC12BITS_BASE          0x50008000
#define MPULCD_BASE             0x5000A000
#define AESCCM_BASE             0x5000C000
/*@}*/ /* end of group PERIPHERAL_MEM_MAP */

/******************************************************************************/
/*                         Peripheral Definitions                             */
/******************************************************************************/
/** @addtogroup PERIPHERAL Peripheral Definitions
    @{
*/
#define FLASH                   ((Flash_Type    *)  FLASH_BASE)
#define POWER                   ((POWER_Type    *)  POWER_BASE)
#define RADIO                   ((RADIO_Type    *)  RADIO_BASE)
#define SPI                     ((SPI_Type      *)  SPI_BASE)
#define RTC                     ((RTC_Type      *)  RTC_BASE)
#define SLPTIMER0               ((SLPTIMER_Type *)  (SLPTIMER_BASE+0x000))
#define SLPTIMER1               ((SLPTIMER_Type *)  (SLPTIMER_BASE+0x010))
#define ADC12B                  ((ADC12B_Type   *)  ADC12BITS_BASE)
#define MPULCD                  ((MPULCD_Type   *)  MPULCD_BASE)
#define AESCCM                  ((AESCCM_Type   *)  AESCCM_BASE)
/*@}*/ /* end of group PERIPHERAL */

#endif  //#ifndef __A8107M_H__
