/***********************************************************************
 * RFpacket.c
 *
 ***********************************************************************/
#include "main.h"
#include "tagdefine.h"
#include "RfPacket.h"
#include "Board.h"

//const uint8_t TagID[5] = {0,0,0,0,1};				//35
//const uint8_t TagID[5] = {0,0,0,0,5};				//32000
//const uint8_t TagID[5] = {0,0,0,0,0xC};		//65000
//const uint8_t TagID[5] = {0,0,0,0,0xA};		//10200
//const uint8_t TagID[5] = {8,0,8,0,9};			//1999
const uint8_t TagID[5] = {1,0,0,0,7};			//35000

uint8_t GatewayID_UQ[3] = {0xFF, 0xFF, 0xFF};			//1999
uint8_t RepeaterID_UQ[3] = {0xFF, 0xFF, 0xFE};			//1999	
uint8_t AgentID[2];
uint8_t RepeaterID[2];
uint8_t Statusbuf[64];
uint8_t updateflag = 0;
uint8_t gState;
uint8_t gTagDonotSleep = 0;
uint16_t DataLen;
//uint8_t EpdRFData[4736];
uint8_t *pEpdbuf;

uint8_t mindex;							//for packet lost check by sequence number
//uint8_t gRxPktResult = 0;		//packet lost flag
uint8_t gTxSeqNo = 0;				//TX result sequce number

uint8_t ghopcount;
uint8_t TAG_UPDATED = 0;
uint8_t TAG_RCV_HIT_ID = 0;
uint8_t TAG_RCV_NO_HIT_ID = 0;
uint8_t gCHindex;
uint32_t gRSSIvalue;			//current RSSI value
uint16_t UpdateCount = 0;				
uint16_t RebootCount = 0;	
uint16_t UpdateFailCount = 0;

extern uint8_t Oneshoutflag;		//RX timeout flag
extern uint32_t gBeaconwait;		//Receive Beacon timeout ms. 
extern uint8_t gDatarate;
extern uint8_t EpdRFData[];
uint8_t TagWakeUpInterval = 1;
uint8_t AllWakeUpTime = 1;
uint8_t WakeUpTimeInList = 1;
uint8_t DefaultWakeUpTime = 40;

/* Spenser added */
extern uint8_t gTagSeq;

/***************************************************************************
 * setSleepTimer(uint32_t sec)
 *		Just supports sec = 1, 2, 5, 10, 20, 40
 *	sec = 2, SLPTIMER1 is 16 bits counter
 *		cks = 0, BEACONTIMEOUT = 2, magicno = 68, sleep time = 1, 2 sec
 *		cks = 1, BEACONTIMEOUT = 3, magicno = 15, sleep time = 10 sec
 *		cks = 2, BEACONTIMEOUT = 2, magicno = 5, sleep time = 20 sec
 *		cks = 3, BEACONTIMEOUT = 2, magicno = 1, sleep time = 40 sec
 ***************************************************************************/
