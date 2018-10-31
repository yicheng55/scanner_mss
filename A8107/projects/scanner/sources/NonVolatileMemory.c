// NonVolatileMemory.c: implementation of the string functions.
//
//////////////////////////////////////////////////////////////////////
#include "NonVolatileMemory.h"
#include "RfCommunication.h"
#include "RfPacket.h"

//==============================================================================
//--- Declarative Variables ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative Global Variables ---
//------------------------------------------------------------------------------
ESL_NVM_DATA g_stNvmMappingData;


// NVM Start address 0x3EF00 (512 bytes) for Scanner parameter.
//const ESL_NVM_DATA g_stNvmData __attribute__( (at(FLASH_ADDRESS_SCANNER_NVM) ) );	
#if 0
// ESL Protocol v1.0
const ESL_NVM_DATA g_stNvmData __attribute__((at(FLASH_ADDRESS_SCANNER_NVM))) = 
	{
		FLASH_MAGIC_CODE, 			// Magic Code
		OSSM_BARCODE_READER_MODE,	// Scanner Mode
		RF_DEFAULT_ENCRYPTION_MODE, // RF Encrption Mode
		RF_DEFAULT_CHANNEL,			// RF Channel
		RF_DEFAULT_DATARATE,			// RF Data Rate
		EUID_SCANNER_ID,			// ESL Scanner Device Unique Identifier (0xFF, 0xFF, 0xFD)
		ESID_SCANNER_ID,			// ESL Device Specific Definition (0x00, 0x00 ~ 0xFF, 0xFF)	
		TEXT_DEVICE_NAME,			// Device Name
		TEXT_MODEL_NAME,			// Model Name
		TEXT_VENDOR_NAME, 			// Vendor Name
		TEXT_MANUFACTURER_NAME		// Manufacturer Name
	};
#endif
// ESL Protovol v1.1	
const ESL_NVM_DATA g_stNvmData __attribute__((at(FLASH_ADDRESS_SCANNER_NVM))) = 
	{
		FLASH_MAGIC_CODE, 			// Magic Code
		OSSM_BARCODE_READER_MODE,	// Scanner Mode
		RF_DEFAULT_ENCRYPTION_MODE, // RF Encrption Mode
		RF_DEFAULT_CHANNEL,			// RF Channel
		RF_DEFAULT_DATARATE,			// RF Data Rate
		EISI_DEFAULT_ID,			// ESL Scanner Device Unique Identifier (0xFF, 0xFF, 0xFD)
		EUSI_SCANNER_ID,			// ESL Device Specific Definition (0x00, 0x00 ~ 0xFF, 0xFF)	
		TEXT_DEVICE_NAME,			// Device Name
		TEXT_MODEL_NAME,			// Model Name
		TEXT_VENDOR_NAME, 			// Vendor Name
		TEXT_MANUFACTURER_NAME		// Manufacturer Name
	};
	


//------------------------------------------------------------------------------
//--- Declarative Member Variables ---
//------------------------------------------------------------------------------


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
void NonVolatileMemory()
{
	NonVolatileMemoryInit();
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

//==============================================================================
// Function     : InitUart
// Purpose      : Init Uart for debug mesage
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//============================================================================
void NonVolatileMemoryInit(void)
{
	//memcpy(&g_stNvmMappingData, &g_stNvmData, sizeof(ESL_NVM_DATA));
	NonVolatileMemoryReadConfiguration(&g_stNvmMappingData);

	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("\r\n=== Device Information ===\r\n"), g_stNvmMappingData.wMagicCode);	
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Magic Code: %04X\r\n"), g_stNvmMappingData.wMagicCode);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Scanner Mode: %04X\r\n"), g_stNvmMappingData.wScannerMode);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Encryption: %s\r\n"), g_stNvmMappingData.fEncryptionMode?_T("TRUE"):_T("FALSE"));
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Device ID: %02X:%02X:%02X:%02X:%02X\r\n"), g_stNvmMappingData.stDeviceID.byEFID[0],g_stNvmMappingData.stDeviceID.byEFID[1], g_stNvmMappingData.stDeviceID.byEFID[2], g_stNvmMappingData.stDeviceID.byEFID[3], g_stNvmMappingData.stDeviceID.byEFID[4]);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Name: %s\r\n"), g_stNvmMappingData.szName);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Model: %s\r\n"), g_stNvmMappingData.szModel);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Vendor: %s\r\n"), g_stNvmMappingData.szVendor);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Manufacturer: %s\r\n\r\n"), g_stNvmMappingData.szManufacturer);
}

