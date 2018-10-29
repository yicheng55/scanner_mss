/**************************************************************************//**
 * @file        uart_stdout.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       The Header file for uart_stdout.c
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/

#ifndef _UART_STDOUT_H_
#define _UART_STDOUT_H_

extern void UartStdOutInit(UART_Type *uart, uint32_t baudrate);
extern unsigned char UartPutc(unsigned char my_ch);
extern unsigned char UartGetc(void);
extern unsigned char UartEndSimulation(void);

#endif