//void setSynTimer(uint32_t sec, uint8_t hitflag)
//{
//	switch(sec) {
//		case 1:		
//			switch(gDatarate) {
//				case DATARATE1M:	
//#if 1						
//					if(hitflag){		//update EPD need about 5 sec.
//						SLPTIMER_Initial(SLPTIMER1, 19642, 1, 1); //JERRY
//					}else SLPTIMER_Initial(SLPTIMER1, (32768 - 69), 0, 1);	//68
//#endif
//					//SLPTIMER_Initial(SLPTIMER1, (32768 - 68), 0, 1);	//68
//					gBeaconwait = 3;		//3ms
//					break;
//					
//				case DATARATE500K:
//#if 1		//Spenser			
//					if(hitflag){		//update EPD need about 5 sec.
//						SLPTIMER_Initial(SLPTIMER1, (19642-2), 1, 1); //JERRY
//					}else SLPTIMER_Initial(SLPTIMER1, (6533 - 2), 1, 1); //JERRY//SLPTIMER_Initial(SLPTIMER1, (32768 - 88), 0, 1);	//68
//#endif                                     //(6533 - 2)
//					
//					//SLPTIMER_Initial(SLPTIMER1, 19642, 1, 1);
//					//SLPTIMER_Initial(SLPTIMER1, 6532, 1, 1);
//					gBeaconwait = 3;		//2ms
//					break;
//					
//				case DATARATE250K:
//#if 1					
//					if(hitflag){		//update EPD need about 5 sec.
//						SLPTIMER_Initial(SLPTIMER1, 19629, 1, 1); //JERRY
//					}else SLPTIMER_Initial(SLPTIMER1, (32768 - 106), 0, 1);	//106
//#endif
//					//SLPTIMER_Initial(SLPTIMER1, (32768 - 106), 0, 1);	//106
//					gBeaconwait = 3;		//2ms
//					break;
//			}
//			break;
//		
//		case 2:
//			switch(gDatarate) {
//				case DATARATE1M:		
//					if(hitflag){		//update EPD need about 5 sec.
//						SLPTIMER_Initial(SLPTIMER1, 26195, 1, 1); //JERRY
//					}else SLPTIMER_Initial(SLPTIMER1, ((32768*2) - 68), 0, 1);
//					
//					gBeaconwait = 2;		//2ms
//					break;
//					
//				case DATARATE500K:
//#if 1				//Spenser	
//					if(hitflag){		//update EPD need about 5 sec.
//						SLPTIMER_Initial(SLPTIMER1, 26195, 1, 1); //JERRY
//					}else SLPTIMER_Initial(SLPTIMER1, 13085, 1, 1);//13085.6//else SLPTIMER_Initial(SLPTIMER1, 6532, 1, 1); //1s BACKUP
//#endif
//					//SLPTIMER_Initial(SLPTIMER1, ((32768*2) - 88), 0, 1);
//					gBeaconwait = 3;		//2ms
//					break;
//					
//				case DATARATE250K:
//#if 1					
//					if(hitflag){		//update EPD need about 5 sec.
//						SLPTIMER_Initial(SLPTIMER1, 26193, 1, 1); //JERRY
//					}else SLPTIMER_Initial(SLPTIMER1, ((32768*2) - 106), 0, 1);
//#endif
//					//SLPTIMER_Initial(SLPTIMER1, ((32768*2) - 106), 0, 1);
//					gBeaconwait = 3;		//2ms
//					break;
//			}
//			break;
//			
//		case 5:
//			switch(gDatarate) {
//				case DATARATE1M:		
//				  SLPTIMER_Initial(SLPTIMER1, 22927, 2, 1); //JERRY//7S
//					gBeaconwait = 2;		//2ms
//					break;
//					
//				case DATARATE500K:
//					if(hitflag){		//update EPD need about 5 sec.
//						SLPTIMER_Initial(SLPTIMER1, 22927, 2, 1); //JERRY//7S
//					}else SLPTIMER_Initial(SLPTIMER1, 16372, 2, 1);//else SLPTIMER_Initial(SLPTIMER1, 6532, 1, 1); 
//					gBeaconwait = 3;		//2ms
//					break;
//					
//				case DATARATE250K:
//					SLPTIMER_Initial(SLPTIMER1, 22927, 2, 1); //JERRY//7S
//					gBeaconwait = 3;		//2ms
//					break;
//			}
//			break;
//			
//		case 10:
//			switch(gDatarate) {
//				case DATARATE1M:		
//					SLPTIMER_Initial(SLPTIMER1, 39313, 2, 1); //JERRY//12S
//					gBeaconwait = 2;		//2ms
//					break;
//					
//				case DATARATE500K:
//					if(hitflag){		//update EPD need about 5 sec.
//					SLPTIMER_Initial(SLPTIMER1, 39313, 2, 1); //JERRY//12S
//					}else SLPTIMER_Initial(SLPTIMER1, (32757 - 1), 2, 1);//else SLPTIMER_Initial(SLPTIMER1, 6532, 1, 1);
//					gBeaconwait = 3;		//2ms
//					break;
//					
//				case DATARATE250K:
//					SLPTIMER_Initial(SLPTIMER1, 39310, 2, 1); //JERRY//12S
//					gBeaconwait = 3;		//2ms
//					break;
//			}
//			break;
//			
//		case 20:
//			switch(gDatarate) {
//				case DATARATE1M:		
//					SLPTIMER_Initial(SLPTIMER1, 36041, 3, 1); //JERRY//22S
//					gBeaconwait = 2;		//2ms
//					break;
//					
//				case DATARATE500K:
//					if(hitflag){		//update EPD need about 5 sec.
//						SLPTIMER_Initial(SLPTIMER1, 36041, 3, 1); //JERRY//22S
//					}SLPTIMER_Initial(SLPTIMER1, (32764 - 2), 3, 1);//else SLPTIMER_Initial(SLPTIMER1, 6532, 1, 1);
//					gBeaconwait = 3;		//2ms
//					break;
//					
//				case DATARATE250K:
//					SLPTIMER_Initial(SLPTIMER1, 36040, 3, 1); //JERRY//22S
//					gBeaconwait = 3;		//2ms
//					break;
//			}
//			break;
//			
//		case 40:
//			switch(gDatarate) {
//				case DATARATE1M:		
//					SLPTIMER_Initial(SLPTIMER1, 65533, 3, 1); //JERRY//22S
//					gBeaconwait = 2;		//2ms
//					break;
//					
//				case DATARATE500K:

