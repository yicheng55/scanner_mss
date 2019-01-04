/* ProjectConfig.h: Davicom scanner device header for ESL system.                     */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: ProjectConfig.h                                                     */
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
#ifndef _PROJECT_CONFIG_H_
#define _PROJECT_CONFIG_H_
#include <stddef.h>

#ifdef __cplusplus
 extern "C" {
#endif
#include "EslConfig.h"
#include "EslDebug.h"

//==============================================================================
//--- Declarative Constants ---
//==============================================================================
#define VERSION_MAJOR				0
#define VERSION_MINOR				1
#define VERSION_RELEASE				1
#define VERSION_BUILD				2
//------------------------------------------------------------------------------
//--- The size of description ---
//------------------------------------------------------------------------------
#define MAX_NAME_SIZE				64
#define MAX_MODEL_SIZE				64
#define MAX_VENDOR_SIZE				64
#define MAX_MANUFACTURER_SIZE		64
	 
//------------------------------------------------------------------------------
//--- Board ---
//------------------------------------------------------------------------------
#define XTAL    						(16000000UL)    // Oscillator frequency

#ifdef UART0_BAUDRATE
#undef UART0_BAUDRATE
#define UART0_BAUDRATE					115200 			// baudrate 115200 for Console. (Debug Message)
#endif

#ifdef UART2_BAUDRATE
#undef UART2_BAUDRATE
#define UART2_BAUDRATE					9600 			// baudrate 9600 for Scanner
#endif

//------------------------------------------------------------------------------
//--- Product/Tag Barcode size ---
//------------------------------------------------------------------------------
#define SIZE_COMPANY_ID					2				// Company ID: DM
#define SIZE_FORMAT_ID					1				// Format ID: T:Tag/C:Command
#define SIZE_MODEL_ID					4				// Model 
#define SIZE_TAG_SCREEN_ID				3				// the first 3 numerical digits for tag size (inch)
#define SIZE_COLOR_ID					1				// 
#define SIZE_DEVICE_ID					10				// Device ID
#define SIZE_COMMAND_ID					10				// Serial
#define SIZE_PRODUCT_ID					10				// Serial
#define SIZE_TAG_ID						10				// Serial

#define SIZE_DAVICOM_COMMAND_KEY		(SIZE_COMPANY_ID + SIZE_FORMAT_ID + SIZE_MODEL_ID + SIZE_COMMAND_ID)
#define SIZE_DAVICOM_PRODUCT_KEY		(SIZE_COMPANY_ID + SIZE_FORMAT_ID + SIZE_MODEL_ID + SIZE_TAG_ID)
#define SIZE_DAVICOM_TAG_KEY			(SIZE_COMPANY_ID + SIZE_FORMAT_ID + SIZE_TAG_SCREEN_ID + SIZE_COLOR_ID + SIZE_TAG_ID)


//------------------------------------------------------------------------------
//--- Time ---
//------------------------------------------------------------------------------
#define TIME_PACKET_RECV				900			// 900 ms
#define TIME_PACKET_RECV_BEACON			1000		// 900 ms
#define TIME_PACKET_RECV_ACK			100			// 900 ms
#define TIME_STORBE_BEACON_DELAY		10			// 10 ms for StrobeCmd

#define TIME_RECEIVE_TIMEOUT			5000		// 5000 ms 

#define MAX_RANDOM_TIME					1000		// 1000 us 
#define MAX_RETRY_LIMIT	      			5000		// Timing trigger time


#define TIME_BEACON_TIMEOUT				10			// Beacon timeout (Secound)
#define	TIME_ONE_SECONE_TICKCOUNT		256			// One second tick count value

//------------------------------------------------------------------------------
//--- Timing Trigger ---
//------------------------------------------------------------------------------
#define TIME_TIMING_TRIGGER				5			// Timing trigger (5ms)
#define TIME_TIMING_SECOND_COUNT		(1000/TIME_TIMING_TRIGGER)		// 1 Second
#define TIME_TIMING_MINUTE_COUNT		(60*TIME_TIMING_SECOND_COUNT)	// 1 Minute
#define TIME_TIMING_HOUR_COUNT			(60*TIME_TIMING_MINUTE_COUNT)	// 1 Hour
#define TIME_TIMING_DAY_COUNT			(24*TIME_TIMING_HOUR_COUNT)		// 1 Day (8640000)
#define TIME_TIMING_30_DAY_COUNT		(30*TIME_TIMING_DAY_COUNT)		// 30 Day
#define TIME_TIMING_90_DAY_COUNT		(90*TIME_TIMING_DAY_COUNT)		// 90 Day
#define TIME_TIMING_180_DAY_COUNT		(180*TIME_TIMING_DAY_COUNT)		// 180 Day
#define TIME_TIMING_365_DAY_COUNT		(365*TIME_TIMING_DAY_COUNT)		// 365 Day (3153600000)


//------------------------------------------------------------------------------
//--- EVENT ---
//------------------------------------------------------------------------------
#define EVENT_TIMER_BEACON_SYNC			1
#define EVENT_TIMER_DATA_PACKET			2
#define EVENT_TIMER_TIMING_TRIGGER		3				// Timing trigger event

#define TIMER_ONESHOT					TIMER0
#define TIMER_TIMING_TRIGGER			TIMER1				// Timing trigger timer


//------------------------------------------------------------------------------
//--- LED and EPD GPIO ---
//------------------------------------------------------------------------------
// LED and EPD GPIO
#define GPIO_LED						GPIO0	
#define GPIO_EPD						GPIO0

//
// PIN
//
//##############################################################################
//******************************************************************************	 
//#define FLAG_LED_DEMO_BOARD						1
//******************************************************************************
//##############################################################################
#define PIN_EPD_RESET					14
#define PIN_EPD_BUSY					13

#ifdef FLAG_LED_DEMO_BOARD
#define PIN_LED1						25		// LED1 Green
#define PIN_LED2						26		// LED2 Yellow
#define PIN_LED1_P						P25		// LED1 Green	
#define PIN_LED2_P						P26		// LED2 Yellow
#else
#define PIN_LED1						11		// LED1 Green
#define PIN_LED2						15		// LED2 Yellow
#define PIN_LED1_P						P11		// LED1 Green
#define PIN_LED2_P						P15		// LED2 Yellow
#endif	


//------------------------------------------------------------------------------
//--- Debug Message Flags ---
//------------------------------------------------------------------------------
//#define FLAG_SHOW_RX_TICK_COUNT					1	// Show Tick count
#define FLAG_SHOW_RX_CURRENT_TIME					1	// Show current time in RfCommunicationProcedure
//#define FLAG_DISABLE_TX_TIMEOUT					1	// Disable Tx timeout (for debug)
//#define FLAG_ENABLE_OSMS_ERROR_MESSAGE			1	// Enable machine state error massage(for debug)
#define FLAG_SHOW_SYSTEM_MODE_SWITCHING_MESSAGE		1	// Display system mode switching message
#define FLAG_SHOW_SCAN_CHANNEL_MESSAGE				1	// Display scan channel switch message
//#define FLAG_SHOW_WAIT_SERVER_RESPONSE			1	// Waiting for the result of the server's response


//==============================================================================
//--- Declarative Machine State ---
//==============================================================================
// Operating system machine state
#define OSMS_PWRBEACON_STATE				1		// 
#define OSMS_BEACON_STATE					2
#define OSMS_HEADER_STATE					3
#define OSMS_DATA_STATE						4
#define OSMS_REPLYRESULT_STATE				5
#define OSMS_EPDDATA_STATE					6
#define OSMS_EPDREFRESH_STATE				7
#define OSMS_ENTERPM1_STATE					8
#define OSMS_HOPPM1_STATE					9
#define OSMS_HOPBEACON_STATE				10
#define OSMS_FAIL_STATE						11
#define OSMS_TAG_LIST_STATE					12


#define OSMS_WAITTING_RESULT_STATE			30

//==============================================================================
//--- Declarative Scanner Mode ---
//==============================================================================
#define OSSM_BARCODE_READER_MODE			0
#define OSSM_BARCODE_PAIRING_MODE			1

//==============================================================================
//--- Declarative Beacon Mode ---
//==============================================================================
#define OSBM_BEACON_FREE_MODE				0	// 
#define OSBM_BEACON_SYNC_MODE				1	// 
#define OSBM_BEACON_BINDING_MODE			2	// 

//==============================================================================
//--- Declarative Structure ---
//==============================================================================
#pragma pack(1)
typedef struct _ESL_DEVICE_STATUS_
{
	ESL_DEVICE_ID 	stDeviceID;			// Self Device Identifier
	ESL_DEVICE_ID 	stGatewayID;		// 	
	ESL_DEVICE_ID 	stRepeaterD;		//
	uint8_t      	uiChannel;			//
	uint8_t 		uiRssi;				// 
	DAVICOM_VERSION stVersion;			// 
	uint16_t      	uiUpdate;			// UPDATE_COUNT; 
	uint16_t      	uiReboot;			// REBOOT_COUNT; 
	uint8_t         uiBattery; 			// BATTERY_LEVEL; 
	uint8_t      	uiWakeupInterval;	// WAKEUP_INTERVAL; 
	uint32_t     	uiSecond; 			// SECOND; 	
	uint16_t     	uiUpdateFailCount;	// UPDATEFAIL_COUNT;
	uint8_t      	Reserved[30];		// UNDEFINED[30]; 
} ESL_DEVICE__STATUS, *PESL_DEVICE_STATUS;

#if 0
typedef struct _ESL_SCANNER_DATA_
{
	ESL_DEVICE_ID 	stDeviceID;			// Self Device Identifier
	ESL_DEVICE_ID 	stGatewayID;		// 	
	ESL_DEVICE_ID 	stRepeaterD;		//
}ESL_SCANNER_DATA, *PESL_SCANNER_DATA;
#endif

typedef struct _SYSTEMTIME {
  WORD wYear;
  WORD wMonth;
  WORD wDayOfWeek;
  WORD wDay;
  WORD wHour;
  WORD wMinute;
  WORD wSecond;
  WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME;

//
//
#define MAX_NVM_PAGE_SIZE				128		// page size for rease
#define MAX_NVM_DATA_SIZE				512		// Non Volatile Data Size
#define MAX_NVM_DATA_RESERVED			(MAX_NVM_DATA_SIZE - 2 - 2 - 1 - MAX_DEVICE_IDENTIFIER - 2 - MAX_NAME_SIZE - MAX_MODEL_SIZE - MAX_VENDOR_SIZE - MAX_MANUFACTURER_SIZE)

typedef struct _ESL_NVM_DATA_			// VOLATILE Data
{
	uint16_t wMagicCode;							// 2 Bytes, Magic Code = 0x3094	 
	uint16_t wScannerMode;							// 2 Bytes, Scanner Mode
	bool fEncryptionMode;							// 1 Bytes, Encryption Mode 
	uint8_t uiChannel;								// 1 Byte, Default Channel
	uint8_t uiDataRate;								// 1 Byte, Default Data Rate
	ESL_DEVICE_ID stDeviceID; 						// 5 Byes, Device ID
	uint8_t szName[MAX_NAME_SIZE]; 					// 64 Byes, Device Name(Nickname/Local name)
	uint8_t szModel[MAX_MODEL_SIZE]; 				// 64 Byes, Device Model
	uint8_t szVendor[MAX_VENDOR_SIZE]; 				// 64 Byes, Davicom Semiconductor, Inc.
	uint8_t szManufacturer[MAX_MANUFACTURER_SIZE]; 	// 64 Byes, Davicom Semiconductor, Inc.	
	uint8_t byReserved[MAX_NVM_DATA_RESERVED];	// 512 - N Bytes, Reserved
}ESL_NVM_DATA, *PESL_NVM_DATA;

//extern ESL_SCANNER_DATA g_stDeviceInfo;

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif		//_PROJECT_CONFIG_H_