void NonVolatileMemoryWriteConfiguration(void)
{
#if 0
	//int nLength = sizeof(ESL_NVM_DATA);  // MAX_NVM_DATA_SIZE
	int nTime;
	LPBYTE lpbyData = (LPBYTE)&g_stNvmMappingData;
	
	for (nTime = 0; nTime < MAX_NVM_DATA_SIZE/MAX_NVM_PAGE_SIZE; nTime++)
	{
		Flash_PageErase(FLASH_ADDRESS_SCANNER_NVM + nTime * MAX_NVM_PAGE_SIZE);
	}
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("*** Write configuration to flash. ***\r\n"));
	// Maximum write length is 128 bytes. (one page size)
	for (nTime = 0; nTime < MAX_NVM_DATA_SIZE/MAX_NVM_PAGE_SIZE; nTime++)
	{
		Flash_Write_U8(FLASH_ADDRESS_SCANNER_NVM + nTime * MAX_NVM_PAGE_SIZE, (uint8_t *)lpbyData, (uint8_t)MAX_NVM_PAGE_SIZE);
		lpbyData += MAX_NVM_PAGE_SIZE;
	}	
#else
	NonVolatileMemoryWrite((LPBYTE)FLASH_ADDRESS_SCANNER_NVM, MAX_NVM_DATA_SIZE, (LPBYTE)&g_stNvmMappingData, sizeof(g_stNvmMappingData));
#endif	
}

bool NonVolatileMemoryReadConfiguration(ESL_NVM_DATA *pstNvmData)
{
	if (pstNvmData == NULL)
		return false;
	*pstNvmData = g_stNvmData;
	return true;
}

bool NonVolatileMemoryFactoryDefault()
{
	uint8_t szName[MAX_NAME_SIZE] = {TEXT_DEVICE_NAME}; 						// 64 Byes, Device Name(Nickname/Local name)
	uint8_t szModel[MAX_MODEL_SIZE] = {TEXT_MODEL_NAME}; 						// 64 Byes, Device Model
	uint8_t szVendor[MAX_VENDOR_SIZE] = {TEXT_VENDOR_NAME}; 					// 64 Byes, Davicom Semiconductor, Inc.
	uint8_t szManufacturer[MAX_MANUFACTURER_SIZE] = {TEXT_MANUFACTURER_NAME}; 	// 64 Byes, Davicom Semiconductor, Inc.	
	
	g_stNvmMappingData.wMagicCode = FLASH_MAGIC_CODE;	
	g_stNvmMappingData.wScannerMode = OSSM_BARCODE_READER_MODE;		
	g_stNvmMappingData.fEncryptionMode = RF_DEFAULT_ENCRYPTION_MODE;
	g_stNvmMappingData.uiChannel = RF_DEFAULT_CHANNEL;
	g_stNvmMappingData.uiDataRate = RF_DEFAULT_DATARATE;
	
	memcpy (g_stNvmMappingData.szName, szName, MAX_NAME_SIZE);
	memcpy (g_stNvmMappingData.szModel, szModel, MAX_MODEL_SIZE);
	memcpy (g_stNvmMappingData.szVendor, szVendor, MAX_VENDOR_SIZE);
	memcpy (g_stNvmMappingData.szManufacturer, szManufacturer, MAX_MANUFACTURER_SIZE);

	NonVolatileMemoryWriteConfiguration();
	
	return true;
}

