 /**************************************************************************//**
 * @file        uart_stdout.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       UART functions for retargetting
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "AMICCOM_CM0.h"
#include "system_A8107M.h"
#include "uart.h"

UART_Type* UartStdOut;

void UartStdOutInit(UART_Type *uart, uint32_t baudrate)
{
    UartStdOut = uart;
    UART_init(UartStdOut, (SystemCoreClock/baudrate), 1, 0, 0, 0, 0, 0 );
    return;
}
// Output a character
unsigned char UartPutc(unsigned char my_ch)
{
    UART_SendChar(UartStdOut, my_ch);
    return (my_ch);
}
// Get a character
unsigned char UartGetc(void)
{
    return UART_ReceiveChar(UartStdOut);
}

void UartEndSimulation(void)
{
    UartPutc((char) 0x4); // End of simulation
    while(1);
}

