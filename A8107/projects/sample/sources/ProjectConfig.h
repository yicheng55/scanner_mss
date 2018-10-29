/* ProjectConfig.h: Davicom scanner device header for ESL system.                     */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: ProjectConfig.h                                                     */
/* Purpose      : ESL scanner device header for ESL system                      */
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
#ifndef _PROJECT_CONFIG_H_
#define _PROJECT_CONFIG_H_
#include "EslConfig.h"

//==============================================================================
//--- Declarative Constants ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Board ---
//------------------------------------------------------------------------------
#define XTAL    (16000000UL)            /* Oscillator frequency               */

#define UART0_BAUDRATE		115200 // baudrate 115200 for Console. (Debug Message)
#define UART2_BAUDRATE		9600 // baudrate 9600 for Scanner



#endif		//_PROJECT_CONFIG_H_
