// Scanner.cpp: implementation of the scanner functions.
//
//////////////////////////////////////////////////////////////////////
#include "Scanner.h"
#include "RfCommunication.h"
#include "RfPacket.h"
#include "System.h"
#include <stdlib.h>
#include "Led.h"
#include "NonVolatileMemory.h"

//==============================================================================
//--- Declarative Variables ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative Global Variables ---
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//--- Declarative Member Variables ---
//------------------------------------------------------------------------------
BARCODE_PAIR m_stBarcodePair; 
TCHAR m_szBarcodeScan[MAX_BARCODE_SCAN+1];
TCHAR m_szBarcodeFormat[MAX_BARCODE_TAG+1];
TCHAR * m_pszBarcodePos = NULL;
unsigned int m_uiScannerLastState = 0;

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
void Scanner(void)
{
	ScannerInit();
}

//==============================================================================
// Function     : ScannerInit
// Purpose      : Init scanner
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void ScannerInit(void)
{
	memset((void *)&m_stBarcodePair, 0, sizeof(BARCODE_PAIR));
	memset((void *)&m_szBarcodeScan, 0, sizeof(m_szBarcodeScan));
	m_pszBarcodePos = &m_szBarcodeScan[0];
	m_uiScannerLastState = SCAN_STAGE_STANDBY;
	m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
}

//==============================================================================
// Function     : ScannerIsPairing
// Purpose      : Detect whether pairing or unbinding packets
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
bool ScannerIsPairing()
{
	if ((m_stBarcodePair.uiCommand == RF_CMD_SCANNER_PAIRING) || (m_stBarcodePair.uiCommand == RF_CMD_SCANNER_UNPAIRING))
		return true;
	return false;
}

bool ScannerIsRegistration()
{
	if (m_stBarcodePair.uiCommand == RF_CMD_SCANNER_REGISTRATION) 
		return true;
	return false;
}

bool ScannerIsBarcode()
{
	if (((g_stNvmMappingData.wScannerMode == OSSM_BARCODE_DEFAULT_MODE) || 
		 (g_stNvmMappingData.wScannerMode == OSSM_BARCODE_READER_MODE)) && 
		 (m_stBarcodePair.uiCommand == RF_CMD_SCANNER_BARCODE) &&
		 (m_stBarcodePair.fSendBarcode == true))
		return true;
	return false;
}

//==============================================================================
// Function     : ScannerIsCommand
// Purpose      : Detect whether factory command packets
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
bool ScannerIsCommand(void)
{
	if (m_stBarcodePair.uiCommand == RF_CMD_SCANNER_COMMAND)
		return true;
	return false;
}

//==============================================================================
// Function     : ScannerStringToArray
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
bool ScannerStringToArray(BYTE * pbyDestArray, int nArrayLength, TCHAR * pszSourceText)
{
	int nLength = strlen(pszSourceText);
	int nIndex;
	if (nLength > nArrayLength)
		return false;
	memset((void *)pbyDestArray, 0, nArrayLength);
	for (nIndex = 0; nIndex < nLength; nIndex++)
	{
		pbyDestArray[nIndex] = pszSourceText[nIndex];
	}
	return true;
}

//==============================================================================
// Function     : ScannerBarcodeCommandToValue
// Purpose      : Convert barcodes to numeric values
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
//              : 
// Return       : 
// Remarks      : The identifier will lose more than 32 digits
//==============================================================================
bool ScannerBarcodeCommandToValue(PTCHAR lpszBarcode, PESL_BARCODE_VALUE pstBarcodeValue)
{
	TCHAR m_szBarcode[MAX_BARCODE_COMMAND+1];
	PTCHAR lpszSpecies = &m_szBarcode[3];
	PTCHAR lpszIdentifier = &m_szBarcode[8];	
	BYTE bySpecies[MAX_DEVICE_SPECIES_ID];
	BYTE byIdentifier[MAX_DEVICE_EFID];
	
	if ((lpszBarcode == NULL) || (pstBarcodeValue == NULL))
		return false;
	
	_tcscpy(m_szBarcode, lpszBarcode);
	m_szBarcode[7] = 0;
	//DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("lpszSpecies: %s\r\n"), lpszSpecies);
	//DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("lpszIdentifier: %s\r\n"), lpszIdentifier);
	
	pstBarcodeValue->byCategory[0] = lpszBarcode[0];
	pstBarcodeValue->byCategory[1] = lpszBarcode[1];
	pstBarcodeValue->byCategory[2] = lpszBarcode[2];
	
	if (GetHexValue(lpszSpecies, bySpecies, MAX_DEVICE_SPECIES_ID, true))
	{
		pstBarcodeValue->uiSpecies = (bySpecies[0] << 8) + bySpecies[1];
	}	
	
	if (GetHexValue(lpszIdentifier, byIdentifier, MAX_DEVICE_EFID, true))
	{
		// It will lose more than 32 digits
#if 1
		pstBarcodeValue->uiIdentifier = (((((((byIdentifier[0] << 8) + byIdentifier[1]) << 8) +byIdentifier[2]) << 8) + byIdentifier[3]) << 8) +byIdentifier[4];
#else
		int nLow = 0;
		int nHight = 0;
		pstBarcodeValue->uiIdentifier = (((((((byIdentifier[0] << 8) + byIdentifier[1]) << 8) +byIdentifier[2]) << 8) + byIdentifier[3]) << 8) +byIdentifier[4];
		nLow = pstBarcodeValue->uiIdentifier;
		nHight = (pstBarcodeValue->uiIdentifier >> 32);		
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("nHight: %d\r\n"), nHight);	
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("nLow: %d\r\n"), nLow);	
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("uiIdentifier: %d\r\n"), pstBarcodeValue->uiIdentifier);	
#endif
	}		
	return true;
}

