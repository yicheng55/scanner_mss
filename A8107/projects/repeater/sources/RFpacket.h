/**********************************************
 * RFpacket.h
 *
 **********************************************/
#ifndef _RF_PACKET_H_
#define _RF_PACKET_H_

#include <stdint.h>
#include "RptComm.h"

#pragma pack(1)
/*
typedef struct{
    uint8_t SeqNo;
    uint8_t PktType;
    uint8_t UpdateCNT;
    uint8_t ChanCtrl;
    uint8_t WakeTime;
    uint8_t Reserved;
    uint8_t GetwayID[5];
    uint8_t TagID[52];
}RPT_RECEIVE_BEACON, *P_RPT_RECEIVE_BEACON;

typedef struct{
    uint8_t Seqno;
    uint8_t PacketType;
    uint8_t TagID[5];
    uint8_t GetwayID[5];
    uint16_t PictureSize;
}AGENTSRV_SENDTO_TAGHEAD, *P_AGENTSRV_SENDTO_TAGHEAD;

typedef struct{
    uint8_t SeqNum;
    uint8_t PckType;    //#define RF_PT_REPEATER_STATUS             0x31
    uint8_t Result;     //Receive Update Tag Data Result
    uint8_t Reserved;

    uint8_t GatewayID[2];
    uint8_t RepeaterID[2];
    uint8_t RptVerMajor;
    uint8_t RptVerMinor;
    uint8_t RptVerRelease;
    uint8_t RptVerBuild;
    uint16_t RptRSSI;

    uint8_t RptCHN;
    uint8_t TagCHN;
    uint32_t RptStandbyTime;
    uint16_t RptRsyncCNT;
    uint8_t TagID[5];
}RPT_STATUS_REPORT, *P_RPT_STATUS_REPORT;

*/

//*********************************************************
//********** For transparent update Tag picture ***********
//*********************************************************
typedef struct {
    uint8_t     Seqno;
    uint8_t     PacketType;
    uint8_t     TagID[5];
    uint8_t     GetwayID[2];
    uint8_t     RepeaterID[2];
    uint8_t     Compress;
    uint16_t    PictureSize;    //for Update Tag = picture size, for OTA = .bin total size
    uint8_t     PacketIndex;    //calculation .bin size index of OTA (size / 4736)
} RPT_UPD_TAG_HEAD, *P_RPT_UPD_TAG_HEAD;

typedef struct {
    uint8_t Seqno;
    uint8_t PacketType;
    uint8_t toTagData[62];
} RPT_UPD_TAG_DATA, *P_RPT_UPD_TAG_DATA;

//*********************************************************

typedef struct {
    uint8_t Seqno;
    uint8_t PacketType;
    union {
        uint8_t Reserved;
    };
} RPT_RECV_PKT_TYPE, *P_RPT_RECV_PKT_TYPE;

typedef struct {
    uint8_t      Seq;
    uint8_t      PacketType;
    uint8_t      UpdTagCnt;//PktCtrl;
    uint8_t      WakeUpTime;
    uint8_t      GatewayID[2];
    uint8_t      RepeaterID[2];
    uint8_t      TAGID[56];
} TAG_UPDATE_LIST, *P_TAG_UPDATE_LIST;

typedef struct {
    uint8_t Result;
    uint8_t TAG_ID[5];
    uint8_t GATEWAY_ID[2];
    uint8_t REPEATER_ID[2];
    uint8_t RF_CHANNEL;
    uint8_t RSSI;
    uint8_t BATTERY_LEVEL;
    uint8_t WAKEUP_INTERVAL;
    //uint8_t Reserved;
    uint16_t TEMPERATURE;
} RPT_COMB_TAG_STATUS, *P_RPT_COMB_TAG_STATUS;

typedef struct {
    uint8_t SeqNum;
    uint8_t PckType; // #define RF_PT_REPEATER_COMBTAG_RS   0x32
    uint8_t Result;  //Receive Update Tag Data Result
    uint8_t MultiTag;
    uint16_t ResultLen; //one Tag result lenght
    //uint8_t Reserved[57];
    //RPT_COMB_TAG_STATUS RxTagResult[4];
} RPT_COMB_TAG_UPDRS, *P_RPT_COMB_TAG_UPDRS;

typedef struct {
    uint8_t SeqNum;
    uint8_t PckType;    //#define RF_PT_REPEATER_STATUS             0x31
    uint8_t Result;     //Receive Update Tag Data Result

    uint8_t GatewayID[2];
    uint8_t RepeaterID[2];
    uint8_t GwCHN;
    uint8_t TagCHN;
    uint8_t RptDataRate;
    uint8_t RptM8107RSSI;
    uint8_t RptA7127RSSI;
    uint8_t RptMode;
    uint8_t TagID[5];
//}RPT_ATTR_LIST_INFO, *P_RPT_ATTR_LIST_INFO;
} RPT_STATUS_REPORT, *P_RPT_STATUS_REPORT;

