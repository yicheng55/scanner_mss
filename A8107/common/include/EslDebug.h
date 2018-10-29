/* EslDebug.h: Davicom ESL debug message header for ESL system.                 */
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
#ifndef _DAVICOM_ESL_DEBUG_MESSAGE_H_
#define _DAVICOM_ESL_DEBUG_MESSAGE_H_

#include "Debug.h"


//==============================================================================
//--- Declarative Message Flags ---
//==============================================================================
// Custom Message Function
// The following declaration for the message architecture classification

#define FLAG_MF_GATEWAY								0x00000100		// Custom Function
#define FLAG_MF_TAG									0x00000200		// Custom Function
#define FLAG_MF_REPEATER							0x00000400		// Custom Function
#define FLAG_MF_SCANNER								0x00000800		// Custom Function
//#define FLAG_MF_CUSTOM5							0x00001000		// Custom Function
//#define FLAG_MF_CUSTOM6							0x00002000		// Custom Function
//#define FLAG_MF_CUSTOM7							0x00004000		// Custom Function
//#define FLAG_MF_CUSTOM8							0x00008000		// Custom Function

//==============================================================================
//--- Declarative Message Macro ---
//==============================================================================
// RF
#define RfDebug(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_COMMUNICATION, FLAG_MT_DEBUG)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)
	
#define RfWarning(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_COMMUNICATION, FLAG_MT_WARNING)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)		
	
#define RfError(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_COMMUNICATION, FLAG_MT_ERROR)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)	
	
#define RfTrace(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_COMMUNICATION, FLAG_MT_TRACE)) \
		{						\
			_tprintf("%s(%d): ", __FILE__, __LINE__); \
			_tprintf fmt; 		\
		}						\
    }while(0)	


// Tag
#define TagDebug(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_TAG, FLAG_MT_DEBUG)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)
	
#define TagWarning(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_TAG, FLAG_MT_WARNING)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)		
	
#define TagError(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_TAG, FLAG_MT_ERROR)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)	
	
#define TagTrace(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_TAG, FLAG_MT_TRACE)) \
		{						\
			_tprintf("%s(%d): ", __FILE__, __LINE__); \
			_tprintf fmt; 		\
		}						\
    }while(0)	
	
// Repeater	
#define RepeaterDebug(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_REPEATER, FLAG_MT_DEBUG)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)
	
#define RepeaterWarning(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_REPEATER, FLAG_MT_WARNING)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)	
	
#define RepeaterError(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_REPEATER, FLAG_MT_ERROR)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)

#define RepeaterTrace(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_REPEATER, FLAG_MT_TRACE)) \
		{						\
			_tprintf("%s(%d): ", __FILE__, __LINE__); \
			_tprintf fmt; 		\
		}						\
    }while(0)	
	
// Scanner	
#define ScannerDebug(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_SCANNER, FLAG_MT_DEBUG)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)
	
#define ScannerWarning(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_SCANNER, FLAG_MT_WARNING)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)	
	
#define ScannerError(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_SCANNER, FLAG_MT_ERROR)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)	

#define ScannerTrace(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_SCANNER, FLAG_MT_TRACE)) \
		{						\
			_tprintf("%s(%d): ", __FILE__, __LINE__); \
			_tprintf fmt; 		\
		}						\
    }while(0)	

#endif		//_DAVICOM_ESL_DEBUG_MESSAGE_H_