//					if(hitflag){		//update EPD need about 5 sec.
//					 SLPTIMER_Initial(SLPTIMER1, 65533, 3, 1); //JERRY//22S
//					 //SLPTIMER_Initial(SLPTIMER1, 26195, 1, 1); //Spenser
//					}SLPTIMER_Initial(SLPTIMER1, (65533-5), 3, 1);//else SLPTIMER_Initial(SLPTIMER1, 6532, 1, 1);
//					gBeaconwait = 3;		//2ms
//					break;
//					
//				case DATARATE250K:
//					SLPTIMER_Initial(SLPTIMER1, 65531, 3, 1); //JERRY//22S
//					gBeaconwait = 3;		//2ms
//					break;
//			}
//			break;
//			
//		default:
//			switch(gDatarate) {
//				case DATARATE1M:		
//					SLPTIMER_Initial(SLPTIMER1, (32768 - 68), 0, 1);	//68
//					gBeaconwait = 2;		//2ms
//					break;
//					
//				case DATARATE500K:
//#if 1					
//					if(hitflag){		//update EPD need about 5 sec.
//						SLPTIMER_Initial(SLPTIMER1, 19642, 1, 1); //JERRY
//					}else SLPTIMER_Initial(SLPTIMER1, 6532, 1, 1); //JERRY//SLPTIMER_Initial(SLPTIMER1, (32768 - 88), 0, 1);	//68
//#endif
//					//SLPTIMER_Initial(SLPTIMER1, (32768 - 88), 0, 1);
//					gBeaconwait = 3;		//2ms					
//					SLPTIMER_Initial(SLPTIMER1, ((32768*2) - 88), 0, 1);
//					gBeaconwait = 3;		//2ms
//					break;
//					
//				case DATARATE250K:
//					SLPTIMER_Initial(SLPTIMER1, ((32768*2) - 106), 0, 1);
//					gBeaconwait = 3;		//2ms
//					break;
//			}
//			break;
//			
//	}

