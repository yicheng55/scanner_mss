 /* Hardware.h: Davicom scanner device header for ESL system.                     */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: Hardware.h                                                     */
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
/* Description	:                                                               */
/* -----------------------------------------------------------------------------*/
/* How to use:                                                                  */
/*                                                                              */
#ifndef _DAVICOM_SAMPLE_BOARD_H_
#define _DAVICOM_SAMPLE_BOARD_H_

#include "AMICCOM_CM0.h"
#include "A8107M.h"
#include "system_A8107M.h"
#include "ProjectConfig.h"
#include "EslDebug.h"

//------------------------------------------------------------------------------
//--- Declarative Public Function ---
//------------------------------------------------------------------------------
void BoardInit(void);
void SetTimer(TIMER_Type *timer, uint32_t msec);

//------------------------------------------------------------------------------
//--- Declarative Protected Function ---
//------------------------------------------------------------------------------
void BoardInitUART0(uint32_t uiBaudrate);
void BoardInitUART2(uint32_t uiBaudrate);
void BoardInitSPI(void);
void BoardInitTimer0(void);
void BoardInitGPIO(void);
void BoardInitRTC(void);

//------------------------------------------------------------------------------
//--- Declarative Private Function ---
//------------------------------------------------------------------------------
void SetOneshotTimer(TIMER_Type *timer, uint32_t msec);


#endif	// _DAVICOM_SAMPLE_BOARD_H_
