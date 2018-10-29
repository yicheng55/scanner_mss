/* RfCommunication.h: Davicom RF communication header for ESL system.           */
/*                                                                              */
/*==============================================================================*/
/* Header Name  : RfCommunication.h                                             */
/* Purpose      : RF communication                                              */
/* Autor        : Richard Chuang                                                */
/* Email        : richard_chung@davicom.com.tw                                  */
/* Description  :                                                               */
/* Reference    :                                                               */
/* Copyright    : (c) Davicom Semiconductor Inc. All Rights Reserved.           */
/* -----------------------------------------------------------------------------*/
/* Version      : 1.0.0.0                                                       */
/* Update       : 2018-03-01                                                    */
/* Modified     : Richard Chung                                                 */
/* Description  :                                                               */
/* -----------------------------------------------------------------------------*/
/* How to use:                                                                  */
/*                                                                              */
#ifndef _DAVIOCM_RF_COMMUNICATION_H_
#define _DAVIOCM_RF_COMMUNICATION_H_
#include "ESLConfig.h"
#include <Stdbool.h>
#include <A8107M.h>
#include <slptimer.h>
#include "SPIconfig.h"
#include "Utility.h"
#include "EslDebug.h"
//==============================================================================
//--- Declarative Device Identifier  ---
//==============================================================================
//------------------------------------------------------------------------------
//--- ID Field Definition ---
//------------------------------------------------------------------------------
// ID[0~4] = ID[0][1][2]3][4]
//      ID[0]= (ID[1]+ID[2]+ID[3]+ID[4]) mod 255
//      Gateway ID  : FFFFFF-XXXX  (ID[0] == 0xFF, ID[2] == 0xFF)
//      Repeater ID : FFFFFE-XXXX  (ID[0] == 0xFF, ID[2] == 0xFE)
//      Scanner ID  : FFFFFD-XXXX  (ID[0] == 0xFF, ID[2] == 0xFD)
//

//------------------------------------------------------------------------------
//--- ESL Device Unique Identifier ---
//------------------------------------------------------------------------------
#define EUID_GATEWAY_ID     0xFF, 0xFF, 0xFF    // ESL Gateway Device Unique Identifier
#define EUID_REPEATER_ID    0xFF, 0xFF, 0xFE    // ESL Repeater Device Unique Identifier
#define EUID_SCANNER_ID     0xFF, 0xFF, 0xFD    // ESL Scanner Device Unique Identifier

#define ESID_SCANNER_ID     0x00, 0x00          // ESL Scanner Device Specific Identifier

//==============================================================================
//--- Declarative RF ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Data Rate ---
//------------------------------------------------------------------------------
/* Data Rate */
#define RF_DATARATE_1M                          0
#define RF_DATARATE_500K                        1
#define RF_DATARATE_250K                        2
#define RF_DEFAULT_DATARATE                     RF_DATARATE_500K

#define RF_TIMEOUT_BEACON                       2

//------------------------------------------------------------------------------
//--- Packet Type  ---
//------------------------------------------------------------------------------
/* RF Packet Type */
#define RF_PT_SYNC_BEACON                       0x01
#define RF_PT_PACKET_HEADER                     0x02
#define RF_PT_DATA_PACKET                       0x03
#define RF_PT_RX_DATA_RESULT                    0x04
#define RF_PT_EPD_DATA                          0x05
#define RF_PT_EPD_REFRESH                       0x06
#define RF_PT_ENTER_PM1                         0x07
#define RF_PT_HOPPING_BEACON                    0x08
#define RF_PT_TAG_STATUS                        0x09
#define RF_PT_ALWAYS_WAKE_BEACON                0x0A
#define RF_PT_TAG_UPDATE_LIST                   0x20
#define RF_PT_OTA_UPDATE_LIST                   0x21
#define RF_PT_TAG_STATUS_REPORT                 0x0B
#define RF_PT_UPDATE_STATUS_REPORT              0x0C
#define RF_PT_TAG_UNBOUND                       0x0D
#define RF_PT_CHANGE_BEACON_SRC                 0x0E
#define RF_PT_WRITE_TAG_NVM_DATA                0x0F
#define RF_PT_WRITE_TAG_ORIGINAL_NVM_DATA       0x10
#define RF_PT_OVER_THE_AIR                      0x11
#define RF_PT_OVER_THE_AIR_REPLY                0x12
#define RF_PT_TAG_BUTTON_NOTIFY                 0x13 //For medicine system
#define RF_PT_TAG_BUTTON_NOTIFY_ACK             0x14 //For medicine system
#define RF_PT_SCANNER_PACKET                    0x30 // Scanner Packet

