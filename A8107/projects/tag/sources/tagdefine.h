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

enum EPD_Size{Size_213, Size_290, Size_430, Size_750};
enum EPD_Color{B_W, R_B_W, Y_B_W};
enum OTA_Attribution{For_RP = 1, For_SCA, For_TAG};
//enum First_Arrived{RP_First,GS_First}
/* Customize TAG feature Switch*/
#define LED_Mode	  0

/* Tag Message */
#define TAG_MSG_PACKET_LOST							0x01
#define TAG_MSG_RX_TIMEOUT							0x02
#define TAG_MSG_RX_COMPLETED						0x03
#define TAG_MSG_HIT_ID								0x04
#define TAG_MSG_NO_HIT_ID							0x05
#define TAG_MSG_HOPPING								0x06
#define TAG_MSG_REPORT_STATUS_OK					0x07
#define TAG_MSG_STATE_NOT_RIGHT						0x08
#define TAG_MSG_TO_RCV_TAG_LIST						0x09
#define TAG_MSG_ONLY_SYNC							0x0A
#define TAG_MSG_SINGAL_PACKET_OK							0x0B
#define TAG_MSG_RCV_UNBOUND_MSG_OK							0x0C
#define TAG_MSG_CHANGE_BEACON_SRC_OK					0x0D
#define TAG_MSG_GATEWAY_ID_NOT_MATCH						0x0E
#define TAG_MSG_REPEATER_ID_NOT_MATCH					0x0F
#define TAG_MSG_AFTER_CHANGE_BEACON_SRC					0x10
#define TAG_MSG_OTA_HIT_ID								       0x11
#define TAG_MSG_OTA_PACKET_REPEATED								0x12
#define TAG_MSG_OTA_RCV_DONE								0x13
#define TAG_MSG_OTA_PACKET_LOST								0x14

#ifdef __cplusplus
extern "C"
{
#endif
	
#include "stdint.h"
void SPI_Set_DataWidth(uint8_t u32DataWidth);
uint32_t BatteryDetect(uint32_t bvt);
void WriteStatusToFlash(uint8_t* Statusbuf);
void GetTagStatus(uint8_t* Statusbuf);	
void GetTagFirmwareVersion(uint8_t* Statusbuf);
void ReadAndWriteRebootCount(void);
uint16_t ReadAndWritePreviousSleepTime(void);
void ReadAndWriteNonVolatileData(uint8_t, uint8_t);
void AES128_Decrypt(uint8_t *p_data,const   uint8_t *p_decrypt_key, uint8_t *p_dataresult);
extern void EPDShowTAGDftStatus(uint8_t);
void SetAlarm(uint32_t, uint32_t);
uint16_t htons(uint16_t val);
extern uint8_t IsTagIDLargerThanMax(uint8_t* , uint8_t* , uint8_t );
extern uint8_t IsNewTagIDLegal(uint8_t*);
extern void WriteOTADataToFlash(uint32_t , uint16_t , uint8_t* );
extern uint8_t Start_OTA_Header_Process(void);	
extern void EraseSectorForOtaData(uint8_t, uint32_t);
	
/**
  * @brief  Write datum to TX0 register.
  * @param[in]  spi is the base address of SPI module.
  * @param[in]  u32TxData is the datum which user attempt to transfer through SPI bus.
  * @return none
  * \hideinitializer
  */
	
/*-------------------- General Purpose Input Output (GPIO) -------------------*/	
#define GPIO_PIN_OUTPUT(gpio,pin) 	                (gpio->OUTENABLESET = (1<<pin))	 ; //OE = 1
#define GPIO_PIN_INPUT(gpio,pin) 	                	(gpio->OUTENABLECLR = (1<<pin))		

#define	 EPD_RETRY_LIMIT	      5

#define	 VERSION_MAJOR	      0x1
#define	 VERSION_MINOR	      0x3
#define	 VERSION_RELEASE	    0x0
#define	 VERSION_BUILD	      0x3

//0~7. 0=1.875V, 1=1.95V, 2=2.025V, 3=2.1V, 4=2.175V, 5=2.25V, 6=2.325V, 7=2.4V
#define BVT_1_875 	                0
#define BVT_1_95 	                1
#define BVT_2_025 	                2
#define BVT_2_1 	                3
#define BVT_2_175 	                4
#define BVT_2_25 	                5
#define BVT_2_325 	                6
#define BVT_2_4 	                7
//
#define GET_HOUR(rtc) 	    (int)((rtc->COUNT & RTC_COUNT_HOUR_Msk)>>RTC_COUNT_HOUR_Pos)
#define GET_MINUTE(rtc) 	  (int)((rtc->COUNT & RTC_COUNT_MIN_Msk)>>RTC_COUNT_MIN_Pos)             
#define GET_SECOND(rtc)	    (int)((rtc->COUNT & RTC_COUNT_SEC_Msk)>>RTC_COUNT_SEC_Pos)              
#define HTONS(n) (uint16_t)((((uint16_t) (n)) << 8) | (((uint16_t) (n)) >> 8))
#define HTONL(n) (uint32_t)((n[0] << 24) | (n[1] << 16) | (n[2] << 8) | (n[3]));

#define YEAR_IN_SEC		31556926
#define DAY_IN_SEC		86400
#define HOUR_IN_SEC		3600 
#define MINUTE_IN_SEC		60

#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t *)(ADDRESS)))
#define HW32_REG(ADDRESS)  (*((volatile uint32_t *)(ADDRESS)))

#define TagStatusFlashPosition      0x20000
#define RebootCountFlashPosition    0x21000
#define PreviousSleepTime						0x21000+3

#define Page_Size 128
#define Tag_OTA_File_StartPosition      0x20000
//extern  uint8_t RF_CHN[];
//extern const uint8_t RF_CHN[];
uint8_t GetChipRfChannel(void);
uint8_t NeedChangeRfChannel(void);

typedef struct {
	uint8_t      HIGH_BYTE; 
	uint8_t      LOW_BYTE;  
} TWO_BYTE_DATA, *P_TWO_BYTE_DATA;
//typedef TWO_BYTE_DATA*  P_TWO_BYTE_DATA;

typedef struct {
	uint8_t      TAG_ID[5]; 
	uint8_t      GATEWAY_ID[2]; 
	uint8_t      REPEATER_ID[2]; 
	uint8_t      RF_CHANNEL; 
	uint8_t 		 RSSI;
	uint8_t      BATTERY_LEVEL; 
	uint8_t      WAKEUP_INTERVAL; 
	int16_t      TEMPERATURE; 
} TAG_STATUS, *P_TAG_STATUS;
//typedef TAG_STATUS*  P_TAG_STATUS;

typedef struct {
	uint8_t      VER_MAJOR; 
	uint8_t      VER_MINOR; 
	uint8_t      VER_RELEASE;
	uint8_t      VER_BUILD; 

} TAG_FIRMWARE_VER, *P_TAG_FIRMWARE_VER;

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

typedef struct{
	uint8_t Seqno;
	uint8_t PacketType;
	uint8_t TagID[5];
	uint8_t GetwayID[5];
	uint16_t PictureSize;
}TAG_RCV_HEADER, *P_TAG_RCV_HEADER;

typedef struct{
	uint8_t Seqno;
	uint8_t PacketType;
	uint8_t TagID[62];
}TAG_OTA_DATA, *P_TAG_OTA_DATA;

typedef struct{

	uint8_t RebootCnt[2];
	uint8_t PreviousSLT[2];
	uint8_t UpdateFailCnt[2];
	uint8_t Reserved[32-6];

}NON_VOLATILE_DATA, *P_NON_VOLATILE_DATA;

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
#endif

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
	uint8_t      Seq; 
	uint8_t      PacketType;  //0x21
//	uint8_t      PktCtrl; 
//	uint8_t      WakeUpTime; 
	uint8_t      GatewayID[2];
	uint8_t      RepeaterID[2];
	uint8_t      ScannerID[2];
	uint8_t      TAGID[5];
	uint8_t      Attribution;         //For Which Device
	uint16_t     BinFileSize;				//32k = 32768 = 0x8000
	uint8_t      TotalOfPacket;     //How many 4736B File.
	uint8_t      BinFileVersionMajor;
	uint8_t      BinFileVersionMinor;

} TAG_OTA_LIST, *P_TAG_OTA_LIST;

