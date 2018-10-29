// Debug.c: implementation of the scanner functions.
//
//////////////////////////////////////////////////////////////////////
#include "Debug.h"
#include <stdarg.h>
#include <stdio.h>

//==============================================================================
//--- Declarative Member Variables ---
//==============================================================================
unsigned long  m_ulMessageFunc = FLAG_MF_APPLICATION | FLAG_MF_KERNEL | FLAG_MF_HARDWARE | FLAG_MF_SYSTEM | FLAG_MF_DRIVER | FLAG_MF_FIRMWARE | FLAG_MF_COMMUNICATION;
#if defined(_DEBUG) || defined(DEBUG)
unsigned long  m_ulMessageLevel = FLAG_MT_ALL;
#else
unsigned long  m_ulMessageLevel = FLAG_MT_INFORMATION;
#endif

//==============================================================================
// Function     : 
// Purpose      : Construction
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void Debug(void)
{

}

//==============================================================================
// Function     : SetMessageFunc
// Purpose      : Set message function
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void SetMessageFunc (unsigned long nMessageFunc)
{
	m_ulMessageFunc = nMessageFunc;
}

//==============================================================================
// Function     : SetMessageLevel
// Purpose      : Set message level
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void SetMessageLevel (unsigned long nMessageLevel)
{
	m_ulMessageLevel = nMessageLevel;
}

//==============================================================================
// Function     : SetMessageFunc
// Purpose      : Set message function
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
unsigned long GetMessageFunc (void)
{
	return m_ulMessageFunc;
}

//==============================================================================
// Function     : SetMessageLevel
// Purpose      : Set message level
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
unsigned long GetMessageLevel (void)
{
	return m_ulMessageLevel;
}

//==============================================================================
// Function     : IsPrintable
// Purpose      : Check whether the output message is allowed
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
bool IsPrintable(unsigned long ulMessageFunc, unsigned long ulMessageType)
{
	if (((m_ulMessageFunc & FLAG_MF_ALL) & (ulMessageFunc & FLAG_MF_ALL)) && 
		((m_ulMessageLevel & FLAG_MT_ALL) >= (ulMessageType & FLAG_MT_ALL))) 
		return true;
	return false;
}
	
//==============================================================================
// Function     : DebugSample
// Purpose      : Sample
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void DebugSample()
{
	// Declare Custom Debug
#define MyDebug(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_CUSTOM1, FLAG_MT_DEBUG)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)
	
#define MyError(fmt) \
	do{ \
		if (IsPrintable(FLAG_MF_CUSTOM1, FLAG_MT_ERROR)) \
		{						\
			_tprintf fmt; 		\
		}						\
    }while(0)
	
	//
	// Initial Message
	//
	unsigned long ulMessageFunc;
	unsigned long ulMessageLevel;
	
	// Add the debug flag of your project. 
	// Ex. FLAG_MF_GATEWAY, FLAG_MF_TAG, FLAG_MF_REPEATER, FLAG_MF_SCANNER, ...
	ulMessageFunc = GetMessageFunc();
	// Allow FLAG_MF_CUSTOM1 project output debug messages
	ulMessageFunc |= FLAG_MF_CUSTOM1;
	SetMessageFunc (ulMessageFunc);
	
	// Usually SetMessageLevel() does not need to be set. 
	// Preset in debug mode will output all debug messages.
	ulMessageLevel = GetMessageLevel();	
	// The debug message with the value equal to or lower than FLAG_MT_DEBUG will be output
#if 0	
	ulMessageLevel = FLAG_MT_DEBUG;
#endif	
	SetMessageLevel (ulMessageLevel);	

	
	//
	// Debug Message
	//
	
	INFORMATION_MESSAGE(FLAG_MF_APPLICATION, _T("\r\nDebug Sample Function\r\n"));
	// 
	DEBUG_MESSAGE(FLAG_MF_APPLICATION, "\r\nMessage for ANSI format\r\n");
	DEBUG_MESSAGE(FLAG_MF_APPLICATION, "Debug Sample\r\n");
	DEBUG_MESSAGE(FLAG_MF_APPLICATION, "Debug Sample: %d\r\n", 1);
	DEBUG_MESSAGE(FLAG_MF_APPLICATION, "Debug Sample: %s\r\n", "String");
	
	// For ANSI/UNICODE format	
	INFORMATION_MESSAGE(FLAG_MF_APPLICATION, "\r\nMessage for ANSI/UNICODE format\r\n");
	DEBUG_MESSAGE(FLAG_MF_APPLICATION, _T("Debug Sample\r\n"));
	DEBUG_MESSAGE(FLAG_MF_APPLICATION, _T("Debug Sample: %d\r\n"), 1);
	DEBUG_MESSAGE(FLAG_MF_APPLICATION, _T("Debug Sample: %s\r\n"), _T("String"));
	
	// DBGMSG
	INFORMATION_MESSAGE(FLAG_MF_APPLICATION, "\r\nMessage for DBGMSG\r\n");
	OUTPUT_MESSAGE(FLAG_MF_APPLICATION, FLAG_MT_INFORMATION, _T("Information Sample\r\n"));	
	OUTPUT_MESSAGE(FLAG_MF_APPLICATION, FLAG_MT_ERROR, _T("Error Sample: %d\r\n"), 1);
	OUTPUT_MESSAGE(FLAG_MF_APPLICATION, FLAG_MT_TRACE, _T("Trace Sample: %s\r\n"), _T("String"));
	
	// Custom Macro
	INFORMATION_MESSAGE(FLAG_MF_APPLICATION, "\r\nMessage for MyDebug\r\n");

	MyDebug(("Custom Debug Sample\r\n"));
	MyDebug(_T("Custom Debug Sample\r\n"));
	MyDebug(("Custom Debug Sample %d\r\n", 123));
	MyDebug((_T("Custom Debug Sample %d\r\n"), 123));	
	MyError((_T("Custom Error Sample: %d\r\n"), 25));	
	
}