/* Jerry add Extra define of Repeater */
#define RF_PT_REPEATER_STATUS                   0x31 //Repeater status
#define RF_PT_REPEATER_COMBTAG_RS               0x32 //Repeater combine Tag result reply of gateway
//------------------------------------------------------------------------------
//--- Action List ---
//------------------------------------------------------------------------------
/* Action List */
#define RF_AL_SYNC                              0x00
#define RF_AL_UPDATE_TAG                        0x01
#define RF_AL_GET_TAG_STATUS                    0x02
#define RF_AL_TAG_ALWAYS_WAKE_UP                0x03
#define RF_AL_GET_REPEATER_STATUS               0x04
#define RF_AL_HOP_TO_TAG                        0x05
#define RF_AL_HOP_TO_REPEATER                   0x06
#define RF_AL_ALL_WU_TIME                       0x07    // Wakeup Time
#define RF_AL_UPDATE_REPEATER_TAG               0x08
#define RF_AL_SET_TAG_LED                       0x09
#define RF_AL_TAG_UNBOUND                       0x0A
#define RF_AL_CHANGE_BEACON_SRC                 0x0B
#define RF_AL_WRITE_TAG_NVM_DATA                0x0C
#define RF_AL_WRITE_TAG_ORIGINAL_NVM_DATA       0x0D
#define RF_AL_OVER_THE_AIR                      0x0E

//------------------------------------------------------------------------------
//--- Command ---
//------------------------------------------------------------------------------
//#define RF_CMD_CONFIRMED_ACK                  0x0A    // Scanner Confirmed Ack


//------------------------------------------------------------------------------
//--- Command / Scanner Command ---
//------------------------------------------------------------------------------
#define RF_CMD_SCANNER_STANDBY                  0x80
#define RF_CMD_SCANNER_CONFIRMED_ACK            0x81    // Scanner Confirmed Ack
#define RF_CMD_SCANNER_PAIRING                  0x82    //
#define RF_CMD_SCANNER_UNPAIRING                0x83    // Unpairing
#define RF_CMD_SCANNER_COMMAND                  0x84    //
#define RF_CMD_SCANNER_REGISTRATION             0x85    // Device Registration
#define RF_CMD_SCANNER_BARCODE                  0x86    // Barcode Data


#define RF_CMD_SCANNER_CONFIRMED_NAK            0x89    // Scanner Confirmed Ack
//#define RF_CMD_FORCE_UPPER_HOST                 0x85    // Repater Pairing


//------------------------------------------------------------------------------
//--- Extension ---
//------------------------------------------------------------------------------
#define RF_FMT_EXTENSION_STATUS                 0x00    // Status Structure
#define RF_FMT_EXTENSION_PAIRING                0x01    // Pairing Structure
#define RF_FMT_EXTENSION_COMMAND                0x02    // Command Structure
#define RF_FMT_EXTENSION_BINDING                0x03    // Binding  Structure
#define RF_FMT_EXTENSION_RSSI                   0x04    // RSSI Structure
#define RF_FMT_EXTENSION_REGISTRATION           0x05    // Registration Structure
#define RF_FMT_EXTENSION_BAROCDE                0x06    // Barcode Structure


//------------------------------------------------------------------------------
//--- Status ---
//------------------------------------------------------------------------------
#define RF_STATUS_OK                            200     //


//------------------------------------------------------------------------------
//--- Factory Command  ---
//------------------------------------------------------------------------------

