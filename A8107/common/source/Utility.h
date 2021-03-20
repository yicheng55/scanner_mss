/* Utility.h: Davicom common utility header for ESL system.                     */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: Utility.h                                                     */
/* Purpose      : Function & Macro Definitions                                  */
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

#ifndef _DAVICOM_COMMON_UTILITY_H_
#define _DAVICOM_COMMON_UTILITY_H_

#include "Eslconfig.h"
#include "A8107M.h"
#include "system_A8107M.h"
#include <stdio.h>
#include "string.h"

#ifdef __cplusplus
 extern "C" {
#endif



//==============================================================================
//--- Declarative Functions ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative constructor ---
//------------------------------------------------------------------------------
void Utility(void);

//------------------------------------------------------------------------------
//--- Declarative Public Functions ---
//------------------------------------------------------------------------------
//void medium_delay(void);
void Delay1ms(uint32_t);
void Delay100us(uint32_t);
void Delay10us(uint32_t);
void Delay1us(uint32_t);
void Set_TAG_Mili_SleepTime(int SLPT);	 

#ifdef __cplusplus
}
#endif

#endif // _DAVICOM_COMMON_UTILITY_H_

