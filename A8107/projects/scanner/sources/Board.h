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
#ifndef _DAVICOM_SCANNER_BOARD_H_
#define _DAVICOM_SCANNER_BOARD_H_

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "AMICCOM_CM0.h"
#include "A8107M.h"
#include "system_A8107M.h"
#include "ProjectConfig.h"
#include "EslDebug.h"
//#include "CUart.h"
	 
	 
 

#if 0
class Board
{
public:
	Board(void);
	~Board(void);
	void BoardInit(void);
	//void SetTimer(TIMER_Type *timer, uint32_t msec);
	UINT_PTR SetTimer(TIMER_Type *timer, UINT_PTR nIDEvent, UINT uiElapse, LPVOID lpTimerFunc);
	void SetOneshotTimer(TIMER_Type *timer, uint32_t msec);

	//------------------------------------------------------------------------------
	//--- Declarative Protected Functions ---
	//------------------------------------------------------------------------------
	void BoardInitUART0(uint32_t uiBaudrate);
	void BoardInitUART2(uint32_t uiBaudrate);
	void BoardInitSPI(void);
	void BoardInitTimer(TIMER_Type *timer, uint32_t msec);
	void BoardInitTimer0(void);
	void BoardInitGPIO(void);
	void BoardInitRTC(void);
	//void BoardPowerOn(void);
    //------------------------------------------------------------------------------
	//--- Declarative Private Function ---
	//------------------------------------------------------------------------------
	
	CUart m_objUart;
	

};	
#endif 
	 
//==============================================================================
//--- Declarative Functions ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative Public Functions ---
//------------------------------------------------------------------------------
void BoardInit(void);
//void SetTimer(TIMER_Type *timer, uint32_t msec);
UINT_PTR SetTimer(TIMER_Type *timer, UINT_PTR nIDEvent, UINT uiElapse, LPVOID lpTimerFunc);
void SetOneshotTimer(TIMER_Type *timer, uint32_t msec);

//------------------------------------------------------------------------------
//--- Declarative Protected Functions ---
//------------------------------------------------------------------------------
void BoardInitUART0(uint32_t uiBaudrate);
void BoardInitUART2(uint32_t uiBaudrate);
void BoardInitSPI(void);
void BoardInitTimer(TIMER_Type *timer, uint32_t msec);
void BoardInitTimer0(void);
void BoardInitGPIO(void);
void BoardInitRTC(void);



//------------------------------------------------------------------------------
//--- Declarative Private Function ---
//------------------------------------------------------------------------------




#ifdef __cplusplus
}
#endif

#endif	// _DAVICOM_SCANNER_BOARD_H_
