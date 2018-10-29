/**************************************************************************//**
 * @file        AMICCOM_CM0.H 
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       Header file for A8107M0 RF SoC microcontroller.
 *
 * @note        V1.00.01: Modify DUALTIMER Define
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#ifndef __AMICCOM_CM0_H__
#define __AMICCOM_CM0_H__

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
*/

/** @addtogroup CMSIS Device Definitions for CMSIS
  Interrupt Number Definition and Configurations for CMSIS
  @{
*/

/**
 * @details  Interrupt Number Definition. The maximum of 32 Specific Interrupts are possible.
 */


typedef enum IRQn
{
    /******  Cortex-M0 Processor Exceptions Numbers *************************************************/

    /* ToDo: use this Cortex interrupt numbers if your device is a CORTEX-M0 device                 */
    NonMaskableInt_IRQn           = -14,    /*!<  2 Cortex-M0 Non Maskable Interrupt                */
    HardFault_IRQn                = -13,    /*!<  3 Cortex-M0 Hard Fault Interrupt                  */
    SVCall_IRQn                   = -5,     /*!< 11 Cortex-M0 SV Call Interrupt                     */
    PendSV_IRQn                   = -2,     /*!< 14 Cortex-M0 Pend SV Interrupt                     */
    SysTick_IRQn                  = -1,     /*!< 15 Cortex-M0 System Tick Interrupt                 */

    /******  AMICCOM M0 Specific Interrupt Numbers **************************************************/
    Reserved0_IRQn                = 0,
    UART0_IRQn                    = 1,      /*!< UART 0 Interrupt                                   */
    SLPTIMER0_IRQn                = 2,      /*!< SLP TIMER 0 Interrupt                              */
    RADIO_IRQn                    = 3,      /*!< RADIO Interrupt                                    */
    Reserved4_IRQn                = 4,      /*!<                                                    */
    UART2_IRQn                    = 5,      /*!< UART 2 Interrupt                                   */
    GPIO0_IRQn                    = 6,      /*!< GPIO0 combined Interrupt                           */
    GPIO1_IRQn                    = 7,      /*!< GPIO1 combined Interrupt                           */
    TIMER0_IRQn                   = 8,      /*!< TIMER 0 Interrupt                                  */
    TIMER1_IRQn                   = 9,      /*!< TIMER 1 Interrupt                                  */
    DUALTIMER_IRQn                = 10,     /*!< Dual Timer Interrupt                               */
    MPU_LCD_IRQn                  = 11,     /*!< LCD DMA Driver Interrupt                           */
    Reserved12_IRQn               = 12,     /*!<                                                    */
    UART1_IRQn                    = 13,     /*!< UART 1 Interrupt                                   */
    Reserved14_IRQn               = 14,     /*!<                                                    */
    WUN_IRQn                      = 15,     /*!<                                                    */
    SPI_IRQn                      = 16,     /*!< SPI Interrupt                                      */
    I2C_IRQn                      = 17,     /*!< I2C Interrupt                                      */
    RTC_IRQn                      = 18,     /*!< RTC Interrupt                                      */
    CODEC_IRQn                    = 19,     /*!< CODEC Interrupt                                    */
    AES_CCM_IRQn                  = 20,     /*!< AES CCM Interrupt                                  */
    ADC_12BIT_IRQn                = 21,     /*!< 12-BIT ADC Interrupt                               */
    RCADC_IRQn                    = 22,     /*!< RCADC Interrupt                                    */
    USB_IRQn                      = 23,     /*!< USB Interrupt                                      */
    SLPTIMER1_IRQn                = 24,     /*!< SLP TIMER 1 Interrupt                              */
    Reserved25_IRQn               = 25,     /*!<                                                    */
    Reserved26_IRQn               = 26,     /*!<                                                    */
    Reserved27_IRQn               = 27,     /*!<                                                    */
    Reserved28_IRQn               = 28,     /*!<                                                    */
    Reserved29_IRQn               = 29,     /*!<                                                    */
    Reserved30_IRQn               = 30,     /*!<                                                    */
    SOFT_IRQn                     = 31,     /*!<                                                    */
} IRQn_Type;

/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M0 Processor and Core Peripherals */
#define __CM0_REV                 0x0000    /*!< Core Revision r0p0                               */
#define __NVIC_PRIO_BITS          2         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */
#define __MPU_PRESENT             0         /*!< MPU present or not                               */

/*@}*/ /* end of group CMSDK_CMSIS */

#include "core_cm0.h"                       /* Cortex-M0 processor and core peripherals  */

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif


typedef struct
{
    __IO uint32_t  P00     :  1;               /*!< Pn.0 */
    __IO uint32_t  P01     :  1;               /*!< Pn.1 */
    __IO uint32_t  P02     :  1;               /*!< Pn.2 */
    __IO uint32_t  P03     :  1;               /*!< Pn.3 */
    __IO uint32_t  P04     :  1;               /*!< Pn.4 */
    __IO uint32_t  P05     :  1;               /*!< Pn.5 */
    __IO uint32_t  P06     :  1;               /*!< Pn.6 */
    __IO uint32_t  P07     :  1;               /*!< Pn.7 */
    __IO uint32_t  P08     :  1;               /*!< Pn.8 */
    __IO uint32_t  P09     :  1;               /*!< Pn.9 */
    __IO uint32_t  P10     :  1;               /*!< Pn.10 */
    __IO uint32_t  P11     :  1;               /*!< Pn.11 */
    __IO uint32_t  P12     :  1;               /*!< Pn.12 */
    __IO uint32_t  P13     :  1;               /*!< Pn.13 */
    __IO uint32_t  P14     :  1;               /*!< Pn.14 */
    __IO uint32_t  P15     :  1;               /*!< Pn.15 */
    __IO uint32_t  P16     :  1;               /*!< Pn.16 */
    __IO uint32_t  P17     :  1;               /*!< Pn.17 */
    __IO uint32_t  P18     :  1;               /*!< Pn.18 */
    __IO uint32_t  P19     :  1;               /*!< Pn.19 */
    __IO uint32_t  P20     :  1;               /*!< Pn.20 */
    __IO uint32_t  P21     :  1;               /*!< Pn.21 */
    __IO uint32_t  P22     :  1;               /*!< Pn.22 */
    __IO uint32_t  P23     :  1;               /*!< Pn.23 */
    __IO uint32_t  P24     :  1;               /*!< Pn.24 */
    __IO uint32_t  P25     :  1;               /*!< Pn.25 */
    __IO uint32_t  P26     :  1;               /*!< Pn.26 */
    __IO uint32_t  P27     :  1;               /*!< Pn.27 */
    __IO uint32_t  P28     :  1;               /*!< Pn.28 */
    __IO uint32_t  P29     :  1;               /*!< Pn.29 */
    __IO uint32_t  P30     :  1;               /*!< Pn.30 */
    __IO uint32_t  P31     :  1;               /*!< Pn.31 */
} sPin32;