//==============================================================================
// Function     : ScannerIsCommandMatch
// Purpose      : Compare whether the barcode matches
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
//              : 
// Return       : 
// Remarks      : The identifier will lose more than 32 digits
//==============================================================================
bool ScannerIsCommandMatch(BYTE * pbyDestArray, int nArrayLength, TCHAR * pszSourceText)
{
	int nLength = strlen(pszSourceText);
	int nIndex;
	if (nLength > nArrayLength)
		return false;
	for (nIndex = 0; nIndex < nLength; nIndex++)
	{
		if (pbyDestArray[nIndex] != pszSourceText[nIndex])
			return false;
	}
	return true;	
}

//==============================================================================
// Function     : ScannerInputData
// Purpose      : Processing scanner data
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void ScannerInputData(TCHAR szInput)
{
	ESL_DEVICE_ID stDeviceID;
	*m_pszBarcodePos = szInput;
	if ((*m_pszBarcodePos == 0x00) || (*m_pszBarcodePos == 0x0D))
	{	
		int nBarcodeLen = 0;
		bool fDeviceID = false;
		TCHAR * pszPos = NULL;
		TCHAR * pszFormatPos = NULL;
		TCHAR * pszDeviceIdString = (TCHAR *)m_stBarcodePair.byDevice;	//Device Barcode (Ex.XXXX-1234567890/1234567890)
		*m_pszBarcodePos = 0x00;

		m_pszBarcodePos = m_szBarcodeScan;	// Set as the starting point
		nBarcodeLen = strlen(m_szBarcodeScan);
		
		//
		// Check if it is the device identification.
		//
		if (nBarcodeLen == SIZE_DEVICE_ID)
		{
			//DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("DeviceID: %s\r\n"), m_szBarcodeScan);	
			if ((m_szBarcodeScan[0] == _T('F')) && (m_szBarcodeScan[1] == _T('F')) && 
				(m_szBarcodeScan[1] == _T('F')) && (m_szBarcodeScan[3] == _T('F')))
			{
				// Device ID: Gateway/Repeater/Scanner				
				fDeviceID = true;
			}
			else
			{
				unsigned char byDeviceID[MAX_DEVICE_IDENTIFIER];
				if (GetDeviceHexID(m_szBarcodeScan, byDeviceID, true))
				{
					if (byDeviceID[0] == (byDeviceID[1]+byDeviceID[2]+byDeviceID[3]+byDeviceID[4])%255)
						fDeviceID = true;				
					else
						fDeviceID = false;
				}		
			}
		}
		
		// Reset LED
		LedSetStatus(OSLS_SCAN_BARCODE_STATE);

		//
		// Step 1: Scan the product barcode
		// Step 2: Scan the eTag or command barcode
		// Step 3: Pairing and sending data
		//
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Scan: %s\r\n"), m_szBarcodeScan);	
		
		#if 0
		if (((m_szBarcodeScan[0] == _T('D')) && (m_szBarcodeScan[1] == _T('M')) && (m_szBarcodeScan[2] == _T('C')) &&
			((nBarcodeLen == SIZE_DAVICOM_COMMAND_KEY) || (nBarcodeLen == SIZE_DAVICOM_TAG_KEY))) || 
			((m_szBarcodeScan[0] == _T('D')) && (m_szBarcodeScan[1] == _T('M')) && (m_szBarcodeScan[2] == _T('C')) && (m_szBarcodeScan[7] == _T('-')) && 
			((nBarcodeLen == SIZE_DAVICOM_COMMAND_KEY+1) || (nBarcodeLen == SIZE_DAVICOM_TAG_KEY+1))))
		{
		}
		#endif
		
		// Detect current scanner status
		// if (m_stBarcodePair.fLock == true)
		if ((m_szBarcodeScan[0] != _T('D')) && (m_szBarcodeScan[1] != _T('M')) && (m_szBarcodeScan[2] != _T('C')) &&
			(m_stBarcodePair.fLock == true))
		{
			WARNING_MESSAGE(FLAG_MF_SCANNER, _T("The system is locked and the scan is invalid.\r\n"));	
			m_szBarcodeScan[0] = 0x00;
			return;
		}

		
		if (m_szBarcodeScan[0] == 0x00)
		{
			m_uiScannerLastState = SCAN_STAGE_STANDBY;
		}

		//
		// Tag 
		// DMT2900-8180000001, DMT4200-8580000005,
		//		
		// Command
		// DMC0000-0123456789
		//
		// strpbrk
		if (((m_szBarcodeScan[0] == _T('D')) && (m_szBarcodeScan[1] == _T('M')) && 
			((nBarcodeLen == SIZE_DAVICOM_COMMAND_KEY) || (nBarcodeLen == SIZE_DAVICOM_TAG_KEY))) || 
			((m_szBarcodeScan[0] == _T('D')) && (m_szBarcodeScan[1] == _T('M')) && (m_szBarcodeScan[7] == _T('-')) && 
			((nBarcodeLen == SIZE_DAVICOM_COMMAND_KEY+1) || (nBarcodeLen == SIZE_DAVICOM_TAG_KEY+1))))
		{
			//pszPos = strstr(m_szBarcodeScan, _T("-"));
			pszPos = _tcsstr(m_szBarcodeScan, _T("-"));		
			if (pszPos == NULL)
			{
				pszPos = m_szBarcodeScan;
				strcpy (m_szBarcodeFormat, m_szBarcodeScan);
				m_szBarcodeFormat[7] = _T('-');
				
				pszPos = m_szBarcodeScan;
				pszPos += (SIZE_COMPANY_ID + SIZE_FORMAT_ID + SIZE_MODEL_ID);
				pszFormatPos = m_szBarcodeFormat;
				pszFormatPos += (SIZE_COMPANY_ID + SIZE_FORMAT_ID + SIZE_MODEL_ID + 1);	// DMT2900-/DMC0001-/etc
				
				strcpy (pszFormatPos, pszPos);											// DMT2900-0123456789
				
				strcpy (m_szBarcodeScan, m_szBarcodeFormat);					
				pszPos = m_szBarcodeScan;
				pszPos += (SIZE_COMPANY_ID + SIZE_FORMAT_ID);
			}
			else
			{
				pszPos = m_szBarcodeScan;
				pszPos += (SIZE_COMPANY_ID + SIZE_FORMAT_ID);	// DMT/DMC/etc
			}
			
			//
			// Scanner Reserved Command
			//
			if (m_szBarcodeScan[2] == _T('C')) //DMCXXXX-
			{
				// Command		
				ESL_BARCODE_VALUE stBarcodeValue;

				ScannerStringToArray(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), pszPos);
				DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Command: %s\r\n"), m_stBarcodePair.byCommand);		
				
				LedSetStatus(OSLS_SUCCEEDED_STATE);

				ScannerBarcodeCommandToValue(m_szBarcodeScan, &stBarcodeValue);	
				//DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Execute the %s instruction.\r\n"), m_stBarcodePair.szCommand);
			
				if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_FORCE_STANDBY_MODE))
				{
					DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Perform forced back to standby mode instructions. (%s)\r\n"), m_stBarcodePair.byCommand);
					m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
					m_stBarcodePair.uiCount = 0;
					m_stBarcodePair.fLock = false;			

					// Show NVM Data
					NonVolatileMemoryShowInforamtion();					

					// Keep the last state
					m_uiScannerLastState = SCAN_STAGE_STANDBY;		
					return;
				}
#if 1
				else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_SWITCH_DEFAULT_MODE))
				{
//					ESL_DEVICE_ID stDeviceID;
					DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Execute switch to default mode instruction. (%s)\r\n"), m_stBarcodePair.byCommand);
					m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
					m_stBarcodePair.uiCount = 0;
					m_stBarcodePair.fLock = false;		
					RfSetBeaconMode(OSBM_BEACON_SYNC_MODE);
					memset(&stDeviceID, 0, sizeof(ESL_DEVICE_ID));
					SetBindingID(stDeviceID);

					// Keep the last state
					m_uiScannerLastState = SCAN_STAGE_STANDBY;
					return;
				}	
