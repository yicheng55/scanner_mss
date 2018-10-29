/**************************************************************************//**
 * @file        uart.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       UART driver source file
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "AMICCOM_CM0.h"

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup UART_Driver UART Driver
    @{
*/

/*UART driver functions*/

/**
 *
 * @brief   Initialises the UART specifying the UART Baud rate divider value and whether the send and recieve functionality is enabled.
 *          It also specifies which of the various interrupts are enabled.
 *
 * @param   *uart           UART_Type Pointer
 * @param   divider         The value to which the UART baud rate divider is to be set
 * @param   tx_en           Defines whether the UART transmit is to be enabled
 * @param   rx_en           Defines whether the UART receive is to be enabled
 * @param   tx_irq_en       Defines whether the UART transmit buffer full interrupt is to be enabled
 * @param   rx_irq_en       Defines whether the UART receive buffer full interrupt is to be enabled
 * @param   tx_ovrirq_en    Defines whether the UART transmit buffer overrun interrupt is to be enabled
 * @param   rx_ovrirq_en    Defines whether the UART receive buffer overrun interrupt is to be enabled
 * @return  1 if initialisation failed, 0 if successful.
 */
uint32_t UART_init(UART_Type *uart, uint32_t divider, uint32_t tx_en, uint32_t rx_en,
                uint32_t tx_irq_en, uint32_t rx_irq_en, uint32_t tx_ovrirq_en, uint32_t rx_ovrirq_en)
{
   uint32_t new_ctrl=0;

   if (tx_en != 0)          new_ctrl |= UART_CTRL_TXEN_Msk;
   if (rx_en != 0)          new_ctrl |= UART_CTRL_RXEN_Msk;
   if (tx_irq_en != 0)      new_ctrl |= UART_CTRL_TXIRQEN_Msk;
   if (rx_irq_en != 0)      new_ctrl |= UART_CTRL_RXIRQEN_Msk;
   if (tx_ovrirq_en != 0)   new_ctrl |= UART_CTRL_TXORIRQEN_Msk;
   if (rx_ovrirq_en != 0)   new_ctrl |= UART_CTRL_RXORIRQEN_Msk;

   uart->CTRL = 0;         /* Disable UART when changing configuration */
   uart->BAUDDIV = divider;
   uart->CTRL = new_ctrl;  /* Update CTRL register to new value */

   if((uart->STATE & (UART_STATE_RXOR_Msk | UART_STATE_TXOR_Msk))) return 1;
   else return 0;
}

/**
 *
 * @brief   Returns whether the RX buffer is full.
 *
 * @param   *uart           UART_Type Pointer
 * @return  RxBufferFull
 */
uint32_t UART_GetRxBufferFull(UART_Type *uart)
{
    return ((uart->STATE & UART_STATE_RXBF_Msk) >> UART_STATE_RXBF_Pos);
}

/**
 *
 * @brief   Returns whether the TX buffer is full.
 *
 * @param   *uart           UART_Type Pointer
 * @return  TxBufferFull
 */
uint32_t UART_GetTxBufferFull(UART_Type *uart)
{
    return ((uart->STATE & UART_STATE_TXBF_Msk) >> UART_STATE_TXBF_Pos);
}

/**
 *
 * @brief   Sends a character to the TX buffer for transmission.
 *
 * @param   *uart           UART_Type Pointer
 * @param   txchar          Character to be sent
 * @return  none
 */
void UART_SendChar(UART_Type *uart, char txchar)
{
    while(uart->STATE & UART_STATE_TXBF_Msk);
    uart->DATA = (uint32_t)txchar;
}

/**
 *
 * @brief   Returns the character from the RX buffer which has been received.
 *
 * @param   *uart           UART_Type Pointer
 * @return  rxchar
 */
char UART_ReceiveChar(UART_Type *uart)
{
    while( ! (uart->STATE & UART_STATE_RXBF_Msk));
    return (char)(uart->DATA);
}

/**
 *
 * @brief   Returns the current overrun status of both the RX & TX buffers.
 *
 * @param   *uart           UART_Type Pointer
 * @return  0 - No overrun
 * @return  1 - TX overrun
 * @return  2 - RX overrun
 * @return  3 - TX & RX overrun
 */
uint32_t UART_GetOverrunStatus(UART_Type *uart)
{
    return ((uart->STATE & (UART_STATE_RXOR_Msk | UART_STATE_TXOR_Msk)) >> UART_STATE_TXOR_Pos);
}

/**
 *
 * @brief   Clears the overrun status of both the RX & TX buffers and then returns the current overrun status.
 *
 * @param   *uart           UART_Type Pointer
 * @return  0 - No overrun
 * @return  1 - TX overrun
 * @return  2 - RX overrun
 * @return  3 - TX & RX overrun
 */
uint32_t UART_ClearOverrunStatus(UART_Type *uart)
{
    uart->STATE = (UART_STATE_RXOR_Msk | UART_STATE_TXOR_Msk);
    return ((uart->STATE & (UART_STATE_RXOR_Msk | UART_STATE_TXOR_Msk)) >> UART_STATE_TXOR_Pos);
}

/**
 *
 * @brief   Returns the current UART Baud rate divider. Note that the Baud rate divider is the difference between the clock frequency and the Baud frequency.
 *
 * @param   *uart           UART_Type Pointer
 * @return  BaudDiv
 */
uint32_t UART_GetBaudDivider(UART_Type *uart)
{
    return uart->BAUDDIV;
}

 /**
 *
 * @brief   Returns the TX interrupt status.
 *
 * @param   *uart           UART_Type Pointer
 * @return  TXStatus
 */
uint32_t UART_GetTxIRQStatus(UART_Type *uart)
{
    return ((uart->INTSTATUS & UART_CTRL_TXIRQ_Msk) >> UART_CTRL_TXIRQ_Pos);
}

/**
 *
 * @brief   Returns the RX interrupt status.
 *
 * @param   *uart           UART_Type Pointer
 * @return  RXStatus
 */
uint32_t UART_GetRxIRQStatus(UART_Type *uart)
{
    return ((uart->INTSTATUS & UART_CTRL_RXIRQ_Msk) >> UART_CTRL_RXIRQ_Pos);
}

 /**
 *
 * @brief   Clears the TX interrupt status.
 *
 * @param   *uart           UART_Type Pointer
 * @return  none
 */
void UART_ClearTxIRQ(UART_Type *uart)
{
    uart->INTCLEAR = UART_CTRL_TXIRQ_Msk;
}

/**
 *
 * @brief   Clears the RX interrupt status.
 *
 * @param   *uart           UART_Type Pointer
 * @return  none
 */

void UART_ClearRxIRQ(UART_Type *uart)
{
   uart->INTCLEAR = UART_CTRL_RXIRQ_Msk;
}

/*@}*/ /* end of group UART_Driver */
/*@}*/ /* end of group Std_Driver */
