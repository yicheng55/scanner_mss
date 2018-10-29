/***********************************************************************
 * RFpacket.c
 *
 ***********************************************************************/
#include "main.h"
#include "tagdefine.h"
#include "RfPacket.h"

////Start address 0x3EF00 (128 bytes) for Tag parameter.
//const uint8_t MagicID[2]  __attribute__( (at(0x3EF00) ) ) = {0x0A, 0x46};			//35000		//35000
//const uint8_t TagID[5]  __attribute__( (at(0x3EF00) ) ) = {0x84,0x80,0x00,0x00,0x04};	
//const uint8_t gDatarate[]  __attribute__( (at(0x3EF00) ) ) = {RF_DATARATE_500K};			//35000
//const uint8_t RF_CHN[]  __attribute__( (at(0x3EF00) ) ) ={184}; //184
//const uint8_t DefaultTagWakeUpInterval[]  __attribute__( (at(0x3EF00) ) ) ={40};
//const uint8_t LongSleepAlarmHour[]  __attribute__( (at(0x3EF00) ) ) ={0};
//const uint8_t LongSleepAlarmMin[]  __attribute__( (at(0x3EF00) ) ) ={5};
//const uint8_t DefaultRepeaterID[5]  __attribute__( (at(0x3EF00) ) ) = {0xFF,0xFF,0xFE,0x00,0x00};
//const uint8_t MaxTagID[5]  __attribute__( (at(0x3EF00) ) ) = {0x91,0x80,0x00,0x00,0x11};			//35000
//const uint8_t NVM_version[2]  __attribute__( (at(0x3EF00) ) ) = {0x00, 0x00};			//35000
//const uint8_t Customer_Code[2]  __attribute__( (at(0x3EF00) ) ) = {0x00, 0x00};			//35000
//const uint8_t EPD_Size[]  __attribute__( (at(0x3EF00) ) ) = {Size_290};			//35000
//const uint8_t EPD_Color[]  __attribute__( (at(0x3EF00) ) ) = {B_W};			//35000
//const uint8_t CheckSum[]  __attribute__( (at(0x3EF00) ) ) = {0x00};			//35000

//const uint8_t MagicID[2]  __attribute__( (at(0x3EF00) ) ) = {0x0A, 0x46};			//35000
//const uint8_t TagID[5]  __attribute__( (at(0x3EF00) ) ) = {0x90,0x80,0x00,0x00,0x10};			//35000
//const uint8_t gDatarate[]  __attribute__( (at(0x3EF00) ) ) = {RF_DATARATE_500K};			//35000
//const uint8_t RF_CHN[]  __attribute__( (at(0x3EF00) ) ) ={150};
//const uint8_t DefaultTagWakeUpInterval[]  __attribute__( (at(0x3EF00) ) ) ={40};
//const uint8_t LongSleepAlarmHour[]  __attribute__( (at(0x3EF00) ) ) ={0};
//const uint8_t LongSleepAlarmMin[]  __attribute__( (at(0x3EF00) ) ) ={5};
//const uint8_t DefaultRepeaterID[5]  __attribute__( (at(0x3EF00) ) ) = {0xFF,0xFF,0xFE,0x00,0x00};
//const uint8_t MaxTagID[5]  __attribute__( (at(0x3EF00) ) ) = {0x91,0x80,0x00,0x00,0x11};			//35000
//const uint8_t NVM_version[2]  __attribute__( (at(0x3EF00) ) ) = {0x00, 0x00};			//35000
//const uint8_t Customer_Code[2]  __attribute__( (at(0x3EF00) ) ) = {0x00, 0x00};			//35000
//const uint8_t EPD_Size[]  __attribute__( (at(0x3EF00) ) ) = {Size_290};			//35000
//const uint8_t EPD_Color[]  __attribute__( (at(0x3EF00) ) ) = {R_B_W};			//35000
//const uint8_t CheckSum[]  __attribute__( (at(0x3EF00) ) ) = {0x00};			//35000

//const uint8_t MagicID[2]  __attribute__( (at(0x3EF00) ) ) = {0x0A, 0x46};			//35000
//const uint8_t TagID[5]  __attribute__( (at(0x3EF00) ) ) = {0x91,0x80,0x00,0x00,0x11};			//35000
//const uint8_t gDatarate[]  __attribute__( (at(0x3EF00) ) ) = {RF_DATARATE_500K};			//35000
//const uint8_t RF_CHN[]  __attribute__( (at(0x3EF00) ) ) ={110};//184
//const uint8_t DefaultTagWakeUpInterval[]  __attribute__( (at(0x3EF00) ) ) ={40};
//const uint8_t LongSleepAlarmHour[]  __attribute__( (at(0x3EF00) ) ) ={0};
//const uint8_t LongSleepAlarmMin[]  __attribute__( (at(0x3EF00) ) ) ={5};
//const uint8_t DefaultRepeaterID[5]  __attribute__( (at(0x3EF00) ) ) = {0xFF,0xFF,0xFE,0x00,0x00};
//const uint8_t MaxTagID[5]  __attribute__( (at(0x3EF00) ) ) = {0x91,0x80,0x00,0x00,0x11};			//35000
//const uint8_t NVM_version[2]  __attribute__( (at(0x3EF00) ) ) = {0x00, 0x00};			//35000
//const uint8_t Customer_Code[2]  __attribute__( (at(0x3EF00) ) ) = {0x00, 0x00};			//35000
//const uint8_t EPD_Size[]  __attribute__( (at(0x3EF00) ) ) = {Size_213};			//35000
//const uint8_t EPD_Color[]  __attribute__( (at(0x3EF00) ) ) = {R_B_W};			//35000
//const uint8_t CheckSum[]  __attribute__( (at(0x3EF00) ) ) = {0x00};			//35000

const uint8_t MagicID[2]  __attribute__( (at(0x3EF00) ) ) = {0x00, 0x00};			//35000
const uint8_t TagID[5]  __attribute__( (at(0x3EF00) ) ) = {0x00,0x00,0x00,0x00,0x00};			//35000
const uint8_t gDatarate[]  __attribute__( (at(0x3EF00) ) ) = {RF_DATARATE_500K};			//35000
const uint8_t RF_CHN[]  __attribute__( (at(0x3EF00) ) ) ={130};
const uint8_t DefaultTagWakeUpInterval[]  __attribute__( (at(0x3EF00) ) ) ={40};
const uint8_t LongSleepAlarmHour[]  __attribute__( (at(0x3EF00) ) ) ={0};
const uint8_t LongSleepAlarmMin[]  __attribute__( (at(0x3EF00) ) ) ={5};
const uint8_t DefaultRepeaterID[5]  __attribute__( (at(0x3EF00) ) ) = {0xFF,0xFF,0xFE,0x00,0x00};
const uint8_t MaxTagID[5]  __attribute__( (at(0x3EF00) ) ) = {0x91,0x80,0x00,0x00,0x11};			//35000
const uint8_t NVM_version[2]  __attribute__( (at(0x3EF00) ) ) = {0x00, 0x00};			//35000
const uint8_t Customer_Code[2]  __attribute__( (at(0x3EF00) ) ) = {0x00, 0x00};			//35000
const uint8_t EPD_Size[]  __attribute__( (at(0x3EF00) ) ) = {Size_290};			//35000
const uint8_t EPD_Color[]  __attribute__( (at(0x3EF00) ) ) = {B_W};			//35000
const uint8_t TempBase[]  __attribute__( (at(0x3EF00) ) ) = {25};			//35000
const uint8_t CheckSum[]  __attribute__( (at(0x3EF00) ) ) = {0x00};			//35000
const uint16_t AdcBase[]  __attribute__( (at(0x3EF00) ) ) = {1376};			//35000


uint8_t GatewayID_UQ[3] = {0xFF, 0xFF, 0xFF};			//1999
uint8_t RepeaterID_UQ[3] = {0xFF, 0xFF, 0xFE};			//1999	
const uint8_t AES_KEY[] =
{
  	0x4C, 0x68, 0x38, 0x41,
 	  0x39, 0xF5, 0x74, 0xD8,
  	0x36, 0xBC, 0xF3, 0x4E,
  	0x9D, 0xFB, 0x01, 0xBF
};
uint8_t EncryptData[16];
uint8_t DecryptData[16];
uint8_t TagNvmData[64] = {0};
uint8_t AgentID[2] = {0};
uint8_t RepeaterID[2] = {0};
uint8_t Statusbuf[64];
uint8_t BeaconDataSnif[16];
uint8_t updateflag = 0;
uint8_t gState;
uint8_t gTagDonotSleep = 0;
uint16_t DataLen;
uint8_t AfterChangeBeaconSrc = 0;
//uint8_t EpdRFData[4736];
uint8_t *pEpdbuf;

uint8_t mindex;							//for packet lost check by sequence number
//uint8_t gRxPktResult = 0;		//packet lost flag
uint8_t gTxSeqNo = 0;				//TX result sequce number
uint16_t TimerCount = 0;
uint8_t ghopcount = 0;
uint8_t TAG_UPDATED = 0;
uint8_t TAG_RCV_HIT_ID = 0;
uint8_t TAG_RCV_NO_HIT_ID = 0;
uint8_t TAG_RCV_OTA = 0;
uint8_t gCHindex;
int16_t gRSSIvalue;			//current RSSI value
uint16_t UpdateCount = 0;				
uint16_t RebootCount = 0;	
uint16_t UpdateFailCount = 0;

extern uint8_t Oneshoutflag;		//RX timeout flag
extern uint32_t gBeaconwait;		//Receive Beacon timeout ms. 
//extern uint8_t gDatarate;
extern uint8_t EpdRFData[];
uint8_t TagWakeUpInterval = 1;
uint8_t AllWakeUpTime = 1;
uint8_t WakeUpTimeInList = 1;
uint8_t DefaultWakeUpTime = 1;
uint8_t m_uiScanChannelRetryCount = 0;
uint8_t ColorFromPacket = 0, PacketCount = 0;
uint8_t OTA_Data_index = 0;
uint8_t Set_All_Wake_time = 0;
//OTA Part//
uint8_t OTA_Attribution = 0;
uint16_t OTA_BinFileSize = 0;
uint8_t OTA_TotalOfPacket = 0;
uint8_t	OTA_Major = 0;
uint8_t OTA_Minor = 0;
uint8_t RP_First = 0;
uint8_t GS_First = 0;
//OTA Part//
/* Spenser added */
extern uint8_t gTagSeq;
extern uint8_t TagIdCompare(uint8_t* TagIDFromRF, const uint8_t* TagID, uint8_t NumOfPkt);