#endif				
				else if (m_stBarcodePair.fLock == true)
				{			
					WARNING_MESSAGE(FLAG_MF_SCANNER, _T("The system is locked and the scan is invalid.\r\n"));	
					m_szBarcodeScan[0] = 0x00;
					return;
				}	
				
				//
				// Parse Support Command
				//
				ScannerParseCommand(stBarcodeValue);
		
			}
			// DMTXXXX-
			else if ((m_szBarcodeScan[2] == _T('T')) && (
//				(m_uiScannerLastState == SCAN_STAGE_PRODUCT) || 
#ifdef FLAG_MODE_MULTIPLE_PAIRING				
				(m_uiScannerLastState == SCAN_STAGE_TAG)|| 
#endif
				(m_uiScannerLastState == SCAN_STAGE_COMMAND) || 
				(m_uiScannerLastState == SCAN_STAGE_BINDING_HOST) || 
				(m_uiScannerLastState == SCAN_STAGE_UNBINDING_TAG) || 
				(m_uiScannerLastState == SCAN_STAGE_CHANGE_DEVICE_ID) || 
				(m_uiScannerLastState == SCAN_STAGE_DEVICE_REGISTRATION)))
			{
				// Tag		
				ScannerStringToArray(m_stBarcodePair.byDevice, sizeof(m_stBarcodePair.byDevice), pszPos);
				GetHexID(pszDeviceIdString, &stDeviceID, true);
				ScannerParseTagCommand(stDeviceID);							
	
				// Keep the last state
//				m_uiScannerLastState = SCAN_STAGE_TAG;					
			}	
			else
			{
				if (g_stNvmMappingData.wScannerMode == OSSM_BARCODE_DEFAULT_MODE)	// Auto
				{
					// Tag		
					ScannerStringToArray(m_stBarcodePair.byDevice, sizeof(m_stBarcodePair.byDevice), pszPos);
//					GetHexID(pszDeviceIdString, &stDeviceID, true);
					
					DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("AUTO_Binding_TAG: %s\r\n"), m_stBarcodePair.byDevice);									

					// Keep the last state Auto Mode		
					m_uiScannerLastState = SCAN_STAGE_PRODUCT;			
				}
				else if (g_stNvmMappingData.wScannerMode == OSSM_BARCODE_PAIRING_MODE)
				{
					// Tag								
					ScannerStringToArray(m_stBarcodePair.byDevice, sizeof(m_stBarcodePair.byDevice), pszPos);
//					GetHexID(pszDeviceIdString, &stDeviceID, true);
					
					DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("PAIRING_Binding_TAG: %s\r\n"), m_stBarcodePair.byDevice);				
					m_uiScannerLastState = SCAN_STAGE_PRODUCT;								
				}							
				else
				{
					DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Bad Request: %s\r\n"), m_szBarcodeScan);
					
					// Pairing has not been completed
					m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;				
					LedSetStatus(OSLS_PREDICTED_STATE);		// Set LED to OSLS_PREDICTED_STATE
					
					// Keep the last state
					m_uiScannerLastState = SCAN_STAGE_STANDBY;				
				}						
			}			
		}	
		else if (fDeviceID == true)// ESL V1.1: Remove the leading word of "DMTXXXX".
		{
			pszPos = pszDeviceIdString = m_pszBarcodePos;							
			
			if (
//							(m_uiScannerLastState == SCAN_STAGE_PRODUCT) || 
			#ifdef FLAG_MODE_MULTIPLE_PAIRING				
							(m_uiScannerLastState == SCAN_STAGE_TAG)|| 
			#endif
							(m_uiScannerLastState == SCAN_STAGE_COMMAND) || 
							(m_uiScannerLastState == SCAN_STAGE_BINDING_HOST) || 
							(m_uiScannerLastState == SCAN_STAGE_UNBINDING_TAG) || 
							(m_uiScannerLastState == SCAN_STAGE_CHANGE_DEVICE_ID) || 
							(m_uiScannerLastState == SCAN_STAGE_DEVICE_REGISTRATION))
			{
				// Tag
				ScannerStringToArray(m_stBarcodePair.byDevice, sizeof(m_stBarcodePair.byDevice), pszPos);
				GetHexID(pszDeviceIdString, &stDeviceID, true);
				ScannerParseTagCommand(stDeviceID);		
			}
			else
			{
				
				if (g_stNvmMappingData.wScannerMode == OSSM_BARCODE_DEFAULT_MODE)	// Auto
				{
					ScannerStringToArray(m_stBarcodePair.byDevice, sizeof(m_stBarcodePair.byDevice), pszPos);
//					GetHexID(pszDeviceIdString, &stDeviceID, true);
					
					DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("AUTO_Binding_TAG: %s\r\n"), m_stBarcodePair.byDevice);
					// Keep the last state Auto Mode
//					m_uiScannerLastState = SCAN_STAGE_BINDING_TAG;		
					m_uiScannerLastState = SCAN_STAGE_PRODUCT;								
				}
				else if (g_stNvmMappingData.wScannerMode == OSSM_BARCODE_PAIRING_MODE)
				{
//					pszPos = pszDeviceIdString = m_pszBarcodePos;		
					ScannerStringToArray(m_stBarcodePair.byDevice, sizeof(m_stBarcodePair.byDevice), pszPos);
//					GetHexID(pszDeviceIdString, &stDeviceID, true);
					
					DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("PAIRING_Binding_TAG: %s\r\n"), m_stBarcodePair.byDevice);		
					m_uiScannerLastState = SCAN_STAGE_PRODUCT;					
				}			
				else
				{
					DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Bad Request: %s\r\n"), pszPos);
					LedSetStatus(OSLS_PREDICTED_STATE);		// Set LED to OSLS_PREDICTED_STATE				
					
					// Keep the last state
					m_uiScannerLastState = SCAN_STAGE_STANDBY;						
				}
	
			}
		}
		else
		{
			pszPos = pszDeviceIdString = m_pszBarcodePos;									
			ScannerStringToArray(m_stBarcodePair.byProduct, sizeof(m_stBarcodePair.byProduct), m_szBarcodeScan);
			DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Product: %s\r\n"), m_stBarcodePair.byProduct);	
			
			if (g_stNvmMappingData.wScannerMode == OSSM_BARCODE_DEFAULT_MODE)	// Auto
			{
				if (m_uiScannerLastState == SCAN_STAGE_PRODUCT)
				{
					DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("%s\r\n"), " Product_Binding_TAG");			
					LedReSetStatus(OSLS_IDENTIFIED_STATE);		// ReSet LED to OSLS_IDENTIFIED_STATE			
//					LedSetStatus(OSLS_IDENTIFIED_STATE);		// Set LED to OSLS_IDENTIFIED_STATE											
					ScannerParseTagCommand(stDeviceID);										
				}
				else
				{					
					// Send barcode information to gateway
					m_stBarcodePair.uiCommand = RF_CMD_SCANNER_BARCODE;	
					m_stBarcodePair.uiCount = 0;
					m_stBarcodePair.fLock = true;				
					m_stBarcodePair.fSendBarcode = true;
					
					LedReSetStatus(OSLS_IDENTIFIED_STATE);		// ReSet LED to OSLS_IDENTIFIED_STATE			
//					LedSetStatus(OSLS_IDENTIFIED_STATE);		// Set LED to OSLS_IDENTIFIED_STATE								
					// Keep the last state	
					m_uiScannerLastState = SCAN_STAGE_STANDBY;											
				}
	
			}			
			else if (g_stNvmMappingData.wScannerMode == OSSM_BARCODE_PAIRING_MODE)
			{
				if (m_uiScannerLastState == SCAN_STAGE_PRODUCT)
				{				
					LedReSetStatus(OSLS_IDENTIFIED_STATE);		// ReSet LED to OSLS_IDENTIFIED_STATE
//					LedSetStatus(OSLS_IDENTIFIED_STATE);		// Set LED to OSLS_IDENTIFIED_STATE			
					
					DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("%s\r\n"), " PAIRING_Binding_TAG");			
					ScannerParseTagCommand(stDeviceID);				
				}
				else
				{			
					DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Bad Request: %s\r\n"), pszPos);
					LedSetStatus(OSLS_PREDICTED_STATE);		// Set LED to OSLS_PREDICTED_STATE
					// Keep the last state
					m_uiScannerLastState = SCAN_STAGE_STANDBY;			
				}					
			}
			else if (g_stNvmMappingData.wScannerMode == OSSM_BARCODE_READER_MODE)
			{
				// Send barcode information to gateway
				m_stBarcodePair.uiCommand = RF_CMD_SCANNER_BARCODE;	
				m_stBarcodePair.uiCount = 0;
				m_stBarcodePair.fLock = true;				
				m_stBarcodePair.fSendBarcode = true;
			
					LedReSetStatus(OSLS_IDENTIFIED_STATE);		// ReSet LED to OSLS_IDENTIFIED_STATE
//					LedSetStatus(OSLS_IDENTIFIED_STATE);		// Set LED to OSLS_IDENTIFIED_STATE			
				// Keep the last state	
				m_uiScannerLastState = SCAN_STAGE_STANDBY;				
			}			
			
		}
		m_szBarcodeScan[0] = 0x00;		
	}
	else
	{
		m_pszBarcodePos++;			
	}
		
	if (m_pszBarcodePos > (m_szBarcodeScan + sizeof(m_szBarcodeScan)))
	{
		m_pszBarcodePos = m_szBarcodeScan;	// Set as the starting point		
		m_szBarcodeScan[0] = 0x00;
		ERROR_MESSAGE(FLAG_MF_SCANNER, _T("This is an error that occurs when a barcode is too large to be processed by available receive memory.\r\n"));
	}			
}

