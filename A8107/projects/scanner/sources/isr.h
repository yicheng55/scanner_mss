/* isr.h: Davicom interrupt service routine header for ESL system.              */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: isr.h                                                         */
/* Purpose      : Interrupt Service Routine                                     */
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
#ifndef _DAVICOM_SCANNER_ISR_H_
#define _DAVICOM_SCANNER_ISR_H_
#include "ProjectConfig.h"
#ifdef __cplusplus
 extern "C" {
#endif

//==============================================================================
//--- Declarative Functions ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative Public Functions ---
//------------------------------------------------------------------------------

void SLPTIMER1_Handler(void);
void DUALTIMER_Handler(void);
void HardFault_Handler(void);
void UART2_Handler(void);

//------------------------------------------------------------------------------
//--- Declarative Extern Functions ---
//------------------------------------------------------------------------------
// Event Function
//extern void OnTimer(UINT_PTR nIDEvent);

#ifdef __cplusplus
}
#endif

#endif	// _DAVICOM_SCANNER_ISR_H_
