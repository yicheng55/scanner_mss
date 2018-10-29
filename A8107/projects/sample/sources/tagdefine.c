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


int PastHour = 0;
int CurrentHour = 0;
int CurrentYear = 0;
int CurrentDay = 0;

extern uint8_t TagID[5];		//RX timeout flag
extern uint8_t GatewayID_UQ[3];
extern uint8_t RepeaterID_UQ[3];
extern uint8_t AgentID[5];
extern uint8_t RepeaterID[5];
extern uint8_t updateflag;
extern uint16_t UpdateCount;
extern uint16_t RebootCount;		
extern uint16_t UpdateFailCount;	
extern uint8_t TagWakeUpInterval;
//	typedef void (*pFunction)(void);
/**
 *
 * @brief   SPI master mode transfer data and receive data throught SPI
 *
 * @param   *spi                    SPI Pointer
 * @param   *txBuf                  8bits buffer for send
 * @param   *rxBuf                  8bits buffer for receive
 * @param   length                  Length for send data
 * @return  none
 */
void SPI_Set_DataWidth(uint8_t u32DataWidth)
{
		SPI->CTRL = (SPI->CTRL & 0xF0FF) | ((u32DataWidth - 1) << SPI_CTL_TX_BIT_LEN_Pos);
		SPI->STATUS |= 0x01;
}

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
    while(POWER->BatteryItem.BDS);
    return POWER->BatteryItem.BDF;
}

