/* main.h: Davicom main header for ESL system.                                  */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: main.h                                                        */
/* Purpose      : Main header                                                   */
/* Autor        : Richard Chuang                                                */
/* Email        : richard_chung@davicom.com.tw                                  */
/* Description  :                                                               */
/* Reference    :                                                               */
/* Copyright    : (c) Davicom Semiconductor Inc. All Rights Reserved.           */
/* -----------------------------------------------------------------------------*/
/* Version      : 0.0.0.5                                                       */
/* Update       : 2018-03-01                                                    */
/* Modified     : Richard Chung                                                 */
/* Description	:                                                               */
/* -----------------------------------------------------------------------------*/
/* Version      : 0.0.0.6                                                       */
/* Update       : 2019-06-21                                                    */
/* Modified     : Yi-Cheng                                                      */
/* Description	: Resend Barcode mode.                                                             */
/* -----------------------------------------------------------------------------*/
/* Version      : 0.0.1.0                                                       */
/* Update       : 2019-08-13                                                    */
/* Modified     : Yi-Cheng                                                      */
/* Description	: Sacnner Auto mode update.                                                             */
/* -----------------------------------------------------------------------------*/
/* How to use:                                                                  */
/*                                                                              */
#ifndef _MAIN_H_
#define _MAIN_H_
#include "ProjectConfig.h"
//#include "basic_windef.h"
//#include "EslDebug.h"


	 
	 
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "A8107M.h"
#include "uart.h"
//#include "uart_cpp.h"
#include "uart_stdout.h"
#include "A8107M_RFLIB.h"
#include "slptimer.h"

#include "Utility.h"
//#include "M8107.h"
#include "isr.h"
#include "Board.h"

#include "timer.h"
#include "RfCommunication.h"

#ifdef __cplusplus
 extern "C" {
#endif





//==============================================================================
//--- Declarative Functions ---
//==============================================================================

//------------------------------------------------------------------------------
//--- Declarative Global Functions ---
//------------------------------------------------------------------------------
extern void initRF(void);
extern void SetCH(uint8_t ch);
extern void StrobeCmd(uint8_t cmd);
	 
//------------------------------------------------------------------------------
//--- Declarative Public Functions ---
//------------------------------------------------------------------------------
//void UserRegister(uint8_t uiDataRate);
void OnTimer(UINT_PTR nIDEvent);
//void BeaconSync(bool fBinding);
void BeaconCount(bool fLost, uint8_t uiBeaconMode);

//------------------------------------------------------------------------------
//--- Declarative Protected Functions ---
//------------------------------------------------------------------------------


//==============================================================================
//--- Declarative Variables ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative Global Variables ---
//------------------------------------------------------------------------------

//extern uint8_t g_MachineState;
//extern ESL_NVM_DATA g_stFlashMappingData;
extern SYSTEMTIME g_stSystemTime;


#ifdef __cplusplus
}
#endif

#endif		//_MAIN_H_