//==============================================================================
// Function     : OutputMessage
// Purpose      : Output message for trace
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int OutputMessage(const TCHAR *szFormat, ...)
{
	va_list arg;
	int nResult = 0;
	va_start (arg, szFormat);	
	nResult = vfprintf (stdout, szFormat, arg);		// for _MBCS
	//nResult = _vftprintf (stdout, szFormat, arg);	// for TCHAR
	va_end (arg);
	
	return nResult;
}

int OUTPUT_MESSAGE(unsigned long ulFuncFlags, unsigned long ulLevelFlags, const TCHAR *szFormat, ...)
{
	va_list arg;
	int nResult = 0;

	if (((m_ulMessageFunc & FLAG_MF_ALL) & (ulFuncFlags & FLAG_MF_ALL)) &&
		((m_ulMessageLevel & FLAG_MT_ALL) >= (ulLevelFlags & FLAG_MT_ALL)))
	{
		if ((ulLevelFlags & FLAG_MT_ALL) == FLAG_MT_TRACE)
			printf("%s(%u): ", __FILE__, __LINE__);
	
		va_start (arg, szFormat);
		nResult = vfprintf (stdout, szFormat, arg);		// for _MBCS (_vftprintf for TCHAR)
		va_end (arg);
	}
	return nResult;
}

//==============================================================================
// Function     : 
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int _tprintf(TCHAR* szFormat, ...)
{
	va_list arg;
	int nResult;
	
	va_start (arg, szFormat);
	nResult = vfprintf (stdout, szFormat, arg);
	va_end (arg);

	return nResult;	
}

//==============================================================================
// Function     : 
// Purpose      : 
// Description	: severe errors that require program exit(e.g., in an application,
//              : you ran out of disk space).
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int	SEVERE_MESSAGE(unsigned long ulFuncFlags, const TCHAR* szFormat, ...)
{
	va_list arg;	
	int nResult = 0;	
	if ((m_ulMessageFunc & FLAG_MF_ALL) & (ulFuncFlags & FLAG_MF_ALL))
	{
		_tprintf(_T("Severe: ")); 
		va_start (arg, szFormat);
		nResult = vfprintf (stdout, szFormat, arg);
		va_end (arg);
	}
	return nResult;
}

//==============================================================================
// Function     : 
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int	ALERTS_MESSAGE(unsigned long ulFuncFlags, const TCHAR* szFormat, ...)
{
	va_list arg;	
	int nResult = 0;	
	if ((m_ulMessageFunc & FLAG_MF_ALL) & (ulFuncFlags & FLAG_MF_ALL))		
	{
		_tprintf(_T("Alerts: ")); 
		va_start (arg, szFormat);
		nResult = vfprintf (stdout, szFormat, arg);
		va_end (arg);
	}
	return nResult;
}

//==============================================================================
// Function     : 
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int	CRITICAL_MESSAGE(unsigned long ulFuncFlags, const TCHAR* szFormat, ...)
{
	va_list arg;	
	int nResult = 0;	
	if ((m_ulMessageFunc & FLAG_MF_ALL) & (ulFuncFlags & FLAG_MF_ALL))		
	{
		_tprintf(_T("Critical: ")); 
		va_start (arg, szFormat);
		nResult = vfprintf (stdout, szFormat, arg);
		va_end (arg);
	}
	return nResult;
}

//==============================================================================
// Function     : 
// Purpose      : 
// Description	: Error Message, error messages that can't be recovered from but 
//              : the program can continue to run (e.g., in a server application,
//              : client sent through bad data but other clients can continue to 
//              : run).
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int	ERROR_MESSAGE(unsigned long ulFuncFlags, const TCHAR* szFormat, ...)
{
	va_list arg;	
	int nResult = 0;	
	if ((m_ulMessageFunc & FLAG_MF_ALL) & (ulFuncFlags & FLAG_MF_ALL))		
	{
		_tprintf(_T("Error: ")); 
		va_start (arg, szFormat);
		nResult = vfprintf (stdout, szFormat, arg);
		va_end (arg);
	}
	return nResult;
}