//==============================================================================
// Function     : ScannerParseTagCommand
// Purpose      : Processing scanner command
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void ScannerParseTagCommand(ESL_DEVICE_ID stDeviceID)
{
	if (m_uiScannerLastState == SCAN_STAGE_PRODUCT)
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("%s and %s paired successfully.\r\n"), m_stBarcodePair.byProduct, m_stBarcodePair.byDevice);	
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_PAIRING;
		m_stBarcodePair.uiCount = 0;
		m_stBarcodePair.fLock = true;
	}
	else if (m_uiScannerLastState == SCAN_STAGE_COMMAND)
	{
		
		//{
			DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Execute the %s instruction.\r\n"), m_stBarcodePair.byCommand);
			m_stBarcodePair.uiCommand = RF_CMD_SCANNER_COMMAND;
			m_stBarcodePair.uiCount = 0;
			m_stBarcodePair.fLock = true;						
		//}
		// Set LED status as successful
		LedSetStatus(OSLS_SUCCEEDED_STATE);
	}
	//
	else if (m_uiScannerLastState == SCAN_STAGE_BINDING_HOST)
	{
		//ESL_DEVICE_ID stDeviceID;
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Execute the specified host instruction. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		m_stBarcodePair.uiCount = 0;
		m_stBarcodePair.fLock = false;					
		//GetHexID((const PTCHAR) m_stBarcodePair.byDevice, &stDeviceID, true);
		//GetHexID(pszDeviceIdString, &stDeviceID, true);
		SetBindingID(stDeviceID);
		//RfSetBindingMode(true);					
		RfSetBeaconMode(OSBM_BEACON_BINDING_MODE);
		
		// Set LED status as successful
		LedSetStatus(OSLS_SUCCEEDED_STATE);					
	}
	else if (m_uiScannerLastState == SCAN_STAGE_UNBINDING_TAG)
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("%s and %s unpaired successfully.\r\n"), m_stBarcodePair.byProduct, m_stBarcodePair.byDevice);			
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Execute unpaired instruction. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_UNPAIRING;
		m_stBarcodePair.uiCount = 0;
		m_stBarcodePair.fLock = true;
		
		// Set LED status as successful
		LedSetStatus(OSLS_SUCCEEDED_STATE);	
	}
	else if (m_uiScannerLastState == SCAN_STAGE_CHANGE_DEVICE_ID)
	{
		//ESL_DEVICE_ID stDeviceID;
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Execute the replacement device ID instruction. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		m_stBarcodePair.uiCount = 0;
		m_stBarcodePair.fLock = false;
		
		//DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Device ID string. (%s)\r\n"), pszDeviceIdString);
		
		//GetHexID((const PTCHAR) m_stBarcodePair.byDevice, &stDeviceID, true);
		//GetHexID(pszDeviceIdString, &stDeviceID, true);
		if (ScannerIsValidIdentifier(stDeviceID))
		{
			ScannerSetDeviceID(stDeviceID);	
			INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Device ID: %02X:%02X:%02X:%02X:%02X\r\n"), g_stNvmMappingData.stDeviceID.byEFID[0],g_stNvmMappingData.stDeviceID.byEFID[1], g_stNvmMappingData.stDeviceID.byEFID[2], g_stNvmMappingData.stDeviceID.byEFID[3], g_stNvmMappingData.stDeviceID.byEFID[4]);
		}
		else
		{
			INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Device ID is invalid. (%02X:%02X:%02X:%02X:%02X)\r\n"), stDeviceID.byEFID[0], stDeviceID.byEFID[1], stDeviceID.byEFID[2], stDeviceID.byEFID[3], stDeviceID.byEFID[4]);
		}
		// Set LED status as successful
		LedSetStatus(OSLS_SUCCEEDED_STATE);					
		
	}
	else if (m_uiScannerLastState == SCAN_STAGE_DEVICE_REGISTRATION)				
	{
		//ESL_DEVICE_ID stDeviceID;
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Execute the device registration instruction. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_REGISTRATION;
		m_stBarcodePair.uiCount = 0;
		m_stBarcodePair.fLock = true;
		
		//GetHexID((const PTCHAR) m_stBarcodePair.byDevice, &stDeviceID, true);
		//GetHexID(pszDeviceIdString, &stDeviceID, true);
		//if (ScannerIsValidIdentifier(stDeviceID))
		//{
		//	ScannerSetDeviceID(stDeviceID);	
		//	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Device ID: %02X:%02X:%02X:%02X:%02X\r\n"), g_stNvmMappingData.stDeviceID.byEFID[0],g_stNvmMappingData.stDeviceID.byEFID[1], g_stNvmMappingData.stDeviceID.byEFID[2], g_stNvmMappingData.stDeviceID.byEFID[3], g_stNvmMappingData.stDeviceID.byEFID[4]);
		//}
		//else
		//{
		//	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Device ID is invalid. (%02X:%02X:%02X:%02X:%02X)\r\n"), stDeviceID.byEFID[0], stDeviceID.byEFID[1], stDeviceID.byEFID[2], stDeviceID.byEFID[3], stDeviceID.byEFID[4]);
		//}	
		INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Device ID: %02X:%02X:%02X:%02X:%02X\r\n"), stDeviceID.byEFID[0], stDeviceID.byEFID[1], stDeviceID.byEFID[2], stDeviceID.byEFID[3], g_stNvmMappingData.stDeviceID.byEFID[4]);
		
		// Set LED status as successful
		LedSetStatus(OSLS_SUCCEEDED_STATE);
	}	
}