//------------------------------------------------------------------------------
//--- Bind / Unbound  --- mod from Jerry add
//------------------------------------------------------------------------------
uint8_t tGwID[2] = {0};
uint8_t Bind_Under_Gateway = 0;
uint8_t Bind_Under_Repeater = 0;
uint8_t bindCnt = 0;
uint8_t RPbindCnt = 0;
uint8_t AgentbindCnt = 0;
uint8_t RepeaterbindCnt = 0;
bool FirstPacket = true;

/***************************************************************************
 * setSleepTimer(uint32_t sec)
 *		Just supports sec = 1, 2, 5, 10, 20, 40
 *	sec = 2, SLPTIMER1 is 16 bits counter
 *		cks = 0, BEACONTIMEOUT = 2, magicno = 68, sleep time = 1, 2 sec
 *		cks = 1, BEACONTIMEOUT = 3, magicno = 15, sleep time = 10 sec
 *		cks = 2, BEACONTIMEOUT = 2, magicno = 5, sleep time = 20 sec
 *		cks = 3, BEACONTIMEOUT = 2, magicno = 1, sleep time = 40 sec
 ***************************************************************************/
void setSynTimer(uint32_t sec, uint8_t hitflag)
{
	switch(sec) {
		case 1:		
			switch(*gDatarate) {
				case RF_DATARATE_1M:	
#if 1						
					if(hitflag){		//update EPD need about 5 sec.
						SLPTIMER_Initial(SLPTIMER1, 19642, 1, 1); //JERRY
					}else SLPTIMER_Initial(SLPTIMER1, (32768 - 69), 0, 1);	//68
#endif
					//SLPTIMER_Initial(SLPTIMER1, (32768 - 68), 0, 1);	//68
					gBeaconwait = 3;		//3ms
					break;
					
				case RF_DATARATE_500K:
#if 1		//Spenser			
					if(hitflag == 1){		//update EPD need about 5 sec.
						SLPTIMER_Initial(SLPTIMER1, (19642-10), 1, 1); //
					}
					else if(hitflag == 2)
					{
						SLPTIMER_Initial(SLPTIMER1, (6533 - 8), 1, 1); //
					}
					else if(hitflag == 3)
					{
						SLPTIMER_Initial(SLPTIMER1, (6333 - 0), 1, 1); //
					}
					else
					{
						SLPTIMER_Initial(SLPTIMER1, (6533 - 6), 1, 1); //SLPTIMER_Initial(SLPTIMER1, (6533 - 3), 1, 1);
					}
#endif                                     //(6533 - 2)
					
					//SLPTIMER_Initial(SLPTIMER1, 19642, 1, 1);
					//SLPTIMER_Initial(SLPTIMER1, 6532, 1, 1);
					gBeaconwait = 3;		//2ms
					break;
					
				case RF_DATARATE_250K:
#if 1					
					if(hitflag){		//update EPD need about 5 sec.
						SLPTIMER_Initial(SLPTIMER1, 19629, 1, 1); //JERRY
					}else SLPTIMER_Initial(SLPTIMER1, (32768 - 106), 0, 1);	//106
#endif
					//SLPTIMER_Initial(SLPTIMER1, (32768 - 106), 0, 1);	//106
					gBeaconwait = 3;		//2ms
					break;
			}
			break;
		
		case 2:
			switch(*gDatarate) {
				case RF_DATARATE_1M:		
					if(hitflag){		//update EPD need about 5 sec.
						SLPTIMER_Initial(SLPTIMER1, 26195, 1, 1); //JERRY
					}else SLPTIMER_Initial(SLPTIMER1, ((32768*2) - 68), 0, 1);
					
					gBeaconwait = 2;		//2ms
					break;
					
				case RF_DATARATE_500K:
					if(hitflag == 1){		//update EPD need about 5 sec.
						SLPTIMER_Initial(SLPTIMER1, 26195, 1, 1);
					}else if(hitflag == 2)
					{
						SLPTIMER_Initial(SLPTIMER1, (13085 - 7), 1, 1); //JERRY//SLPTIMER_Initial(SLPTIMER1, (32768 - 88), 0, 1);	//68
					}
					else
					{
						SLPTIMER_Initial(SLPTIMER1, (13085 - 1), 1, 1);
					}
					gBeaconwait = 3;		//2ms
					break;
					
				case RF_DATARATE_250K:
#if 1					
					if(hitflag){		//update EPD need about 5 sec.
						SLPTIMER_Initial(SLPTIMER1, 26193, 1, 1); //JERRY
					}else SLPTIMER_Initial(SLPTIMER1, ((32768*2) - 106), 0, 1);
#endif
					//SLPTIMER_Initial(SLPTIMER1, ((32768*2) - 106), 0, 1);
					gBeaconwait = 3;		//2ms
					break;
			}
			break;
			
		case 5:
			switch(*gDatarate) {
				case RF_DATARATE_1M:		
				  SLPTIMER_Initial(SLPTIMER1, 22927, 2, 1); //JERRY//7S
					gBeaconwait = 2;		//2ms
					break;
					
				case RF_DATARATE_500K:
					
					if(hitflag == 1){		//update EPD need about 5 sec.
						SLPTIMER_Initial(SLPTIMER1, 22927 , 2, 1);
					}
					else if(hitflag == 2)
					{
						SLPTIMER_Initial(SLPTIMER1, (16372 - 7), 2, 1);
					}
					else
					{
						SLPTIMER_Initial(SLPTIMER1, (16372 - 1), 2, 1);
					}				
					gBeaconwait = 3;		//2ms
					break;
					
				case RF_DATARATE_250K:
					SLPTIMER_Initial(SLPTIMER1, 22927, 2, 1); //JERRY//7S
					gBeaconwait = 3;		//2ms
					break;
			}
			break;
			
		case 10:
			switch(*gDatarate) {
				case RF_DATARATE_1M:		
					SLPTIMER_Initial(SLPTIMER1, 39313, 2, 1); //JERRY//12S
					gBeaconwait = 2;		//2ms
					break;
					
				case RF_DATARATE_500K:
					if(hitflag == 1){		//update EPD need about 5 sec.
						SLPTIMER_Initial(SLPTIMER1, 39313, 2, 1);
					}
					else if(hitflag == 2)
					{
						SLPTIMER_Initial(SLPTIMER1, (32757 - 8), 2, 1);
					}
					else
					{
						SLPTIMER_Initial(SLPTIMER1, (32757 - 2), 2, 1);
					}
					gBeaconwait = 3;		//2ms
					break;
					
				case RF_DATARATE_250K:
					SLPTIMER_Initial(SLPTIMER1, 39310, 2, 1); //JERRY//12S
					gBeaconwait = 3;		//2ms
					break;
			}
			break;
			
		case 20:
			switch(*gDatarate) {
				case RF_DATARATE_1M:		
					SLPTIMER_Initial(SLPTIMER1, 36041, 3, 1); //JERRY//22S
					gBeaconwait = 2;		//2ms
					break;
					
				case RF_DATARATE_500K:
					if(hitflag == 1){		//update EPD need about 5 sec.
						SLPTIMER_Initial(SLPTIMER1, 36041, 3, 1);
					}
					else if(hitflag == 2)
					{
						SLPTIMER_Initial(SLPTIMER1, (32764 - 9), 3, 1);
					}
					else
					{
						SLPTIMER_Initial(SLPTIMER1, (32764 - 3), 3, 1);
					}	
					gBeaconwait = 3;		//2ms
					break;
					
				case RF_DATARATE_250K:
					SLPTIMER_Initial(SLPTIMER1, 36040, 3, 1); //JERRY//22S
					gBeaconwait = 3;		//2ms
					break;
			}
			break;
			
		case 40:
			switch(*gDatarate) {
				case RF_DATARATE_1M:		
					SLPTIMER_Initial(SLPTIMER1, 65533, 3, 1); //JERRY//22S
					gBeaconwait = 2;		//2ms
					break;
					
				case RF_DATARATE_500K:
					if(hitflag == 1)
					{		//update EPD need about 5 sec.
						SLPTIMER_Initial(SLPTIMER1, 65533, 3, 1); //JERRY//22S
					}
					else if(hitflag == 2)
					{
						SLPTIMER_Initial(SLPTIMER1, (65533-12), 3, 1);
					}
					else if(hitflag == 3)
					{
						SLPTIMER_Initial(SLPTIMER1, (65478-0), 3, 1);
					}
					else
					{
						SLPTIMER_Initial(SLPTIMER1, (65533-6), 3, 1);
					}	
					gBeaconwait = 3;		//2ms			(65533-5)	
					break;
					
				case RF_DATARATE_250K:
					SLPTIMER_Initial(SLPTIMER1, 65531, 3, 1); //JERRY//22S
					gBeaconwait = 3;		//2ms
					break;
			}
			break;
			
		default:
			switch(*gDatarate) {
				case RF_DATARATE_1M:		
					SLPTIMER_Initial(SLPTIMER1, (32768 - 68), 0, 1);	//68
					gBeaconwait = 2;		//2ms
					break;
					
				case RF_DATARATE_500K:
#if 1					
					if(hitflag){		//update EPD need about 5 sec.
						SLPTIMER_Initial(SLPTIMER1, 19642, 1, 1); //JERRY
					}else SLPTIMER_Initial(SLPTIMER1, 6532, 1, 1); //JERRY//SLPTIMER_Initial(SLPTIMER1, (32768 - 88), 0, 1);	//68
#endif
					//SLPTIMER_Initial(SLPTIMER1, (32768 - 88), 0, 1);
					gBeaconwait = 3;		//2ms					
//					SLPTIMER_Initial(SLPTIMER1, ((32768*2) - 88), 0, 1);
//					gBeaconwait = 3;		//2ms
					break;
					
				case RF_DATARATE_250K:
					SLPTIMER_Initial(SLPTIMER1, ((32768*2) - 106), 0, 1);
					gBeaconwait = 3;		//2ms
					break;
			}
			break;
			
	}

}