/*------------- Universal Asynchronous Receiver Transmitter (UART) -----------*/
/** @addtogroup UART Universal Asynchronous Receiver/Transmitter
    memory mapped structure for UART
    @{
*/
typedef struct
{
    __IO   uint32_t  DATA;          /*!< Offset: 0x000 Data Register    (R/W) */
    __IO   uint32_t  STATE;         /*!< Offset: 0x004 Status Register  (R/W) */
    __IO   uint32_t  CTRL;          /*!< Offset: 0x008 Control Register (R/W) */
    union {
        __I    uint32_t  INTSTATUS;   /*!< Offset: 0x00C Interrupt Status Register (R/ ) */
        __O    uint32_t  INTCLEAR;    /*!< Offset: 0x00C Interrupt Clear Register ( /W) */
    };
        __IO   uint32_t  BAUDDIV;       /*!< Offset: 0x010 Baudrate Divider Register (R/W) */
} UART_Type;

/* UART DATA Register Definitions */

#define UART_DATA_Pos               0                                             /*!< UART_DATA_Pos: DATA Position */
#define UART_DATA_Msk              (0xFFul << UART_DATA_Pos)                /*!< UART DATA: DATA Mask */

#define UART_STATE_RXOR_Pos         3                                             /*!< UART STATE: RXOR Position */
#define UART_STATE_RXOR_Msk         (0x1ul << UART_STATE_RXOR_Pos)          /*!< UART STATE: RXOR Mask */

#define UART_STATE_TXOR_Pos         2                                             /*!< UART STATE: TXOR Position */
#define UART_STATE_TXOR_Msk         (0x1ul << UART_STATE_TXOR_Pos)          /*!< UART STATE: TXOR Mask */

#define UART_STATE_RXBF_Pos         1                                             /*!< UART STATE: RXBF Position */
#define UART_STATE_RXBF_Msk         (0x1ul << UART_STATE_RXBF_Pos)          /*!< UART STATE: RXBF Mask */

#define UART_STATE_TXBF_Pos         0                                             /*!< UART STATE: TXBF Position */
#define UART_STATE_TXBF_Msk         (0x1ul << UART_STATE_TXBF_Pos )         /*!< UART STATE: TXBF Mask */

#define UART_CTRL_HSTM_Pos          6                                             /*!< UART CTRL: HSTM Position */
#define UART_CTRL_HSTM_Msk          (0x01ul << UART_CTRL_HSTM_Pos)          /*!< UART CTRL: HSTM Mask */

#define UART_CTRL_RXORIRQEN_Pos     5                                             /*!< UART CTRL: RXORIRQEN Position */
#define UART_CTRL_RXORIRQEN_Msk     (0x01ul << UART_CTRL_RXORIRQEN_Pos)     /*!< UART CTRL: RXORIRQEN Mask */

#define UART_CTRL_TXORIRQEN_Pos     4                                             /*!< UART CTRL: TXORIRQEN Position */
#define UART_CTRL_TXORIRQEN_Msk     (0x01ul << UART_CTRL_TXORIRQEN_Pos)     /*!< UART CTRL: TXORIRQEN Mask */

#define UART_CTRL_RXIRQEN_Pos       3                                             /*!< UART CTRL: RXIRQEN Position */
#define UART_CTRL_RXIRQEN_Msk       (0x01ul << UART_CTRL_RXIRQEN_Pos)       /*!< UART CTRL: RXIRQEN Mask */

#define UART_CTRL_TXIRQEN_Pos       2                                             /*!< UART CTRL: TXIRQEN Position */
#define UART_CTRL_TXIRQEN_Msk       (0x01ul << UART_CTRL_TXIRQEN_Pos)       /*!< UART CTRL: TXIRQEN Mask */

#define UART_CTRL_RXEN_Pos          1                                             /*!< UART CTRL: RXEN Position */
#define UART_CTRL_RXEN_Msk          (0x01ul << UART_CTRL_RXEN_Pos)          /*!< UART CTRL: RXEN Mask */

#define UART_CTRL_TXEN_Pos          0                                             /*!< UART CTRL: TXEN Position */
#define UART_CTRL_TXEN_Msk          (0x01ul << UART_CTRL_TXEN_Pos)          /*!< UART CTRL: TXEN Mask */

#define UART_INTSTATUS_RXORIRQ_Pos  3                                             /*!< UART CTRL: RXORIRQ Position */
#define UART_CTRL_RXORIRQ_Msk       (0x01ul << UART_INTSTATUS_RXORIRQ_Pos)  /*!< UART CTRL: RXORIRQ Mask */

#define UART_CTRL_TXORIRQ_Pos       2                                             /*!< UART CTRL: TXORIRQ Position */
#define UART_CTRL_TXORIRQ_Msk       (0x01ul << UART_CTRL_TXORIRQ_Pos)       /*!< UART CTRL: TXORIRQ Mask */

#define UART_CTRL_RXIRQ_Pos         1                                             /*!< UART CTRL: RXIRQ Position */
#define UART_CTRL_RXIRQ_Msk         (0x01ul << UART_CTRL_RXIRQ_Pos)         /*!< UART CTRL: RXIRQ Mask */

#define UART_CTRL_TXIRQ_Pos         0                                             /*!< UART CTRL: TXIRQ Position */
#define UART_CTRL_TXIRQ_Msk         (0x01ul << UART_CTRL_TXIRQ_Pos)         /*!< UART CTRL: TXIRQ Mask */

#define UART_BAUDDIV_Pos            0                                             /*!< UART BAUDDIV: BAUDDIV Position */
#define UART_BAUDDIV_Msk            (0xFFFFFul << UART_BAUDDIV_Pos)         /*!< UART BAUDDIV: BAUDDIV Mask */

/*@}*/ /* end of group UART */

/*----------------------------- Timer (TIMER) -------------------------------*/
/** @addtogroup TIMER
    memory mapped structure for Timer
    @{
*/
typedef struct
{
  __IO   uint32_t  CTRL;          /*!< Offset: 0x000 Control Register (R/W) */
  __IO   uint32_t  VALUE;         /*!< Offset: 0x004 Current Value Register (R/W) */
  __IO   uint32_t  RELOAD;        /*!< Offset: 0x008 Reload Value Register  (R/W) */
  union {
    __I    uint32_t  INTSTATUS;   /*!< Offset: 0x00C Interrupt Status Register (R/ ) */
    __O    uint32_t  INTCLEAR;    /*!< Offset: 0x00C Interrupt Clear Register ( /W) */
    };

} TIMER_Type;