//==============================================================================
// Function     : ScannerParseCommand
// Purpose      : Processing scanner command
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void ScannerParseCommand(ESL_BARCODE_VALUE stBarcodeValue)
{
	// Command		
	//ESL_BARCODE_VALUE stBarcodeValue;	
	
	// Default Mode
	if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_SWITCH_DEFAULT_MODE))
	{
		ESL_DEVICE_ID stDeviceID;
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Execute switch to default mode instruction. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		m_stBarcodePair.uiCount = 0;
		m_stBarcodePair.fLock = false;		
		RfSetBeaconMode(OSBM_BEACON_SYNC_MODE);
		memset(&stDeviceID, 0, sizeof(ESL_DEVICE_ID));
		SetBindingID(stDeviceID);

		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_STANDBY;					
	}	
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_AUTO_SCAN_CHANNEL))
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Perform automatic detection of channel instructions. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		if (RfIsScanningChannel())
		{
			// Stop automatic detection
			//DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Stop automatic detection.\r\n"));
			//m_stBarcodePair.fLock = false;	
			RfSetScanChannel(false);
		}
		else
		{
			// Start automatic detection
			//DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Start automatic detection.\r\n"));
			//m_stBarcodePair.fLock = true;	
			RfSetScanChannel(true);
		}
		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_CHANNEL;
	}
	// Binding Host
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_FORCE_UPPER_HOST))
	{				
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Perform the specified host instruction. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;

		// Set the LED to wait for confirmation
		LedSetStatus(OSLS_SCAN_BARCODE_STATE);					
	
		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_BINDING_HOST;				
	}	
	// Unbinding Tag
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_UNPAIRING))
	{			
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Perform unpaired instruction. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;

		// Set the LED to wait for confirmation
		LedSetStatus(OSLS_SCAN_BARCODE_STATE);					
	
		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_UNBINDING_TAG;				
	}	

	// Change Device ID
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_CHANGE_DEVICE_ID))
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Perform a change of device identifier. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;

		// Set the LED to wait for confirmation
		LedSetStatus(OSLS_SCAN_BARCODE_STATE);					
	
		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_CHANGE_DEVICE_ID;				
	}				
	// Write Configuration to NVM
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_WRITE_CONFIGURATION))
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Perform write configuration to NVM instructions. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;

		// Set the LED to wait for confirmation
		LedSetStatus(OSLS_SCAN_BARCODE_STATE);
		
		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_WRITE_CONFIGURATION;
	}	
	// Enable Encryption Mode
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_ENABLE_ENCRYPTION))
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Turn on encryption mode. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		g_stNvmMappingData.fEncryptionMode = true;
	
		// Keep the last state
		//m_uiScannerLastState = SCAN_STAGE_STANDBY;	
		m_uiScannerLastState = SCAN_STAGE_STANDBY;
	}	
	// Disable Encryption Mode
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_DISABLE_ENCRYPTION))
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Turn off encryption mode. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		
		g_stNvmMappingData.fEncryptionMode = false;
	
		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_STANDBY;
	}	

	// Device Registration
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_DEVICE_REGISTRATION))
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Perform device registration. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		
		// Set the LED to wait for confirmation
		LedSetStatus(OSLS_SCAN_BARCODE_STATE);

		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_DEVICE_REGISTRATION;
	}	
	
	// Factory default
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_FACTORY_DEFAULT))
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Perform factory default. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		
		// Set the LED to wait for confirmation
		LedSetStatus(OSLS_SCAN_BARCODE_STATE);					
	
		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_FACTORY_DEFAULT;
	}					
	
	// Barcode Default Mode
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_BARCODE_DEFAULT_MODE))
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Make sure to switch to barcode default mode. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		
		// Set the LED to wait for confirmation
		LedSetStatus(OSLS_SCAN_BARCODE_STATE);					
	
		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_BARCODE_DEFAULT_MODE;
	}					
	
	// Barcode Pairing Mode
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_BARCODE_PAIRING_MODE))
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Make sure to switch to barcode pairing mode. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		
		// Set the LED to wait for confirmation
		LedSetStatus(OSLS_SCAN_BARCODE_STATE);					
	
		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_BARCODE_PAIRING_MODE;
	}			

	// Barcode Reader Mode
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_BARCODE_READER_MODE))
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Make sure to switch to barcode reader mode. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		
		// Set the LED to wait for confirmation
		LedSetStatus(OSLS_SCAN_BARCODE_STATE);					
	
		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_BARCODE_READER_MODE;
	}
	
	
	// Cancel (Used by Scanner) 
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_COMMAND_CANCEL))
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Cancel the execution of the command. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		
		// Set LED status as successful
		LedSetStatus(OSLS_SUCCEEDED_STATE);

		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_STANDBY;
	}	
	// Confirm (Used by Scanner) 
	else if (ScannerIsCommandMatch(m_stBarcodePair.byCommand, sizeof(m_stBarcodePair.byCommand), RF_FC_COMMAND_CONFIRM))
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Confirm the execution of the command. (%s)\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		
		if (m_uiScannerLastState == SCAN_STAGE_WRITE_CONFIGURATION)
		{
			// Write configuration
			m_stBarcodePair.fLock = true;
			NonVolatileMemoryWriteConfiguration();
			m_stBarcodePair.fLock = false;	
		}
		if (m_uiScannerLastState == SCAN_STAGE_FACTORY_DEFAULT)					
		{
			// Reset configuration to default
			m_stBarcodePair.fLock = true;
			NonVolatileMemoryFactoryDefault();
			m_stBarcodePair.fLock = false;
		}
		else if (m_uiScannerLastState == SCAN_STAGE_BARCODE_READER_MODE)
		{
			// Switch to barcode reader mode
			m_stBarcodePair.fLock = true;
			g_stNvmMappingData.wScannerMode = OSSM_BARCODE_READER_MODE;
			DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Switch to barcode reader mode.\r\n"));	
			m_stBarcodePair.fLock = false;						
		}
		else if (m_uiScannerLastState == SCAN_STAGE_BARCODE_PAIRING_MODE)
		{
			// Switch to barcode paring mode
			m_stBarcodePair.fLock = true;
			g_stNvmMappingData.wScannerMode = OSSM_BARCODE_PAIRING_MODE;
			DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Switch to barcode pairing mode.\r\n"));
			m_stBarcodePair.fLock = false;						
		}
		else if (m_uiScannerLastState == SCAN_STAGE_BARCODE_DEFAULT_MODE)
		{
			// Switch to barcode default mode
			m_stBarcodePair.fLock = true;
			g_stNvmMappingData.wScannerMode = OSSM_BARCODE_DEFAULT_MODE;
			DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Switch to barcode default mode.\r\n"));
			m_stBarcodePair.fLock = false;						
		}
		
		// Set LED status as successful
		LedSetStatus(OSLS_SUCCEEDED_STATE);

		// Keep the last state	
		m_uiScannerLastState = SCAN_STAGE_STANDBY;
	}		
	// Change Channe to XXX
	else if (stBarcodeValue.uiSpecies == 0x0001)
	{
		int nChannel = 0;
		nChannel = stBarcodeValue.uiIdentifier;
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Forced to switch channels to channel %d. (%s)\r\n"), nChannel, m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		if (RfIsScanningChannel())
		{
			// Stop automatic detection
			RfSetScanChannel(false);
		}		
		RfSetChannel(nChannel);
		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_STANDBY;					
	}
	// Force data rate
	else if (stBarcodeValue.uiSpecies == 0x0002)
	{
		int nDataRateIndex = RF_DATARATE_500K;
		uint32_t nDataRateValue = 500*1024;

		nDataRateValue = stBarcodeValue.uiIdentifier;
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Force data rate to switch to %dK. (%s)\r\n"), nDataRateValue/1024, m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
		switch(nDataRateValue)
		{
			case (250*1024):
				nDataRateIndex = RF_DATARATE_250K;							
				break;					
			case (500*1024):
				nDataRateIndex = RF_DATARATE_500K;
				break;							
			case (1024*1024):
				nDataRateIndex = RF_DATARATE_1M;
				break;	
			default:
				nDataRateIndex = RF_DATARATE_500K;							
		}
		
		RfSetDataRate(nDataRateIndex);
		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_STANDBY;							
	}	
	// Command 0003-XXXXXXXXXX
	else if (stBarcodeValue.uiSpecies >= 0x0003)
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Execute the %s instruction.\r\n"), m_stBarcodePair.byCommand);
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_COMMAND;
		m_stBarcodePair.uiCount = 0;
		m_stBarcodePair.fLock = true;						
	}				
	else
	{
		// Pairing has not been completed
		m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;
	
		// Keep the last state
		m_uiScannerLastState = SCAN_STAGE_COMMAND;
	}	
}