/*
 * Jerry add
*/
bool compare_array_data(uint8_t *src_array, uint8_t *dest_array, unsigned int array_size)
{
		uint8_t i;

		if(dest_array != NULL){
			for(i = 0; i < array_size; i++){
				if(src_array[i] != dest_array[i]){
					return false;
				}
			}
		}

		return true;
}

bool check_array_zero(uint8_t *array, unsigned int array_size)
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

bool Is_Default_RepeaterID_Zero(const uint8_t *array, unsigned int array_size)
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

void Set_To_First_Packet()
{
	FirstPacket = true;
}

void Set_First_Arrived(uint8_t* first)
{
	*first = 1;
	printf("Result %x-%x \r\n", GS_First, RP_First);
}

void Not_First_Packet()
{
	FirstPacket = false;
}

bool Is_First_Packet()
{
//	bool result;
	return FirstPacket;
}

void NonStop_Searching_New_Beacon_Src(uint32_t Timeout, uint32_t Total)
{
	bool ret, RP_ret;
	uint8_t mReturn;
	uint8_t recv[64]; 
	uint8_t payloadLength = 64;
	int i, Z;
	P_SYNC_BEACON pSyncBeacon;
	
//	TagDebug(("NonStop_Searching_New_Beacon_Src.\r\n"));
	SLPTIMER_StopTimer(SLPTIMER1); // Stop Sleep Timer First.
	
	while(1)
	{
		StrobeCmd(CMD_RX);
		mReturn = SearchBeacon(Timeout, Total);
		if(mReturn == TAG_MSG_RX_TIMEOUT)
		{
//			TagDebug(("NonStop_Searching_New_Beacon_Src_TIMER_OUT.\r\n"));
			mReturn = 0;
			setSynTimer(1, 0);
			break;
		}
//	if(gState == PWRBEACON_STATE || gState == BEACON_STATE)
//	{
//		payloadLength = 16;
//		TagDebug(("gState == PWRBEACON_STATE || gState == BEACON_STATE.\r\n"));
//	}
	payloadLength = 16;	
	for(i=0; i <payloadLength; i++)	
  {
		recv[i] = RADIO->RX_DATA[i];	
	}
	if( payloadLength == 16)
	{
		AES128_Decrypt(recv,AES_KEY,DecryptData);	
		if(DecryptData[1] == 0x01)
		{
			memcpy(recv,DecryptData,16);
		}
		else
		{
			//do nothing, because it is RAW data.
		}
	}		
	
	pSyncBeacon = (P_SYNC_BEACON)recv;
	
	if(Bind_Under_Gateway)
	{
//		TagDebug(("Bind_Under_Gateway_SRC.\r\n"));
		pSyncBeacon = (P_SYNC_BEACON)recv;
		ret = compare_array_data(AgentID, pSyncBeacon->GatewayID, 2);
		RP_ret = check_array_zero(pSyncBeacon->RepeaterID, 2);
		if((ret == false || RP_ret == false))
		{
			continue;	
		}
		else
		{
			setSynTimer(1, 0); // if get beacon
			break;
		}
	}
	else if(Bind_Under_Repeater)
	{
//		TagDebug(("Bind_Under_Repeater_SRC.\r\n"));
		pSyncBeacon = (P_SYNC_BEACON)recv;
		ret = compare_array_data(RepeaterID, pSyncBeacon->RepeaterID, 2);
		if(ret == false)
		{
//			printf("FALSE\r\n"); 
			continue;
		}
		else
		{
			setSynTimer(1, 0); // if get beacon
			printf("Got Repeater %02x-%02x\r\n", pSyncBeacon->RepeaterID[0],pSyncBeacon->RepeaterID[1]);
			break;
		}
	}	
	
		
	}

}

void Restart_Binding_Process()
{
	Set_To_First_Packet();
	Bind_Under_Gateway = 0;
	Bind_Under_Repeater = 0;
	bindCnt = 0;
	RPbindCnt = 0;
	RepeaterbindCnt = 0;
	AgentbindCnt = 0;
	GS_First = 0;
	RP_First = 0;
}

void Start_Binding_Process(P_SYNC_BEACON pSyncBeacon)
{
	bool ret, Default_Repeater_ID_is_NULL;
	int16_t iRssi = 0;
	ret = check_array_zero(AgentID, 2);
	Default_Repeater_ID_is_NULL = Is_Default_RepeaterID_Zero(&DefaultRepeaterID[3], 2); //DefaultRepeaterID
	
	if(Default_Repeater_ID_is_NULL == false) //Need to Bind under Default Repeater
	{
		memcpy(RepeaterID, &DefaultRepeaterID[3], 2);
		Bind_Under_Repeater = 1;
		printf("Bind_Under_Default Repeater...........\r\n");
	}
	else if(Default_Repeater_ID_is_NULL == true) //Default Repeater ID is 0, prepare to Bind.
	{
//		printf("Default_Repeater_ID_is_NULL...........\r\n");
		ret = check_array_zero(AgentID, 2);
		if(Is_First_Packet() == true)//(ret == true) Catch First Arrived Packet
		{
			printf("First Packet IS %x \r\n", Is_First_Packet());
			memcpy(AgentID, pSyncBeacon->GatewayID, 2);
			memcpy(RepeaterID, pSyncBeacon->RepeaterID, 2);
			ret = check_array_zero(pSyncBeacon->RepeaterID, 2);
			if(ret == true)
			{
				bindCnt = 1;
				Set_First_Arrived(&GS_First);
			}
			else
			{
				RPbindCnt = 1;
				Set_First_Arrived(&RP_First);
			}
			Not_First_Packet();
			printf("First_GW = %02x-%02x\r\n", AgentID[0],AgentID[1]);
			printf("First_RP = %02x-%02x\r\n", RepeaterID[0],RepeaterID[1]);
		}
		else
		{
			// Bind_Under_Gateway...........
			ret = check_array_zero(pSyncBeacon->RepeaterID, 2);
			if(ret == true) //Repeater ID in packet is Zero
			{
				iRssi = RSSI_measurement();
				printf("GwID_Under_GW = %02x-%02x, RSSI = %d\r\n", pSyncBeacon->GatewayID[0],pSyncBeacon->GatewayID[1], iRssi);
				printf("RPID_Under_GW = %02x-%02x, RSSI = %d\r\n", pSyncBeacon->RepeaterID[0],pSyncBeacon->RepeaterID[1], iRssi);
				if(iRssi > -100)
				{
					ret = compare_array_data(AgentID, pSyncBeacon->GatewayID, 2);
					if(ret == true)
					{
//						memcpy(AgentID, pSyncBeacon->GatewayID, 2);
						AgentbindCnt = 0;
						bindCnt++;
						if(bindCnt >= BINDING_THRESHOLD)//10
						{
							Bind_Under_Gateway = 1;
							printf("bindCnt = %d RPbindCnt %d\r\n", bindCnt,RPbindCnt);
							printf("GATEWAY = %d, Repeater = %d !\r\n",Bind_Under_Gateway,Bind_Under_Repeater);
							printf("Bind_Under_Gateway...........\r\n");
							memcpy(AgentID, pSyncBeacon->GatewayID, 2);
							memcpy(RepeaterID, pSyncBeacon->RepeaterID, 2);		
							EPDShowTAGDftStatus(1);
//							Restart_Binding_Process();
						}
					}
					else
					{
//						memcpy(AgentID, pSyncBeacon->GatewayID, 2);
						AgentbindCnt++;
						if(AgentbindCnt >= 5)
						{
							bindCnt = 0;
							AgentbindCnt = 0;
							memcpy(AgentID, pSyncBeacon->GatewayID, 2);
						}
						printf("AGENT ID no Match in Beacon...........\r\n");
					}
				}
			}
			else //Repeater ID in packet is NOT Zero, means under reprater
			{
				iRssi = RSSI_measurement();
				printf("GwIDs_Under_RP = %02x-%02x, RSSI = %d\r\n", pSyncBeacon->GatewayID[0],pSyncBeacon->GatewayID[1], iRssi);
				printf("RPIDs_Under_RP = %02x-%02x, RSSI = %d\r\n", pSyncBeacon->RepeaterID[0],pSyncBeacon->RepeaterID[1], iRssi);
				if(iRssi > -100)
				{
					ret = compare_array_data(RepeaterID, pSyncBeacon->RepeaterID, 2);
////					ret = compare_array_data(AgentID, pSyncBeacon->GatewayID, 2);
//					ret = check_array_zero(pSyncBeacon->RepeaterID, 2);
//					if(GS_First && ret == false) //AGENT == PKT GATEWAY && PKT RP IS NOT ZERO
//					{
//						memcpy(RepeaterID, pSyncBeacon->RepeaterID, 2);
//						
//					}
					if(ret == true)
					{
//							printf("GWID001 = %02x-%02x\r\n", AgentID[0],AgentID[1]);
//							printf("RPID001 = %02x-%02x\r\n", RepeaterID[0],RepeaterID[1]);
//						memcpy(AgentID, pSyncBeacon->GatewayID, 2);
//						memcpy(RepeaterID, pSyncBeacon->RepeaterID, 2);
						RepeaterbindCnt = 0;
						RPbindCnt++;
//						RPbindCnt = RPbindCnt + 2;
						if(RPbindCnt >= BINDING_THRESHOLD)
						{
							Bind_Under_Repeater = 1;
							memcpy(AgentID, pSyncBeacon->GatewayID, 2);
							memcpy(RepeaterID, pSyncBeacon->RepeaterID, 2);
							printf("GATEWAY = %d, Repeater = %d !\r\n",Bind_Under_Gateway,Bind_Under_Repeater);
							printf("bindCnt = %d RPbindCnt %d\r\n", bindCnt,RPbindCnt);
							printf("Bind_Under__Repeater...........\r\n");
							printf("GWID = %02x-%02x\r\n", AgentID[0],AgentID[1]);
							printf("RPID = %02x-%02x\r\n", RepeaterID[0],RepeaterID[1]);
							EPDShowTAGDftStatus(1);
						}
					}
					else
					{
						ret = check_array_zero(pSyncBeacon->RepeaterID, 2);
						if(GS_First && ret == false) //AGENT == PKT GATEWAY && PKT RP IS NOT ZERO
						{
							printf("if(GS_First && ret == false)\r\n");
							memcpy(RepeaterID, pSyncBeacon->RepeaterID, 2);
							RepeaterbindCnt = 0;
							RPbindCnt = RPbindCnt + 5;
	//						RPbindCnt = RPbindCnt + 2;
							if(RPbindCnt >= BINDING_THRESHOLD)
							{
								Bind_Under_Repeater = 1;
								memcpy(AgentID, pSyncBeacon->GatewayID, 2);
								memcpy(RepeaterID, pSyncBeacon->RepeaterID, 2);
								printf("GATEWAY = %d, Repeater = %d !\r\n",Bind_Under_Gateway,Bind_Under_Repeater);
								printf("Bind_Under__Repeater...........\r\n");
								printf("GWID = %02x-%02x\r\n", AgentID[0],AgentID[1]);
								printf("RPID = %02x-%02x\r\n", RepeaterID[0],RepeaterID[1]);
								EPDShowTAGDftStatus(1);
							}
							
						}
						else
						{
							//						memcpy(RepeaterID, pSyncBeacon->RepeaterID, 2);
							RepeaterbindCnt++;
							if(RepeaterbindCnt >= 5)
							{
								RPbindCnt = 0;
								RepeaterbindCnt = 0;
								memcpy(AgentID, pSyncBeacon->GatewayID, 2);
								memcpy(RepeaterID, pSyncBeacon->RepeaterID, 2);
							}
							printf("Repeater ID no Match in Beacon...........\r\n");
						}
						
					}

				}

			}
		}
	}
	else
	{
		
	}
	
	
	
	
	
	
//	if(RP_ret != true){
//		memcpy(RepeaterID, DefaultRepeaterID, 2);
//		printf("GateWay ID initial is NULL...........\r\n");
//	}
//	if(ret == true){
//		memcpy(AgentID, pSyncBeacon->GatewayID, 2);
//		printf("GateWay ID initial is NULL...........\r\n");
//	}
//	else
//	{
//		iRssi = RSSI_measurement();
//		printf("GwID = %02x-%02x, RSSI = %d\r\n", pSyncBeacon->GatewayID[0],pSyncBeacon->GatewayID[1], iRssi);
//		printf("RPID = %02x-%02x, RSSI = %d\r\n", pSyncBeacon->RepeaterID[0],pSyncBeacon->RepeaterID[1], iRssi);
//		if(iRssi > 50) //origin 120
//		{
//			ret = compare_array_data(AgentID, pSyncBeacon->GatewayID, 2);
//			if(ret == true){
//				bindCnt++;
//				if(bindCnt >= 10){
//					Bind_Under_Gateway = 1;
//					printf("Bind_Under_Gateway...........\r\n");
//				}
//			}else{
//				memcpy(AgentID, pSyncBeacon->GatewayID, 2);
//			}
//		}
//	}
}

