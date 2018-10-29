/**********************************************
 * RFpacket.h
 *
 **********************************************/
#ifndef _RF_PACKET_H_
#define _RF_PACKET_H_

#include <stdint.h>

#define COMPARE_RSSI_VALUE			50

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
*/

typedef struct{
	uint8_t Seqno;
	uint8_t PacketType;
	uint8_t toTagData[62];
}RPT_UPD_TAG_DATA, *P_RPT_UPD_TAG_DATA;

typedef struct{
	uint8_t Seqno;
	uint8_t PacketType;
	uint8_t TagID[5];
	uint8_t GetwayID[2];
	uint8_t RepeaterID[2];
	uint8_t Compress;
	uint16_t PictureSize;
}RPT_UPD_TAG_HEAD, *P_RPT_UPD_TAG_HEAD;

typedef struct{
	uint8_t SeqNum;
	uint8_t PckType; 	//#define REPEATER_STATUS				0x31
	uint8_t Result; 	//Receive Update Tag Data Result
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

typedef struct{
	uint8_t SeqNum;
	uint8_t PckType; // #define RPT_COMBTAG_RS	0x32
	uint8_t Result;  //Receive Update Tag Data Result
	uint8_t MultiTag;
	uint16_t ResultLen; //one Tag result lenght
	uint8_t Reserved[57];
	//RPT_COMB_TAG_STATUS RxTagResult[4];
}RPT_COMB_TAG_UPDRS, *P_RPT_COMB_TAG_UPDRS;

#pragma pack()

/* Jerry add */
extern uint8_t TagID[], AgentID[];
extern uint8_t ghopcount, gCHindex;;
void Rpt_RxTask_Proc(uint8_t TaskState);

#endif /* _RF_PACKET_H_ */
