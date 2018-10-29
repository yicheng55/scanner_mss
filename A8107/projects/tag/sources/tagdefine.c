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
#include "EslDebug.h"
#include "RTC.h"
#include "RfPacket.h"

int PastHour = 0;
int CurrentHour = 0;
int CurrentYear = 0;
int CurrentDay = 0;

extern uint8_t TagID[5];		//RX timeout flag
extern uint8_t DefaultRepeaterID[5];		//RX timeout flag
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
	for(bvt = 4; bvt < 8; bvt++)
	{
		Battery_Level += BatteryDetect(bvt);
//		printf("Battery_Level is %d ", Battery_Level);
	}
//	Battery_Value = 12.5 * Battery_Level;
	Battery_Value = 25 * Battery_Level;
//	printf("Battery_Value is %d ", Battery_Value);	
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

void SetAlarm(uint32_t hour, uint32_t min)
{
	uint32_t SetHour, SetMin;
	
	SLPTIMER_StopTimer(SLPTIMER1);							
	RTC_Interrupt_Enable(1, 0, 0, 0, 0);
	CurrentHour = ((RTC->COUNT & RTC_COUNT_HOUR_Msk)>>RTC_COUNT_HOUR_Pos);
	
	SetMin =  ((min % 60) + ((RTC->COUNT & RTC_COUNT_MIN_Msk)>>RTC_COUNT_MIN_Pos)) % 60;
	SetHour = (CurrentHour + hour + (((min % 60) + ((RTC->COUNT & RTC_COUNT_MIN_Msk)>>RTC_COUNT_MIN_Pos)) / 60)) % 24;
	
	if(SetHour == 0 && SetMin == 0)
	{
		SetHour = 1;
	}
		
	RTC_SetAlarm(SetHour, SetMin);	
	printf("SetHour = %d\r\n", SetHour);
	printf("SetMin = %d\r\n", SetMin);
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
//		TagDebug(("printTime: [%dd:%2dh:%2dm:%2ds.%02d0]\r\n,CurrentDay,(int)((RTC->COUNT & RTC_COUNT_HOUR_Msk)>>RTC_COUNT_HOUR_Pos),(int)((RTC->COUNT & RTC_COUNT_MIN_Msk)>>RTC_COUNT_MIN_Pos),(int)((RTC->COUNT & RTC_COUNT_SEC_Msk)>>RTC_COUNT_SEC_Pos),(int)((RTC->COUNT & RTC_COUNT_TMS_Msk)>>RTC_COUNT_TMS_Pos)\r\n"));				
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
	Flash_PageErase(0x3EF00);
	Flash_Write_U8(0x3EF00, Statusbuf, 64);
}

void EraseSectorForOtaData(uint8_t PacketCount, uint32_t address)
{
	int Count = 0;
	for(Count = 0; Count < PacketCount; Count++)
	{
		Flash_SectorErase(address + (Count * 4096));
	}
}

void WriteOTADataToFlash(uint32_t address, uint16_t OTAFileSize, uint8_t* Statusbuf)
{
	int Count = 0;
//	int PacketCount = 0;
	for(Count = 0; Count < (OTAFileSize / 128); Count++)
	{
		Flash_Write_U8((address + Count*128), (Statusbuf + Count*128), 128);
//		printf("Flash_Write_U8 ..\r\n");
	}
//	Flash_Write_U8(0x20000, Statusbuf, 64);
}

uint32_t GetStandbyTimeInSecond()
{
	uint32_t Second = 0;
	getCurrentDay();
	Second = (CurrentYear * YEAR_IN_SEC) + (CurrentDay * DAY_IN_SEC) + (GET_HOUR(RTC) * HOUR_IN_SEC)+ (GET_MINUTE(RTC) * MINUTE_IN_SEC)+ GET_SECOND(RTC);
	return htonl(Second);
}

void GetTagFirmwareVersion(uint8_t* Statusbuf)
{
	P_TAG_FIRMWARE_VER pTagFirmVer = (P_TAG_FIRMWARE_VER)(Statusbuf + sizeof(TAG_STATUS));
	pTagFirmVer->VER_MAJOR = VERSION_MAJOR;
	pTagFirmVer->VER_MINOR = VERSION_MINOR;
	pTagFirmVer->VER_RELEASE = VERSION_RELEASE;
	pTagFirmVer->VER_BUILD = VERSION_BUILD;
	
}

void GetTagStatus(uint8_t* Statusbuf)//, uint8_t ChangeWakeUpTime)
{
	int16_t RSSI;
	P_TAG_STATUS pTagStatus = (P_TAG_STATUS)Statusbuf;
	//TAG_ID
	memcpy(pTagStatus->TAG_ID, TagID, 5);
	//AGENT_ID
	memcpy(pTagStatus->GATEWAY_ID, AgentID, 2);
	//REEATER_ID
	memcpy(pTagStatus->REPEATER_ID, RepeaterID, 2);
	//RF_CHANNEL
	pTagStatus->RF_CHANNEL = GetChipRfChannel();
	//RSSI
	RSSI = RSSI_measurement();
//	RSSI = ~RSSI + 1;
	pTagStatus->RSSI = (uint8_t)RSSI;

	//BATTERY_LEVEL
	pTagStatus->BATTERY_LEVEL = 	BatteryValue();
	pTagStatus->WAKEUP_INTERVAL = TagWakeUpInterval;
//	printf("%0d ",TagWakeUpInterval);
	
	pTagStatus->TEMPERATURE = (int16_t )ConvertTemperature_measurement();
//	pTagStatus->TEMPERATURE = (int16_t )Temperature_measurement();

//	GetTagFirmwareVersion(Statusbuf);
}