// Jump to standby mode
void ScannerResetToStandbyMode(void)
{
	DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Reset to standby mode.\r\n"));
	m_uiScannerLastState = SCAN_STAGE_STANDBY;
	m_stBarcodePair.uiCommand = RF_CMD_SCANNER_STANDBY;	
	m_stBarcodePair.uiCount = 0;
	m_stBarcodePair.uiSendingTime = 0;
	m_stBarcodePair.fLock = false;
	m_stBarcodePair.fSendBarcode = false;	
}

uint32_t ScannerSendPairing()
{
	uint32_t uiResult = ERROR_NOT_READY;
	if (m_stBarcodePair.uiCommand == RF_CMD_SCANNER_STANDBY)
		return uiResult;
	
	
	
#ifndef FLAG_DISABLE_TX_TIMEOUT
	if (m_stBarcodePair.uiCount >= MAX_SEND_PAIRING_RETRY_TIME)
	{
		ScannerResetToStandbyMode();
		return ERROR_WAIT_TIMEOUT;
	}
#endif
	
	if (RfSendBarcodePairing(&m_stBarcodePair))
	{
		if (m_stBarcodePair.uiSendingTime == 0)
			m_stBarcodePair.uiSendingTime = GetTickCount();
		m_stBarcodePair.uiCount++;	
		SetMachineState(OSMS_WAITTING_RESULT_STATE);
		//DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Switch to wait mode.\r\n"));
		uiResult = ERROR_SUCCESS;		
	}

	return uiResult;
}

