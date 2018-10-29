 /* System.h: Davicom system header for ESL system.                     */
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
#ifndef _DAVICOM_SYSTEM_H_
#define _DAVICOM_SYSTEM_H_

#include "AMICCOM_CM0.h"
#include "A8107M.h"
#include "system_A8107M.h"
#include "ProjectConfig.h"
#include "EslDebug.h"
#include "ProjectConfig.h"

#ifdef __cplusplus
 extern "C" {
#endif


//==============================================================================
//--- Declarative Macro ---
//==============================================================================
#define GET_HOUR(rtc) 	    	(int)((rtc->COUNT & RTC_COUNT_HOUR_Msk)>>RTC_COUNT_HOUR_Pos)
#define GET_MINUTE(rtc) 	  	(int)((rtc->COUNT & RTC_COUNT_MIN_Msk)>>RTC_COUNT_MIN_Pos)             
#define GET_SECOND(rtc)	    	(int)((rtc->COUNT & RTC_COUNT_SEC_Msk)>>RTC_COUNT_SEC_Pos)              
#define HTONS(n) 				(uint16_t)((((uint16_t) (n)) << 8) | (((uint16_t) (n)) >> 8))
#define HTONL(n) 				(uint32_t)((n[0] << 24) | (n[1] << 16) | (n[2] << 8) | (n[3]));

#ifdef _UNICODE
#define _tcslen		wstring
#define _tcsstr		wcsstr
#define _ttoi		_wtoi
#define _tcscpy		wcscpy 
#else
#define _tcslen		strlen
#define _tcsstr		strstr
#define _ttoi		atoi
#define _tcscpy		strcpy 
#endif

//==============================================================================
//--- Declarative Constants ---
//==============================================================================

#define YEAR_IN_SEC				31556926
#define DAY_IN_SEC				86400
#define HOUR_IN_SEC				3600 
#define MINUTE_IN_SEC			60

//==============================================================================
//--- Declarative Functions ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative constructor ---
//------------------------------------------------------------------------------
void System(void);
//------------------------------------------------------------------------------
//--- Declarative Public Functions ---
//------------------------------------------------------------------------------

void ResetReadPointer(void);
void BackupIOSetting(void);
void RestoreIOSetting(void);
void ClearIOSetting(void);

void SetFifoLength(uint32_t uiFifoLen);
void TurnOnExternalRTC(void);		// Turn On External RTC Crystal

//--- Time Functions ---
uint32_t GetTickCount(void);
void ShowCurrentTime(void);
void ShowTickCount(uint32_t uiTime);

//-- Message Functions ---
void ShowErrorMessage(PTCHAR lpszTitle, uint32_t uiError);

//-- Message Functions ---
//bool GetHexID(const PTCHAR lpszDeviceID, unsigned char byDeviceID[MAX_DEVICE_IDENTIFIER], bool fLeftToRight);
bool GetHexID(const PTCHAR lpszDeviceID, PESL_DEVICE_ID pstDeviceID, bool fLeftToRight);
bool GetDeviceHexID(const PTCHAR lpszDeviceID, unsigned char byDeviceID[MAX_DEVICE_IDENTIFIER], bool fLeftToRight);
bool GetHexValue(const PTCHAR lpszText, LPBYTE lpbyValue, uint8_t uiSize, bool fLeftToRight);

bool IsGatewayID(const unsigned char byDeviceID[MAX_DEVICE_IDENTIFIER]);
bool IsRepeaterID(const unsigned char byDeviceID[MAX_DEVICE_IDENTIFIER]);
bool IsScannerID(const unsigned char byDeviceID[MAX_DEVICE_IDENTIFIER]);

// SPI
void SetSpiDataWidth(uint8_t uiDataWidth);

// Battery
uint32_t BatteryDetect(uint32_t bvt);
uint8_t BatteryValue(void);

//------------------------------------------------------------------------------
//--- Declarative Protected Functions ---
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//--- Declarative Private Function ---
//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif	// _DAVICOM_SYSTEM_H_