typedef struct{
	uint8_t Seqno;
	uint8_t PacketType;
	uint8_t TagID[5];
	uint8_t GetwayID[2];
	uint8_t RepeaterID[2];
	uint8_t Compress;
	uint16_t PictureSize; //4736 or less
	uint8_t Color; //Option
} HEADER_PACKET, *P_HEADER_PACKET;

typedef struct {
	uint8_t      Seq; 
	uint8_t      PacketType;  
	uint8_t      TAG_ID[5]; 
	uint8_t			 MagicID[2];
	uint8_t      TAG_NEW_ID[5]; 
	uint8_t      DataRate; 
	uint8_t      RF_CHANNEL; 
	uint8_t      WAKEUP_INTERVAL; 
	uint8_t      Long_Sleep_Alarm_Hour; 	
	uint8_t      Long_Sleep_Alarm_Min; 
	uint8_t      REPEATER_ID[5]; 
	uint8_t      Max_TAG_ID[5]; 
	uint8_t			 NVM_version[2];
	uint8_t      Customer_Code[2];
	uint8_t      EPD_Size;
	uint8_t      EPD_Color;
	uint8_t      TempBase;
	uint8_t 		 CheckSum;
	
} TAG_NVM_DATA, *P_TAG_NVM_DATA;

typedef struct {
	uint8_t      Seq; 
	uint8_t      PacketType;  
	uint8_t      PacketIndex; 
	uint8_t      Result; 
} TAG_OTA_REPLY, *P_TAG_OTA_REPLY;


//typedef struct {
//	uint8_t      Seq; 
//	uint8_t      PacketType;  
////	uint8_t      PktCtrl;
//	uint8_t      TAG_ID[5]; 
//	uint8_t      TAG_NEW_ID[5]; 
//	uint8_t      RF_CHANNEL; 
//	uint8_t      WAKEUP_INTERVAL; 
//	uint8_t      Long_Sleep_Alarm_Hour; 	
//	uint8_t      Long_Sleep_Alarm_Min; 
//	uint8_t      REPEATER_ID[2]; 
//} TAG_NVM_DATA, *P_TAG_NVM_DATA;

//const uint8_t code AES_KEY[] =
//{
//  	0x4C, 0x68, 0x38, 0x41,
// 	 	0x39, 0xF5, 0x74, 0xD8,
//  	0x36, 0xBC, 0xF3, 0x4E,
//  	0x9D, 0xFB, 0x01, 0xBF,
//};


#endif