uint8_t IsBindingIdMatch(P_SYNC_BEACON pSyncBeacon, uint8_t* recv, uint8_t AfterChangeBeaconSrc)
{
	bool ret, RP_ret;
	//is binding id match
//	printf("AfterChangeBeaconSrc = %d\r\n", AfterChangeBeaconSrc);
	if(Bind_Under_Gateway)
	{
		pSyncBeacon = (P_SYNC_BEACON)recv;
		ret = compare_array_data(AgentID, pSyncBeacon->GatewayID, 2);
		RP_ret = check_array_zero(pSyncBeacon->RepeaterID, 2);
		if((ret == false || RP_ret == false) && pSyncBeacon->BeaconType != RF_AL_CHANGE_BEACON_SRC)
		{
			return TAG_MSG_GATEWAY_ID_NOT_MATCH;			
		}
	}
	else if(Bind_Under_Repeater)
	{
		pSyncBeacon = (P_SYNC_BEACON)recv;
		ret = compare_array_data(RepeaterID, pSyncBeacon->RepeaterID, 2);
		if(ret == false && pSyncBeacon->BeaconType != RF_AL_CHANGE_BEACON_SRC)
		{
			if(AfterChangeBeaconSrc == 1)
			{
				printf("AfterChangeBeaconSrc = %d\r\n", AfterChangeBeaconSrc);
				printf("RPID = %02x-%02x\r\n", RepeaterID[0],RepeaterID[1]);
				printf("RPID_PACKET = %02x-%02x\r\n", pSyncBeacon->RepeaterID[0],pSyncBeacon->RepeaterID[1]);
				return TAG_MSG_AFTER_CHANGE_BEACON_SRC;
			}
			printf("RPID_1 = %02x-%02x\r\n", RepeaterID[0],RepeaterID[1]);
			printf("RPID_PACKET_1 = %02x-%02x\r\n", pSyncBeacon->RepeaterID[0],pSyncBeacon->RepeaterID[1]);
			return TAG_MSG_REPEATER_ID_NOT_MATCH;			
		}
	}
	return 1;
}

