// System.c: implementation of the system functions.
//
//////////////////////////////////////////////////////////////////////
#include "main.h"
#include "System.h"

//==============================================================================
//--- Declarative Variables ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative Global Variables ---
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//--- Declarative Member Variables ---
//------------------------------------------------------------------------------
uint32_t m_uiAltFuncSet;
uint32_t m_uiOutEnableSet;
uint32_t m_uiPun; 				//IO Setting Info.


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
void System()
{
	
}


//==============================================================================
// Function     : 
// Purpose      : ResetReadPointer
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void ResetReadPointer(void)
{
	RADIO->RESET = 0x20;
}

//==============================================================================
// Function     : BackupIOSetting
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
    // Backup IO setting and any need peripherals setting
    // There include 
    //      GPIO0 / I2C / SPI / UART0,1,2 / TIMER0, 1 / Dualtimer / Watchdog / 
    //      MPU_LCD / RTC (Real Time Counter) / RF 
    // User need backup setting for restore if need.
void BackupIOSetting(void)
{
	m_uiAltFuncSet = GPIO0->ALTFUNCSET;
	m_uiOutEnableSet = GPIO0->OUTENABLESET;
	m_uiPun = GPIO0->PUN;
}


//==============================================================================
// Function     : RestoreIOSetting
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
    // restore IO setting and any need peripherals re-setting
void RestoreIOSetting(void)
{
	GPIO0->ALTFUNCSET   = m_uiAltFuncSet;			// restore ALTFUNCSET
	GPIO0->OUTENABLESET = m_uiOutEnableSet;		// restore OUTENABLESET
	GPIO0->PUN          = m_uiPun;				// restore PUN
}

//==============================================================================
// Function     : ClearIOSetting
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void ClearIOSetting()
{
//	GPIO0->OUTENABLESET = 0;
	GPIO0->ALTFUNCCLR   = 0xFFFFFFFF;       // clean all IO function
	GPIO0->OUTENABLECLR = 0xFFFFFFFF;       // set all IO to input
	GPIO0->PUN          = 0; 								// set all IO to pull-high
	
	GPIO0->OUTENABLESET = (1<<0) | (1<<14);  // set IO to output
	GPIO0->PUN_PIN.P00 = 1;
	GPIO0->PUN_PIN.P14 = 1;

	GPIO0->OUTENABLECLR = (1<<12);
	GPIO0->PUN_PIN.P12 = 1; //without pull-up

}

//==============================================================================
// Function     : SetFifoLength
// Purpose      : Set the length of FIFO
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void SetFifoLength(uint32_t uiFifoLen)
{
	RADIO->FIFOCTRL = uiFifoLen - 1;	// 
}
	

//==============================================================================
// Function     : TurnOnExternalRTC
// Purpose      : Turn On External RTC Crystal
// Description	: Initial external RTC Crystal for sleep timer
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void TurnOnExternalRTC(void)
{
  POWER->RCCTRL = POWER_RCCTRL_RCTS_Msk;
  POWER->RCCTRL2 = 0x03040012;
  POWER->RCTARGET = 977;
  POWER->RCCTRL = POWER_RCCTRL_MAN_Msk | POWER_RCCTRL_TMRE_Msk | POWER_RCCTRL_RCTS_Msk;
  Delay1ms(100);
}

//==============================================================================
// Function     : TurnOnExternalRTC
// Purpose      : Turn On External RTC Crystal
// Description	: Initial external RTC Crystal for sleep timer
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
uint32_t GetTickCount(void)
{
	return RTC->COUNT;
}

//==============================================================================
// Function     : ShowCurrentTime
// Purpose      : Show current time
// Description	: Display current time in the console
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void ShowCurrentTime(void)
{
	INFORMATION_MESSAGE(FLAG_MF_SYSTEM, _T("%02d:%02d:%02d.%02d0 "), 	
            (int)((RTC->COUNT & RTC_COUNT_HOUR_Msk)>>RTC_COUNT_HOUR_Pos),
            (int)((RTC->COUNT & RTC_COUNT_MIN_Msk)>>RTC_COUNT_MIN_Pos),
            (int)((RTC->COUNT & RTC_COUNT_SEC_Msk)>>RTC_COUNT_SEC_Pos),		// Sec
            (int)((RTC->COUNT & RTC_COUNT_TMS_Msk)>>RTC_COUNT_TMS_Pos));	// Ten ms
}