/* TIMER CTRL Register Definitions */

#define TIMER_CTRL_IRQEN_Pos          3                                              /*!< TIMER CTRL: IRQEN Position */
#define TIMER_CTRL_IRQEN_Msk          (0x01ul << TIMER_CTRL_IRQEN_Pos)         /*!< TIMER CTRL: IRQEN Mask */

#define TIMER_CTRL_SELEXTCLK_Pos      2                                              /*!< TIMER CTRL: SELEXTCLK Position */
#define TIMER_CTRL_SELEXTCLK_Msk      (0x01ul << TIMER_CTRL_SELEXTCLK_Pos)     /*!< TIMER CTRL: SELEXTCLK Mask */

#define TIMER_CTRL_SELEXTEN_Pos       1                                              /*!< TIMER CTRL: SELEXTEN Position */
#define TIMER_CTRL_SELEXTEN_Msk       (0x01ul << TIMER_CTRL_SELEXTEN_Pos)      /*!< TIMER CTRL: SELEXTEN Mask */

#define TIMER_CTRL_EN_Pos             0                                              /*!< TIMER CTRL: EN Position */
#define TIMER_CTRL_EN_Msk             (0x01ul << TIMER_CTRL_EN_Pos)            /*!< TIMER CTRL: EN Mask */

#define TIMER_VAL_CURRENT_Pos         0                                              /*!< TIMER VALUE: CURRENT Position */
#define TIMER_VAL_CURRENT_Msk         (0xFFFFFFFFul << TIMER_VAL_CURRENT_Pos)  /*!< TIMER VALUE: CURRENT Mask */

#define TIMER_RELOAD_VAL_Pos          0                                              /*!< TIMER RELOAD: RELOAD Position */
#define TIMER_RELOAD_VAL_Msk          (0xFFFFFFFFul << TIMER_RELOAD_VAL_Pos)   /*!< TIMER RELOAD: RELOAD Mask */

#define TIMER_INTSTATUS_Pos           0                                              /*!< TIMER INTSTATUS: INTSTATUSPosition */
#define TIMER_INTSTATUS_Msk           (0x01ul << TIMER_INTSTATUS_Pos)          /*!< TIMER INTSTATUS: INTSTATUSMask */

#define TIMER_INTCLEAR_Pos            0                                              /*!< TIMER INTCLEAR: INTCLEAR Position */
#define TIMER_INTCLEAR_Msk            (0x01ul << TIMER_INTCLEAR_Pos)           /*!< TIMER INTCLEAR: INTCLEAR Mask */

/*@}*/ /* end of group TIMER */


/** @addtogroup DUALTIMER Dual Timer
    @{
*/

typedef struct
{
  __IO uint32_t LOAD;       // <h> Timer Load </h>
  __I  uint32_t VALUE;      // <h> Timer Counter Current Value <r></h>
  __IO uint32_t CTRL;       // <h> Timer Control
                                //   <o.7> TimerEn: Timer Enable
                                //   <o.6> TimerMode: Timer Mode
                                //     <0=> Freerunning-mode
                                //     <1=> Periodic mode
                                //   <o.5> IntEnable: Interrupt Enable
                                //   <o.2..3> TimerPre: Timer Prescale
                                //     <0=> / 1
                                //     <1=> / 16
                                //     <2=> / 256
                                //     <3=> Undefined!
                                //   <o.1> TimerSize: Timer Size
                                //     <0=> 16-bit counter
                                //     <1=> 32-bit counter
                                //   <o.0> OneShot: One-shoot mode
                                //     <0=> Wrapping mode
                                //     <1=> One-shot mode
                            // </h>
  __O  uint32_t INTCLR;     // <h> Timer Interrupt Clear <w></h>
  __I  uint32_t RIS;        // <h> Timer Raw Interrupt Status <r></h>
  __I  uint32_t MIS;        // <h> Timer Masked Interrupt Status <r></h>
  __IO uint32_t BGLOAD;     // <h> Background Load Register </h>
} DUALTIMER_Type;

#define DUALTIMER_LOAD_Pos             0                                            /*!< DUALTIMER LOAD: LOAD Position */
#define DUALTIMER_LOAD_Msk             (0xFFFFFFFFul << DUALTIMER_LOAD_Pos)         /*!< DUALTIMER LOAD: LOAD Mask */

#define DUALTIMER_VALUE_Pos            0                                            /*!< DUALTIMER VALUE: VALUE Position */
#define DUALTIMER_VALUE_Msk            (0xFFFFFFFFul << DUALTIMER_VALUE_Pos)        /*!< DUALTIMER VALUE: VALUE Mask */

#define DUALTIMER_CTRL_EN_Pos          7                                            /*!< DUALTIMER CTRL_EN: CTRL Enable Position */
#define DUALTIMER_CTRL_EN_Msk          (0x1ul << DUALTIMER_CTRL_EN_Pos)             /*!< DUALTIMER CTRL_EN: CTRL Enable Mask */

#define DUALTIMER_CTRL_MODE_Pos        6                                            /*!< DUALTIMER CTRL_MODE: CTRL MODE Position */
#define DUALTIMER_CTRL_MODE_Msk        (0x1ul << DUALTIMER_CTRL_MODE_Pos)           /*!< DUALTIMER CTRL_MODE: CTRL MODE Mask */

#define DUALTIMER_CTRL_INTEN_Pos       5                                            /*!< DUALTIMER CTRL_INTEN: CTRL Int Enable Position */
#define DUALTIMER_CTRL_INTEN_Msk       (0x1ul << DUALTIMER_CTRL_INTEN_Pos)          /*!< DUALTIMER CTRL_INTEN: CTRL Int Enable Mask */

#define DUALTIMER_CTRL_PRESCALE_Pos    2                                            /*!< DUALTIMER CTRL_PRESCALE: CTRL PRESCALE Position */
#define DUALTIMER_CTRL_PRESCALE_Msk    (0x3ul << DUALTIMER_CTRL_PRESCALE_Pos)       /*!< DUALTIMER CTRL_PRESCALE: CTRL PRESCALE Mask */

#define DUALTIMER_CTRL_SIZE_Pos        1                                            /*!< DUALTIMER CTRL_SIZE: CTRL SIZE Position */
#define DUALTIMER_CTRL_SIZE_Msk        (0x1ul << DUALTIMER_CTRL_SIZE_Pos)           /*!< DUALTIMER CTRL_SIZE: CTRL SIZE Mask */

