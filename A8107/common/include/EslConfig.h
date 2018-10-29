/* EslConfig.h: Davicom ESL configure header for ESL system.                    */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: EslConfig.h                                                   */
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
#ifndef _DAVICOM_ESL_CONFIG_H_
#define _DAVICOM_ESL_CONFIG_H_

#include <Stdbool.h>
#include <Stdint.h>
#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "basic_windef.h"
#include "AMICCOM_CM0.h"
#include "common_error.h"

//==============================================================================
//--- Declarative Constants ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Board ---
//------------------------------------------------------------------------------
#define XTAL    						(16000000UL)    // Oscillator frequency
#define UART0_BAUDRATE					115200 			// baudrate 115200 for Console. (Debug Message)
#define UART2_BAUDRATE					9600 			// baudrate 9600 for Scanner

//------------------------------------------------------------------------------
//--- Scanner ---
//------------------------------------------------------------------------------
// [Category][Species]-[Identifier]
#define MAX_BARCODE_CATEGORY			3				// Category
#define MAX_BARCODE_SPECIES				4				// Species
#define MAX_BARCODE_DASH				1				// Dash	
#define MAX_BARCODE_IDENTIFIER			10				// Identifier
#define MAX_BARCODE_SCAN				(128)			// The maximum size of the scan buffer (DMC-XXXX-1234567890 / 6889420075...)
#define MAX_BARCODE_TAG					(4+1+10)		// The maximum  size of the tag ID (XXXX-1234567890)
#define MAX_BARCODE_COMMAND				(4+1+10)		// The maximum  size of the command ID. (XXXX-1234567890)
#define MAX_BARCODE_PRODUCT				(16)			// The maximum  size of the product ID. (1234567890...)
#define MAX_BARCODE_FACTORY				(16)			// The maximum  size of the command ID. (XXXX-1234567890)


#define MAX_SEND_PAIRING_RETRY_TIME		20				//

#define CMD_PAIR_PRODUCT				1				// Commodity and tag pairing
#define CMD_PAIR_COMMAND				2				// Command and tag pairing

//------------------------------------------------------------------------------
//--- Scanner ---
//------------------------------------------------------------------------------
#define MAX_BEACON_FIFO_LENGTH			16				// Set Beacon FIFO Length
#define MAX_DATA_FIFO_LENGTH			64				// Set Data FIFO Length

//------------------------------------------------------------------------------
//--- Device ID ---
//------------------------------------------------------------------------------
#define MAX_DEVICE_SPECIES_ID			2				// Species
//#define MAX_BARCODE_DASH				1				// Dash	
//#define MAX_BARCODE_IDENTIFIER		10				// Identifier

#define MAX_DEVICE_EUID					3
#define MAX_DEVICE_ESID					2
#define MAX_DEVICE_EFID					(MAX_DEVICE_EUID+MAX_DEVICE_ESID)
#define MAX_DEVICE_IDENTIFIER			MAX_DEVICE_EFID


//------------------------------------------------------------------------------
//--- ESL Message ---
//------------------------------------------------------------------------------
#define ESL_MSG_UNRELATED_PACKET					0x00		// Unrelated packet
#define ESL_MSG_PACKET_LOST							0x01
#define ESL_MSG_RX_TIMEOUT							0x02		// Receive packet timeout
#define ESL_MSG_RX_COMPLETED						0x03
// Tag Message
#define ESL_MSG_HIT_ID								0x04
#define ESL_MSG_NO_HIT_ID							0x05
#define ESL_MSG_HOPPING								0x06
#define ESL_MSG_REPORT_STATUS_OK					0x07
#define ESL_MSG_STATE_NOT_RIGHT						0x08
#define ESL_MSG_TO_RCV_TAG_LIST						0x09
#define ESL_MSG_BEACON_SYNC							0x0A		// 
// Scanner Message
//#define ESL_MSG_SEND_PAIRING						0x30		// 
//#define ESL_MSG_NO_PAIRING_DATA					0x31		// 
//#define ESL_MSG_PAIRING_COMPLETED					0x32		// 
//#define ESL_MSG_PAIRING_ACK						0x32		// Pairing Completed
//#define ESL_MSG_PAIRING_NAK						0x33		// Pairing failed
//#define ESL_MSG_TX_TIMEOUT						0x34		// Send packet timeout
//
// ACK, acknowledgement (ACK)
// NAK, Negative-Acknowledgment (NAK or NACK)