//==============================================================================
// Function     : ShowTickCount
// Purpose      : Show tick count
// Description	: Display tick counts in the console
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void ShowTickCount(uint32_t uiTime)
{
	INFORMATION_MESSAGE(FLAG_MF_SYSTEM, _T("%02d:%02d:%02d.%02d0 "),	
            (int)((uiTime & RTC_COUNT_HOUR_Msk)>>RTC_COUNT_HOUR_Pos),
            (int)((uiTime & RTC_COUNT_MIN_Msk)>>RTC_COUNT_MIN_Pos),
            (int)((uiTime & RTC_COUNT_SEC_Msk)>>RTC_COUNT_SEC_Pos),		// Sec
            (int)((uiTime & RTC_COUNT_TMS_Msk)>>RTC_COUNT_TMS_Pos));	// Ten ms
}


void ShowErrorMessage(PTCHAR lpszTitle, uint32_t uiError)
{
	PTCHAR lpszDefaultTitle = _T("");
	PTCHAR lpszJunction = _T(": ");
	if (lpszTitle == NULL)
	{
		lpszTitle = lpszDefaultTitle;
		lpszJunction = _T("");
	}

	switch (uiError) 
	{
		case ERROR_SUCCESS:
			DEBUG_MESSAGE(FLAG_MF_SYSTEM, _T("%s%s%s"), lpszTitle, lpszJunction, _T("The operation completed successfully.\r\n"));
			break;
		case ERROR_INVALID_HANDLE:
			DEBUG_MESSAGE(FLAG_MF_SYSTEM, _T("%s%s%s"), lpszTitle, lpszJunction, _T("The handle is invalid.\r\n"));
			break;
		case ERROR_NOT_ENOUGH_MEMORY:
			DEBUG_MESSAGE(FLAG_MF_SYSTEM, _T("%s%s%s"), lpszTitle, lpszJunction, _T("Not enough storage is available to process this command.\r\n"));
			break;
		case ERROR_WAIT_TIMEOUT:
			DEBUG_MESSAGE(FLAG_MF_SYSTEM, _T("%s%s%s"), lpszTitle, lpszJunction, _T("The wait operation timed out.\r\n"));			
			break;
		default:
			break;					
	}	
}


//bool GetHexID(const PTCHAR lpszDeviceID, unsigned char byDeviceID[MAX_DEVICE_IDENTIFIER], bool fLeftToRight)
bool GetHexID(const PTCHAR lpszDeviceID, PESL_DEVICE_ID pstDeviceID, bool fLeftToRight)
{
	TCHAR * pszPos = NULL;	
	PTCHAR lpszDeviceIdPos = (PTCHAR) lpszDeviceID;
	
	if ((lpszDeviceID == NULL) || (pstDeviceID == NULL))
		return false;
	
	pszPos = _tcsstr(lpszDeviceID, _T("-"));
	if (pszPos == NULL)
	{
		lpszDeviceIdPos = lpszDeviceID;
	}
	else
	{
		lpszDeviceIdPos = pszPos;
		lpszDeviceIdPos++;
	}	
	
	if (GetHexValue(lpszDeviceIdPos, pstDeviceID->byEFID, MAX_DEVICE_IDENTIFIER, fLeftToRight))
	{
		return true;
	}
	return false;
}


bool GetDeviceHexID(const PTCHAR lpszDeviceID, unsigned char byDeviceID[MAX_DEVICE_IDENTIFIER], bool fLeftToRight)
{
	if (lpszDeviceID == NULL)
		return FALSE;
	if (GetHexValue(lpszDeviceID, byDeviceID, MAX_DEVICE_IDENTIFIER, fLeftToRight))
	{
		return true;
	}
	return false;
}