bool NonVolatileMemoryWrite(LPBYTE lpbyDestination, uint32_t uiDestinationLength, LPBYTE lpbySource, uint32_t uiSourceLength)
{
	int nBlock;
	LPBYTE lpbyDestPos = (LPBYTE)lpbyDestination;
	LPBYTE lpbySrcPos = (LPBYTE)lpbySource;	
	int nDestBlocks = (uiDestinationLength/MAX_NVM_PAGE_SIZE) + (uiDestinationLength%MAX_NVM_PAGE_SIZE)?1:0 ;
	int nSourceBlocks = (uiSourceLength/MAX_NVM_PAGE_SIZE) + (uiSourceLength%MAX_NVM_PAGE_SIZE)?1:0 ;
	
	if ((lpbyDestination == NULL) || (lpbySource == NULL) || (uiDestinationLength < MAX_NVM_PAGE_SIZE) || (uiSourceLength < MAX_NVM_PAGE_SIZE))
		return false;
	if ((uiDestinationLength < uiSourceLength) || ( nDestBlocks < nSourceBlocks))
		return false;
	
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("*** Write data to flash. ***\r\n"));
	for (nBlock = 0; nBlock < nSourceBlocks; nBlock++)
	{
		lpbyDestPos = (LPBYTE)lpbyDestination +  nBlock * MAX_NVM_PAGE_SIZE;
		lpbySrcPos = (LPBYTE)lpbySource + nBlock * MAX_NVM_PAGE_SIZE;
		if (memcmp(lpbyDestPos, lpbySrcPos, MAX_NVM_PAGE_SIZE) != 0)
		{
			Flash_PageErase((uint32_t)lpbyDestPos);
			Flash_Write_U8((uint32_t)lpbyDestPos, (uint8_t *)lpbySrcPos, (uint8_t)MAX_NVM_PAGE_SIZE);
		}
	}
	return true;
}


void NonVolatileMemoryShowInforamtion(void)
{
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("\r\n=== NVM Device Information ===\r\n"), g_stNvmData.wMagicCode);	
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Magic Code: %04X\r\n"), g_stNvmData.wMagicCode);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Scanner Mode: %04X\r\n"), g_stNvmData.wScannerMode);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Encryption: %s\r\n"), g_stNvmData.fEncryptionMode?_T("TRUE"):_T("FALSE"));
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Channel: %02d\r\n"), g_stNvmData.uiChannel);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Data Rate: 0x%02X\r\n"), g_stNvmData.uiDataRate);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Device ID: %02X:%02X:%02X:%02X:%02X\r\n"), g_stNvmData.stDeviceID.byEFID[0],g_stNvmData.stDeviceID.byEFID[1], g_stNvmData.stDeviceID.byEFID[2], g_stNvmData.stDeviceID.byEFID[3], g_stNvmData.stDeviceID.byEFID[4]);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Name: %s\r\n"), g_stNvmData.szName);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Model: %s\r\n"), g_stNvmData.szModel);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Vendor: %s\r\n"), g_stNvmData.szVendor);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Manufacturer: %s\r\n\r\n"), g_stNvmData.szManufacturer);
	
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("\r\n=== Current Device Information ===\r\n"), g_stNvmMappingData.wMagicCode);	
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Magic Code: %04X\r\n"), g_stNvmMappingData.wMagicCode);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Scanner Mode: %04X\r\n"), g_stNvmMappingData.wScannerMode);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Encryption: %s\r\n"), g_stNvmMappingData.fEncryptionMode?_T("TRUE"):_T("FALSE"));
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Channel: %02d\r\n"), g_stNvmMappingData.uiChannel);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Data Rate: 0x%02X\r\n"), g_stNvmMappingData.uiDataRate);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Device ID: %02X:%02X:%02X:%02X:%02X\r\n"), g_stNvmMappingData.stDeviceID.byEFID[0],g_stNvmMappingData.stDeviceID.byEFID[1], g_stNvmMappingData.stDeviceID.byEFID[2], g_stNvmMappingData.stDeviceID.byEFID[3], g_stNvmMappingData.stDeviceID.byEFID[4]);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Name: %s\r\n"), g_stNvmMappingData.szName);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Model: %s\r\n"), g_stNvmMappingData.szModel);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Vendor: %s\r\n"), g_stNvmMappingData.szVendor);
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Manufacturer: %s\r\n\r\n"), g_stNvmMappingData.szManufacturer);	
}