#define DUALTIMER_CTRL_ONESHOOT_Pos    0                                            /*!< DUALTIMER CTRL_ONESHOOT: CTRL ONESHOOT Position */
#define DUALTIMER_CTRL_ONESHOOT_Msk    (0x1ul << DUALTIMER_CTRL_ONESHOOT_Pos)       /*!< DUALTIMER CTRL_ONESHOOT: CTRL ONESHOOT Mask */

#define DUALTIMER_INTCLR_Pos           0                                            /*!< DUALTIMER INTCLR: INT Clear Position */
#define DUALTIMER_INTCLR_Msk           (0x1ul << DUALTIMER_INTCLR_Pos)              /*!< DUALTIMER INTCLR: INT Clear  Mask */

#define DUALTIMER_RAWINTSTAT_Pos       0                                            /*!< DUALTIMER RAWINTSTAT: Raw Int Status Position */
#define DUALTIMER_RAWINTSTAT_Msk       (0x1ul << DUALTIMER_RAWINTSTAT_Pos)          /*!< DUALTIMER RAWINTSTAT: Raw Int Status Mask */

#define DUALTIMER_MASKINTSTAT_Pos      0                                            /*!< DUALTIMER MASKINTSTAT: Mask Int Status Position */
#define DUALTIMER_MASKINTSTAT_Msk      (0x1ul << DUALTIMER_MASKINTSTAT_Pos)         /*!< DUALTIMER MASKINTSTAT: Mask Int Status Mask */

#define DUALTIMER_BGLOAD_Pos           0                                            /*!< DUALTIMER BGLOAD: Background Load Position */
#define DUALTIMER_BGLOAD_Msk           (0xFFFFFFFFul << DUALTIMER_BGLOAD_Pos)       /*!< DUALTIMER BGLOAD: Background Load Mask */

/*@}*/ /* end of group DUALTIMER */


/*-------------------- General Purpose Input Output (GPIO) -------------------*/

/** @addtogroup GPIO GPIO
    @{
*/

typedef struct
{
    union {
        __IO    uint32_t    DATA;           /*!< Offset: 0x000  DATA Register (R/W)                             */
        __IO    sPin32      DATA_PIN;       /*!< Offset: 0x000  Data Register access by Pin                     */
    };
    union {
        __IO    uint32_t    DATAOUT;        /*!< Offset: 0x004  Data Output Latch Register (R/W) */
        __IO    sPin32      DATAOUT_PIN;    /*!< Offset: 0x004  GPIO Output Data Register access by Pin         */
    };
    union {
        __IO    uint32_t    PUN;            /*!< Offset: 0x008  GPIO Pull-Up Resistor NOT Register              */
        __IO    sPin32      PUN_PIN;        /*!< Offset: 0x008  GPIO Pull-Up Resistor NOT Register access by pin*/
    };
    union {
        __IO    uint32_t    WUN;            /*!< Offset: 0x00C  GPIO Wakeup NOT Register                        */
        __IO    sPin32      WUN_PIN;        /*!< Offset: 0x00C  GPIO Wakeup NOT Register access by pin          */
    };
    __IO    uint32_t    OUTENABLESET;       /*!< Offset: 0x010  Output Enable Set Register  (R/W)               */
    __IO    uint32_t    OUTENABLECLR;       /*!< Offset: 0x014  Output Enable Clear Register  (R/W)             */
    __IO    uint32_t    ALTFUNCSET;         /*!< Offset: 0x018  Alternate Function Set Register  (R/W)          */
    __IO    uint32_t    ALTFUNCCLR;         /*!< Offset: 0x01C  Alternate Function Clear Register  (R/W)        */
    __IO    uint32_t    INTENSET;           /*!< Offset: 0x020  Interrupt Enable Set Register  (R/W)            */
    __IO    uint32_t    INTENCLR;           /*!< Offset: 0x024  Interrupt Enable Clear Register  (R/W)          */
    __IO    uint32_t    INTTYPESET;         /*!< Offset: 0x028  Interrupt Type Set Register  (R/W)              */
    __IO    uint32_t    INTTYPECLR;         /*!< Offset: 0x02C  Interrupt Type Clear Register  (R/W)            */
    __IO    uint32_t    INTPOLSET;          /*!< Offset: 0x030  Interrupt Polarity Set Register  (R/W)          */
    __IO    uint32_t    INTPOLCLR;          /*!< Offset: 0x034  Interrupt Polarity Clear Register  (R/W)        */
    union {
        __I     uint32_t    INTSTATUS;      /*!< Offset: 0x038  Interrupt Status Register (R/ )                 */
        __O     uint32_t    INTCLEAR;       /*!< Offset: 0x038  Interrupt Clear Register ( /W)                  */
    };
            uint32_t    RESERVED1[241];
    __IO    uint32_t    LB_MASKED[256];     /*!< Offset: 0x400 - 0x7FC Lower byte Masked Access Register (R/W)  */
    __IO    uint32_t    UB_MASKED[256];     /*!< Offset: 0x800 - 0xBFC Upper byte Masked Access Register (R/W)  */
} GPIO_Type;

#define GPIO_DATA_Pos           0                                          /*!< GPIO DATA: DATA Position */
#define GPIO_DATA_Msk           (0xFFFFFFFFul << GPIO_DATA_Pos)          /*!< GPIO DATA: DATA Mask */

#define GPIO_DATAOUT_Pos        0                                          /*!< GPIO DATAOUT: DATAOUT Position */
#define GPIO_DATAOUT_Msk        (0xFFFFFFFFul << GPIO_DATAOUT_Pos)       /*!< GPIO DATAOUT: DATAOUT Mask */

#define GPIO_PUN_Pos            0                                          /*!< GPIO DATAOUT: DATAOUT Position */
#define GPIO_PUN_Msk            (0xFFFFFFFFul << GPIO_PUN_Pos)       /*!< GPIO DATAOUT: DATAOUT Mask */

#define GPIO_WUN_Pos            0                                          /*!< GPIO DATAOUT: DATAOUT Position */
#define GPIO_WUN_Msk            (0xFFFFFFFFul << GPIO_WUN_Pos)       /*!< GPIO DATAOUT: DATAOUT Mask */

#define GPIO_OUTENSET_Pos       0                                          /*!< GPIO OUTEN: OUTEN Position */
#define GPIO_OUTENSET_Msk       (0xFFFFFFFFul << GPIO_OUTEN_Pos)         /*!< GPIO OUTEN: OUTEN Mask */

#define GPIO_OUTENCLR_Pos       0                                          /*!< GPIO OUTEN: OUTEN Position */
#define GPIO_OUTENCLR_Msk       (0xFFFFFFFFul << GPIO_OUTEN_Pos)         /*!< GPIO OUTEN: OUTEN Mask */

