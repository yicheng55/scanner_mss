/* Scanner.h: Davicom scanner device header for ESL system.                     */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: Scanner.h                                                     */
/* Purpose      : ESL scanner device header for ESL system                      */
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

//------------------------------------------------------------------------------
//--- Declare Barcode ---
//------------------------------------------------------------------------------
// Format: [COMPANY][TYPE][MODEL][TAG-ID]
//      COMPANY : 2 CharacterS, DM
//      FORMAT  : 1 Character, C: Command, T: Tag,                                                              
//      MODEL   : 4 CharacterS, 
//              :     SCREEN: 3 digit
//              :     COLOR: 1 digit
//      SERIAL  : 10 CharacterS, 8180000001
// 
// Ex.
// DMT-XXXX-8180000001 (XXXX-1234567890)
// DMC-XXXX-1234567890 (XXXX-1234567890) 
// 
// DMT-290c-xxxxxxxxxx (DMT290cxxxxxxxxxx)
//    where  ---
//        DMT --- as leading characters
//        290 --- the first 3 numerical digits for tag size.  290 for 2.9", 213 for 2.13",.... 420 for 4.2"
//        c   --- the color digit.   0 for B/W,  1 for B/W/R,  2 for B/W/Y,....
//
// For example,     
//    DMT2901  stands for   2.9"  BWR 3-color Tag   
//    DMT2130          ---> 2.13"  BW mono tag
//
#ifndef _DAVICOM_SCANNER_H_
#define _DAVICOM_SCANNER_H_

#include "ProjectConfig.h"
#include "EslDebug.h"
#include "AMICCOM_CM0.h"
#include "Board.h"
#include "stdio.h"
#include "string.h"
//#include "EPaperSSD1608.h"


#ifdef __cplusplus
 extern "C" {
#endif



//------------------------------------------------------------------------------
//--- Declarative and Imperative Definitions ---
//------------------------------------------------------------------------------

#define FLAG_MODE_MULTIPLE_PAIRING 		1		//Multiple pairing mode

//------------------------------------------------------------------------------
//--- Declarative Structure ---
//------------------------------------------------------------------------------
#pragma pack(1)


#pragma pack()




//------------------------------------------------------------------------------
//--- Declarative Scanner Stage ---
//------------------------------------------------------------------------------
#define SCAN_STAGE_STANDBY					0x00000000
#define SCAN_STAGE_TAG						0x00000001
#define SCAN_STAGE_PRODUCT					0x00000002
//--- COMMADN ---
#define SCAN_STAGE_COMMAND					0x00000003
#define SCAN_STAGE_BINDING_HOST				0x00000004
#define SCAN_STAGE_UNBINDING_TAG			0x00000005	
#define SCAN_STAGE_CHANNEL					0x00000006							// Channel detection
#define SCAN_STAGE_CHANGE_DEVICE_ID			0x00000007							// Change device identifier
#define SCAN_STAGE_WRITE_CONFIGURATION		0x00000008							// Write configuration to NVM
#define SCAN_STAGE_DEVICE_REGISTRATION		0x00000009							// Device Registration	
#define SCAN_STAGE_FACTORY_DEFAULT			0x0000000A							// Factory Default	
#define SCAN_STAGE_BARCODE_DEFAULT_MODE		0x0000000B							// Barcode Default Mode 	
#define SCAN_STAGE_BARCODE_PAIRING_MODE		0x0000000C							// Barcode Pairing Mode 
#define SCAN_STAGE_BARCODE_READER_MODE		0x0000000D							// Barcode Reader Mode 	

//==============================================================================
//--- Declarative Functions ---
//==============================================================================


//==============================================================================
//--- Declarative Functions ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative constructor ---
//------------------------------------------------------------------------------
void Scanner(void);
//------------------------------------------------------------------------------
//--- Declarative Public Function ---
//------------------------------------------------------------------------------
void ScannerInit(void);
void ScannerInputData(TCHAR szData);
bool ScannerIsPairing(void);
bool ScannerIsRegistration(void);
bool ScannerIsCommand(void);
bool ScannerIsBarcode(void);		// Only send barcode information
bool ScannerIsValidIdentifier(ESL_DEVICE_ID stDeviceID); // Is it a valid identifier


uint32_t ScannerSendPairing(void);
uint32_t ScannerSendRegistration(void);
uint32_t ScannerSendCommand(void);
uint32_t ScannerSendBarcode(void);
//uint32_t ScannerSendUnpairing(void);

//------------------------------------------------------------------------------
//--- Declarative Protected Function ---
//------------------------------------------------------------------------------
bool ScannerStringToArray(BYTE * pbyDestArray, int nArrayLength, TCHAR * pszSourceText);
void ScannerResetToStandbyMode(void);
bool ScannerBarcodeCommandToValue(PTCHAR lpszBarcode, PESL_BARCODE_VALUE pstBarcodeValue);
//bool ScannerIsArray(BYTE * pbyDestArray, int nArrayLength, TCHAR * pszSourceText);

bool ScannerIsCommandMatch(BYTE * pbyDestArray, int nArrayLength, TCHAR * pszSourceText);
bool ScannerSetDeviceID(ESL_DEVICE_ID stDeviceID);

//void ScannerParseTagCommand(TCHAR * pszDeviceIdString);
void ScannerParseTagCommand(ESL_DEVICE_ID stDeviceID);
void ScannerParseCommand(ESL_BARCODE_VALUE stBarcodeValue);

#ifdef __cplusplus
}
#endif

#endif		//_DAVICOM_SCANNER_H_