#if 1
/*********************************************************************
** RxPacket
*********************************************************************/
uint8_t RxPacket(void)
{
//	bool ret, RP_ret;
//	uint8_t iRssi = 0;
	bool ret;
	uint8_t recv[64]; 
	int i;
	int index = 0;
	int CheckSum = 0;
	uint8_t payloadLength = 64;
	uint8_t mtag;
	uint8_t mhopcount;
	uint8_t mResult = 0;
	P_SYNC_BEACON pSyncBeacon;
	P_TAG_UPDATE_LIST pTagUpdateList;
	P_TAG_NVM_DATA pTagNvmData;
	P_HEADER_PACKET pHeaderPacket;
	P_TAG_OTA_LIST pTagOtaList;
	if(gState == PWRBEACON_STATE || gState == BEACON_STATE)
	{
		payloadLength = 16;
	}
	for(i=0; i <payloadLength; i++)	
  {
		recv[i] = RADIO->RX_DATA[i];	
	}
	if( payloadLength == 16)
	{
//				for(Z=0; Z<16; Z++)
//				{
//					printf("%02x ",recv[Z]);
//				}	
//				printf("\r\n ");
		AES128_Decrypt(recv,AES_KEY,DecryptData);	
		if(DecryptData[1] == 0x01)
		{
			memcpy(recv,DecryptData,16);
//				for(Z=0; Z<16; Z++)
//				{
//					printf("%02x ",recv[Z]);
//				}	
//				printf("\r\n ");
		}
		else
		{
			//do nothing, because it is RAW data.
		}
	}

	gTagSeq = 10;		//Spenser - Max. Tags update one second.
	switch(recv[1]) {
		case RF_PT_SYNC_BEACON:	
			pSyncBeacon = (P_SYNC_BEACON)recv;
			if(Bind_Under_Gateway || Bind_Under_Repeater)
			{
//					IsBindingIdMatch(pSyncBeacon,recv);
					if(IsBindingIdMatch(pSyncBeacon,recv,AfterChangeBeaconSrc) != true)
					{	
						printf("MATCH RESULT IS %x \r\n", IsBindingIdMatch(pSyncBeacon,recv,AfterChangeBeaconSrc));
						if(AfterChangeBeaconSrc == 1)
						{
							return TAG_MSG_AFTER_CHANGE_BEACON_SRC;
						}
						else
						{
							return TAG_MSG_GATEWAY_ID_NOT_MATCH;
						}
					}
					switch(pSyncBeacon->Action)
					{
						case RF_AL_SYNC:
							if(TAG_UPDATED == 1 || Set_All_Wake_time == 1)
							{
								//Load Previous Sleep Time
								setSynTimer(TagWakeUpInterval, 0);		//Load Previous Sleep Time
		//						setSynTimer(1, 0);
							}
							else
							{
								setSynTimer(1, 0);
		//						setSynTimer(TagWakeUpInterval, 0);		//Sync time sec	, Test for YC
							}
							mResult = TAG_MSG_ONLY_SYNC;
							break;
							
						case RF_AL_UPDATE_TAG:
//						case RF_AL_UPDATE_REPEATER_TAG:
							AfterChangeBeaconSrc = 0;
							mResult = TAG_MSG_TO_RCV_TAG_LIST;
							break;	
						
						case RF_AL_HOP_TO_TAG:
							setSynTimer(TagWakeUpInterval, 0);		//Sync time sec
							mhopcount = (pSyncBeacon->ChnCtrl) & 0xF;
							if(ghopcount == 0) {			//first time received hopping sync beacon
									ghopcount = mhopcount;
									gCHindex = ((pSyncBeacon->ChnCtrl) >> 4) & 0xF;
							}
							mResult = TAG_MSG_HOPPING;
						 break;
							
						case RF_AL_GET_TAG_STATUS:
							setSynTimer(TagWakeUpInterval, 0);
							mResult = TAG_MSG_TO_RCV_TAG_LIST;
						 break;		
						
						case RF_AL_ALL_WU_TIME:
							setSynTimer(pSyncBeacon->WakeUpTime, 0);
							Set_All_Wake_time = 1;
							TagWakeUpInterval = pSyncBeacon->WakeUpTime;
							mResult = TAG_MSG_ONLY_SYNC;
							break;
						
						case RF_AL_TAG_UNBOUND:
		//					printf("RF_AL_TAG_UNBOUND..........\r\n");
		//					setSynTimer(pSyncBeacon->WakeUpTime, 0);
							setSynTimer(TagWakeUpInterval, 0);
							mResult = TAG_MSG_TO_RCV_TAG_LIST;
							break;
			
						case RF_AL_CHANGE_BEACON_SRC:
							setSynTimer(TagWakeUpInterval, 0);
							mResult = TAG_MSG_TO_RCV_TAG_LIST;
						 break;
						
						case RF_AL_WRITE_TAG_NVM_DATA:
							setSynTimer(TagWakeUpInterval, 0);
							mResult = TAG_MSG_TO_RCV_TAG_LIST;
						 break;	
						
						case RF_AL_OVER_THE_AIR:
//							printf("RF_AL_OVER_THE_AIR..........\r\n");
//							setSynTimer(TagWakeUpInterval, 0);
							SLPTIMER_StopTimer(SLPTIMER1);
							TAG_RCV_OTA = 1;
							mResult = TAG_MSG_TO_RCV_TAG_LIST;
						 break;							

						default:
							setSynTimer(DefaultWakeUpTime, 0);		//Load Default Sleep Time
							mResult = TAG_MSG_NO_HIT_ID;
							break;
					}
			}
			else
			{
				Start_Binding_Process(pSyncBeacon);
			}
			break;

		case RF_PT_TAG_UPDATE_LIST: //TAGUPDATELIST:		
		case RF_PT_ALWAYS_WAKE_BEACON:	
//		printf("RF_PT_TAG_UPDATE_LIST..........\r\n");
#if 0			
			printf("\r\n");	
			for(i=0; i <64; i++)	
		  {
				printf("%X ", recv[i]);	
			}				
			printf("\r\n");
			printf("Received packet. %x:%x:%x:%x:%x\r\n", TagID[0],TagID[1],TagID[2], TagID[3], TagID[4]);
			printf("Received packet. %X  %x:%x:%x:%x:%x\r\n", recv[2], recv[31],recv[32],recv[33], recv[34], recv[35]);
#endif			

			pTagUpdateList = (P_TAG_UPDATE_LIST)recv;
//			printf("Received packet!!. %x:%x:%x:%x:%x\r\n", pTagStatus->BeaconSeq,pTagStatus->BeaconType,pTagStatus->PktCtrl, pTagStatus->ChnCtrl, pTagStatus->WakeUpTime);
//			printf("Received packet. %x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x\r\n", recv[0],recv[1],recv[2],recv[3],recv[4],recv[5],recv[6],recv[7],recv[8],recv[9],recv[10], recv[11], recv[12]);
			mtag = (pTagUpdateList->PktCtrl) & 0x7;		//want update number
			if(mtag > 0) {

				for(i=0; i<mtag; i++){
					if(TagIdCompare(pTagUpdateList->TAGID,TagID,i))
					{
						updateflag = 1;
						setSynTimer(1, updateflag);		//Sync time sec	
						//Match update
						if(TAG_RCV_OTA == 1)
						{
							mResult = TAG_MSG_HIT_ID;
						}
						else
						{
							mResult = TAG_MSG_HIT_ID;
						}
						
						gTagSeq = (uint8_t)i;		//Spenser - Save Hit Tag sequence number
//							printf("gTagSeq = %d\n", gTagSeq);
						//if(gTagSeq > 0) printf("gggTagSeq = %d\n", gTagSeq);
//						memcpy(&AgentID[0], pTagUpdateList->GatewayID, 2);
//						memcpy(&RepeaterID[0], pTagUpdateList->RepeaterID, 2);
						TagWakeUpInterval = pTagUpdateList->WakeUpTime;//recv[4];	
//						WakeUpTimeInList = pTagUpdateList->WakeUpTime;//recv[4];	
//							printf("TagWakeUpInterval %d!\r\n",TagWakeUpInterval);
						TAG_UPDATED = 0;
						TAG_RCV_NO_HIT_ID = 2;
						break;
					}
					else // No Hit ID 
					{
//						printf("NO  HIT ID!!!!!!!!!!!!!!!!!!!!!!11.\r\n");
						if(TAG_UPDATED == 1)
						{
							setSynTimer(TagWakeUpInterval, 2);		//Load Previous Sleep Time
//							setSynTimer(20, 2);		//Load Previous Sleep Time
//								setSynTimer(1, 2);		//Load Previous Sleep Time
						}
						else
						{
//							setSynTimer(40, 2);		//Sync time sec	
							setSynTimer(1, 2);		//Load Previous Sleep Time
						}
						mResult = TAG_MSG_NO_HIT_ID;
						updateflag = 0;
					}
				}							
			}

			break;
			
		case RF_PT_OTA_UPDATE_LIST: //TAGUPDATELIST:
//			printf("RF_PT_OTA_UPDATE_LIST..........\r\n");
#if 0			
			printf("\r\n");	
			for(i=0; i <64; i++)	
		  {
				printf("%X ", recv[i]);	
			}				
			printf("\r\n");
			printf("Received packet. %x:%x:%x:%x:%x\r\n", TagID[0],TagID[1],TagID[2], TagID[3], TagID[4]);
			printf("Received packet. %X  %x:%x:%x:%x:%x\r\n", recv[2], recv[31],recv[32],recv[33], recv[34], recv[35]);
#endif			

			pTagOtaList = (P_TAG_OTA_LIST)recv;
//			printf("Received packet!!. %x:%x:%x:%x:%x\r\n", pTagStatus->BeaconSeq,pTagStatus->BeaconType,pTagStatus->PktCtrl, pTagStatus->ChnCtrl, pTagStatus->WakeUpTime);
//			printf("Received packet. %x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x\r\n", recv[0],recv[1],recv[2],recv[3],recv[4],recv[5],recv[6],recv[7],recv[8],recv[9],recv[10], recv[11], recv[12], recv[13], recv[14], recv[15], recv[16]);
			mtag = 0x01;		//want update number
			if(mtag > 0) {

				for(i=0; i<mtag; i++){
					if(TagIdCompare(pTagOtaList->TAGID,TagID,i))
					{
						OTA_Attribution = pTagOtaList->Attribution;
						OTA_BinFileSize = pTagOtaList->BinFileSize;//16
//						printf("OTA_BinFileSize %d!\r\n",OTA_BinFileSize);
						OTA_TotalOfPacket = pTagOtaList->TotalOfPacket;
						OTA_Major = pTagOtaList->BinFileVersionMajor;
						OTA_Minor = pTagOtaList->BinFileVersionMinor;
						
						if(OTA_Attribution != For_TAG)
						{
							mResult = TAG_MSG_NO_HIT_ID;
						}
						else
						{
							mResult = TAG_MSG_OTA_HIT_ID;
						}
						break;
					}
					else // No Hit ID 
					{
						if(TAG_UPDATED == 1)
						{
							setSynTimer(TagWakeUpInterval, 2);		//Load Previous Sleep Time
						}
						else
						{
//							setSynTimer(40, 2);		//Sync time sec	
							setSynTimer(1, 2);		//Load Previous Sleep Time
						}
						mResult = TAG_MSG_NO_HIT_ID;
						updateflag = 0;
					}
				}							
			}

			break;			
			
		case RF_PT_PACKET_HEADER:
//			printf("TAG_RCV_OTA.....%d.....\r\n",TAG_RCV_OTA);
			pHeaderPacket = (P_HEADER_PACKET)recv; //P_HEADER_PACKET pHeaderPacket;
//			printf("Received Header packet. %x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x\r\n", recv[0],recv[1],recv[2],recv[3], recv[4], recv[5],recv[6],recv[7],recv[8],recv[9],recv[10],recv[11],recv[12],recv[13],recv[14]);
			if(gState != HEADER_STATE && gState != OTA_STATE){
				printf("Not inside HEADER_STATE %d!\r\n",gState);
//				if(gState == SYNCBEACON) {
					Oneshoutflag = 1; //Spenser - not current state like receive timeout 
					mResult = TAG_MSG_NO_HIT_ID;
					if(TAG_RCV_OTA == 1)
					{
						mResult = TAG_MSG_PACKET_LOST;
					}
//				}
				break;
			} 
			mindex = recv[0];
			if(TagIdCompare(pHeaderPacket->TagID,TagID,0))
			{
//				DataLen = pHeaderPacket->PictureSize;
//				pEpdbuf = &EpdRFData[0];	
//				PacketCount = pHeaderPacket->Color;
				if(TAG_RCV_OTA == 1)
				{
					DataLen = pHeaderPacket->PictureSize;
//					printf("DataLen %d!\r\n",DataLen);
					pEpdbuf = &EpdRFData[0];	
					OTA_Data_index = pHeaderPacket->Color;	
//					printf("OTA_Data_index %d!\r\n",OTA_Data_index);					
					mResult = TAG_MSG_OTA_HIT_ID;	
				}
				else
				{
					DataLen = pHeaderPacket->PictureSize;
					pEpdbuf = &EpdRFData[0];	
					ColorFromPacket = pHeaderPacket->Color;
					mResult = TAG_MSG_HIT_ID;					
				}

				//gState = DATA_STATE;
			}else {
				mResult = TAG_MSG_NO_HIT_ID;
				//gState = ENTERPM1_STATE;
			}			
			break;
			
		case RF_PT_DATA_PACKET:
			if(gState != DATA_STATE && gState != OTA_STATE) {
				printf("Not inside DATA_STATE %d!\r\n",gState);
					Oneshoutflag = 1; //Spenser - not current state like receive timeout 
//				mResult = TAG_MSG_STATE_NOT_RIGHT;
				return TAG_MSG_STATE_NOT_RIGHT;
//				break;
			} 
			if((recv[0] - mindex) > 1) {
				printf("Lost some data...%x - %x\r\n", recv[0], mindex);
//				mResult = TAG_MSG_PACKET_LOST;		//indicate packet lost
				return TAG_MSG_PACKET_LOST;
//				break;
			}
			mindex = recv[0];
			
			memcpy(pEpdbuf, &recv[2], 62);		//62: -two bytes header, 64-2
			pEpdbuf += 62;
			return TAG_MSG_SINGAL_PACKET_OK;

		case RF_PT_OVER_THE_AIR:
			if(gState != OTA_STATE) {
				printf("Not inside OTA_DATA_STATE %d!\r\n",gState);
					Oneshoutflag = 1; 
				return TAG_MSG_STATE_NOT_RIGHT;
			} 
			
			if((recv[0] - mindex) > 1) {
				printf("Lost some data...%x - %x\r\n", recv[0], mindex);
				return TAG_MSG_PACKET_LOST;
			}
			mindex = recv[0];
			memcpy(pEpdbuf, &recv[2], 62);		//62: -three bytes header, 64-3
			pEpdbuf += 62;
			return TAG_MSG_SINGAL_PACKET_OK;			
			
		case RF_PT_TAG_STATUS:		
#if 0			
			printf("\r\n");	
			for(i=0; i <64; i++)	
		  {
				printf("%X ", recv[i]);	
			}				
			printf("\r\n");
			printf("Received packet. %x:%x:%x:%x:%x\r\n", TagID[0],TagID[1],TagID[2], TagID[3], TagID[4]);
			printf("Received packet. %X  %x:%x:%x:%x:%x\r\n", recv[2], recv[31],recv[32],recv[33], recv[34], recv[35]);
#endif			

//			pTagStatus = (P_TAG_RCV_BEACON)recv;
			pTagUpdateList = (P_TAG_UPDATE_LIST)recv;
//			printf("Received packet!!. %x:%x:%x:%x:%x\r\n", pTagStatus->BeaconSeq,pTagStatus->BeaconType,pTagStatus->PktCtrl, pTagStatus->ChnCtrl, pTagStatus->WakeUpTime);
			mtag = (pTagUpdateList->PktCtrl) & 0x7;		//want update number
			if(mtag > 0) {
				//have update				
		
				for(i=0; i<mtag; i++){
					if(TagIdCompare(pTagUpdateList->TAGID,TagID,i))
					{
//						setSynTimer(1, 0);		//Sync time sec	
						//Match update
						mResult = TAG_MSG_HIT_ID;
						gTagSeq = (uint8_t)i;		//Spenser - Save Hit Tag sequence number
//							printf("gTagSeq = %d\n", gTagSeq);
						//if(gTagSeq > 0) printf("gggTagSeq = %d\n", gTagSeq);
//						memcpy(&AgentID[0], pTagUpdateList->GatewayID, 2);
//						memcpy(&RepeaterID[0], pTagUpdateList->RepeaterID, 2);
//						Delay1ms(i*128);		//(120.3: 127ms) (132 : 138)
//						Delay1us(i*850);		//(500) 850
				replyTagStatus(TAG_MSG_RX_COMPLETED);
				mResult = TAG_MSG_REPORT_STATUS_OK;
						TagWakeUpInterval = pTagUpdateList->WakeUpTime;//recv[4];	
//							printf("TagWakeUpInterval %d!\r\n",TagWakeUpInterval);
//						TAG_UPDATED = 0;
						break;
					}
					else // No Hit ID 
					{

						setSynTimer(TagWakeUpInterval, 2); //TagWakeUpInterval
//						printf("gTagSeq = %d\r\n", TagWakeUpInterval);
//						GetTagStatus(Statusbuf);
						mResult = TAG_MSG_NO_HIT_ID;
					}
				}							
			}

			break;			

		case RF_PT_TAG_UNBOUND:		
			pTagUpdateList = (P_TAG_UPDATE_LIST)recv;
			mtag = (pTagUpdateList->PktCtrl) & 0x7;		//want update number
			if(mtag > 0) {
				for(i=0; i<mtag; i++){
					if(TagIdCompare(pTagUpdateList->TAGID,TagID,i))
					{
//						printf("TAG_MSG_RCV_UNBOUND_MSG_HIT ID\r\n");
						gTagSeq = (uint8_t)i;
						replyTagStatus(TAG_MSG_RCV_UNBOUND_MSG_OK);
						EPDShowTAGDftStatus(0);
						mResult = TAG_MSG_RCV_UNBOUND_MSG_OK;
						TagWakeUpInterval = pTagUpdateList->WakeUpTime;//recv[4];	
						break;
					}
					else // No Hit ID 
					{
						setSynTimer(TagWakeUpInterval, 2); //TagWakeUpInterval
						printf("TagWakeUpInterval = %d\r\n", TagWakeUpInterval);
						mResult = TAG_MSG_NO_HIT_ID;
					}
				}							
			}
			break;	
			
		case RF_PT_CHANGE_BEACON_SRC:
//			printf("RF_PT_CHANGE_BEACON_SRC\r\n");
			pTagUpdateList = (P_TAG_UPDATE_LIST)recv;
			mtag = (pTagUpdateList->PktCtrl) & 0x7;		//want update number
			if(mtag > 0) {
				for(i=0; i<mtag; i++){
					if(TagIdCompare(pTagUpdateList->TAGID,TagID,i))
					{
						memcpy(AgentID, pTagUpdateList->GatewayID, 2);
//						memcpy(RepeaterID, pTagUpdateList->RepeaterID, 2);
//						replyTagStatus(TAG_MSG_CHANGE_BEACON_SRC_OK);
						ret = check_array_zero(pTagUpdateList->RepeaterID, 2);
						if(ret == true) //repeater id is ZERO
						{
							replyTagStatus(TAG_MSG_CHANGE_BEACON_SRC_OK);
							printf("Change To Gateway SRC\r\n");
							Bind_Under_Gateway = 1;
							Bind_Under_Repeater = 0;
						}
						else //indicate a repeater id.
						{
							ret = check_array_zero(RepeaterID, 2);
							if(ret == true)
							{
								Delay1ms(700);
							}
							else
							{
								Delay1ms(65);
							}
							memcpy(RepeaterID, pTagUpdateList->RepeaterID, 2);							
							replyTagStatus(TAG_MSG_CHANGE_BEACON_SRC_OK);
							printf("Change To Repeater SRC\r\n"); 
							Bind_Under_Gateway = 0;
							Bind_Under_Repeater = 1;
						}
//						NonStop_Searching_New_Beacon_Src();
//						TagDebug(("END_NonStop_Searching_New_Beacon_Src_END......\r\n"));
						mResult = TAG_MSG_CHANGE_BEACON_SRC_OK;
						TagWakeUpInterval = pTagUpdateList->WakeUpTime;//recv[4];	
						AfterChangeBeaconSrc = 1;
//						
//						EPDShowTAGDftStatus();
						break;
					}
					else // No Hit ID 
					{
						setSynTimer(TagWakeUpInterval, 2); //TagWakeUpInterval
						mResult = TAG_MSG_NO_HIT_ID;
					}
				}							
			}
			break;			

		case RF_PT_WRITE_TAG_NVM_DATA:
		case RF_PT_WRITE_TAG_ORIGINAL_NVM_DATA:	
			pTagNvmData = (P_TAG_NVM_DATA)recv; //P_TAG_NVM_DATA pTagNvmData;
			index = sizeof(pTagNvmData->Seq) + sizeof(pTagNvmData->PacketType) + sizeof(pTagNvmData->TAG_ID);
				for(i=0; i<64; i++)
				{
					printf("%02x ",recv[i]);
				}	
				printf("\r\n");
				printf("sizeof(TAG_NVM_DATA) is %d ",sizeof(TAG_NVM_DATA));
				printf("\r\n");
				for(i = index; i <= (sizeof(TAG_NVM_DATA) - 2); i++)
				{
					CheckSum = CheckSum + recv[i]; //TAG_NVM_DATA
//					printf("%d ",i);
					printf("%02x ",recv[i]);
				}
				printf("before  is %d \r\n",sizeof(pTagNvmData->Seq) + sizeof(pTagNvmData->PacketType) + sizeof(pTagNvmData->TAG_ID));
				printf("SIZEOF is TAG_NVM_DATA %d \r\n",sizeof(TAG_NVM_DATA));
				printf("CheckSum is %d \r\n",CheckSum);			
				
				CheckSum = (CheckSum % 255 + pTagNvmData->TAG_NEW_ID[4]) & 0xFF;
				printf("CheckSum is %d \r\n",CheckSum);
				printf("CheckSumPKT is %d \r\n",pTagNvmData->CheckSum);

				
			if((IsTagIDLargerThanMax(pTagNvmData->TAG_NEW_ID,pTagNvmData->Max_TAG_ID,5)) && (IsNewTagIDLegal(pTagNvmData->TAG_NEW_ID))) 
			{
					if((TagIdCompare(pTagNvmData->TAG_ID,TagID,0)) && (CheckSum == pTagNvmData->CheckSum) && ((pTagNvmData->MagicID[0] == 0x0A) && (pTagNvmData->MagicID[1] == 0x46)) )
//					if((TagIdCompare(pTagNvmData->TAG_ID,TagID,0)) && ((pTagNvmData->MagicID[0] == 0x0A) && (pTagNvmData->MagicID[1] == 0x46)) )
					{
						if(pTagNvmData->PacketType == RF_PT_WRITE_TAG_NVM_DATA)
						{
//							memcpy(TagNvmData, pTagNvmData->TAG_NEW_ID, (sizeof(TAG_NVM_DATA) - 7));
//							memcpy(TagNvmData, pTagNvmData->MagicID, (sizeof(TAG_NVM_DATA) - 7));							
							index = (sizeof(TAG_NVM_DATA) - 7);
							memcpy(TagNvmData, pTagNvmData->MagicID, index);
							
							i= GetAdcTemperature_measurement();
							*(TagNvmData+index) = i;					//Save ADC low byte
							*(TagNvmData+index+1) = i >>8;		//Save ADC high byte
							
							WriteStatusToFlash(TagNvmData);
							NVIC_SystemReset();
						}
						else
						{
//							memset(TagNvmData,0xFF,sizeof(TagNvmData));
//							WriteStatusToFlash(TagNvmData);
//							NVIC_SystemReset();
						}

						break;
					}
					else // No Hit ID 
					{
						setSynTimer(TagWakeUpInterval, 2); //TagWakeUpInterval
						mResult = TAG_MSG_NO_HIT_ID;
					}						
			}
			break;			
			
		default:
			break;	
	}
	return mResult;
}