// Factory Command: [GroupID]-[CommandID]
// 0000-XXXXXXXXXX
// Group: Differentiating group commands
//      0000: Scanner Command
//      0001:
//      ... :
//      FFFF: etc...
//
// Command: The first two characters are check codes.
//      XX00000000 ~ XXFFFFFFFF
//

//
// Scanner Command
//
#define RF_FC_FORCE_STANDBY_MODE                _T("0000-0000000000")       // Standby Mode (Used by Scanner)
#define RF_FC_SWITCH_DEFAULT_MODE               _T("0000-0000000001")       // Execute switch to default mode instruction
#define RF_FC_FORCE_UPPER_HOST                  _T("0000-0000000002")       // Force the upper host (Used by Scanner)
#define RF_FC_UNPAIRING                         _T("0000-0000000003")       // Device Unpairing (Used by Scanner)
#define RF_FC_CHANGE_DEVICE_ID                  _T("0000-0000000004")       // Change Device ID (Used by Scanner)
#define RF_FC_WRITE_CONFIGURATION               _T("0000-0000000005")       // Write Configuration to NVM (Used by Scanner)
#define RF_FC_AUTO_SCAN_CHANNEL                 _T("0000-0000000006")       // Auto Scan Channel
//#define RF_FC_CHANNEL_SCAN                        _T("0001-0000000000")       // Scan Channel (Used by Scanner)
#define RF_FC_ENABLE_ENCRYPTION                 _T("0000-0000000007")       // Enable Encryption Mode
#define RF_FC_DISABLE_ENCRYPTION                _T("0000-0000000008")       // Disable Encryption Mode

#define RF_FC_DEVICE_REGISTRATION               _T("0000-0000000009")       // Device Registration
#define RF_FC_FACTORY_DEFAULT                   _T("0000-000000000A")       // Factory default (Configuration)
#define RF_FC_BARCODE_READER_MODE               _T("0000-000000000B")       // Barcode Reader Mode
#define RF_FC_BARCODE_PAIRING_MODE              _T("0000-000000000C")       // Barcode Pairing Mode
//#define RF_FC_GATEWAY_REGISTRATION            _T("0000-0000000008")       // Registration
//#define RF_FC_REPEATER_REGISTRATION           _T("0000-0000000008")       // Registration
//#define RF_FC_SCANNER_REGISTRATION            _T("0000-0000000008")       // Registration



#define RF_FC_COMMAND_CANCEL                    _T("0000-FFFFFFFF00")       // Cancel (Used by Scanner)
#define RF_FC_COMMAND_CONFIRM                   _T("0000-FFFFFFFF01")       // Confirm (Used by Scanner)

#define RF_FC_DIAGNOSTIC_MODE                   _T("0000-FF00000001")       // Diagnostic
#define RF_FC_LOW_POWER_MODE                    _T("0000-FF00000002")       // Low Power Mode
#define RF_FC_BAUD_RATE                         _T("0000-FF00000002")       // Baud Rate(115200/19200/...
#define RF_FC_DATE_RATE                         _T("0000-FF00000002")       // Date Rate
// System Information


//
// Scanner Command
// Adjust the Scanner's RF Frequency (40~60, 92~104)  (Force the channel)
//
//#define RF_FC_CHANNEL_SCAN                        _T("0001-0000000000")       // Scan Channel (Used by Scanner)
#define RF_FC_CHANNEL_040                       _T("0001-0000000028")   // Channel 40(0x28)  (Used by Scanner)
//...
#define RF_FC_CHANNEL_060                       _T("0001-000000003C")   // Channel 60(0x3C)
//..
#define RF_FC_CHANNEL_092                       _T("0001-000000005C")   // Channel 92(0x5C)
// ...
#define RF_FC_CHANNEL_168                       _T("0001-0000000068")   // Channel 104(0x68)


//
// Data Rate
//
#define RF_FC_DATA_RATE_0003E800                _T("0002-000003E800")   // Data Rate 250K (256000)
#define RF_FC_DATA_RATE_0007D000                _T("0002-000007D000")   // Data Rate 500K (512000)
#define RF_FC_DATA_RATE_00100000                _T("0002-0000100000")   // Data Rate 1024K (1048576)


