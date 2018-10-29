/****************************************************************************//**
 * @file     tagdefine.c
 * @version  V1.00
 * $Revision: 0 $
 * $Date: 17/09/222 10:00 $
 * @brief    TAG define eader file
 *
 * @note
 * Copyright (C) 2017 Corp. All rights reserved.
*****************************************************************************/
#include "RfCommunication.h"
#include <stdio.h>
#include <string.h>
#include "A8107M.h"
#include "tagdefine.h"
#include <time.h>
#include "M8107.h"
#include "Flash.h"

uint8_t updateflag = 0;
uint16_t UpdateCount = 0;
uint16_t RebootCount = 0;
uint16_t UpdateFailCount = 0;
uint8_t TagWakeUpInterval;
//  typedef void (*pFunction)(void);

//extern uint8_t TagID[5];      //RX timeout flag
extern uint8_t GatewayID_UQ[3];
extern uint8_t RepeaterID_UQ[3];
extern uint8_t GatewayID[5];
//extern uint8_t RepeaterID[5];

/**
 *
 * @brief   Battery Detect.
 *
 * @param   bvt             0~7. 0=1.875V, 1=1.95V, 2=2.025V, 3=2.1V, 4=2.175V, 5=2.25V, 6=2.325V, 7=2.4V
 * @return  0 if V(REGI) < V(bvt).
 *          1 if V(regi) > V(bvt).
 */
uint32_t BatteryDetect(uint32_t bvt)
{
    POWER->BatteryItem.BVT = bvt;
    POWER->BatteryItem.BDS = 1;

    while (POWER->BatteryItem.BDS);

    return POWER->BatteryItem.BDF;
}

uint8_t BatteryValue()
{
    uint32_t bvt;
    uint8_t Battery_Level = 0;
    uint8_t Battery_Value = 0;

    for (bvt = 0; bvt < 8; bvt++)
    {
        Battery_Level += BatteryDetect(bvt);
    }

    Battery_Value = 12.5 * Battery_Level;

    return Battery_Value;
}

uint16_t htons(uint16_t val)
{
    return HTONS(val);
}

uint32_t htonl(uint32_t x)
{
    char *s = (char *)&x;
    return HTONL(s);
}

void ReadAndWriteRebootCount()
{
    uint8_t RebootCnt[2];

    RebootCount = (((HW8_REG(0x21000)) << 8 ) | HW8_REG(0x21001)) + 1;
    if (RebootCount == 0x00)
    {
        RebootCount = 0x01;
    }

    Flash_PageErase(0x21000);
    RebootCnt[0] = RebootCount >> 8;
    RebootCnt[1] = RebootCount & 0xFF;
    Flash_Write_U8(0x21000, RebootCnt, 2);
}

void ReadAndWriteNonVolatileData(uint8_t Reboot, uint8_t UpdateFail)
{
    uint8_t NVDbuf[32] = {0xFF};
    uint16_t UpFail = 0;
    P_NON_VOLATILE_DATA pNVData = (P_NON_VOLATILE_DATA)NVDbuf;

    if (Reboot == 1)
    {
        RebootCount = (((HW8_REG(0x21000)) << 8 ) | HW8_REG(0x21001)) + 1;
        if (RebootCount == 0x00)
        {
            RebootCount = 0x01;
        }
    }
    else
    {
        RebootCount = (((HW8_REG(0x21000)) << 8 ) | HW8_REG(0x21001));
    }
    UpFail = (((HW8_REG(0x21004)) << 8 ) | HW8_REG(0x21005));
    if (UpdateFail == 1)
    {
        UpFail = UpFail + 1;
//          TagWakeUpInterval = 0x01;
    }
    pNVData->RebootCnt[0] = RebootCount >> 8;
    pNVData->RebootCnt[1] = RebootCount & 0xFF;
    pNVData->PreviousSLT[0] = TagWakeUpInterval >> 8;
    pNVData->PreviousSLT[1] = TagWakeUpInterval & 0xFF;
    pNVData->UpdateFailCnt[0] = UpFail >> 8;
    pNVData->UpdateFailCnt[1] = UpFail & 0xFF;
    Flash_PageErase(0x21000);
    Flash_Write_U8(0x21000, NVDbuf, 32);
}

