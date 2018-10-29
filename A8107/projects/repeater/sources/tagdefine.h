/****************************************************************************//**
 * @file     tagdefine.h
 * @version  V1.00
 * $Revision: 0 $
 * $Date: 17/09/222 10:00 $
 * @brief    TAG define eader file
 *
 * @note
 * Copyright (C) 2017 Corp. All rights reserved.
*****************************************************************************/
#ifndef __TAGDEFINE_H__
#define __TAGDEFINE_H__
#include "ProjectConfig.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if 0
#define  RF_CHN                                 184
#define  EPD_RETRY_LIMIT                        5

#define  VERSION_MAJOR                          0x1
#define  VERSION_MINOR                          0x2
#define  VERSION_RELEASE                        0x0
#define  VERSION_BUILD                          0x1


//0~7. 0=1.875V, 1=1.95V, 2=2.025V, 3=2.1V, 4=2.175V, 5=2.25V, 6=2.325V, 7=2.4V
#define BVT_1_875                               0
#define BVT_1_95                                1
#define BVT_2_025                               2
#define BVT_2_1                                 3
#define BVT_2_175                               4
#define BVT_2_25                                5
#define BVT_2_325                               6
#define BVT_2_4                                 7

#endif //0

#define HW8_REG(ADDRESS)                        (*((volatile uint8_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)                       (*((volatile uint16_t *)(ADDRESS)))
#define HW32_REG(ADDRESS)                       (*((volatile uint32_t *)(ADDRESS)))

#define TagStatusFlashPosition                  0x20000
#define RebootCountFlashPosition                0x21000
#define PreviousSleepTime                       0x21000 + 3

typedef struct {
    uint8_t      HIGH_BYTE;
    uint8_t      LOW_BYTE;
} TWO_BYTE_DATA, *P_TWO_BYTE_DATA;
//typedef TWO_BYTE_DATA*  P_TWO_BYTE_DATA;

typedef struct {
    uint8_t      TAG_ID[5];
    uint8_t      GATEWAY_ID_UNIQUE[3];
    uint8_t      GATEWAY_ID[2];
    uint8_t      REPEATER_ID_UNIQUE[3];
    uint8_t      REPEATER_ID[2];
    uint8_t      RF_CHANNEL;
//  TWO_BYTE_DATA      RSSI;
    uint8_t      RSSI;
//  uint8_t      RSSI[2];
    uint8_t      VER_MAJOR;
    uint8_t      VER_MINOR;
    uint8_t      VER_RELEASE;
    uint8_t      VER_BUILD;
    uint16_t     UPDATE_COUNT;
    uint16_t     REBOOT_COUNT;
    uint8_t      BATTERY_LEVEL;
    uint8_t      WAKEUP_INTERVAL;
    uint32_t     SECOND;
    uint16_t     UPDATEFAIL_COUNT;
    uint8_t      UNDEFINED[30];
//  TWO_BYTE_DATA      DAY;
//  uint8_t      HOUR;
//  uint8_t      MINUTE;
//  uint8_t      SECOND_2;

} TAG_STATUS, *P_TAG_STATUS;
//typedef TAG_STATUS*  P_TAG_STATUS;

typedef struct {
    uint8_t      BeaconSeq;
    uint8_t      BeaconType;
    uint8_t      PktCtrl;
    uint8_t      ChnCtrl;
    uint8_t      WakeUpTime;
    uint8_t      Reserved;
    uint8_t      AgentID[5];
    uint8_t      TAGID[50];
} TAG_RCV_BEACON, *P_TAG_RCV_BEACON;

typedef struct {
    uint8_t Seqno;
    uint8_t PacketType;
    uint8_t TagID[5];
    uint8_t GetwayID[5];
    uint16_t PictureSize;
} TAG_RCV_HEADER, *P_TAG_RCV_HEADER;

typedef struct {

    uint8_t RebootCnt[2];
    uint8_t PreviousSLT[2];
    uint8_t UpdateFailCnt[2];
    uint8_t Reserved[32 - 6];

} NON_VOLATILE_DATA, *P_NON_VOLATILE_DATA;

#if 0
// Move to EslConfig.h
typedef struct {
    uint8_t      BeaconSeq;
    uint8_t      BeaconType;
    uint8_t      Action;
    uint8_t      ChnCtrl;
    uint8_t      WakeUpTime;
    uint8_t      GatewayID[2];
} SYNC_BEACON, *P_SYNC_BEACON;
#endif //0

typedef struct {
    uint8_t      Seq;
    uint8_t      PacketType;
    uint8_t      PktCtrl;
    uint8_t      WakeUpTime;
    uint8_t      GatewayID[2];
    uint8_t      RepeaterID[2];
    uint8_t      TAGID[56];
} TAG_UPDATE_LIST, *P_TAG_UPDATE_LIST;

typedef struct {
    uint8_t Seqno;
    uint8_t PacketType;
    uint8_t TagID[5];
    uint8_t GetwayID[2];
    uint8_t RepeaterID[2];
    uint8_t Compress;
    uint16_t PictureSize;
} HEADER_PACKET, *P_HEADER_PACKET;


//const uint8_t code AES_KEY[] =
//{
//      0x4C, 0x68, 0x38, 0x41,
//      0x39, 0xF5, 0x74, 0xD8,
//      0x36, 0xBC, 0xF3, 0x4E,
//      0x9D, 0xFB, 0x01, 0xBF,
//};

//void SPI_Set_DataWidth(uint8_t u32DataWidth);
uint32_t BatteryDetect(uint32_t bvt);
void WriteStatusToFlash(uint8_t *Statusbuf);
void GetTagStatus(uint8_t *Statusbuf);
void ReadAndWriteRebootCount(void);
uint16_t ReadAndWritePreviousSleepTime(void);
void ReadAndWriteNonVolatileData(uint8_t, uint8_t);
uint16_t htons(uint16_t val);
uint32_t htonl(uint32_t x);
uint32_t GetStandbyTimeInSecond(void);

#endif /* __TAGDEFINE_H__ */