uint32_t ScannerSendRegistration()
{
	uint32_t uiResult = ERROR_NOT_READY;
	if (m_stBarcodePair.uiCommand == RF_CMD_SCANNER_STANDBY)
		return uiResult;
	
#ifndef FLAG_DISABLE_TX_TIMEOUT
	if (m_stBarcodePair.uiCount >= MAX_SEND_PAIRING_RETRY_TIME)
	{
		ScannerResetToStandbyMode();
		return ERROR_WAIT_TIMEOUT;
	}
#endif
	
	if (RfSendBarcodeRegistration(&m_stBarcodePair))
	{
		if (m_stBarcodePair.uiSendingTime == 0)
			m_stBarcodePair.uiSendingTime = GetTickCount();
		m_stBarcodePair.uiCount++;	
		SetMachineState(OSMS_WAITTING_RESULT_STATE);
		//DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Switch to wait mode.\r\n"));
		uiResult = ERROR_SUCCESS;		
	}

	return uiResult;
}

uint32_t ScannerSendCommand()
{
	uint32_t uiResult = ERROR_NOT_READY;
	if (m_stBarcodePair.uiCommand == RF_CMD_SCANNER_STANDBY)
		return uiResult;
	
#ifndef FLAG_DISABLE_TX_TIMEOUT
	if (m_stBarcodePair.uiCount >= MAX_SEND_PAIRING_RETRY_TIME)
	{
		ScannerResetToStandbyMode();
		return ERROR_WAIT_TIMEOUT;
	}
#endif
	
	if (RfSendBarcodeCommand(&m_stBarcodePair))
	{
		if (m_stBarcodePair.uiSendingTime == 0)
			m_stBarcodePair.uiSendingTime = GetTickCount();
		m_stBarcodePair.uiCount++;	
		SetMachineState(OSMS_WAITTING_RESULT_STATE);
		//DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Switch to wait mode.\r\n"));
		uiResult = ERROR_SUCCESS;		
	}

	return uiResult;
}

