/* Hardware.h: Davicom scanner device header for ESL system.                     */
/*                                                                              */
/*==============================================================================*/
/* Header Name  : Hardware.h                                                     */
/* Purpose      : ESL hardware configure header for ESL system                      */
/* Autor        : Richard Chuang                                                */
/* Email        : richard_chung@davicom.com.tw                                  */
/* Description  :                                                               */
/* Reference    :                                                               */
/* Copyright    : (c) Davicom Semiconductor Inc. All Rights Reserved.           */
/* -----------------------------------------------------------------------------*/
/* Version      : 1.0.0.0                                                       */
/* Update       : 2018-03-01                                                    */
/* Modified     : Richard Chung                                                 */
/* Description  :                                                               */
/* -----------------------------------------------------------------------------*/
/* How to use:                                                                  */
/*                                                                              */
#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include "AMICCOM_CM0.h"
#include "A8107M.h"
#include "system_A8107M.h"

//------------------------------------------------------------------------------
//--- Declare Public Function ---
//------------------------------------------------------------------------------
void BoardInit(void);
void BoardInitRTC(void);
void SetTimer(TIMER_Type *timer, uint32_t msec);

//------------------------------------------------------------------------------
//--- Declare Protected Function ---
//------------------------------------------------------------------------------
void BoardInitUART0(uint32_t uiBaudrate);
void BoardInitUART2(uint32_t uiBaudrate);
void BoardInitSPI(void);
void BoardInitTimer0(void);

void BoardInitGPIO(void);
void SetOneshotTimer(TIMER_Type *timer, uint32_t msec);
void SetOneshotTimer0(uint32_t msec);
void SetOneshotTimer1(uint32_t msec);
#endif  // _HARDWARE_H_