//==============================================================================
//--- Declarative Macro ---
//==============================================================================
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t *)(ADDRESS)))
#define HW32_REG(ADDRESS)  (*((volatile uint32_t *)(ADDRESS)))
	
/*-------------------- General Purpose Input Output (GPIO) -------------------*/	
#define GPIO_PIN_OUTPUT(gpio, pin) 	      (gpio->OUTENABLESET = (1<<pin))	 ; //OE = 1
#define GPIO_PIN_INPUT(gpio, pin) 	      (gpio->OUTENABLECLR = (1<<pin))	

//==============================================================================
//--- Declarative Structure ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Common ---
//------------------------------------------------------------------------------
#pragma pack(1)
typedef	struct _DAVICOM_VERSION_
{
	union _VERSION_TYPE_
	{
		struct _VERSION_
		{
			BYTE	byMajor;
			BYTE	byMinor;
			BYTE	byRelease;
			BYTE	byBuild;
		} Version;
		DWORD	dwVersion;
	}Version;
} DAVICOM_VERSION, *PDAVICOM_VERSION;

typedef struct _ESL_DEVICE_SPLIT_ID_
{
	uint8_t	byEUID[MAX_DEVICE_EUID]; 	// ESL Device Unique Identifier
	uint8_t	byESID[MAX_DEVICE_ESID]; 	// ESL Device Specific Definition	
}ESL_DEVICE_SPLIT_ID, *PESL_DEVICE_SPLIT_ID;

typedef struct _ESL_DEVICE_ID_
{
	union
	{
		uint8_t	byEFID[MAX_DEVICE_IDENTIFIER]; 		// ESL Device Full Identifier
		ESL_DEVICE_SPLIT_ID  Split;	// ESL Device Split Identifier
	};
}ESL_DEVICE_ID, *PESL_DEVICE_ID;

typedef struct _ESL_BARCODE_ID_
{
	uint8_t	byCategory[MAX_BARCODE_CATEGORY]; 	// ESL Category Definition		
	uint8_t	bySpecies[MAX_BARCODE_SPECIES]; 	// ESL Species Definition		
	uint8_t byDash;								// Dash
	union
	{
		uint8_t	byEFID[MAX_DEVICE_IDENTIFIER]; 		// ESL Device Full Identifier
		ESL_DEVICE_SPLIT_ID  Split;	// ESL Device Split Identifier
	};
}ESL_BARCODE_ID, *PESL_BARCODE_ID;

typedef struct _ESL_BARCODE_VALUE_
{
	uint8_t	byCategory[MAX_BARCODE_CATEGORY]; 	// ESL Category 		
	uint16_t uiSpecies; 						// ESL Species 		
	uint64_t uiIdentifier;						// Identifier Value
}ESL_BARCODE_VALUE, *PESL_BARCODE_VALUE;



#pragma pack()

//------------------------------------------------------------------------------
//--- Scanner ---
//------------------------------------------------------------------------------
// make a pair
#pragma pack(1)
typedef struct _BARCODE_PAIR
{
	uint8_t uiCommand;							// Pair command
	uint8_t uiCount;							// Send Count
	bool fLock;									// Scanner Lock
	BYTE byDevice[MAX_BARCODE_TAG];				// Device Barcode
	uint32_t uiSendingTime;						// Sending time
	union
	{
		BYTE byProduct[MAX_BARCODE_PRODUCT];	// Product Barcode
		BYTE byCommand[MAX_BARCODE_COMMAND];	// Command Barcode
	};
}BARCODE_PAIR, *PBARCODE_PAIR;


#pragma pack()

#ifdef __cplusplus
}
#endif

#endif		//_DAVICOM_ESL_CONFIG_H_