#define GPIO_ALTFUNCSET_Pos     0                                          /*!< GPIO ALTFUNC: ALTFUNC Position */
#define GPIO_ALTFUNCSET_Msk     (0xFFFFFFFFul << GPIO_ALTFUNC_Pos)       /*!< GPIO ALTFUNC: ALTFUNC Mask */

#define GPIO_ALTFUNCCLR_Pos     0                                          /*!< GPIO ALTFUNC: ALTFUNC Position */
#define GPIO_ALTFUNCCLR_Msk     (0xFFFFFFFFul << GPIO_ALTFUNC_Pos)       /*!< GPIO ALTFUNC: ALTFUNC Mask */

#define GPIO_INTENSET_Pos       0                                          /*!< GPIO INTEN: INTEN Position */
#define GPIO_INTENSET_Msk       (0xFFFFFFFFul << GPIO_INTEN_Pos)         /*!< GPIO INTEN: INTEN Mask */

#define GPIO_INTENCLR_Pos       0                                          /*!< GPIO INTEN: INTEN Position */
#define GPIO_INTENCLR_Msk       (0xFFFFFFFFul << GPIO_INTEN_Pos)         /*!< GPIO INTEN: INTEN Mask */

#define GPIO_INTTYPESET_Pos     0                                          /*!< GPIO INTTYPE: INTTYPE Position */
#define GPIO_INTTYPESET_Msk     (0xFFFFFFFFul << GPIO_INTTYPE_Pos)       /*!< GPIO INTTYPE: INTTYPE Mask */

#define GPIO_INTTYPECLR_Pos     0                                          /*!< GPIO INTTYPE: INTTYPE Position */
#define GPIO_INTTYPECLR_Msk     (0xFFFFFFFFul << GPIO_INTTYPE_Pos)       /*!< GPIO INTTYPE: INTTYPE Mask */

#define GPIO_INTPOLSET_Pos      0                                          /*!< GPIO INTPOL: INTPOL Position */
#define GPIO_INTPOLSET_Msk      (0xFFFFFFFFul << GPIO_INTPOL_Pos)        /*!< GPIO INTPOL: INTPOL Mask */

#define GPIO_INTPOLCLR_Pos      0                                          /*!< GPIO INTPOL: INTPOL Position */
#define GPIO_INTPOLCLR_Msk      (0xFFFFFFFFul << GPIO_INTPOL_Pos)        /*!< GPIO INTPOL: INTPOL Mask */

#define GPIO_INTSTATUS_Pos      0                                          /*!< GPIO INTSTATUS: INTSTATUS Position */
#define GPIO_INTSTATUS_Msk      (0xFFFFFFFFul << GPIO_INTSTATUS_Pos)       /*!< GPIO INTSTATUS: INTSTATUS Mask */

#define GPIO_INTCLEAR_Pos       0                                          /*!< GPIO INTCLEAR: INTCLEAR Position */
#define GPIO_INTCLEAR_Msk       (0xFFFFFFFFul << GPIO_INTCLEAR_Pos)        /*!< GPIO INTCLEAR: INTCLEAR Mask */


/*@}*/ /* end of group GPIO */

/*------------- System Control (SYSCON) --------------------------------------*/
/** @addtogroup SYSCON System Control
    @{
*/
typedef struct
{
    __IO   uint32_t  REMAP;          /*!< Offset: 0x000 Remap Control Register (R/W) */
    __IO   uint32_t  PMUCTRL;        /*!< Offset: 0x004 PMU Control Register (R/W) */
    __IO   uint32_t  RESETOP;        /*!< Offset: 0x008 Reset Option Register  (R/W) */
    __IO   uint32_t  EMICTRL;        /*!< Offset: 0x00C EMI Control Register  (R/W) */
    __IO   uint32_t  RSTINFO;        /*!< Offset: 0x010 Reset Information Register (R/W) */
} SYSCON_Type;

#define SYSCON_REMAP_Pos                 0
#define SYSCON_REMAP_Msk                 (0x01ul << SYSCON_REMAP_Pos)               /*!< SYSCON MEME_CTRL: REMAP Mask */

#define SYSCON_PMUCTRL_EN_Pos            0
#define SYSCON_PMUCTRL_EN_Msk            (0x01ul << SYSCON_PMUCTRL_EN_Pos)          /*!< SYSCON PMUCTRL: PMUCTRL ENABLE Mask */

#define SYSCON_LOCKUPRST_RESETOP_Pos     0
#define SYSCON_LOCKUPRST_RESETOP_Msk     (0x01ul << SYSCON_LOCKUPRST_RESETOP_Pos)   /*!< SYSCON SYS_CTRL: LOCKUP RESET ENABLE Mask */

#define SYSCON_EMICTRL_SIZE_Pos          24
#define SYSCON_EMICTRL_SIZE_Msk          (0x00001ul << SYSCON_EMICTRL_SIZE_Pos)     /*!< SYSCON EMICTRL: SIZE Mask */

#define SYSCON_EMICTRL_TACYC_Pos         16
#define SYSCON_EMICTRL_TACYC_Msk         (0x00007ul << SYSCON_EMICTRL_TACYC_Pos)    /*!< SYSCON EMICTRL: TURNAROUNDCYCLE Mask */

#define SYSCON_EMICTRL_WCYC_Pos          8
#define SYSCON_EMICTRL_WCYC_Msk          (0x00003ul << SYSCON_EMICTRL_WCYC_Pos)     /*!< SYSCON EMICTRL: WRITECYCLE Mask */

#define SYSCON_EMICTRL_RCYC_Pos          0
#define SYSCON_EMICTRL_RCYC_Msk          (0x00007ul << SYSCON_EMICTRL_RCYC_Pos)     /*!< SYSCON EMICTRL: READCYCLE Mask */

#define SYSCON_RSTINFO_SYSRESETREQ_Pos   0
#define SYSCON_RSTINFO_SYSRESETREQ_Msk   (0x00001ul << SYSCON_RSTINFO_SYSRESETREQ_Pos) /*!< SYSCON RSTINFO: SYSRESETREQ Mask */

#define SYSCON_RSTINFO_WDOGRESETREQ_Pos  1
#define SYSCON_RSTINFO_WDOGRESETREQ_Msk  (0x00001ul << SYSCON_RSTINFO_WDOGRESETREQ_Pos) /*!< SYSCON RSTINFO: WDOGRESETREQ Mask */

#define SYSCON_RSTINFO_LOCKUPRESET_Pos   2
#define SYSCON_RSTINFO_LOCKUPRESET_Msk   (0x00001ul << SYSCON_RSTINFO_LOCKUPRESET_Pos) /*!< SYSCON RSTINFO: LOCKUPRESET Mask */