uint8_t TagIdCompare(uint8_t* TagIDFromRF, uint8_t* TagID, uint8_t NumOfPkt)
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

uint8_t IsNewTagIDLegal(uint8_t* NewTagID)
{
	uint8_t Remainder = 0;
	Remainder = (NewTagID[1] + NewTagID[2] +NewTagID[3] +NewTagID[4]) % 255;
	if(Remainder == NewTagID[0])
	{
		return true;
	}
	else
	{
		return false;
	}
}

uint8_t IsTagIDLargerThanMax(uint8_t* NewTagID, uint8_t* MaxTagID, uint8_t NumOfPkt)
{
//	uint8_t Count = 0;
	uint32_t MaxID = 0;
	uint32_t NewID = 0;
//	MaxID = (MaxTagID[0] << 24) + (MaxTagID[1] << 16) + (MaxTagID[2] << 8) + MaxTagID[3];
//	NewID = (NewTagID[0] << 24) + (NewTagID[1] << 16) + (NewTagID[2] << 8) + NewTagID[3];
	MaxID = (MaxTagID[1] << 16) + (MaxTagID[2] << 8) + MaxTagID[3];
	NewID = (NewTagID[1] << 16) + (NewTagID[2] << 8) + NewTagID[3];
	printf("MaxID is %x \r\n",MaxID);
	printf("NewID is %x \r\n",NewID);
	if(MaxID > NewID)
	{
		printf("MaxID > NEW \r\n");
		return true;
	}
	else if(MaxID == NewID)
	{
		printf("MaxID = NEW \r\n");
		if(MaxTagID[4] >= NewTagID[4])
		{
			printf("MaxID4 > NEW4 \r\n");
			return true;
		}
		else
		{
			return false;
		}
	}
	else //Max < New
	{
		return false;
	}
	
}

bool Is_Default_RepeaterID_Null(const uint8_t *array, unsigned int array_size)
{
		uint8_t i;
	
		if(array != NULL){
			for(i = 0; i < array_size; i++){
				if(array[i] != 0){
					return false;
				}
			}
		}
		return true;
}

void EPDShowTAGDftStatus(uint8_t Online)
{
	extern void EPaperShowTagDefaultStatus(const TCHAR *szTagID, const TCHAR *szTagStatus, const TCHAR *szTagMsg1, const TCHAR *szTagMsg2, const TCHAR *szTagMsg4);
//	extern uint8_t gDatarate;
	uint8_t DataRate = 0;
//	uint8_t Return_Value = 0;
	char strTagID[10];
	char RF_CHANNEL[1];
	char RF_DATARATE[1];
	
//	char GW[4];
	char RP[10];
	
	switch(GetRfDatarate())
	{
		case 0:
			DataRate = 1;
			break;
		case 1:
			DataRate = 2;
			break;
		case 2:
			DataRate = 3;
			break;
	}
	
	if(Is_Default_RepeaterID_Null(&DefaultRepeaterID[3], 2) == true)
	{
			sprintf(RP,"%02X%02X%s%02X%02X",AgentID[0],AgentID[1],"-",RepeaterID[0],RepeaterID[1]);
	}
	else
	{
			sprintf(RP,"%02X%02X%s%02X%02X",AgentID[0],AgentID[1],"-",DefaultRepeaterID[3],DefaultRepeaterID[4]);
	}
	sprintf(strTagID,"%02X%02X%02X%02X%02X",TagID[0],TagID[1],TagID[2],TagID[3],TagID[4]);		//
	sprintf(RF_CHANNEL,"%d",GetChipRfChannel());		//RF_DATARATE_500K
	sprintf(RF_DATARATE,"%d",DataRate);
	
	//sprintf(strTagID,"%03d:%03d:%03d:%03d:%03d",TagID[0],TagID[1],TagID[2],TagID[3],TagID[4]);	
	switch(Online)
	{
		case 0:
			EPaperShowTagDefaultStatus( _T(strTagID),_T("OFFLINE"), _T(RF_CHANNEL), _T(RF_DATARATE), _T(RP));		//TagID
			break;
		case 1:
			EPaperShowTagDefaultStatus( _T(strTagID),_T("ONLINE"), _T(RF_CHANNEL), _T(RF_DATARATE), _T(RP));		//TagID	
			break;
		case 2:
			EPaperShowTagDefaultStatus( _T(strTagID),_T("SLEEP"), _T(RF_CHANNEL), _T(RF_DATARATE), _T(RP));		//TagID	
			break;
		case 3:
			EPaperShowTagDefaultStatus( _T(strTagID),_T("LOST"), _T(RF_CHANNEL), _T(RF_DATARATE), _T(RP));		//TagID	
			break;		
	}
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

uint8_t Start_OTA_Header_Process()
{
  uint8_t mReturn;
	SetFIFOLvl(64);
//				printf("Start_OTA_Header_Process...\r\n");
	StrobeCmd(CMD_RX);
	mReturn = getpacket(20);

//	switch(mReturn)
//	{
//		case TAG_MSG_HIT_ID:
//			gState = DATA_STATE;
//			break;
//		case TAG_MSG_OTA_HIT_ID:
//			gState = OTA_STATE;
//			break;
//		case TAG_MSG_RX_TIMEOUT:
//			printf("Receive Header timer out.\r\n");
//			LossCnt++;
//			gState = ENTERPM1_STATE;
//			break;
//#if 0 	//Remarked by Spenser				
//		default:
//			gState = ENTERPM1_STATE;
//			break;
//#endif								
//	}
	return mReturn;

}

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
void AES128_Decrypt(uint8_t *p_data,const   uint8_t *p_decrypt_key, uint8_t *p_dataresult)
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
