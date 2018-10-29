/**********************************************
 * RFpacket.h
 *
 **********************************************/
#ifndef _RF_PACKET_H_
#define _RF_PACKET_H_

#include "RFCommunication.h"

#define VENDOR                   	"Davicom Semiconductor"
#define JAMES                   	"James007"
#define BINDING_THRESHOLD                 10
#define TAG_PIX_NEVER_UPDATED             0x00
#define TAG_PIX_UPDATED             			0x01
#define TAG_PIX_FIRST_TIME_POWER_UP       0x02
uint8_t RxPacket(void);
uint8_t getpacket(uint32_t mSec);
uint8_t SearchBeacon(uint32_t , uint32_t);
uint8_t getdatapacket(void);
void replypacket(uint8_t result);
extern void ReplyUpdateResult(uint8_t result);
void replyTagStatus(uint8_t result);
void setSynTimer(uint32_t sec, uint8_t hitflag);
extern uint8_t GetTagWakeUpInterval(void);
uint8_t GetRfChannel(void);
uint8_t GetLongSleepAlarmHour(void);
uint8_t GetLongSleepAlarmMin(void);

uint8_t GetColor(void);
extern uint8_t GetRfDatarate(void);		
uint8_t GetPacketCount(void);
void replyOTAStatus(uint8_t , uint8_t ,uint8_t );
uint8_t GetOtaPacketIndex(void);

uint8_t GetTempBase(void);
uint16_t GetAdcBase(void);

extern uint16_t GetBinFileSize(void);
extern uint8_t GetTotalOfPacket(void);
extern void SetOtaStatusDone(void);
extern uint8_t GetEpdColor(void);		
extern uint8_t GetEpdSize(void);		
extern void NonStop_Searching_New_Beacon_Src(uint32_t, uint32_t );

#endif	// _RF_PACKET_H_