/*@}*/ /* end of group SYSCON */

/*------------------- Watchdog ----------------------------------------------*/
/** @addtogroup WATCHDOG Watchdog
    @{
*/
typedef struct
{
    __IO    uint32_t  LOAD;         // <h> Watchdog Load Register </h>
    __I     uint32_t  VALUE;        // <h> Watchdog Value Register </h>
    __IO    uint32_t  CTRL;         // <h> Watchdog Control Register
                                    //   <o.1>    RESEN: Reset enable
                                    //   <o.0>    INTEN: Interrupt enable
                                    // </h>
    __O     uint32_t  INTCLR;       // <h> Watchdog Clear Interrupt Register </h>
    __I     uint32_t  RAWINTSTAT;   // <h> Watchdog Raw Interrupt Status Register </h>
    __I     uint32_t  MASKINTSTAT;  // <h> Watchdog Interrupt Status Register </h>
        uint32_t  RESERVED0[762];
    __IO    uint32_t  LOCK;         // <h> Watchdog Lock Register </h>
        uint32_t  RESERVED1[191];
    __IO    uint32_t  ITCR;         // <h> Watchdog Integration Test Control Register </h>
    __O     uint32_t  ITOP;         // <h> Watchdog Integration Test Output Set Register </h>
} WATCHDOG_Type;

#define WATCHDOG_LOAD_Pos               0                                              /*!< WATCHDOG LOAD: LOAD Position */
#define WATCHDOG_LOAD_Msk              (0xFFFFFFFFul << WATCHDOG_LOAD_Pos)       /*!< WATCHDOG LOAD: LOAD Mask */

#define WATCHDOG_VALUE_Pos              0                                              /*!< WATCHDOG VALUE: VALUE Position */
#define WATCHDOG_VALUE_Msk             (0xFFFFFFFFul << WATCHDOG_VALUE_Pos)      /*!< WATCHDOG VALUE: VALUE Mask */

#define WATCHDOG_CTRL_RESEN_Pos         1                                              /*!< WATCHDOG CTRL_RESEN: Enable Reset Output Position */
#define WATCHDOG_CTRL_RESEN_Msk        (0x1ul << WATCHDOG_CTRL_RESEN_Pos)        /*!< WATCHDOG CTRL_RESEN: Enable Reset Output Mask */

#define WATCHDOG_CTRL_INTEN_Pos         0                                              /*!< WATCHDOG CTRL_INTEN: Int Enable Position */
#define WATCHDOG_CTRL_INTEN_Msk        (0x1ul << WATCHDOG_CTRL_INTEN_Pos)        /*!< WATCHDOG CTRL_INTEN: Int Enable Mask */

#define WATCHDOG_INTCLR_Pos             0                                              /*!< WATCHDOG INTCLR: Int Clear Position */
#define WATCHDOG_INTCLR_Msk            (0x1ul << WATCHDOG_INTCLR_Pos)            /*!< WATCHDOG INTCLR: Int Clear Mask */

#define WATCHDOG_RAWINTSTAT_Pos         0                                              /*!< WATCHDOG RAWINTSTAT: Raw Int Status Position */
#define WATCHDOG_RAWINTSTAT_Msk        (0x1ul << WATCHDOG_RAWINTSTAT_Pos)        /*!< WATCHDOG RAWINTSTAT: Raw Int Status Mask */

#define WATCHDOG_MASKINTSTAT_Pos        0                                              /*!< WATCHDOG MASKINTSTAT: Mask Int Status Position */
#define WATCHDOG_MASKINTSTAT_Msk       (0x1ul << WATCHDOG_MASKINTSTAT_Pos)       /*!< WATCHDOG MASKINTSTAT: Mask Int Status Mask */

#define WATCHDOG_LOCK_Pos               0                                              /*!< WATCHDOG LOCK: LOCK Position */
#define WATCHDOG_LOCK_Msk              (0x1ul << WATCHDOG_LOCK_Pos)              /*!< WATCHDOG LOCK: LOCK Mask */

#define WATCHDOG_INTEGTESTEN_Pos        0                                              /*!< WATCHDOG INTEGTESTEN: Integration Test Enable Position */
#define WATCHDOG_INTEGTESTEN_Msk       (0x1ul << WATCHDOG_INTEGTESTEN_Pos)       /*!< WATCHDOG INTEGTESTEN: Integration Test Enable Mask */

#define WATCHDOG_INTEGTESTOUTSET_Pos    1                                              /*!< WATCHDOG INTEGTESTOUTSET: Integration Test Output Set Position */
#define WATCHDOG_INTEGTESTOUTSET_Msk   (0x1ul << WATCHDOG_INTEGTESTOUTSET_Pos)   /*!< WATCHDOG INTEGTESTOUTSET: Integration Test Output Set Mask */

/*@}*/ /* end of group  WATCHDOG */


/*------------------- PWM ----------------------------------------------*/
/** @addtogroup PWM Pulse Width Modulation Controller
    @{
*/
typedef struct
{
    __IO    uint32_t    CTRL;       // PWM control register
                                    //      BITS[0:2] - Prescal by power(2, BITS[0:2])
                                    //      BITS[3]   - clock source select. 0=system clock, 1=RTC or IRC
                                    //      BITS[7]   - Enable PWM
    __IO    uint32_t    HIGH_LOW;   //  PWM set high/low register
                                    //      BITS[0:7] - Set LOW width by 256-LOW
                                    //      BITS[8:16]- Set HIGH wdith by 256-HIGH
                                    //      The HIGH must large then LOW.
} PWM_Type;

#define PWM_CTRL_PRESCAL_Pos        0
#define PWM_CTRL_PRESCAL_Msk        (0x7ul << PWM_CTRL_PRESCAL_Pos)

#define PWM_CTRL_CKS_Pos            3
#define PWM_CTRL_CKS_Msk            (0x1ul << PWM_CTRL_CKS_Pos)

#define PWM_CTRL_EN_Pos             7
#define PWM_CTRL_EN_Msk             (0x1ul << PWM_CTRL_EN_Pos)

#define PWM_HIGH_LOW_LOW_Pos        0
#define PWM_HIGH_LOW_LOW_Msk        (0xFFul << PWM_HIGH_LOW_LOW_Pos)

#define PWM_HIGH_LOW_HIGH_Pos       8
#define PWM_HIGH_LOW_HIGH_Msk       (0xFFul << PWM_HIGH_LOW_HIGH_Pos)

/*@}*/ /* end of group  PWM */