typedef struct {
    uint8_t SeqNum;
    uint8_t PktType;
    uint8_t Result;

    union {
        uint8_t Reserved;
    };
} TAG_STATUS_HEAD, *P_TAG_STATUS_HEAD;

typedef struct {
    uint8_t      TAG_ID[5];
    uint8_t      GATEWAY_ID[2];
    uint8_t      REPEATER_ID[2];
    uint8_t      RF_CHANNEL;
    uint8_t          RSSI;
    uint8_t      BATTERY_LEVEL;
    uint8_t      WAKEUP_INTERVAL;
    uint16_t     TEMPERATURE;
} TAG_STATUS, *P_TAG_STATUS;

//*********************************************************
//************* Scanner Packet Head ***********************
//*********************************************************
typedef struct _SCAN_PACKET_HEAD {
    //--- Common Header ---
    uint8_t bySequenceNumber;       // Sequence Number
    uint8_t byPacketType;               // Packet Type: RF_PT_SCANNER_PACKET
    uint8_t byCommand;                  // Command: RF_CMD_SCANNER_PAIRING, RF_CMD_SCANNER_UNPAIRING, RF_CMD_CONFIRMED_ACK, etc...
    uint8_t byDeviceID[2];          // Scanner ID/Tag ID/etc...
    uint8_t byGatewayID[2];         // Gateway ID
    uint8_t byRepeaterID[2];        // Repeater ID
    //uint8_t Reserverd[55];
} SCAN_PACKET_HEAD, *P_SCAN_PACKET_HEAD;
//*********************************************************

//*********************************************************
//************* For Over The Air **************************
//*********************************************************
typedef struct {
    uint8_t      Seq;
    uint8_t      PacketType;                //RF_PT_OTA_UPDATE_LIST
    uint8_t      GatewayID[2];
    uint8_t      RepeaterID[2];
    uint8_t      ScannerID[2];
    uint8_t      TAGID[5];
    uint8_t      Attribution;               //For which Device: Repeater: 0x01, Scanner: 0x02, TAG: 0x03
    uint16_t     BinFileTotalSize;          //32k = 32768 = 0x8000
    uint8_t      TotalOfPacket;             //How many 4736B File.
    uint8_t      BinFileVersionMajor;
    uint8_t      BinFileVersionMinor;
} DEVICE_OTA_TAG_LIST, *P_DEVICE_OTA_TAG_LIST;

typedef struct {
    uint8_t     Seqno;
    uint8_t     PacketType;
    uint8_t     TagID[5];
    uint8_t     GetwayID[2];
    uint8_t     RepeaterID[2];
    uint8_t     Compress;
    uint16_t    SeqmentSize;
    uint8_t     SegmentIndex;
} DEVICE_OTA_HEAD, *P_DEVICE_OTA_HEAD;

typedef struct {
    uint8_t     Seq;
    uint8_t   PacketType;
    uint8_t   SegmentIndex; //(ref head packet format segmentindex)
    uint8_t   Result;
} DEVICE_OTA_REPLY_RESULT, *P_DEVICE_OTA_REPLY_RESULT;

//*********************************************************
/*
typedef struct _ESL_EXTENSION_BARCODE_PACKET {
    uint8_t nLength;                             // The size of the extension structure
    uint8_t nFormat;                             //Format:0x06 (RF_FMT_EXTENSION_BAROCDE 0x30)
    uint8_t byProduct[16];      // Commodity, goods
} ESL_EXTENSION_BARCODE, *PESL_EXTENSION_BARCODE;

typedef struct _ESL_BARCODE_PACKET {
    //--- Common Header ---
    uint8_t bySequenceNumber;                    // Sequence Number
    uint8_t byPacketType;                        // Packet Type: RF_PT_SCANNER_PACKET
    uint8_t byCommand;                           // Command: 0x86 (RF_CMD_SCANNER_BARCODE)
    uint8_t byDeviceID[2];                       // Scanner ID/Tag ID/etc...            
    uint8_t byGatewayID[2];                      // Gateway ID
    uint8_t byRepeaterID[2];                     // Repeater ID

    //--- Externsion Data --- 
    ESL_EXTENSION_BARCODE stBarcode;
} ESL_BARCODE_PACKET, *PESL_BARCODE_PACKET;
*/

#pragma pack()

/* Jerry add */
//extern uint8_t TagID[], GatewayID[];
//extern uint8_t ghopcount, gCHindex;;
//void Rpt_RxModeTask(uint8_t TaskState);
//void Rpt_RxModeTask(uint8_t *RxState, uint8_t *RptRxLen, uint16_t *RptRxTime, uint8_t *RxPktBuf);
uint8_t Rpt_HandleRecvPktType(P_RPT_RUNTIME_PARAMETER RunTimePara, uint8_t *recv);
P_RPT_RUNTIME_PARAMETER Rpt_RxModeTask(P_RPT_RUNTIME_PARAMETER RunTimePara, uint8_t *RxPktBuf);

#endif /* _RF_PACKET_H_ */