#endif

/*****************************************************************
 *
 *	uint8_t mSec: receive time out.
 *****************************************************************/
uint8_t getpacket(uint32_t mSec)
{
	uint8_t RxPktResult;
	uint8_t WTRFlag = 0;
	
	Oneshoutflag = 0;		//TIme out flag
	
	startTimer(TIMER1, mSec);		//Start receive timeout timer
	
	while(1) {
		if(WTRFlag == 0) //Wait CMD_RX action, WTR = 1.
		{	
			if((RADIO->STATUS & (1<<7))) 
			{
				WTRFlag = 1;
			}
		}
		else if(!(RADIO->STATUS & (1<<7)))
		{
			Timer_StopTimer(TIMER1);
//			StrobeCmd(CMD_STBY);				//Back to Standby
			if(!Oneshoutflag)
			{
				RxPktResult = RxPacket();
				break;
			}
		}
		else if(Oneshoutflag)
		{
			TagDebug(("TagDebug: Wait RX timeout..\r\n"));
			if(gState == BEACON_STATE) {
			//gBeaconwait += 20;
			//if(gBeaconwait > 700) gBeaconwait = 700;
			//printf("gBeaconwait = %d\r\n", gBeaconwait);
			}
			//					RxPktResult = RXTIMEOUT;
			return TAG_MSG_RX_TIMEOUT;
		}
	}
		return RxPktResult;
}