/*------------------- I2CM ----------------------------------------------*/
/** @addtogroup I2CM I2C Master
    @{
*/
typedef struct
{
    __IO    uint32_t  SLAVE_ADDR;       // Offset: 0x000    I2C Slave Address register
                                        //  [0]     - R/S(receive / not send). 0=SEND, 1=RECEIVE
                                        //  [7:1]   - 7 address bits (A6-A0)
    union {
        __O     uint32_t  CTRL;         // Offset: 0x004    I2C Master Control register
                                        //  [0]     - RUN
                                        //  [1]     - START
                                        //  [2]     - STOP
                                        //  [3]     - ACK
                                        //  [4]     - HS
                                        //  [5]     -
                                        //  [6]     - SLRST
                                        //  [7]     - RSTB
        __I     uint32_t  STATUS;       // Offset: 0x004    I2C Master Status register
                                        //  [0]     - BUSY      -
                                        //  [1]     - ERROR
                                        //  [2]     - ADDR_ACK
                                        //  [3]     - DATA_ACK
                                        //  [4]     - ARB_LOST
                                        //  [5]     - IDLE
                                        //  [6]     - BUS_BUSY
                                        //  [7]     - reserved
    };
    __IO    uint32_t  BUF;              // Offset: 0x008    I2C Master Buffer data
    __IO    uint32_t  TP;               // Offset: 0x00C    I2C Master Timer Period Register
    __IO    uint32_t  SCL_LP;           // Offset: 0x010    I2C Master SCL Low Period.
    __IO    uint32_t  SCL_HP;           // Offset: 0x014    I2C Master SCL High Period.
    __IO    uint32_t  DATA_SETUP_TIME;  // Offset: 0x018    I2C Master SDA setup time
    __IO    uint32_t  INT;              // Offset: 0x01C    I2C Master Interrupt Control
                                        //  [0]     - ENABLE    Write 1 to Enable INT. Write 0 to disable INT.
                                        //  [1]     - FLAG      Write 1 to Clean
    __O     uint32_t  IO_SEL;           // Offset: 0x020
                                        //  [0]     - SEL
} I2CM_Type;

#define I2CM_SLAVE_ADDR_RS_Pos      0
#define I2CM_SLAVE_ADDR_RS_Msk      (0x1ul<<I2CM_SLAVE_ADDR_RS_Pos)

#define I2CM_SLAVE_ADDR_ADDR_Pos    1
#define I2CM_SLAVE_ADDR_ADDR_Msk    (0x7Ful<<I2CM_SLAVE_ADDR_Pos)

#define I2CM_CTRL_RUN_Pos           0
#define I2CM_CTRL_RUN_Msk           (0x1ul<<I2CM_CTRL_RUN_Pos)

#define I2CM_CTRL_START_Pos         1
#define I2CM_CTRL_START_Msk         (0x1ul<<I2CM_CTRL_START_Pos)

#define I2CM_CTRL_STOP_Pos          2
#define I2CM_CTRL_STOP_Msk          (0x1ul<<I2CM_CTRL_STOP_Pos)

#define I2CM_CTRL_ACK_Pos           3
#define I2CM_CTRL_ACK_Msk           (0x1ul<<I2CM_CTRL_ACK_Pos)

#define I2CM_CTRL_HS_Pos            4
#define I2CM_CTRL_HS_Msk            (0x1ul<<I2CM_CTRL_HS_Pos)

#define I2CM_CTRL_ADDR_Pos          5
#define I2CM_CTRL_ADDR_Msk          (0x1ul<<I2CM_CTRL_ADDR_Pos)

#define I2CM_CTRL_SLRST_Pos         6
#define I2CM_CTRL_SLRST_Msk         (0x1ul<<I2CM_CTRL_SLRST_Pos)

#define I2CM_CTRL_RSTB_Pos          7
#define I2CM_CTRL_RSTB_Msk          (0x1ul<<I2CM_CTRL_RSTB_Pos)

#define I2CM_STATUS_BUSY_Pos        0
#define I2CM_STATUS_BUSY_Msk        (0x1ul<<I2CM_STATUS_BUSY_Pos)

#define I2CM_STATUS_ERROR_Pos       1
#define I2CM_STATUS_ERROR_Msk       (0x1ul<<I2CM_STATUS_ERROR_Pos)

#define I2CM_STATUS_ADDR_ACK_Pos    2
#define I2CM_STATUS_ADDR_ACK_Msk    (0x1ul<<I2CM_STATUS_ADDR_ACK_Pos)

#define I2CM_STATUS_DATA_ACK_Pos    3
#define I2CM_STATUS_DATA_ACK_Msk    (0x1ul<<I2CM_STATUS_DATA_ACK_Pos)

#define I2CM_STATUS_ARB_LOST_Pos    4
#define I2CM_STATUS_ARB_LOST_Msk    (0x1ul<<I2CM_STATUS_ARB_LOST_Pos)

#define I2CM_STATUS_IDLE_Pos        5
#define I2CM_STATUS_IDLE_Msk        (0x1ul<<I2CM_STATUS_IDLE_Pos)

#define I2CM_STATUS_BUS_BUSY_Pos    6
#define I2CM_STATUS_BUS_BUSY_Msk    (0x1ul<<I2CM_STATUS_BUS_BUSY_Pos)

#define I2CM_BUF_Pos                0
#define I2CM_BUF_Msk                (0xFFul<<I2CM_BUF_Pos)

#define I2CM_TP_Pos                 0
#define I2CM_TP_Msk                 (0x7Ful<<I2CM_TP_Pos)

#define I2CM_SCL_LOW_Pos            0
#define I2CM_SCL_LOW_Msk            (0xFul<<I2CM_SCL_LOW_Pos)

#define I2CM_SCL_HIGH_Pos           0
#define I2CM_SCL_HIGH_Msk           (0xFul<<I2CM_SCL_HIGH_Pos)

#define I2CM_DATA_SETUP_TIME_Pos    0
#define I2CM_DATA_SETUP_TIME_Msk    (0xFul<<I2CM_DATA_SETUP_TIME_Pos)

#define I2CM_INT_ENABLE_Pos         0
#define I2CM_INT_ENABLE_Msk         (0x1ul<<I2CM_INT_ENABLE_Pos)

#define I2CM_INT_FLAG_Pos           1
#define I2CM_INT_FLAG_Msk           (0x1ul<<I2CM_INT_FLAG_Pos)

#define I2CM_IO_SEL_Pos             0
#define I2CM_IO_SEL_Msk             (0x1ul<<I2CM_IO_SEL_Pos)

/*@}*/ /* end of group  I2CM */