bool GetHexValue(const PTCHAR lpszText, LPBYTE lpbyValue, uint8_t uiSize, bool fLeftToRight)
{
	int nIndex = 0;
	LPBYTE lpbyPos = lpbyValue;
	UINT uiCount = 0;
	PTCHAR lpszTextPos = (PTCHAR) lpszText;
	
	if ((_tcslen(lpszTextPos) > 2) && (
		((lpszTextPos[0] == _T('0')) && (lpszTextPos[1] == _T('x'))) ||
		((lpszTextPos[0] == _T('0')) && (lpszTextPos[1] == _T('X')))))
		lpszTextPos += 2;

	memset(lpbyValue, 0, uiSize);

	for (*lpbyValue = 0, nIndex = 0; *lpszTextPos; lpszTextPos++, nIndex++)
	{
		uiCount = 0;
		while (lpszTextPos[0] == _T('-'))
		{
			lpszTextPos++;
			uiCount++;
			if (uiCount >= uiSize)
				return false;
		}

		if (*lpszTextPos >= _T('0') && *lpszTextPos <= _T('9'))
			*lpbyPos = (*lpbyPos << 4) + (BYTE)(*lpszTextPos - _T('0'));
		else if (*lpszTextPos >= _T('a') && *lpszTextPos <= _T('f'))
			*lpbyPos = (*lpbyPos << 4) + (BYTE)(*lpszTextPos - _T('a') + 10U);
		else if (*lpszTextPos >= _T('A') && *lpszTextPos <= _T('F'))
			*lpbyPos = (*lpbyPos << 4) + (BYTE)(*lpszTextPos - _T('A') + 10U);
		else
			return false;
		if (nIndex % 2 == 1)
			lpbyPos++;
		if ((lpbyPos - lpbyValue) >= uiSize)
			break;
	}
	return true;
}

//------------------------------------------------------------------------------
//--- ESL Device Unique Identifier ---
//------------------------------------------------------------------------------
//#define EUID_GATEWAY_ID		0xFF, 0xFF, 0xFF	// ESL Gateway Device Unique Identifier
//#define EUID_REPEATER_ID	0xFF, 0xFF, 0xFE	// ESL Repeater Device Unique Identifier
//#define EUID_SCANNER_ID		0xFF, 0xFF, 0xFD	// ESL Scanner Device Unique Identifier

bool IsGatewayID(const unsigned char byDeviceID[MAX_DEVICE_IDENTIFIER])
{
	if ((byDeviceID[0] == 0xFF) && (byDeviceID[1] == 0xFF) && (byDeviceID[2] == 0xFF))
		return true;
	return false;
}

bool IsRepeaterID(const unsigned char byDeviceID[MAX_DEVICE_IDENTIFIER])
{
	if ((byDeviceID[0] == 0xFF) && (byDeviceID[1] == 0xFF) && (byDeviceID[2] == 0xFE))
		return true;
	return false;
}

bool IsScannerID(const unsigned char byDeviceID[MAX_DEVICE_IDENTIFIER])
{
	if ((byDeviceID[0] == 0xFF) && (byDeviceID[1] == 0xFF) && (byDeviceID[2] == 0xFD))
		return true;
	return false;
}



//==============================================================================
// Function     : SetSpiDataWidth
// Purpose      :    xxxSPI master mode transfer data and receive data throught SPI
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-05-07
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void SetSpiDataWidth(uint8_t uiDataWidth)
{
	SPI->CTRL = (SPI->CTRL & 0xF0FF) | ((uiDataWidth - 1) << SPI_CTL_TX_BIT_LEN_Pos);
	SPI->STATUS |= 0x01;
}


//==============================================================================
// Function     : BatteryDetect
// Purpose      : Battery Detect
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-05-07
// -----------------------------------------------------------------------------
// Parameters   : bvt (0~7)
//              :     0=1.875V, 1=1.95V, 2=2.025V, 3=2.1V, 4=2.175V, 5=2.25V, 6=2.325V, 7=2.4V
// Return       : 0 if V(REGI) < V(bvt).
//              : 1 if V(regi) > V(bvt).
// Remarks      : 
//==============================================================================

uint32_t BatteryDetect(uint32_t bvt)
{
    POWER->BatteryItem.BVT = bvt;
    POWER->BatteryItem.BDS = 1;
    while(POWER->BatteryItem.BDS);
    return POWER->BatteryItem.BDF;
}

//==============================================================================
// Function     : BatteryValue
// Purpose      : Battery Detect
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-05-07
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : Returns a value between 0 and 100
// Remarks      : bvt (0~7)
//              :     0=1.875V, 1=1.95V, 2=2.025V, 3=2.1V, 4=2.175V, 5=2.25V, 6=2.325V, 7=2.4V
//==============================================================================
uint8_t BatteryValue()
{
	uint32_t bvt;
	uint8_t Battery_Level = 0;
	uint8_t Battery_Value = 0;
	for(bvt = 0; bvt < 8; bvt++)
	{
		Battery_Level += BatteryDetect(bvt);
	}
	Battery_Value = 12.5 * Battery_Level;
	return Battery_Value;
}