uint8_t SearchBeacon(uint32_t mSec, uint32_t Total)
{
	uint8_t RxPktResult;
	uint8_t WTRFlag = 0;
	
	Oneshoutflag = 0;		//TIme out flag
	
	startTimer(TIMER1, mSec);		//Start receive timeout timer
	
	while(1) {
		if(WTRFlag == 0) //Wait CMD_RX action, WTR = 1.
		{	
			if((RADIO->STATUS & (1<<7))) 
			{
				WTRFlag = 1;
			}
		}
		else if(!(RADIO->STATUS & (1<<7)))
		{
			Timer_StopTimer(TIMER1);
//			StrobeCmd(CMD_STBY);				//Back to Standby
			if(!Oneshoutflag)
			{
//				RxPktResult = RxPacket();
				break;
			}
		}
		else if(Oneshoutflag)
		{
			TimerCount++;
//			printf("TimerCount = %d\r\n", TimerCount);
			if(TimerCount >= Total)
			{
//				TagDebug(("TimerCount >= 500..\r\n"));
				TimerCount = 0;
				return TAG_MSG_RX_TIMEOUT;
			}
			break;
//			Oneshoutflag = 0;
			
//			TagDebug(("TagDebug: Wait RX timeout..\r\n"));
//			if(gState == BEACON_STATE) {
//			//gBeaconwait += 20;
//			//if(gBeaconwait > 700) gBeaconwait = 700;
//			//printf("gBeaconwait = %d\r\n", gBeaconwait);
//			}
//			//					RxPktResult = RXTIMEOUT;
//			return TAG_MSG_RX_TIMEOUT;
		}
	}
		return RxPktResult;
}

/*******************************************************
 * getdatapacket() - Get one Tag refresh data.  Spenser
 *
 *******************************************************/
uint8_t getdatapacket(void)
{
	uint8_t RxPktResult;
	uint8_t WTRFlag = 1;
	StrobeCmd(CMD_RX);
	
	while(1) {
		if(WTRFlag == 0) //Wait CMD_RX action, WTR = 1.
		{	

			WTRFlag = 1;
		}
		else if(WTRFlag == 1)//if(!(RADIO->STATUS & (1<<7)))
		{
//			Timer_StopTimer(TIMER1);
//			StrobeCmd(CMD_STBY);				//Back to Standby
			while((RADIO->STATUS & (1<<7)));
//			{
//				if(Oneshoutflag)
//				{
//					printf("Data packet timeout.\r\n");
//					return TAG_MSG_RX_TIMEOUT;
//				}
//			}
			
			if(!Oneshoutflag)
			{
				RxPktResult = RxPacket();
			}
			else if(Oneshoutflag)
			{
				return TAG_MSG_RX_TIMEOUT;
			}
			break;
		}
		else if(Oneshoutflag)
		{
//			printf("Data packet timeout.\r\n");
			return TAG_MSG_RX_TIMEOUT;
//			break;
		}
	}	

		return RxPktResult;
}
//uint8_t getdatapacket(void)
//{
//	uint8_t RxPktResult;
//	uint8_t WTRFlag = 1;
//	StrobeCmd(CMD_RX);
//	
//	while(1) {
//		if(WTRFlag == 0) //Wait CMD_RX action, WTR = 1.
//		{	
//				if((RADIO->STATUS & (1 << 7)))
//				{
//					WTRFlag = 1; //if RF Rx ready toggle flag to Rx packet
//				}
//		}
//		else if(WTRFlag == 1)//if(!(RADIO->STATUS & (1<<7)))
//		{
//			if((!(RADIO->STATUS & (1 << 7))) || (Oneshoutflag)) 
//			{
//			StrobeCmd(CMD_STBY);
//			if(!Oneshoutflag)
//			{
//				RxPktResult = RxPacket();
//			}
//			else
//			{
//				RxPktResult = TAG_MSG_RX_TIMEOUT;
//			}
//			break;
//			}
//				
//		}
//		else if(Oneshoutflag)
//		{
////			printf("Data packet timeout.\r\n");
//			return TAG_MSG_RX_TIMEOUT;
////			break;
//		}
//	}	

//		return RxPktResult;
//}

void ReplyUpdateResult(uint8_t result)
{

	uint8_t txbuf[64];
	int i;
	Oneshoutflag = 0;		//TIme out flag
	
	startTimer(TIMER1, 50);		//Start receive timeout timer
		
	txbuf[0] = gTxSeqNo++;
	txbuf[1] = RF_PT_UPDATE_STATUS_REPORT;
	txbuf[2] = result;
	GetTagStatus(Statusbuf);
	memcpy(&txbuf[3], &Statusbuf[0], 64-3);

	for(i=0; i<64; i++) {
		RADIO->TX_DATA[i] = txbuf[i]; //

//	printf("%02x ",txbuf[i]);

	}
	for(i=0; i<10; i++) { //Reply TO GW/ RP
		StrobeCmd(CMD_TX); //entry tx & transmit
		Delay10us(1);
		while((RADIO->STATUS & (1<<7)))  //WTR
		{
			if(Oneshoutflag)
			{
//				printf("%d ",i);
				printf("Oneshoutflag in while((RADIO->STATUS & (1<<7)));\r\n");	
				break;
			}
		}
		if(Oneshoutflag)
		{
			printf("Oneshoutflag_02\r\n");	
			break;
		}
		Delay10us(1);
	}
	Timer_StopTimer(TIMER1);
	
}

void replypacket(uint8_t result)
{

	uint8_t txbuf[64];
	int i;
	Oneshoutflag = 0;		//TIme out flag
	
	startTimer(TIMER1, 20);		//Start receive timeout timer
		
	txbuf[0] = gTxSeqNo++;
	txbuf[1] = RF_PT_UPDATE_STATUS_REPORT;
	txbuf[2] = result;
	GetTagStatus(Statusbuf);
	memcpy(&txbuf[3], &Statusbuf[0], 64-3);

	for(i=0; i<64; i++) {
		RADIO->TX_DATA[i] = txbuf[i]; //

//	printf("%02x ",txbuf[i]);

	}

	StrobeCmd(CMD_TX); //entry tx & transmit
	Delay10us(1);

	while((RADIO->STATUS & (1<<7)))  //WTR
	{
		if(Oneshoutflag)
		{
			printf("Oneshoutflag in while((RADIO->STATUS & (1<<7)));\r\n");	
			break;
		}
	}
	Timer_StopTimer(TIMER1);
//	printf("AFTER while((RADIO->STATUS & (1<<7)));\r\n");	

}

void replyOTAStatus(uint8_t result, uint8_t index, uint8_t times)
{

	uint8_t txbuf[64] = {0};
	int i;
	Oneshoutflag = 0;		//TIme out flag
	
	startTimer(TIMER1, 100);		//Start receive timeout timer
		
	txbuf[0] = gTxSeqNo++;
	txbuf[1] = RF_PT_OVER_THE_AIR_REPLY;
	txbuf[2] = index;
	txbuf[3] = result;
//	GetTagStatus(Statusbuf);
//	memcpy(&txbuf[3], &Statusbuf[0], 64-3);

	for(i=0; i<64; i++) {
		RADIO->TX_DATA[i] = txbuf[i]; //
	}
	
	for(i=0; i < times; i++) { //Reply TO GW/ RP
		StrobeCmd(CMD_TX); //entry tx & transmit
		Delay10us(1);
		while((RADIO->STATUS & (1<<7)))  //WTR
		{
			if(Oneshoutflag)
			{
				printf("TIMEOUT LOOP IS %d ",i);
//				printf("Oneshoutflag in OTA while((RADIO->STATUS & (1<<7)));\r\n");	
				break;
			}
		}
		if(Oneshoutflag)
		{
			printf("Oneshoutflag_02_OTA\r\n");	
			break;
		}
		Delay10us(1);
	}
	Timer_StopTimer(TIMER1);

}

void replyTagStatus(uint8_t result)
{
	uint8_t txbuf[64];
	int i;
	Oneshoutflag = 0;		//TIme out flag
	startTimer(TIMER1, 50);		//Start receive timeout timer
	
	txbuf[0] = gTxSeqNo++;
	txbuf[1] = RF_PT_TAG_STATUS_REPORT;
	txbuf[2] = result;

	GetTagStatus(Statusbuf);
	GetTagFirmwareVersion(Statusbuf);
	memcpy(&txbuf[3], &Statusbuf[0], 64-3);

	
	for(i=0; i<64; i++) {
		RADIO->TX_DATA[i] = txbuf[i]; //
	}
	for(i=0; i<10; i++) { //Reply TO GW/ RP
		StrobeCmd(CMD_TX); //entry tx & transmit
		Delay10us(1);
		while((RADIO->STATUS & (1<<7)))  //WTR
		{
			if(Oneshoutflag)
			{
//				printf("%d ",i);
				printf("Oneshoutflag in while((RADIO->STATUS & (1<<7)));\r\n");	
				break;
			}
		}
		if(Oneshoutflag)
		{
			printf("Oneshoutflag_02\r\n");	
			break;
		}
		Delay10us(1);
	}
	Timer_StopTimer(TIMER1);	
}

uint8_t GetRfChannel(void)		
{
	printf("RADIO->CHANNEL = %d\r\n", *RF_CHN);
	return *RF_CHN;//pStatusbuf->RF_CHANNEL;
}

