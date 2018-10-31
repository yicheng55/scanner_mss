/* isr.h: Davicom radio frequency packet header for ESL system.                 */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: RfPacket.h                                                    */
/* Purpose      : RF Packet                                                     */
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
/* Notice       : Copy RX_DATA without using the memcpy function, which will    */
/*                result in missing packets.                                    */
/* -----------------------------------------------------------------------------*/
/* How to use:                                                                  */
/*                                                                              */
#ifndef _DAVICOM_RF_PACKET_H_
#define _DAVICOM_RF_PACKET_H_
#include "ProjectConfig.h"
#include "RfCommunication.h"
//#include "Scanner.h"
//#include <Stdbool.h>

// WTR (Wait until TX or RX finished)

//==============================================================================
//--- Declarative Constants ---
//==============================================================================
#define VENDOR                   			"Davicom Semiconductor"
#define RF_DEFAULT_ENCRYPTION_MODE	true 	// Encrption

//------------------------------------------------------------------------------
//--- ESL Device Specific Definition ---		
//------------------------------------------------------------------------------
//#define ESID_DEFAULT_ID				{0x00, 0x07}	// Device Specific
//#define EDID_DEFAULT_ID				{EUID_SCANNER_ID, 0x00, 0x07}	// ESL Device ID

// ESL Protocol v1.0
//#define EDID_DEFAULT_ID				{EUID_SCANNER_ID, ESID_SCANNER_ID}	// ESL Device ID

// ESL Protocol v1.1
#define EUSI_DEFAULT_ID				{EISI_DEFAULT_ID, EUSI_SCANNER_ID}	// ESL Device ID


//------------------------------------------------------------------------------
//--- RF ---
//------------------------------------------------------------------------------
#define	RF_DEFAULT_CHANNEL	      	44			// Default channel
#define RF_CHANNEL_INTERVAL			1			// Interval
#define RF_CHANNEL_MINIMUM			1			// Maximum
#define RF_CHANNEL_MAXIMUM			255			// Maximum
#define MAX_CHANNEL_RETRY			10			// Channel Retry
#define MIN_CHANNEL_BEACON			3			// Channel Beacon



//==============================================================================
//--- Declarative Macro ---
//==============================================================================
//void ShowPacket(uint8_t aryPacket[64], uint8_t uiLength);
#ifdef _DEBUG
#define SHOW_RECEIVE_PACKET	ShowReceivePacket

#else
#define SHOW_RECEIVE_PACKET	

#endif

//==============================================================================
//--- Declarative Functions ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative constructor ---
//------------------------------------------------------------------------------
void RfPacket(void);

//------------------------------------------------------------------------------
//--- Declarative Public Functions ---
//------------------------------------------------------------------------------
void RfPacketInit(void);
uint32_t RfGetPacket(LPBYTE lpbyBuffer, uint8_t uiBufferSize, uint8_t uiPayloadLength, uint32_t uiTimeout);


// RF Setting
void UserRegister(uint8_t uiDataRate);

// RF Data Rate
void RfSetDataRate(uint8_t uiDataRate);
uint8_t RfGetDataRate(void);

// RF Channel
void RfSetChannel(uint8_t uiChannel);
uint8_t RfGetChannel(void);

//bool RfSaveChannel(uint8_t uiChannel);
//uint8_t RfLoadChannel(void);

void RfSetScanChannel(bool fScan);
void RfScanChannelFailed(void);
void RfScanChannelSucceeded(void);
bool RfIsScanningChannel(void);


// RF RSSI
uint16_t RfGetRssiCode(void);
int16_t RfConvertRssiInputPower(uint16_t uiRssiCode);
bool RfIsRssiEnableAGC(void);

// Machine State
void SetMachineState(uint8_t uiMachineState);
uint8_t GetMachineState(void);
uint8_t * GetMachineStatePointer(void);

// Encrption Mode
void RfSetEncryptionMode(bool fEnable);
uint8_t RfGetEncryptionMode(void);

// Rx Timeout
void RfSetRxTimeout(bool fTimeout);
bool RfIsRxTimeout(void);

// Get Device ID
bool SetDeviceID(ESL_DEVICE_ID stID);
bool GetDeviceID(ESL_DEVICE_ID * pstID);

bool SetRepeaterID(ESL_DEVICE_ID stID);
bool GetRepeaterID(ESL_DEVICE_ID *pstID);

bool SetGatewayID(ESL_DEVICE_ID stID);
bool GetGatewayID(ESL_DEVICE_ID *pstID);

bool SetBindingID(ESL_DEVICE_ID stID);
bool GetBindingID(ESL_DEVICE_ID *pstID);

// Send Barcode Command
bool RfSendBarcodePairing(BARCODE_PAIR *pstBarcode);
bool RfSendBarcodeRegistration(BARCODE_PAIR *pstBarcode);
bool RfSendBarcodeCommand(BARCODE_PAIR *pstBarcode);
bool RfSendBarcodeInformation(BARCODE_PAIR *pstBarcode);

// Binding Mode/Free Mode
//bool RfIsBindingMode(void);
//void RfSetBindingMode(bool fBindingMode);

// Beacon Mode (OSBM_BEACON_FREE_MODE/OSBM_BEACON_SYNC_MODE/OSBM_BEACON_BINDING_MODE)
uint8_t RfGetBeaconMode(void);
void RfSetBeaconMode(uint8_t uiBeaconMode);
bool RfIsBeaconSyncTimeout(void);	// for OSBM_BEACON_SYNC_MODE
void RfResetBindingID(void);

//------------------------------------------------------------------------------
//--- Declarative Proteted Functions ---
//------------------------------------------------------------------------------
void ShowReceivePacket(uint8_t aryPacket[64], uint8_t uiLength);

// Random delay for TX
uint32_t RfRandomDelay(uint32_t uiMaximum);


uint32_t RfRxPacket(LPBYTE lpbyBuffer, uint8_t uiBufferSize);

//
//--- Communication Procedure Functions ---
//
uint32_t RfCommunicationProcedure(uint32_t uiMessage, LPVOID lpParam1, LPVOID lpParam2);
uint32_t RfDefCommProc(uint32_t uiMessage, LPVOID lpParam1, LPVOID lpParam2);

//
//--- Command Functions ---
//
uint32_t RfOnBeaconSync(uint32_t uiCommand, PSYNC_BEACON pstSyncBeacon);
uint32_t RfOnScannerPacket(uint32_t uiCommand, PESL_COMMON_PACKET pstPacket);


bool SendBarcodeCommand(BARCODE_PAIR *pstBarcode);
bool SendBarcodePair(BARCODE_PAIR *pstBarcode);

#endif //_DAVICOM_RF_PACKET_H_


