/* Debug.h: Davicom debug message header for ESL system.                        */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: Debug.h                                                       */
/* Purpose      : Debug message header                                          */
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
#ifndef _DAVICOM_DEBUG_MESSAGE_H_
#define _DAVICOM_DEBUG_MESSAGE_H_

#include "basic_windef.h"
#include <Stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif



//========================================================================
//--- Message Type ---
//	0 - Emergency(emerg)
//	1 - Alerts(alert)
//	2 - Critical(crit)
//	3 - Errors(err)
//	4 - Warnings(warn)
//	5 - Notification(notice)
//	6 - Information(info)
//	7 - Debug(debug)
//========================================================================
// each level also logged messages in 'lower' levels.
// There was sometimes a question as to whether a debug message should be 
// DEBUG or HIDEBUG but we mostly based it on the number of lines it would 
// push out to the log file.
// https://stackoverflow.com/questions/312378/debug-levels-when-writing-an-application


//==============================================================================
//--- Declarative Message Flags ---
//==============================================================================
// Message Function
// The following declaration for the message architecture classification
#define FLAG_MF_KERNEL								0x00000001		// Kernel Function
#define FLAG_MF_HARDWARE							0x00000002		// Hardware Function
#define FLAG_MF_SYSTEM								0x00000004		// System Function
#define FLAG_MF_DRIVER								0x00000008		// Driver Function
#define FLAG_MF_FIRMWARE							0x00000010		// Firmware Function
#define FLAG_MF_COMMUNICATION						0x00000020		// Communication Function
#define FLAG_MF_APPLICATION							0x00000040		// Application Function
#define FLAG_MF_USER_INTERFACE						0x00000080		// User Interface Function
#define FLAG_MF_CUSTOM1								0x00000100		// Custom Function
#define FLAG_MF_CUSTOM2								0x00000200		// Custom Function
#define FLAG_MF_CUSTOM3								0x00000400		// Custom Function
#define FLAG_MF_CUSTOM4								0x00000800		// Custom Function
#define FLAG_MF_CUSTOM5								0x00001000		// Custom Function
#define FLAG_MF_CUSTOM6								0x00002000		// Custom Function
#define FLAG_MF_CUSTOM7								0x00004000		// Custom Function
#define FLAG_MF_CUSTOM8								0x00008000		// Custom Function
#define FLAG_MF_ALL									0xFFFFFFFF		// All Functions
//------------------------------------------------------------------------------
// Message Type
// The following declaration for the message classification
#define FLAG_MT_SEVERE								0x00000000		// severe errors that require program exit(e.g., in an application, you ran out of disk space).
#define FLAG_MT_ALERTS								0x00000001		// 	
#define FLAG_MT_CRITICAL							0x00000002		// 
#define FLAG_MT_ERROR								0x00000003		// Error Message, error messages that can't be recovered from but the program can continue to run (e.g., in a server application, client sent through bad data but other clients can continue to run).
#define FLAG_MT_WARNING								0x00000004		// Warning, recoverable problem that you should be notified about (e.g., invalid value in a configuration file, so you fell back to the default).
#define FLAG_MT_NOTIFICATION						0x00000005		// 
#define FLAG_MT_INFORMATION							0x00000006		// informational messages. 
// The following declaration is only used in debug mode 
#define FLAG_MT_ENTRY								0x00000007		// log entry and exit to all functions.
#define FLAG_MT_PARM								0x00000008		// log entry and exit to all functions with parameters passed and values returned (including global effects if any).
#define FLAG_MT_TRACE								0x00000009		// Trace
#define FLAG_MT_DEBUG								0x0000000A		// Debug Message
#define FLAG_MT_HIDEBUG								0x0000000B		// Debug Message for more detailed debugging messages such as hex dumps of buffers.
#define FLAG_MT_ALL									0xFFFFFFFF		// All messages

//==============================================================================
//--- Declarative Functions ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative constructor ---
//------------------------------------------------------------------------------
void Debug(void);
//------------------------------------------------------------------------------
//--- Public Function ---
//------------------------------------------------------------------------------
// Setup message functions
void SetMessageLevel (unsigned long ulMessageLevel);
void SetMessageFunc (unsigned long ulMessageFunc);
unsigned long GetMessageLevel (void);
unsigned long GetMessageFunc (void);

void DebugSample(void);	// Sample code
bool IsPrintable(unsigned long ulMessageFunc, unsigned long ulMessageType);

// Message functions
int OUTPUT_MESSAGE(unsigned long ulFuncFlags, unsigned long ulLevelFlags, const TCHAR *szFormat, ...);

// Hierarchical message output functions
int	SEVERE_MESSAGE(unsigned long ulFuncFlags, const TCHAR *szFormat, ...);
int	ALERTS_MESSAGE(unsigned long ulFuncFlags, const TCHAR *szFormat, ...);
int	CRITICAL_MESSAGE(unsigned long ulFuncFlags, const TCHAR *szFormat, ...);
int	ERROR_MESSAGE(unsigned long ulFuncFlags, const TCHAR *szFormat, ...);
int	WARNING_MESSAGE(unsigned long ulFuncFlags, const TCHAR *szFormat, ...);
int	NOTIFICATION_MESSAGE(unsigned long ulFuncFlags, const TCHAR *szFormat, ...);
int	INFORMATION_MESSAGE(unsigned long ulFuncFlags, const TCHAR *szFormat, ...);
int	ENTRY_MESSAGE(unsigned long ulFuncFlags, const TCHAR *szFormat, ...);
int	PARM_MESSAGE(unsigned long ulFuncFlags, const TCHAR *szFormat, ...);
int	TRACE_MESSAGE(unsigned long ulFuncFlags, const TCHAR *szFormat, ...);
int	DEBUG_MESSAGE(unsigned long ulFuncFlags, const TCHAR *szFormat, ...);
int	HIDEBUG_MESSAGE(unsigned long ulFuncFlags, const TCHAR *szFormat, ...);



//------------------------------------------------------------------------------
//--- Protected Function ---
//------------------------------------------------------------------------------
int OutputMessage (const TCHAR *szFormat, ...);
int _tprintf(TCHAR* szFormat, ...);

#ifdef __cplusplus
}
#endif

#endif		//_DAVICOM_DEBUG_MESSAGE_H_