//==============================================================================
// Function     : 
// Purpose      : 
// Description	: Warning, recoverable problem that you should be notified about 
//              : (e.g., invalid value in a configuration file, so you fell back 
//              : to the default).
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int	WARNING_MESSAGE(unsigned long ulFuncFlags, const TCHAR* szFormat, ...)
{
	va_list arg;	
	int nResult = 0;	
	if ((m_ulMessageFunc & FLAG_MF_ALL) & (ulFuncFlags & FLAG_MF_ALL))	
	{
		_tprintf(_T("Warning: ")); 
		va_start (arg, szFormat);
		nResult = vfprintf (stdout, szFormat, arg);
		va_end (arg);
	}
	return nResult;
}

//==============================================================================
// Function     : 
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int	NOTIFICATION_MESSAGE(unsigned long ulFuncFlags, const TCHAR* szFormat, ...)
{
	va_list arg;	
	int nResult = 0;	
	if ((m_ulMessageFunc & FLAG_MF_ALL) & (ulFuncFlags & FLAG_MF_ALL))		
	{
		_tprintf(_T("Notification: ")); 
		va_start (arg, szFormat);
		nResult = vfprintf (stdout, szFormat, arg);
		va_end (arg);
	}
	return nResult;
}

//==============================================================================
// Function     : 
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
// Informational messages. 
int	INFORMATION_MESSAGE(unsigned long ulFuncFlags, const TCHAR* szFormat, ...)
{
	va_list arg;
	int nResult = 0;	
	if ((m_ulMessageFunc & FLAG_MF_ALL) & (ulFuncFlags & FLAG_MF_ALL))	
	{
		//_tprintf(_T("Information: ")); 
		va_start (arg, szFormat);
		nResult = vfprintf (stdout, szFormat, arg);
		va_end (arg);
	}
	return nResult;	
}

//==============================================================================
// Function     : 
// Purpose      : log entry and exit to all functions.
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int	ENTRY_MESSAGE(unsigned long ulFuncFlags, const TCHAR* szFormat, ...)
{	
	va_list arg;
	int nResult = 0;
	if ((m_ulMessageFunc & FLAG_MF_ALL) & (ulFuncFlags & FLAG_MF_ALL))	
	{
		//_tprintf(_T("Entry: ")); 
		va_start (arg, szFormat);
		nResult = vfprintf (stdout, szFormat, arg);
		va_end (arg);
	}
	return nResult;	
}


//==============================================================================
// Function     : 
// Purpose      : 
// Description	: log entry and exit to all functions with parameters passed and 
//              : values returned (including global effects if any).
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int	PARM_MESSAGE(unsigned long ulFuncFlags, const TCHAR* szFormat, ...)
{	
	va_list arg;	
	int nResult = 0;	
	if ((m_ulMessageFunc & FLAG_MF_ALL) & (ulFuncFlags & FLAG_MF_ALL))		
	{
		//_tprintf(_T("Parm: ")); 
		va_start (arg, szFormat);
		nResult = vfprintf (stdout, szFormat, arg);
		va_end (arg);
	}
	return nResult;	
}

//==============================================================================
// Function     : 
// Purpose      : Trace
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int	TRACE_MESSAGE(unsigned long ulFuncFlags, const TCHAR* szFormat, ...)
{
	va_list arg;	
	int nResult = 0;	
	if ((m_ulMessageFunc & FLAG_MF_ALL) & (ulFuncFlags & FLAG_MF_ALL))	
	{
		//_tprintf(_T("Trace: ")); 
		_tprintf("%s(%d): ", __FILE__, __LINE__);

		va_start (arg, szFormat);
		nResult = vfprintf (stdout, szFormat, arg);
		va_end (arg);
	}
	return nResult;	
}

//==============================================================================
// Function     : DEBUG_MESSAGE
// Purpose      : Debug Message
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int	DEBUG_MESSAGE(unsigned long ulFuncFlags, const TCHAR* szFormat, ...)
{
	va_list arg;	
	int nResult = 0;	

	if ((m_ulMessageFunc & FLAG_MF_ALL) & (ulFuncFlags & FLAG_MF_ALL))	
	{
		//_tprintf(_T("Debug: ")); 
		va_start (arg, szFormat);
		nResult = vfprintf (stdout, szFormat, arg);
		va_end (arg);
	}
	return nResult;	
}

//==============================================================================
// Function     : 
// Purpose      : 
// Description	: Debug Message for more detailed debugging messages such as hex dumps of buffers.
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int	HIDEBUG_MESSAGE(unsigned long ulFuncFlags, const TCHAR* szFormat, ...)
{
	va_list arg;	
	int nResult = 0;	
	if ((m_ulMessageFunc & FLAG_MF_ALL) & (ulFuncFlags & FLAG_MF_ALL))	
	{
		//_tprintf(_T("Debug: ")); 
		va_start (arg, szFormat);
		nResult = vfprintf (stdout, szFormat, arg);
		va_end (arg);
	}
	return nResult;	
}