uint32_t ScannerSendBarcode()
{
	uint32_t uiResult = ERROR_NOT_READY;
	if (m_stBarcodePair.uiCommand == RF_CMD_SCANNER_STANDBY)
		return uiResult;
	
#ifndef FLAG_DISABLE_TX_TIMEOUT
	if (m_stBarcodePair.uiCount >= MAX_SEND_PAIRING_RETRY_TIME)
	{
		ScannerResetToStandbyMode();
		return ERROR_WAIT_TIMEOUT;
	}
#endif
	
	if (RfSendBarcodeInformation(&m_stBarcodePair))
	{
		if (m_stBarcodePair.uiSendingTime == 0)
			m_stBarcodePair.uiSendingTime = GetTickCount();
		m_stBarcodePair.uiCount++;	
		// Original:
		// SetMachineState(OSMS_WAITTING_RESULT_STATE);
		//
		
		// Modified:
//		if (g_stNvmMappingData.wScannerMode == OSSM_BARCODE_READER_MODE)    //2019-08-13 del.
			SetMachineState(OSMS_WAITTING_RESULT_STATE);
		
//		m_stBarcodePair.fSendBarcode = false;				//2019-06-21  del. Resend Barcode mode.
		
		uiResult = ERROR_SUCCESS;		
	}

	return uiResult;
}

// Is it a valid identifier?
bool ScannerIsValidIdentifier(ESL_DEVICE_ID stDeviceID)
{
	uint8_t byDeviceID[MAX_DEVICE_IDENTIFIER] = EUSI_DEFAULT_ID;	
	//if (memcmp (stDeviceID.stComboID.byEUSI, byDeviceID, sizeof(stDeviceID.Split.byEUID)) == 0)
	if (stDeviceID.byEFID[EFID_DEVICE_TYPE_INDEX] == byDeviceID[EFID_DEVICE_TYPE_INDEX])
		return true;
	
	return false;
}

bool ScannerSetDeviceID(ESL_DEVICE_ID stDeviceID)
{
	memcpy(g_stNvmMappingData.stDeviceID.byEFID, stDeviceID.byEFID, MAX_DEVICE_IDENTIFIER);
	return true;
}