uint8_t BatteryValue()
{
	uint32_t bvt;
	uint8_t Battery_Level = 0;
	uint8_t Battery_Value = 0;
	for(bvt = 0; bvt < 8; bvt++)
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

void DateAndHour(int Date, int Hour)
{
	PastHour = Hour;
	CurrentDay = Date;
}

void getCurrentDay(void)
{
	CurrentHour = ((RTC->COUNT & RTC_COUNT_HOUR_Msk)>>RTC_COUNT_HOUR_Pos);
	if(CurrentHour < PastHour)
	{
		CurrentDay += 1;
		PastHour = CurrentHour;
	}
	else
	{
		PastHour = CurrentHour;
	}
//	printf("CurrentHour = %d\r\n", CurrentHour);
//	printf("PastHour = %d\r\n", PastHour);
	
	if(CurrentDay > 365)
	{
		CurrentYear += 1;
		CurrentDay -= 366;
	}
}

void printTime(void)
{
	  getCurrentDay();
    printf("[%dd:%2dh:%2dm:%2ds.%02d0]\r\n",
            CurrentDay,
            (int)((RTC->COUNT & RTC_COUNT_HOUR_Msk)>>RTC_COUNT_HOUR_Pos),
            (int)((RTC->COUNT & RTC_COUNT_MIN_Msk)>>RTC_COUNT_MIN_Pos),
            (int)((RTC->COUNT & RTC_COUNT_SEC_Msk)>>RTC_COUNT_SEC_Pos),
            (int)((RTC->COUNT & RTC_COUNT_TMS_Msk)>>RTC_COUNT_TMS_Pos));
}

void ReadAndWriteRebootCount()
{
		uint8_t RebootCnt[2];
		RebootCount = (((HW8_REG(0x21000)) << 8 )| HW8_REG(0x21001)) + 1;
		if(RebootCount == 0x00)
		{
			RebootCount = 0x01;
		}
		Flash_PageErase(0x21000);
		RebootCnt[0] = RebootCount>>8;
		RebootCnt[1] = RebootCount&0xFF;
		Flash_Write_U8(0x21000, RebootCnt, 2);
}

void ReadAndWriteNonVolatileData(uint8_t Reboot, uint8_t UpdateFail)
{
		uint8_t NVDbuf[32] = {0xFF};
		uint16_t UpFail = 0;
		P_NON_VOLATILE_DATA pNVData = (P_NON_VOLATILE_DATA)NVDbuf;
		
		if(Reboot == 1)
		{
			RebootCount = (((HW8_REG(0x21000)) << 8 )| HW8_REG(0x21001))+1;
			if(RebootCount == 0x00)
			{
				RebootCount = 0x01;
			}
		}
		else
		{
			RebootCount = (((HW8_REG(0x21000)) << 8 )| HW8_REG(0x21001));
		}
		UpFail = (((HW8_REG(0x21004)) << 8 )| HW8_REG(0x21005));
		if(UpdateFail == 1)
		{
			UpFail = UpFail+1;
//			TagWakeUpInterval = 0x01;
		}
		pNVData->RebootCnt[0] = RebootCount>>8; 
		pNVData->RebootCnt[1] = RebootCount&0xFF;
		pNVData->PreviousSLT[0] = TagWakeUpInterval>>8;
		pNVData->PreviousSLT[1] = TagWakeUpInterval&0xFF;
		pNVData->UpdateFailCnt[0] = UpFail>>8;
		pNVData->UpdateFailCnt[1] = UpFail&0xFF;
		Flash_PageErase(0x21000);
		Flash_Write_U8(0x21000, NVDbuf, 32);
}

uint16_t ReadAndWritePreviousSleepTime()
{
		uint16_t SleepTime;
		SleepTime = (((HW8_REG(0x21002)) << 8 )| HW8_REG(0x21003));
		return SleepTime;

}

void ReadStatusFromFlash(void)
{
	int Count;
		for(Count = 0; Count<64; Count++ )
		{
			printf("FLASH_R_DATA8[%d]= 0x%X\r\n",Count, HW8_REG(0x20000+Count));
		}
}

void WriteStatusToFlash(uint8_t* Statusbuf)
{
	Flash_PageErase(0x20000);
	Flash_Write_U8(0x20000, Statusbuf, 64);
}

uint32_t GetStandbyTimeInSecond()
{
	uint32_t Second = 0;
	getCurrentDay();
	Second = (CurrentYear * YEAR_IN_SEC) + (CurrentDay * DAY_IN_SEC) + (GET_HOUR(RTC) * HOUR_IN_SEC)+ (GET_MINUTE(RTC) * MINUTE_IN_SEC)+ GET_SECOND(RTC);
	return htonl(Second);
}


void GetTagStatus(uint8_t* Statusbuf)//, uint8_t ChangeWakeUpTime)
{
	uint32_t RSSI;
	P_TAG_STATUS pTagStatus = (P_TAG_STATUS)Statusbuf;
	//TAG_ID
	memcpy(pTagStatus->TAG_ID, TagID, 5);
	//AGENT_ID
	pTagStatus->GATEWAY_ID_UNIQUE[0] = 0xFF;
	pTagStatus->GATEWAY_ID_UNIQUE[1] = 0xFF;
	pTagStatus->GATEWAY_ID_UNIQUE[2] = 0xFF;
//	memcpy(pTagStatus->GATEWAY_ID_UNIQUE, GatewayID_UQ, 3);//GatewayID_UQ
	memcpy(pTagStatus->GATEWAY_ID, AgentID, 2);
	//REEATER_ID
	pTagStatus->REPEATER_ID_UNIQUE[0] = 0xFF;
	pTagStatus->REPEATER_ID_UNIQUE[1] = 0xFF;
	pTagStatus->REPEATER_ID_UNIQUE[2] = 0xFE;
//	memcpy(pTagStatus->REPEATER_ID_UNIQUE, RepeaterID_UQ, 3);//GatewayID_UQ
	memcpy(pTagStatus->REPEATER_ID, RepeaterID, 2);
	//RF_CHANNEL
	pTagStatus->RF_CHANNEL = RF_CHN;
	//RSSI
	RSSI = RSSI_measurement();
//	RSSI = ~RSSI + 1;
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
	pTagStatus->BATTERY_LEVEL = 	BatteryValue();
	pTagStatus->WAKEUP_INTERVAL = TagWakeUpInterval;
	//STANDBY_TIME
	pTagStatus->SECOND = GetStandbyTimeInSecond();
	//UPDATEFAIL_COUNT
	pTagStatus->UPDATEFAIL_COUNT = htons(UpdateFailCount);
	//printf("%x ",pTagStatus->UPDATEFAIL_COUNT);
	WriteStatusToFlash(Statusbuf);
}

uint8_t TagIdCompare(const uint8_t* TagIDFromRF, const uint8_t* TagID, uint8_t NumOfPkt)
{
	uint8_t Count = 0;
	for(Count = 0; Count < 5 ; Count++)
	{
		if(TagIDFromRF[Count + (NumOfPkt*5)] != TagID[Count]) //01234  //56789 //
		{
			return 0;
		}
	}
	return 1;
}


//uint8_t TagIdCompare(uint8_t* TagIDFromRF, uint8_t* TagID)
//{
//	uint8_t Count = 0;
//	for(Count = 0; Count < 5 ; Count++)
//	{
//		if(TagIDFromRF[Count] != TagID[Count])
//		{
//			return 0;
//		}
//	}
//	return 1;
//}

/************************************************************************
** AES128_Encrypt
** Description:
** Parameters:		p_data=address start position for AES_DATA
**					p_encrypt_key=address start position for AES_KEY
**					p_dataresult=address start position for encrypted AES_data
** Return value: 	none
** Note:
************************************************************************/
void AES128_Encrypt(uint8_t *p_data, uint8_t *p_encrypt_key, uint8_t *p_dataresult)
{
    uint8_t i;

    for(i = 0; i < 16; i++)
        HW8_REG(0x5000C00C + 15 - i) = *(p_encrypt_key + i); //AES_KEY

    for(i = 0; i < 16; i++)
        HW8_REG(0x5000C01C + 15 - i) = *(p_data + i); //AES_DATA

    HW8_REG(0x5000C001) = 0x00;     //AES_ENC
    HW8_REG(0x5000C000) = 0x01;     //AES_GO

    while(HW8_REG(0x5000C000) & 0x01);

    for(i = 0; i < 16; i++)
        *(p_dataresult + i) = HW8_REG(0x5000C01C + 15 - i);
}
/************************************************************************
** AES128_Decrypt
** Description:
** Parameters:		p_data=address start position for AES_DATA
**					p_encrypt_key=address start position for AES_KEY
**					p_dataresult=address start position for decrypted AES_data
** Return value: 	none
** Note:
************************************************************************/
void AES128_Decrypt(uint8_t *p_data, uint8_t *p_decrypt_key, uint8_t *p_dataresult)
{
    uint8_t i;

    for(i = 0; i < 16; i++)
        HW8_REG(0x5000C00C + 15 - i) = *(p_decrypt_key + i); //AES_KEY

    for(i = 0; i < 16; i++)
        HW8_REG(0x5000C01C + 15 - i) = *(p_data + i); //AES_DATA

    HW8_REG(0x5000C001) = 0x01;//AES_DEC
    HW8_REG(0x5000C000) = 0x01;//AES_GO

    while(HW8_REG(0x5000C000) & 0x01);

    for(i = 0; i < 16; i++)
        *(p_dataresult + i) = HW8_REG(0x5000C01C + 15 - i);
}