/*------------------- I2CS ----------------------------------------------*/
/** @addtogroup I2CS I2C Slave
    @{
*/
typedef struct
{
    __IO    uint32_t  ADDR;             // Offset: 0x000    I2C Slave Address
    union {
        __O     uint32_t  CTRL;         // Offset: 0x004    I2C Slave Control
                                        //  [2]     - SENDFINCLR    Write 1 to Clean SENDFIN
                                        //  [3]     - RECFINCLR     Write 1 to Clean RECFIN
                                        //  [6]     - DA            Device Active
                                        //  [7]     - RSTB          I2CS reset bit
        __I     uint32_t  STATUS;       // Offset: 0x004    I2C Slave Status
                                        //  [0]     - RREQ          Read BUF to Clean
                                        //  [1]     - TREQ          Write BUF to Clean
                                        //  [2]     - SENDFIN       Send Finish
                                        //  [3]     - RECFIN        Receive Finish
                                        //  [4]     - BUSACTIVE     Bus actived
                                        //  [6]     - DA            Device Actived
    };
    __IO    uint32_t  BUF;              // Offset: 0x008    I2C Slave Data Buffer
    __IO    uint32_t  ADDR10BITS;       // Offset: 0x00C    I2C Slave Address 10bits extern
    __IO    uint32_t  INT;              // Offset: 0x010    I2C Slave Interrupt

} I2CS_Type;

#define I2CS_ADDR_Pos               0
#define I2CS_ADDR_Msk               (0x7Ful<<I2CS_ADDR_Pos)

#define I2CS_CTRL_SENDFINCLR_Pos    2
#define I2CS_CTRL_SENDFINCLR_Msk    (0x1ul<<I2CS_CTRL_SENDFINCLR_Pos)

#define I2CS_CTRL_RECFINCLR_Pos     3
#define I2CS_CTRL_RECFINCLR_Msk     (0x1ul<<I2CS_CTRL_RECFINCLR_Pos)

#define I2CS_CTRL_DA_Pos            6
#define I2CS_CTRL_DA_Msk            (0x1ul<<I2CS_CTRL_DA_Pos)

#define I2CS_CTRL_RSTB_Pos          7
#define I2CS_CTRL_RSTB_Msk          (0x1ul<<I2CS_CTRL_RSTB_Pos)

#define I2CS_STATUS_RREQ_Pos        0
#define I2CS_STATUS_RREQ_Msk        (0x1ul<<I2CS_STATUS_RREQ_Pos)

#define I2CS_STATUS_TREQ_Pos        1
#define I2CS_STATUS_TREQ_Msk        (0x1ul<<I2CS_STATUS_TREQ_Pos)

#define I2CS_STATUS_SENDFIN_Pos     2
#define I2CS_STATUS_SENDFIN_Msk     (0x1ul<<I2CS_STATUS_SENDFIN_Pos)

#define I2CS_STATUS_RECFIN_Pos      3
#define I2CS_STATUS_RECFIN_Msk      (0x1ul<<I2CS_STATUS_RECFIN_Pos)

#define I2CS_STATUS_BUSACTIVE_Pos   4
#define I2CS_STATUS_BUSACTIVE_Msk   (0x1ul<<I2CS_STATUS_BUSACTIVE_Pos)

#define I2CS_STATUS_DA_Pos          6
#define I2CS_STATUS_DA_Msk          (0x1ul<<I2CS_STATUS_DA_Pos)

#define I2CS_BUF_Pos                0
#define I2CS_BUF_Msk                (0xFFul<<I2CS_BUF_Pos)

#define I2CS_ADDR10BITS_A9A8A7_Pos  0
#define I2CS_ADDR10BITS_A9A8A7_Msk  (0x7ul<<I2CS_ADDR10BITS_A9A8A7_Pos)

#define I2CS_ADDR10BITS_EN_Pos      3
#define I2CS_ADDR10BITS_EN_Msk      (0x1ul<<I2CS_ADDR10BITS_EN_Pos)

#define I2CS_INT_ENABLE_Pos         0
#define I2CS_INT_ENABLE_Msk         (0x1ul<<I2CS_INT_ENABLE_Pos)

#define I2CS_INT_FLAG_Pos           1
#define I2CS_INT_FLAG_Msk           (0x1ul<<I2CS_INT_FLAG_Pos)

/*@}*/ /* end of group  I2CS */



/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/** @addtogroup PERIPHERAL_MEM_MAP Peripheral Memory Map
    @{
*/
#define TIMER0_BASE         0x40000000
#define TIMER1_BASE         0x40001000
#define DUALTIMER1_BASE     0x40002000
#define DUALTIMER2_BASE     0x40002020
#define UART0_BASE          0x40004000
#define UART1_BASE          0x40005000
#define UART2_BASE          0x40006000
#define WATCHDOG_BASE       0x40008000
#define GPIO0_BASE          0x40010000
#define GPIO1_BASE          0x40011000
#define SYSCTRL_BASE        0x4001F000
#define I2CM_BASE           0x50003000
#define I2CS_BASE           0x50003800
#define PWM0_BASE           0x50004000
#define PWM1_BASE           0x50004100
#define PWM2_BASE           0x50004200
#define PWM3_BASE           0x50004300
/*@}*/ /* end of group PERIPHERAL_MEM_MAP */

/******************************************************************************/
/*                         Peripheral Definitions                             */
/******************************************************************************/
/** @addtogroup PERIPHERAL Peripheral Definitions
    @{
*/
#define TIMER0          ((TIMER_Type        *)  TIMER0_BASE)
#define TIMER1          ((TIMER_Type        *)  TIMER1_BASE)
#define DUALTIMER1      ((DUALTIMER_Type    *)  DUALTIMER1_BASE)
#define DUALTIMER2      ((DUALTIMER_Type    *)  DUALTIMER2_BASE)
#define UART0           ((UART_Type         *)  UART0_BASE)
#define UART1           ((UART_Type         *)  UART1_BASE)
#define UART2           ((UART_Type         *)  UART2_BASE)
#define WATCHDOG        ((WATCHDOG_Type     *)  WATCHDOG_BASE)
#define GPIO0           ((GPIO_Type         *)  GPIO0_BASE)
#define GPIO1           ((GPIO_Type         *)  GPIO1_BASE)
#define SYSCTRL         ((SYSCON_Type       *)  SYSCTRL_BASE )
#define I2CM            ((I2CM_Type         *)  I2CM_BASE)
#define I2CS            ((I2CS_Type         *)  I2CS_BASE)
#define PWM0            ((PWM_Type          *)  PWM0_BASE)
#define PWM1            ((PWM_Type          *)  PWM1_BASE)
#define PWM2            ((PWM_Type          *)  PWM2_BASE)
#define PWM3            ((PWM_Type          *)  PWM3_BASE)
/*@}*/ /* end of group PERIPHERAL */

#endif