//}
void setSynTimer(uint32_t sec, uint8_t hitflag)
{
	switch(sec) {
		case 1:		
			switch(gDatarate) {
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
						SLPTIMER_Initial(SLPTIMER1, (19642-10), 1, 1); //JERRY //(19642-2)
					}else if(hitflag == 2)
					{
						SLPTIMER_Initial(SLPTIMER1, (6533 - 8), 1, 1); //JERRY//SLPTIMER_Initial(SLPTIMER1, (32768 - 88), 0, 1);	//68
					}
					else
					{
						SLPTIMER_Initial(SLPTIMER1, (6533 - 2), 1, 1);
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
			switch(gDatarate) {
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
			switch(gDatarate) {
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
			switch(gDatarate) {
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
			switch(gDatarate) {
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
			switch(gDatarate) {
				case RF_DATARATE_1M:		
					SLPTIMER_Initial(SLPTIMER1, 65533, 3, 1); //JERRY//22S
					gBeaconwait = 2;		//2ms
					break;
					
				case RF_DATARATE_500K:
					if(hitflag == 1){		//update EPD need about 5 sec.
						SLPTIMER_Initial(SLPTIMER1, 65533, 3, 1); //JERRY//22S
					}
					else if(hitflag == 2)
					{
						SLPTIMER_Initial(SLPTIMER1, (65533-12), 3, 1);
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
			switch(gDatarate) {
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

#if 1
/*********************************************************************
** RxPacket
*********************************************************************/
uint8_t RxPacket(void)
{
	uint8_t recv[64]; 
	int i;
	uint8_t payloadLength = 64;
	uint8_t mtag;
	uint8_t mhopcount;
	uint8_t mResult = 0;
	//P_TAG_RCV_BEACON pTagStatus; //= (P_TAG_RCV_BEACON)recv;
	P_SYNC_BEACON pSyncBeacon;
	P_TAG_UPDATE_LIST pTagUpdateList;
	P_HEADER_PACKET pHeaderPacket;
	if(gState == PWRBEACON_STATE || gState == BEACON_STATE)
	{
		payloadLength = 16;
	}
	for(i=0; i <payloadLength; i++)	
  {
		recv[i] = RADIO->RX_DATA[i];	
	}
//			printf("Received Header packet. %x:%x:%x:%x:%x:%x\r\n",recv[0],recv[1],recv[2],recv[3], recv[4], recv[5]);
	/** check Beacon **/
//	updateflag = 0;
	gTagSeq = 10;		//Spenser - Max. Tags update one second.
	switch(recv[1]) {
		case RF_PT_SYNC_BEACON:	
			pSyncBeacon = (P_SYNC_BEACON)recv;
			switch(pSyncBeacon->Action)
			{
				case RF_AL_SYNC:
//					printf("SYNC.\r\n");
					if(TAG_UPDATED == 1)
					{
						//Load Previous Sleep Time
//						setSynTimer(TagWakeUpInterval, 0);		//Load Previous Sleep Time
						setSynTimer(1, 0);
					}
					else
					{
						setSynTimer(1, 0);		//Sync time sec	
					}
				  mResult = TAG_MSG_ONLY_SYNC;
				break;
				case RF_AL_UPDATE_TAG:
//					printf("UPDATE_TAG.\r\n");
//					setSynTimer(1, 1);		//Sync time sec	
				  mResult = TAG_MSG_TO_RCV_TAG_LIST;
				break;					
//				case UPDATE_TAG:
//					if(TAG_RCV_NO_HIT_ID == 2) //HITTED ID
//					{
//						if(TAG_UPDATED) //HIT AND Success
//						{
////							setSynTimer(WakeUpTimeInList, 0); //Ref.
//							setSynTimer(2, 0); //Ref.
//						}
//						else //hit but Fail
//						{
//							setSynTimer(1, 1);		//Prepare to Update.
//						}
//					}
//					else if(TAG_RCV_NO_HIT_ID == 1)
//					{
//						if(TAG_UPDATED) //Success
//						{
//							setSynTimer(WakeUpTimeInList, 0); //Ref.
////							setSynTimer(5, 0); //Ref.
//						}
//						else //Fail
//						{
//							setSynTimer(1, 1);		//Prepare to Update.
//						}
//					}
//					else // never or first time.
//					{
//						setSynTimer(1, 1);		//Prepare to Update.
//					}
//							
////					if(TAG_UPDATED == 1)
////					{
////						//Load Previous Sleep Time
//////						setSynTimer(TagWakeUpInterval, 0);		//Load Previous Sleep Time
////						setSynTimer(10, 0);
////					}
////					else
////					{
////						setSynTimer(1, 1);		//Prepare to Update.
////					}
//////					setSynTimer(1, 1);		//Sync time sec	
//////					TagWakeUpInterval = pSyncBeacon->WakeUpTime;
//				  mResult = TORCVTAGLIST;
//				break;
				
				case RF_AL_GET_TAG_STATUS:
//					printf("GET_TAG_STATUS..\n");
				  mResult = TAG_MSG_TO_RCV_TAG_LIST;
				break;		
				case RF_AL_ALL_WU_TIME:
					setSynTimer(pSyncBeacon->WakeUpTime, 0);
					mResult = TAG_MSG_ONLY_SYNC;
					break;
//				case ALWAYSWAKEBEACON:
//					break;
				default:
					setSynTimer(DefaultWakeUpTime, 0);		//Load Default Sleep Time
				  mResult = TAG_MSG_NO_HIT_ID;
					break;
			}
			break;
		
#if 0
		case SYNCBEACON:	
			pSyncBeacon = (P_SYNC_BEACON)recv;
			switch(pSyncBeacon->Action)
			{
				case SYNC:
					setSynTimer(1, 0);		//Sync time sec	
				  mResult = NOHITID;
				break;
				case UPDATE_TAG:
//					setSynTimer(1, 1);		//Sync time sec	
				  mResult = TORCVTAGLIST;
				break;
				
				default:
					setSynTimer(1, 0);		//Sync time sec	
				  mResult = NOHITID;
					break;
			}
			break;
#endif			
			
//		case TAGUPDATELIST:	
//			pTagUpdateList = (P_TAG_UPDATE_LIST)recv;
//			switch(pSyncBeacon->Action)
//			{
//				case SYNC:
//					setSynTimer(1, 0);		//Sync time sec	
//				  mResult = NOHITID;
//				break;
//				
//				default:
//					break;
//			}
//			break;			
			
		//case SYNCBEACON:	
		case RF_PT_TAG_UPDATE_LIST: //TAGUPDATELIST:		
		case RF_PT_ALWAYS_WAKE_BEACON:	
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
//			printf("Received packet. %x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x\r\n", recv[0],recv[1],recv[2],recv[3],recv[4],recv[5],recv[6],recv[7],recv[8],recv[9],recv[10], recv[11], recv[12]);
			mtag = (pTagUpdateList->PktCtrl) & 0x7;		//want update number
			if(mtag > 0) {
				//have update				
		
				for(i=0; i<mtag; i++){
					if(TagIdCompare(pTagUpdateList->TAGID,TagID,i))
					{
//						printf("HIT ID!!!!!!!!!!!!!!!!!!!!!!11.\r\n");
//					if((recv[11+(i*5)] == TagID[0]) && (recv[12+(i*5)] == TagID[1]) && (recv[13+(i*5)] == TagID[2]) && 
//						(recv[14+(i*5)] == TagID[3]) && (recv[15+(i*5)] == TagID[4])) {
//						printf("Received packet. %x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x\r\n", recv[0],recv[1],recv[2],recv[3],recv[4],recv[5],recv[6],recv[7],recv[8],recv[9],recv[10], recv[11], recv[12]);
						updateflag = 1;
						setSynTimer(1, updateflag);		//Sync time sec	
						//Match update
						mResult = TAG_MSG_HIT_ID;
						gTagSeq = (uint8_t)i;		//Spenser - Save Hit Tag sequence number
//							printf("gTagSeq = %d\n", gTagSeq);
						//if(gTagSeq > 0) printf("gggTagSeq = %d\n", gTagSeq);
						memcpy(&AgentID[0], pTagUpdateList->GatewayID, 2);
						memcpy(&RepeaterID[0], pTagUpdateList->RepeaterID, 2);
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
//							setSynTimer(TagWakeUpInterval, 2);		//Load Previous Sleep Time
//							setSynTimer(20, 2);		//Load Previous Sleep Time
								setSynTimer(1, 2);		//Load Previous Sleep Time
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

		case RF_PT_HOPPING_BEACON:		//Sync. hop channel			
			setSynTimer(recv[4], updateflag);		//Sync time sec
			mhopcount = recv[3] & 0xF;
			if(ghopcount == 0) {			//first time received hopping sync beacon
					ghopcount = mhopcount;
					gCHindex = (recv[3] >> 4) & 0xF;
			}
			//setSynTimer(recv[4], updateflag);		//Sync time sec
			mResult = TAG_MSG_HOPPING;
			//gState = HOPPM1_STATE;
			break;
						
		case RF_PT_PACKET_HEADER:
			pHeaderPacket = (P_HEADER_PACKET)recv; //P_HEADER_PACKET pHeaderPacket;
//			printf("Received Header packet. %x:%x:%x:%x:%x\r\n", recv[2],recv[3], recv[4], recv[5],recv[6]);
			if(gState != HEADER_STATE){
				printf("Not inside HEADER_STATE %d!\r\n",gState);
//				if(gState == SYNCBEACON) {
					Oneshoutflag = 1; //Spenser - not current state like receive timeout 
					mResult = TAG_MSG_NO_HIT_ID;
//				}
				break;
			} 
			mindex = recv[0];
//			printf("Received Header packet. %x:%x:%x:%x:%x\r\n", recv[2],recv[3], recv[4], recv[5],recv[6]);
			//if((recv[2] == TagID[0]) && (recv[3] == TagID[1]) && (recv[4] == TagID[2]) && (recv[5] == TagID[3]) && (recv[6] == TagID[4])) {
			if(TagIdCompare(pHeaderPacket->TagID,TagID,0))
			{
				DataLen = pHeaderPacket->PictureSize;
//				printf("Received Header packet.\r\n");
//				memcpy(&DataLen, &recv[12], 2);
//				memcpy(DataLen, pHeaderPacket->PictureSize, 1);
//				printf("Received Header packet. %x:%x:%x:%x\r\n", recv[18], recv[19], recv[14],recv[15]);
			 //printf("Received Header packet. %x:%x:%x:%x\r\n", recv[12], recv[13], recv[14],recv[15]);
				pEpdbuf = &EpdRFData[0];
				
				mResult = TAG_MSG_HIT_ID;
				//gState = DATA_STATE;
			}else {
				mResult = TAG_MSG_NO_HIT_ID;
				//gState = ENTERPM1_STATE;
			}			
			break;
			
		case RF_PT_DATA_PACKET:
			if(gState != DATA_STATE) {
				printf("Not inside DATA_STATE %d!\r\n",gState);
					Oneshoutflag = 1; //Spenser - not current state like receive timeout 
				mResult = TAG_MSG_NO_HIT_ID;
//				if(gState == BEACON_STATE || gState == PWRBEACON_STATE) {
////					Oneshoutflag = 1; //Spenser - not current state like receive timeout 
////					mResult = NOHITID;	
//					mResult = STATENOTRIGHT;
//				}
				break;
			} 
			if((recv[0] - mindex) > 1) {
//				printf("Lost some data...%x - %x\r\n", recv[0], mindex);
				mResult = TAG_MSG_PACKET_LOST;		//indicate packet lost
			}
			mindex = recv[0];
			
			memcpy(pEpdbuf, &recv[2], 62);		//62: -two bytes header, 64-2
			pEpdbuf += 62;
			
			break;
			
//		case TAGSTATUS:		//Sync. hop channel	
////	printf("Received packet. %x:%x:%x:%x:%x:%x:%x\r\n", recv[11],recv[12],recv[13], recv[14], recv[15],recv[6],recv[7]);				
//			if((recv[11] == TagID[0]) && (recv[12] == TagID[1]) && (recv[13] == TagID[2]) && 
//				(recv[14] == TagID[3]) && (recv[15] == TagID[4])) {
//				setSynTimer(recv[4], 0);
//				memcpy(&AgentID[0], &recv[6], 5);
//				memcpy(&RepeaterID[0], &recv[6], 5);
//				replyTagStatus(RXCOMPLETED);
//				mResult = REPORTSTATUSOK;
//				TagWakeUpInterval = recv[4];
//				updateflag = 1;					//Match update
//				break;
//			}
//			else
//			{
//				setSynTimer(recv[4], 0);
////				setSynTimer(ReadAndWritePreviousSleepTime(), 0);		//Sync time sec	
//				GetTagStatus(Statusbuf);
//				mResult = NOHITID;
//				updateflag = 0;					//Match update
////				replyTagStatus(mResult);
//			}
//			break;	

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
						setSynTimer(1, 0);		//Sync time sec	
						//Match update
						mResult = TAG_MSG_HIT_ID;
						gTagSeq = (uint8_t)i;		//Spenser - Save Hit Tag sequence number
//							printf("gTagSeq = %d\n", gTagSeq);
						//if(gTagSeq > 0) printf("gggTagSeq = %d\n", gTagSeq);
						memcpy(&AgentID[0], pTagUpdateList->GatewayID, 2);
						memcpy(&RepeaterID[0], pTagUpdateList->RepeaterID, 2);
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
						setSynTimer(TagWakeUpInterval, 0);
						GetTagStatus(Statusbuf);
				mResult = TAG_MSG_NO_HIT_ID;
					}
				}							
			}

			break;			

		default:
			printf("Received Header packet. %x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x \r\n",recv[0],recv[1],recv[2],recv[3], recv[4], recv[5], recv[6], recv[7], recv[8],recv[9],recv[10], recv[11], recv[12], recv[13], recv[14], recv[15]);
//			printf("Received dumming packet.\r\n");
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
	
	SetTimer(TIMER0, mSec);		//Start receive timeout timer

	while(1) {
		if(WTRFlag == 0) {		//Wait CMD_RX action, WTR = 1.
			if((RADIO->STATUS & (1<<7))) WTRFlag = 1;
		}else {
			if(!(RADIO->STATUS & (1<<7)) || Oneshoutflag) {		//WTR = 0.
				Timer_StopTimer(TIMER0);
				StrobeCmd(CMD_STBY);				//Back to Standby
				if(!Oneshoutflag) {		//Received packet
					
					RxPktResult = RxPacket();
				}else {		//Time out
					printf("Wait RX timeout.\r\n");
					if(gState == BEACON_STATE) {
						//gBeaconwait += 20;
						//if(gBeaconwait > 700) gBeaconwait = 700;
						//printf("gBeaconwait = %d\r\n", gBeaconwait);
					}
//					RxPktResult = RXTIMEOUT;
					return TAG_MSG_RX_TIMEOUT;
				}
				break;
			}	
		}
	}
	return RxPktResult;
}

/*******************************************************
 * getdatapacket() - Get one Tag refresh data.  Spenser
 *
 *******************************************************/
void getdatapacket(void)
{
	
	uint8_t WTRFlag = 0;

	while(1) {
		if(WTRFlag == 0) {		//Wait CMD_RX action, WTR = 1.
			if((RADIO->STATUS & (1<<7))) WTRFlag = 1;
		}else {
			if(!(RADIO->STATUS & (1<<7)) || Oneshoutflag) {		//WTR = 0.
				
				StrobeCmd(CMD_STBY);				//Back to Standby
				if(!Oneshoutflag) {		//Received packet
					
					RxPacket();
				}else {		//Time out
					printf("Data packet timeout.\r\n");
					
				}
				break;
			}	
		}
	}

}

void replypacket(uint8_t result)
{

	uint8_t txbuf[64];
	int i;

	txbuf[0] = gTxSeqNo++;
	txbuf[1] = RF_PT_UPDATE_STATUS_REPORT;
	txbuf[2] = result;
	
	GetTagStatus(Statusbuf);
	memcpy(&txbuf[3], &Statusbuf[0], 64-3);
	
	for(i=0; i<64; i++) {
		RADIO->TX_DATA[i] = txbuf[i]; //
	}
	StrobeCmd(CMD_TX); //entry tx & transmit
	Delay10us(1);

	while((RADIO->STATUS & (1<<7))); 	//WTR

}

void replyTagStatus(uint8_t result)
{
	uint8_t txbuf[64];
	
	int i;
	txbuf[0] = gTxSeqNo++;
	txbuf[1] = RF_PT_TAG_STATUS_REPORT;
	txbuf[2] = result;

	GetTagStatus(Statusbuf);
	memcpy(&txbuf[3], &Statusbuf[0], 64-3);

	
	for(i=0; i<64; i++) {
		RADIO->TX_DATA[i] = txbuf[i]; //
	}
	StrobeCmd(CMD_TX); //entry tx & transmit
	Delay10us(1);
	while((RADIO->STATUS & (1<<7))); 	//WTR
	WriteStatusToFlash(Statusbuf);
	
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