uint16_t ReadAndWritePreviousSleepTime()
{
    uint16_t SleepTime;
    SleepTime = (((HW8_REG(0x21002)) << 8 ) | HW8_REG(0x21003));

    return SleepTime;
}

void ReadStatusFromFlash(void)
{
    int Count;

    for (Count = 0; Count < 64; Count++ )
    {
        printf("FLASH_R_DATA8[%d]= 0x%X\r\n", Count, HW8_REG(0x20000 + Count));
    }
}

void WriteStatusToFlash(uint8_t *Statusbuf)
{
    Flash_PageErase(0x20000);
    Flash_Write_U8(0x20000, Statusbuf, 64);
}

#if 0
void GetTagStatus(uint8_t *Statusbuf)//, uint8_t ChangeWakeUpTime)
{
    uint32_t RSSI;
    P_TAG_STATUS pTagStatus = (P_TAG_STATUS)Statusbuf;
    //TAG_ID
    //memcpy(pTagStatus->TAG_ID, TagID, 5);
    //AGENT_ID
    pTagStatus->GATEWAY_ID_UNIQUE[0] = 0xFF;
    pTagStatus->GATEWAY_ID_UNIQUE[1] = 0xFF;
    pTagStatus->GATEWAY_ID_UNIQUE[2] = 0xFF;
    //  memcpy(pTagStatus->GATEWAY_ID_UNIQUE, GatewayID_UQ, 3);//GatewayID_UQ
    memcpy(pTagStatus->GATEWAY_ID, &GatewayID[3], 2);
    //REEATER_ID
    pTagStatus->REPEATER_ID_UNIQUE[0] = 0xFF;
    pTagStatus->REPEATER_ID_UNIQUE[1] = 0xFF;
    pTagStatus->REPEATER_ID_UNIQUE[2] = 0xFE;
    //  memcpy(pTagStatus->REPEATER_ID_UNIQUE, RepeaterID_UQ, 3);//GatewayID_UQ
    memcpy(pTagStatus->REPEATER_ID, RepeaterID, 2);
    //RF_CHANNEL
    //pTagStatus->RF_CHANNEL = RF_CHN;
    //RSSI
    RSSI = RSSI_measurement();
    //  RSSI = ~RSSI + 1;
    pTagStatus->RSSI = (uint8_t)RSSI;
    //FIRMWARE_VERSION
    pTagStatus->VER_MAJOR = VERSION_MAJOR;
    pTagStatus->VER_MINOR = VERSION_MINOR;
    pTagStatus->VER_RELEASE = VERSION_RELEASE;
    pTagStatus->VER_BUILD = VERSION_BUILD;
    //UPDATE_COUNT
    pTagStatus->UPDATE_COUNT = htons(UpdateCount);
    //REBOOT_COUNT
    pTagStatus->REBOOT_COUNT = htons(HW8_REG(0x21000) << 8 | HW8_REG(0x21001));
    //BATTERY_LEVEL
    pTagStatus->BATTERY_LEVEL =     BatteryValue();
    pTagStatus->WAKEUP_INTERVAL = TagWakeUpInterval;
    //STANDBY_TIME
    pTagStatus->SECOND = GetStandbyTimeInSecond();
    //UPDATEFAIL_COUNT
    pTagStatus->UPDATEFAIL_COUNT = htons(UpdateFailCount);
    //printf("%x ",pTagStatus->UPDATEFAIL_COUNT);
    WriteStatusToFlash(Statusbuf);
}
#endif //0

uint8_t TagIdCompare(uint8_t *TagIDFromRF, uint8_t *TagID, uint8_t NumOfPkt)
{
    uint8_t Count = 0;
    for (Count = 0; Count < 5 ; Count++)
    {
        if (TagIDFromRF[Count + (NumOfPkt * 5)] != TagID[Count]) //01234  //56789 //
        {
            return 0;
        }
    }
    return 1;
}

//uint8_t TagIdCompare(uint8_t* TagIDFromRF, uint8_t* TagID)
//{
//  uint8_t Count = 0;
//  for(Count = 0; Count < 5 ; Count++)
//  {
//      if(TagIDFromRF[Count] != TagID[Count])
//      {
//          return 0;
//      }
//  }
//  return 1;
//}
