/* LED.h: Davicom string header for ESL system.                     */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: LED.h                                                       */
/* Purpose      : LED header                                          */
/* Autor        : Richard Chuang                                                */
/* Email        : richard_chung@davicom.com.tw                                  */
/* Description  :                                                               */
/* Reference    :                                                               */
/* Copyright    : (c) Davicom Semiconductor Inc. All Rights Reserved.           */
/* -----------------------------------------------------------------------------*/
/* Version      : 1.0.0.0                                                       */
/* Update       : 2018-04-18                                                    */
/* Modified     : Richard Chung                                                 */
/* Description	:                                                               */
/* -----------------------------------------------------------------------------*/
/* How to use:                                                                  */
/*                                                                              */
#ifndef _DAVICOM_LED_STATUS_H_
#define _DAVICOM_LED_STATUS_H_

#include "ProjectConfig.h"

#pragma pack(1)
#pragma pack()

//==============================================================================
//--- Declarative Constants ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Board ---
//------------------------------------------------------------------------------
#ifdef FLAG_LED_DEMO_BOARD
#define LED_INDICATOR_OFF					1				//Indicators
#define LED_INDICATOR_ON					0	
#else
#define LED_INDICATOR_OFF					0				//Indicators
#define LED_INDICATOR_ON					1	
#endif

#define LED_INDICATOR_NOT_BRIGHT			0x00000000
#define LED_INDICATOR_LED1_BIT				0x00000001
#define LED_INDICATOR_LED2_BIT				0x00000002
#define LED_INDICATOR_FULL_BRIGHT			0xFFFFFFFF	// Full bright


//==============================================================================
//--- Declarative LED State ---
//==============================================================================
// Operating system led state
#define OSLS_POWER_ON_STATE					0		// Power On
#define OSLS_ONLINE_STATE					1		// Online/Beacon
#define OSLS_SCAN_BARCODE_STATE				2		// Scan action
#define OSLS_IDENTIFIED_STATE				3		// Identified
#define OSLS_TRANSMISSION_STATE				4		// Transmission 
//#define OSLS_ACKNOWLEDGE_STATE			5		// acknowledge (Transmission Succeeded)
#define OSLS_SUCCEEDED_STATE				5		// acknowledge (Transmission Succeeded)
#define OSLS_FAILED_STATE					6		// failed (Transmission Failed/Pairing Failed/etc)
#define OSLS_NO_DEVICE_ID					7	
#define OSLS_PREDICTED_STATE				8		// Predicted failure ???
#define OSLS_SPUN_DOWN_STATE				9		// Spun down
#define OSLS_STANDBY_STATE					10		// Standby

// Operating system led array index
#define OSLI_INTERVAL_TIME					0
#define OSLI_CYCLE_TIME						1
#define OSLI_DATA_SIZE						2
#define OSLI_DATA_BEGIN						3

// LED Status Indicators
// 
// LED1 = GPIO P0_15/P0_25 Status LED1 (Green)            PIN28
// LED2 = GPIO P0_11/P0_26 Status LED2 (Amber/Orange)     PIN24 
// LED3 =                  Power LED   (Red)

//
// Scanner LED Blink Error Codes
// LED Troubleshooting	                    Description
//-----------------------------------------+--------------------------------------------------------------------------------
// Off										The power is off and the scanner has not been discovered by the system.
// Green/amber flashing (Green On 250 ms / Amber On 250 ms, Off 250 ms)  Power on. 			
// Steady green								Scanner is online. (Beacon synchronization, Received ACK, Succeeded)
// Green flashing (250 milliseconds [ms])	Scanner is being identified. (Lost beacon, Auto scan channel)
// Amber flashing (200 ms)					The scanner is transmitting data. 
// Green/amber flashing (Green and amber On 100 ms, Off 100 ms, On 100 ms, Off 500ms) Scan barcode input.
// Steady amber 							Scanner has failed.

// Green flashing (On 400 ms, Off 100 ms)	Scanner is rebuilding.
// Green/amber flashing (Green and amber On 100 ms, Off 100 ms, On 100 ms, Off 500ms) Scan barcode input.
// Green/amber flashing (Green On 500 ms / Amber On 500 ms, Off 1000 ms)	Predicted failure reported by Scanner.
// Green flashing (Green On 3000 ms, Off 3000 ms, Amber On 3000 ms, Off 3000 ms)	Scanner being spun down by user request or other non-failure condition.

// [Name][Interval, Max] [Data Size][Data...			
// Power On: 			250ms, 1000ms, 2, [0x03, 0x00] ... 						Green/amber flashing (Green On 250 ms / Amber On 250 ms, Off 250 ms) 
// Online: 				1000ms, 1000ms, 1, [0x01] ... 							Steady green
// Identified: 			250ms, 1000ms: 2, [0x01, 0x00] ...						Green flashing (250 milliseconds [ms])	
// Transmission: 		200ms 1000ms: 2, [0x02, 0x00] ... 						Amber flashing (200 ms)	RF transmission data			
// Succeeded: 			1000ms, 1000ms, 1, [0x01] ... 							Steady green, Receive reply  
// Failed: 				1000ms, 1000ms, 1, [0x02] ... 							Steady amber, Scanner has failed.
// No DeviceID			500ms, 1000ms, 2, [0x03, 0x00] ...						Green/amber flashing (Green On 500 ms / Amber On 500 ms, Off 500 ms) 
// Predicted failure: 	500ms, 1500ms, 3, [0x03, 0x00, 0x00] ... 				Green/amber flashing (Green On 500 ms / Amber On 500 ms, Off 1000 ms) Predicted failure reported by Scanner.
// Spun down: 			3000ms, 12000ms: 4, [0x01, 0x00, 0x02, 0x00] ... 		Green flashing (Green On 3000 ms, Off 3000 ms, Amber On 3000 ms, Off 3000 ms)
// Standby				250ms, 2000ms, 8, [0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]


//==============================================================================
//--- Declarative Functions ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative constructor ---
//------------------------------------------------------------------------------
void Led(void);

//------------------------------------------------------------------------------
//--- Declarative Public Functions ---
//------------------------------------------------------------------------------
void LedSetStatus(uint8_t uiStatus);	// Set LED State
void LedReSetStatus(uint8_t uiStatus);

//------------------------------------------------------------------------------
//--- Declarative Callback Functions ---
//------------------------------------------------------------------------------
void OnLedTriggerTimer(uint8_t uiTime);	// Trigger Timer


//------------------------------------------------------------------------------
//--- Declarative Protected Functions ---
//------------------------------------------------------------------------------
void LedSetValue(uint32_t uiStatus);	// Set LED Value
void LedShowStatus(void);				// Change LED status according to time
//------------------------------------------------------------------------------


//==============================================================================
//--- Declarative Other Functions ---
//==============================================================================
#if 0
void SetLed1(bool fOn);
void SetLed2(bool fOn);
#endif
#endif		//_DAVICOM_LED_STATUS_H_