//
// Factory Command
//
#define RF_FC_0003_COMMAND1                     _T("0003-0000000001")   // Execute command instruction
//...
#define RF_FC_XXXX_COMMAND1                     _T("XXXX-0000000001")   // Execute command instruction


//
// Scanner Packet
//
//      SN  : Sequence Number
//      PID : Product ID(Product/Commodity/Goods)
//          : Ex. 6889420075, 4710104022407, etc
//      CID : Command ID
//          : Ex. 0000-0000000001
//      TID : Device ID(Tag ID)
//          : Ex. 2901-8280000002
//      CMD : Command (RF_AL_FACTORY_XXXX)
//          : Ex. RF_AL_FACTORY_PAIRING, RF_AL_FACTORY_UNPAIRING
//
// Packet: PAIRING/UNPAIRING
//
// 0         1         2         3
// 01234567890123456789012345678901
// +------+-------+-------+-------+
// + SN   | TYPE  |  CMD  |PID[0] |
// +------+-------+-------+-------+
// +         PID/CID[1-4]         |
// +------+-------+-------+-------+
// |             ...              |
// +------+-------+-------+-------+
// +    PID/CID[13-15]    |TID[0] |
// +------+-------+-------+-------+
// |             ...              |
// +------+-------+-------+-------+
// +      TID[13-15]      |       |
// +------+-------+-------+-------+
//
//
//
// Packet: COMMAND
//
// 0         1         2         3
// 01234567890123456789012345678901
// +------+-------+-------+-------+
// + SN   | TYPE  |  CMD  |CID[0] |
// +------+-------+-------+-------+
// +           CID[1-4]           |
// +------+-------+-------+-------+
// |             ...              |
// +------+-------+-------+-------+
// +      CID[13-15]      |       |
// +------+-------+-------+-------+
//


//==============================================================================
//--- Declarative Structure ---
//==============================================================================
#pragma pack(1)

typedef struct _ESL_COMMON_HEADER 
{
    //--- Common Header ---
    uint8_t bySequenceNumber;                   // Sequence Number
    uint8_t byPacketType;                       // Packet Type: RF_PT_SCANNER_PACKET
    uint8_t byCommand;                          // Command: RF_CMD_SCANNER_PAIRING, RF_CMD_SCANNER_UNPAIRING, RF_CMD_CONFIRMED_ACK, etc...
    //--- Externsion Data ---
    uint8_t ExtensionData[1];                   // Extension Data
} ESL_COMMON_HEADER, *PESL_COMMON_HEADER;

typedef struct 
{
    uint8_t      BeaconSeq;
    uint8_t      BeaconType;
    uint8_t      Action;
    uint8_t      ChnCtrl;
    uint8_t      WakeUpTime;
    uint8_t      GatewayID[2];
    uint8_t      RepeaterID[2];
} SYNC_BEACON, *P_SYNC_BEACON, *PSYNC_BEACON;

typedef struct _ESL_EXTENSION_PAIRING_PACKET 
{
    uint8_t nLength;                            // The size of the extension structure
    uint8_t nFormat;                            // Format
    uint8_t byProduct[MAX_BARCODE_PRODUCT];     // Commodity, goods
    uint8_t byDevice[MAX_BARCODE_FACTORY];      // Tag
} ESL_EXTENSION_PAIRING, *PESL_EXTENSION_PAIRING;

typedef struct _ESL_EXTENSION_FACTORY_PACKET 
{
    uint8_t nLength;                            // The size of the extension structure
    uint8_t nFormat;                            // Format
    uint8_t byCommand[MAX_BARCODE_FACTORY];     // Command ID
    uint8_t byReserved[1];                      // Reserved
} ESL_EXTENSION_FACTORY, *PESL_EXTENSION_FACTORY;

