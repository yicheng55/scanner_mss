/**************************************************************************//**
 * @file        uart.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       UART Driver Header File
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/

#ifndef __UART_H__
#define __UART_H__


/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup UART_Driver UART Driver
    @{
*/

/**
 * @brief   Initialises the UART specifying the UART Baud rate divider value and whether the send and recieve functionality is enabled.
 *          It also specifies which of the various interrupts are enabled.
*/
extern uint32_t UART_init(UART_Type *uart, uint32_t divider, uint32_t tx_en, uint32_t rx_en,
                            uint32_t tx_irq_en, uint32_t rx_irq_en, uint32_t tx_ovrirq_en, uint32_t rx_ovrirq_en);

/**
* @brief Returns whether the UART RX Buffer is Full.
*/
extern uint32_t UART_GetRxBufferFull(UART_Type *uart);

/**
* @brief Returns whether the UART TX Buffer is Full.
*/
extern uint32_t UART_GetTxBufferFull(UART_Type *uart);

/**
* @brief Sends a character to the UART TX Buffer.
*/
extern void UART_SendChar(UART_Type *uart, char txchar);

/**
* @brief Receives a character from the UART RX Buffer.
*/
extern char UART_ReceiveChar(UART_Type *uart);

/**
* @brief Returns UART Overrun status.
*/
extern uint32_t UART_GetOverrunStatus(UART_Type *uart);

/**
* @brief Clears UART Overrun status Returns new UART Overrun status.
*/
extern uint32_t UART_ClearOverrunStatus(UART_Type *uart);

/**
* @brief Returns UART Baud rate Divider value.
*/
extern uint32_t UART_GetBaudDivider(UART_Type *uart);

/**
* @brief Return UART TX Interrupt Status.
*/
extern uint32_t UART_GetTxIRQStatus(UART_Type *uart);

/**
* @brief Return UART RX Interrupt Status.
*/
extern uint32_t UART_GetRxIRQStatus(UART_Type *uart);

/**
* @brief Clear UART TX Interrupt request.
*/
extern void UART_ClearTxIRQ(UART_Type *uart);

/**
* @brief Clear UART RX Interrupt request.
*/
extern void UART_ClearRxIRQ(UART_Type *uart);

/*@}*/ /* end of group UART_Driver */
/*@}*/ /* end of group Std_Driver */

#endif
