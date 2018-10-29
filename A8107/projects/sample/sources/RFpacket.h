/**********************************************
 * RFpacket.h
 *
 **********************************************/
#ifndef _DAVICOM_RF_PACKET_H_
#define _DAVICOM_RF_PACKET_H_

#include "RFCommunication.h"

#define VENDOR                   	"Davicom Semiconductor"
uint8_t RxPacket(void);
uint8_t getpacket(uint32_t mSec);
void getdatapacket(void);
void replypacket(uint8_t result);
void replyTagStatus(uint8_t result);

#endif	// _DAVICOM_RF_PACKET_H_