typedef struct _ESL_EXTENSION_BARCODE_PACKET 
{
    uint8_t nLength;                            // The size of the extension structure
    uint8_t nFormat;                            // Format
    uint8_t byProduct[MAX_BARCODE_PRODUCT];     // Commodity, goods barcode
} ESL_EXTENSION_BARCODE, *PESL_EXTENSION_BARCODE;


typedef struct _ESL_EXTENSION_STATUS_PACKET 
{
    uint8_t nLength;                            // The size of the extension structure
    uint8_t nFormat;                            // Format
    uint16_t uiStatus;
} ESL_EXTENSION_STATUS, *PESL_EXTENSION_STATUS;

typedef struct _ESL_EXTENSION_RSSI 
{
    uint8_t nLength;                            // The size of the extension structure
    uint8_t nFormat;                            // Format
    bool fEnableAGC;                            // Enable Automatic Gain Control(AGC)
    uint16_t uiRssiCode;                        // RSSI Code
    //int16_t nInputPower;                      // Input Power (dBm)
} ESL_EXTENSION_RSSI, *PESL_EXTENSION_RSSI;

typedef struct _ESL_EXTENSION_BINDING 
{
    uint8_t nLength;                            // The size of the extension structure
    uint8_t nFormat;                            // Format
    ESL_DEVICE_ID stDeviceID;                   // Host device ID
} ESL_EXTENSION_BINDING, *PESL_EXTENSION_BINDING;

typedef struct _ESL_EXTENSION_REGISTRATION 
{
    uint8_t nLength;                            // The size of the extension structure
    uint8_t nFormat;                            // Format
    //uint8_t byProduct[MAX_BARCODE_PRODUCT];   // Commodity, goods
    uint8_t byDevice[MAX_BARCODE_FACTORY];      // Device ID

} ESL_EXTENSION_REGISTRATION, *PESL_EXTENSION_REGISTRATION;


//Request & ACK
typedef struct _ESL_COMMON_PACKET
{
    //--- Common Header ---
    uint8_t bySequenceNumber;                   // Sequence Number
    uint8_t byPacketType;                       // Packet Type: RF_PT_SCANNER_PACKET
    uint8_t byCommand;                          // Command: RF_CMD_SCANNER_PAIRING, RF_CMD_SCANNER_UNPAIRING, RF_CMD_CONFIRMED_ACK, etc...
    uint8_t byDeviceID[2];                      // Scanner ID/Tag ID/etc...
    uint8_t byGatewayID[2];                     // Gateway ID
    uint8_t byRepeaterID[2];                    // Repeater ID
    //--- Externsion Data ---
    uint8_t ExtensionData[1];                   // Extension Data
} ESL_COMMON_PACKET, *PESL_COMMON_PACKET;


typedef struct _ESL_PAIRING_PACKET 
{
    //--- Common Header ---
    uint8_t bySequenceNumber;                   // Sequence Number
    uint8_t byPacketType;                       // Packet Type: RF_PT_SCANNER_PACKET
    uint8_t byCommand;                          // Command: RF_CMD_SCANNER_PAIRING, RF_CMD_SCANNER_UNPAIRING, RF_CMD_CONFIRMED_ACK, etc...
    uint8_t byDeviceID[2];                      // Scanner ID/Tag ID/etc...
    uint8_t byGatewayID[2];                     // Gateway ID
    uint8_t byRepeaterID[2];                    // Repeater ID
    //--- Externsion Data ---
    ESL_EXTENSION_PAIRING stPairing;            // Pairing Information
    ESL_EXTENSION_BINDING stBinding;            // Binding Information
    ESL_EXTENSION_RSSI stRssi;                  // RSSI Information
} ESL_PAIRING_PACKET, *PESL_PAIRING_PACKET;