uint8_t GetChipRfChannel(void)		
{
//	printf("RADIO->CHANNEL = %d\r\n", RADIO->CHANNEL);
	return RADIO->CHANNEL;
}

uint8_t NeedChangeRfChannel(void)
{
	m_uiScanChannelRetryCount++;
	if(m_uiScanChannelRetryCount >= 8)
	{
		 SetCH(GetChipRfChannel() + 2);
		printf("New Channel;\r\n");	
		return 1;
	}
	return 0;
}

uint8_t GetTagWakeUpInterval(void)
{
	return TagWakeUpInterval;
}
uint8_t GetDefaultTagWakeUpInterval(void)
{
	return *DefaultTagWakeUpInterval;
}
uint8_t GetLongSleepAlarmHour(void)
{
	return *LongSleepAlarmHour;
}
uint8_t GetLongSleepAlarmMin(void)
{
	return *LongSleepAlarmMin;
}

uint8_t GetColor(void)
{
	return ColorFromPacket;
}
uint8_t GetRfDatarate(void)		
{
	return *gDatarate;//pStatusbuf->RF_CHANNEL;
}

uint8_t GetPacketCount(void)
{
	return PacketCount;
}

uint8_t GetOtaPacketIndex(void)
{
	return OTA_Data_index;
}	

uint16_t GetBinFileSize(void)
{
	return OTA_BinFileSize;
}	

uint8_t GetTotalOfPacket(void)
{
	return OTA_TotalOfPacket;
}	

void SetOtaStatusDone(void)
{
	TAG_RCV_OTA = 0;
}

uint8_t GetEpdColor(void)
{
	return *EPD_Color;
}

uint8_t GetEpdSize(void)
{
	return *EPD_Size;
}

uint8_t GetCurrentIndex(void)
{
	return mindex;
}

void SetCurrentIndexZero()
{
	mindex = 0;
}



uint8_t GetTempBase(void)
{
	return *TempBase;
}


uint16_t GetAdcBase(void)
{
	return *AdcBase;
}


#if 0
/*********************************************************************
** RxPacket
*********************************************************************/
uint8_t RxPacket(void)
{
	uint8_t recv[64]; 
	int i;
	uint8_t mtag;
	uint8_t mhopcount;
	uint8_t mResult = 0;
	P_TAG_RCV_BEACON pTagStatus; //= (P_TAG_RCV_BEACON)recv;
	
	for(i=0; i <64; i++)	
  {
		recv[i] = RADIO->RX_DATA[i];	
	}

	/** check Beacon **/
//	updateflag = 0;
	gTagSeq = 10;		//Spenser - Max. Tags update one second.
	switch(recv[1]) {
		case SYNCBEACON:	
		case ALWAYSWAKEBEACON:	
#if 0			
			printf("\r\n");	
			for(i=0; i <64; i++)	
		  {
				printf("%X ", recv[i]);	
			}				
			printf("\r\n");
			printf("Received packet. %x:%x:%x:%x:%x\r\n", TagID[0],TagID[1],TagID[2], TagID[3], TagID[4]);
			printf("Received packet. %X  %x:%x:%x:%x:%x\r\n", recv[2], recv[31],recv[32],recv[33], recv[34], recv[35]);
#endif			

			pTagStatus = (P_TAG_RCV_BEACON)recv;
//			printf("Received packet!!. %x:%x:%x:%x:%x\r\n", pTagStatus->BeaconSeq,pTagStatus->BeaconType,pTagStatus->PktCtrl, pTagStatus->ChnCtrl, pTagStatus->WakeUpTime);
			mtag = recv[2] & 0x7;		//want update number
			if(mtag > 0) {
				//have update				
		
				for(i=0; i<mtag; i++){
						if(TagIdCompare(pTagStatus->TAGID,TagID,i))
						{
//					if((recv[11+(i*5)] == TagID[0]) && (recv[12+(i*5)] == TagID[1]) && (recv[13+(i*5)] == TagID[2]) && 
//						(recv[14+(i*5)] == TagID[3]) && (recv[15+(i*5)] == TagID[4])) {
//						printf("Received packet. %x:%x:%x:%x:%x:%x:%x\r\n", recv[11],recv[12],recv[13], recv[14], recv[15],recv[6],recv[7]);
						updateflag = 1;
						setSynTimer(1, updateflag);		//Sync time sec	
						//Match update
						mResult = HITID;
						gTagSeq = (uint8_t)i;		//Spenser - Save Hit Tag sequence number
//							printf("gTagSeq = %d\n", gTagSeq);
						//if(gTagSeq > 0) printf("gggTagSeq = %d\n", gTagSeq);
						memcpy(&AgentID[0], &recv[6], 5);
						memcpy(&RepeaterID[0], &recv[6], 5);
						TagWakeUpInterval = recv[4];	
						TAG_UPDATED = 0;
						break;
					}
					else // No Hit ID 
					{
						if(TAG_UPDATED == 1)
						{
							setSynTimer(TagWakeUpInterval, 0);		//Load Previous Sleep Time
						}
						else
						{
							setSynTimer(1, 0);		//Sync time sec	
						}
						/* Mark by Spenser for multiple Tags update*/
						
//						GetTagStatus(Statusbuf);
						/* Mark by Spenser for multiple Tags update*/
						updateflag = 0;
//						return mResult;
					}
				}							
			}
			else //mtag = 0
			{
				if(TAG_UPDATED == 1)
				{
//					printf("MTAG < 0 SET 10.\r\n");
//					setSynTimer(TagWakeUpInterval, 0);
//					setSynTimer(40, 0);		//Load Previous Sleep Time
					setSynTimer(20, 0);		//Load Previous Sleep Time
				}
				else
				{
					setSynTimer(1, 0);		//Sync time sec	
				}
				updateflag = 0;
				mResult = NOHITID;
				return mResult;
			}
			if(recv[1] == SYNCBEACON)
			{
				mindex = recv[0];			// Beacon seqno
				gTagDonotSleep = 0;
			}
			else if(recv[1] == ALWAYSWAKEBEACON)
			{
				/* RSSI_measurement */
				gRSSIvalue = RSSI_measurement();
				mindex = recv[0];			// Beacon seqno
				gTagDonotSleep = 1;
			}

			break;

		case HOPPINGBEACON:		//Sync. hop channel			
			setSynTimer(recv[4], updateflag);		//Sync time sec
			mhopcount = recv[3] & 0xF;
			if(ghopcount == 0) {			//first time received hopping sync beacon
					ghopcount = mhopcount;
					gCHindex = (recv[3] >> 4) & 0xF;
			}
			//setSynTimer(recv[4], updateflag);		//Sync time sec
			mResult = HOPPING;
			//gState = HOPPM1_STATE;
			break;
						
		case PACKETHEADER:
			
			if(gState != HEADER_STATE){
				printf("Not inside HEADER_STATE %d!\r\n",gState);
//				if(gState == SYNCBEACON) {
					Oneshoutflag = 1; //Spenser - not current state like receive timeout 
					mResult = NOHITID;
//				}
				break;
			} 
			mindex = recv[0];
//			printf("Received Header packet. %x:%x:%x:%x:%x\r\n", recv[2],recv[3], recv[4], recv[5],recv[6]);
			if((recv[2] == TagID[0]) && (recv[3] == TagID[1]) && (recv[4] == TagID[2]) && (recv[5] == TagID[3]) && (recv[6] == TagID[4])) {
//				printf("Received Header packet.\r\n");
				memcpy(&DataLen, &recv[12], 2);
//				printf("Received Header packet. %x:%x:%x:%x\r\n", recv[18], recv[19], recv[14],recv[15]);
			 //printf("Received Header packet. %x:%x:%x:%x\r\n", recv[12], recv[13], recv[14],recv[15]);
				pEpdbuf = &EpdRFData[0];
				
				mResult = HITID;
				//gState = DATA_STATE;
			}else {
				mResult = NOHITID;
				//gState = ENTERPM1_STATE;
			}			
			break;
			
		case DATAPACKET:
			if(gState != DATA_STATE) {
				printf("Not inside DATA_STATE %d!\r\n",gState);
					Oneshoutflag = 1; //Spenser - not current state like receive timeout 
					mResult = NOHITID;
//				if(gState == BEACON_STATE || gState == PWRBEACON_STATE) {
////					Oneshoutflag = 1; //Spenser - not current state like receive timeout 
////					mResult = NOHITID;	
//					mResult = STATENOTRIGHT;
//				}
				break;
			} 
			if((recv[0] - mindex) > 1) {
//				printf("Lost some data...%x - %x\r\n", recv[0], mindex);
				mResult = PACKET_LOST;		//indicate packet lost
			}
			mindex = recv[0];
			
			memcpy(pEpdbuf, &recv[2], 62);		//62: -two bytes header, 64-2
			pEpdbuf += 62;
			
			break;
			
		case TAGSTATUSBEACON:		//Sync. hop channel	
//	printf("Received packet. %x:%x:%x:%x:%x:%x:%x\r\n", recv[11],recv[12],recv[13], recv[14], recv[15],recv[6],recv[7]);				
			if((recv[11] == TagID[0]) && (recv[12] == TagID[1]) && (recv[13] == TagID[2]) && 
				(recv[14] == TagID[3]) && (recv[15] == TagID[4])) {
				setSynTimer(recv[4], 0);
				memcpy(&AgentID[0], &recv[6], 5);
				memcpy(&RepeaterID[0], &recv[6], 5);
				replyTagStatus(RXCOMPLETED);
				mResult = REPORTSTATUSOK;
				TagWakeUpInterval = recv[4];
				updateflag = 1;					//Match update
				break;
			}
			else
			{
				setSynTimer(recv[4], 0);
//				setSynTimer(ReadAndWritePreviousSleepTime(), 0);		//Sync time sec	
				GetTagStatus(Statusbuf);
				mResult = NOHITID;
				updateflag = 0;					//Match update
//				replyTagStatus(mResult);
			}
			break;		

		default:
			printf("Received dumming packet.\r\n");
			break;	
	}
	return mResult;
}


#endif