typedef struct _ESL_REGISTRATION_PACKET 
{
    //--- Common Header ---
    uint8_t bySequenceNumber;                   // Sequence Number
    uint8_t byPacketType;                       // Packet Type: RF_PT_SCANNER_PACKET
    uint8_t byCommand;                          // Command: RF_CMD_SCANNER_PAIRING, RF_CMD_SCANNER_UNPAIRING, RF_CMD_CONFIRMED_ACK, etc...
    uint8_t byDeviceID[2];                      // Scanner ID/Tag ID/etc...
    uint8_t byGatewayID[2];                     // Gateway ID
    uint8_t byRepeaterID[2];                    // Repeater ID
    //--- Externsion Data ---
    ESL_EXTENSION_REGISTRATION stRegistration;  // Registration Information
} ESL_REGISTRATION_PACKET, *PESL_REGISTRATION_PACKET;


typedef struct _ESL_FACTORY_PACKET 
{
    //--- Common Header ---
    uint8_t bySequenceNumber;                   // Sequence Number
    uint8_t byPacketType;                       // Packet Type: RF_PT_SCANNER_PACKET
    uint8_t byCommand;                          // Command: RF_CMD_SCANNER_COMMAND, RF_CMD_CONFIRMED_ACK, etc...
    uint8_t byDeviceID[2];                      // Scanner ID/Tag ID/etc...
    uint8_t byGatewayID[2];                     // Gateway ID
    uint8_t byRepeaterID[2];                    // Repeater ID
    //--- Externsion Data ---
    ESL_EXTENSION_FACTORY stFactory;
} ESL_FACTORY_PACKET, *PESL_FACTORY_PACKET;

typedef struct _ESL_BARCODE_PACKET 
{
    //--- Common Header ---
    uint8_t bySequenceNumber;                   // Sequence Number
    uint8_t byPacketType;                       // Packet Type: RF_PT_SCANNER_PACKET
    uint8_t byCommand;                          // Command: RF_CMD_SCANNER_COMMAND, RF_CMD_CONFIRMED_ACK, etc...
    uint8_t byDeviceID[2];                      // Scanner ID/Tag ID/etc...
    uint8_t byGatewayID[2];                     // Gateway ID
    uint8_t byRepeaterID[2];                    // Repeater ID
    //--- Externsion Data ---
    ESL_EXTENSION_BARCODE stBarcode;
} ESL_BARCODE_PACKET, *PESL_BARCODE_PACKET;

typedef struct _ESL_REPLY_PACKET 
{
    //--- Common Header ---
    uint8_t bySequenceNumber;                   // Sequence Number
    uint8_t byPacketType;                       // Packet Type: RF_PT_SCANNER_PACKET
    uint8_t byCommand;                          // Command: RF_CMD_SCANNER_COMMAND, RF_CMD_CONFIRMED_ACK, etc...
    uint8_t byDeviceID[2];                      // Scanner ID/Tag ID/etc...
    uint8_t byGatewayID[2];                     // Gateway ID
    uint8_t byRepeaterID[2];                    // Repeater ID
    //--- Externsion Data ---
    ESL_EXTENSION_STATUS stStatus;
} ESL_REPLY_PACKET, *PESL_REPLY_PACKET;


typedef struct _ESL_RSSI_PACKET 
{
    //--- Common Header ---
    uint8_t bySequenceNumber;                   // Sequence Number
    uint8_t byPacketType;                       // Packet Type: RF_PT_SCANNER_PACKET
    uint8_t byCommand;                          // Command: RF_CMD_SCANNER_PAIRING, RF_CMD_SCANNER_UNPAIRING, RF_CMD_CONFIRMED_ACK, etc...
    uint8_t byDeviceID[2];                      // Scanner ID/Tag ID/etc...
    uint8_t byGatewayID[2];                     // Gateway ID
    uint8_t byRepeaterID[2];                    // Repeater ID
    //--- Externsion Data ---
    ESL_EXTENSION_RSSI stRssi;                  // RSSI Information
} ESL_RSSI_PACKET, *PESL_RSSI_PACKET;


#pragma pack()
//==============================================================================
//--- Declarative Functions ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative constructor ---
//------------------------------------------------------------------------------
void RfCommunication(void);

//------------------------------------------------------------------------------
//--- Declarative Public Functions ---
//------------------------------------------------------------------------------


#endif //_DAVIOCM_RF_COMMUNICATION_H_


