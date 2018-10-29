/***********************************************************************
 * RFpacket.c
 *
 ***********************************************************************/
#include "main.h"
#include "RptProc.h"
#include "isr.h"
#include <math.h>
#include "RptDataflash.h"
#include "A7127RF_Drv.h"

uint8_t EpdRFData[EpdUpdateSize] = {0};
uint8_t *pEpdBuf = (uint8_t *)EpdRFData;

uint8_t TagID[30] = {0}; // = {0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t AgentID[5]; // = {0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t RepeaterID[5] = {0xFF, 0xFF, 0xFE, 0x00, 0x01};//{0x52, 0x00, 0x00, 0x00, 0x01};

uint8_t mindex; //for packet lost check
uint8_t ghopcount;
uint8_t gCHindex;
//uint8_t gRxPktResult = 0;		//packet lost flag
//uint8_t updateflag = 0;
//uint8_t	LossCnt = 0;			//Lost Beacon count -- SPenser
//uint8_t TagWakeUpInterval;
//uint8_t RxTimeOutCnt = 0;
//uint8_t gTagSeq = 0;
//uint16_t UpdateCount = 0;
//uint16_t RebootCount = 0;		
//uint16_t UpdateFailCount = 0;	
//uint32_t gRSSIvalue = 0;			//current RSSI value
//int16_t DataLen = 0;

/*
 * Jerry add
*/
extern uint8_t AES_KEY[];
extern void RstTimerValue(TIMER_Type *timer, uint32_t msec);
extern void SetOneShotTimer1(uint32_t msec);
extern void printTime(void);

/**
  * @brief      Repeater receive update data lost or error reply to gateway
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used reply lost packet status to gateway.
  */
void RptRx_UpdPktLost(uint8_t result)
{
		//int i;
		uint8_t SeqNo = RptGlblVar.gTxSeqNo;
	
		RptGlblVar.TxBuffer[0] = SeqNo++;
		RptGlblVar.TxBuffer[1] = RF_PT_RX_DATA_RESULT;
		//memcpy(&RptGlblVar.TxBuffer[2], &TagID[0], 5);
		memcpy(&RptGlblVar.TxBuffer[2], &TagID[0 + ((RptGlblVar.UpdCntTmp2) * 5)], 5);
		RptGlblVar.TxBuffer[7] = result;		//reply RX data result
	
		/*for(i = 0; i < 64; i++) {
			RADIO->TX_DATA[i] = RptGlblVar.TxBuffer[i];
		}
		StrobeCmd(CMD_TX); //entry tx & transmit
		Delay10us(1);
		while((RADIO->STATUS & (1 << 7))); //WTR
		*/
	
		Rpt_TxPktProc(PAYLOAD_LEN, RptGlblVar.TxBuffer);
}

/**
  * @brief      Send repeater stauts to gatway
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used send repeater status to gateway.
  */
void Rpt_Rpy_RptStatus(uint8_t result)
{	
		uint8_t SeqNo = RptGlblVar.gTxSeqNo;
		P_RPT_STATUS_REPORT RptRpyStatus = (P_RPT_STATUS_REPORT)RptGlblVar.TxBuffer;
			
	RptRpyStatus->SeqNum = RptGlblVar.gTxSeqNo++;
		RptRpyStatus->PckType = RF_PT_REPEATER_STATUS;
		RptRpyStatus->Result = result;
	
		memcpy(&RptRpyStatus->GatewayID[0], &AgentID[3], 2);
		memcpy(&RptRpyStatus->RepeaterID[0], &RptDefSet.RptID[3], 2);
			
		RptRpyStatus->RptRSSI = RSSI_measurement();
		RptRpyStatus->GwCHN = RptDefSet.Gchn;
		RptRpyStatus->TagCHN = RptDefSet.Tchn;
		RptRpyStatus->RptDataRate = RptDefSet.RptDataRate;
		RptRpyStatus->RptMode = RPT_ONERF_ONE_CHN_MODE;
		memcpy(RptRpyStatus->TagID, &TagID[RptGlblVar.MultiTagCnt], 5);

		Rpt_TxPktProc(PAYLOAD_LEN, RptGlblVar.TxBuffer);
			
		RptGlblVar.MultiTagCnt += 5;
}

uint8_t Rpt_UpdDataType_Proc(uint8_t PktType, uint8_t *UpdDataBuf)
{
		bool ret;
		uint8_t mResult = 0, i = 0;
		P_RPT_UPD_TAG_HEAD pTagUpdHead;
			
		switch(PktType){
			case RF_PT_PACKET_HEADER:
				pTagUpdHead = (P_RPT_UPD_TAG_HEAD)UpdDataBuf;
	
				//ret = compare_array_date(&RptDefSet.RptID[3], &pTagUpdHead->RepeaterID[0], 2);
				//ret = check_array_zero(pTagUpdHead->RepeaterID, 2);
				ret = 1;
				if(ret == true){
					//DataLen = 0;
					//mindex = pTagUpdHead->Seqno;
					//DataLen = pTagUpdHead->PictureSize;
					RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2] = pTagUpdHead->PictureSize;
					//pEpdBuf = &EpdRFData[RptGlblVar.RxUpdBufPtrAddr];
					
					A7127_XmitPkt_Proc(PAYLOAD_LEN, UpdDataBuf);
					
					mResult = TAG_MSG_HIT_ID;
					//RptGlblVar.gState = DATA_STATE;
				}else {
					return TAG_MSG_NO_HIT_ID;
				}
				break;
			
			case RF_PT_DATA_PACKET:
				if((UpdDataBuf[0] - mindex) > 1) {
					DBGMSG("Lost some data...%x - %x\r\n", UpdDataBuf[0], mindex);
					return TAG_MSG_PACKET_LOST; //indicate packet lost
				}else{
					mindex = UpdDataBuf[0];	
					memcpy(pEpdBuf, &UpdDataBuf[2], PAYLOAD_LEN - 2);		//62: -two bytes header, 64-2
					pEpdBuf += (PAYLOAD_LEN - 2);
				}
				break;
				
			default:
				break;
		}

		return mResult;
}

uint8_t Rpt_TagListType_Proc(uint8_t *TagListBuf)
{
		bool ret;
		uint8_t mtag = 0, mResult = 0;
		P_TAG_UPDATE_LIST pTagUpdateList = (P_TAG_UPDATE_LIST)TagListBuf;

		//ret = compare_array_date(pTagUpdateList->RepeaterID, &RptDefSet.RptID[3], 2);
		//ret = check_array_zero(pTagUpdateList->RepeaterID, 2);
		ret = 1;
		
		memset(TagID, 0, sizeof(TagID));
		memset(RptGlblVar.RxTtlUpdDataLenBuf, 0, sizeof(RptGlblVar.RxTtlUpdDataLenBuf));
	
		switch(pTagUpdateList->PacketType){
			case RF_PT_TAG_UPDATE_LIST:
				if(ret == true){
					RptGlblVar.TxUpdTagCnt = RptGlblVar.UpdCntTmp1 = mtag = (pTagUpdateList->PktCtrl) & 0x7;		//want update number
					
					// Repeater cannot receive more than 3 updates
					/*if(mtag > RPT_RX_MAX_UPDATE){
						mtag = RptGlblVar.TxUpdTagCnt = RptGlblVar.UpdCntTmp1 = RPT_RX_MAX_UPDATE;
					}
					
					if(mtag > 0){ //have update
						//updateflag = 1;
						mResult = TAG_MSG_HIT_ID;  //Match update
						memcpy(&AgentID[3], pTagUpdateList->GatewayID, 2);
						memcpy(&TagID[0], &pTagUpdateList->TAGID[0], mtag * 5);
					}
					memcpy(RptGlblVar.TagLstBuf, TagListBuf, PAYLOAD_LEN); //Copy received Tag list data to buffer
					*/
					//A7127_XmitPkt_Proc(PAYLOAD_LEN, TagListBuf);
					
					mResult = TAG_MSG_HIT_ID;  //Match update
				}else{
					mResult = TAG_MSG_NO_HIT_ID;  //Match update
				}
				break;
				
			case RF_PT_TAG_STATUS:
				memcpy(RptGlblVar.TagLstBuf, TagListBuf, PAYLOAD_LEN); //Copy received Tag list data to buffer
				mResult = TAG_MSG_NO_HIT_ID;  //Match update
				break;
			
			case RF_PT_REPEATER_STATUS:
				if(ret == true){
					Rpt_Rpy_RptStatus(TAG_MSG_REPORT_STATUS_OK);
					mResult = TAG_MSG_NO_HIT_ID;  //Match update
				}
				break;			
		}
		
		return mResult;
}

uint8_t Rpt_SyncType_Proc(uint8_t *SyncBuf)
{
		bool ret;
		uint8_t iRssi = 0, mResult = 0, mhopcount;
		uint16_t Tmr1Cnt = 0;
		P_SYNC_BEACON pSyncBeacon = (P_SYNC_BEACON)SyncBuf;
		
		if(RptGlblVar.gMode != RPT_PWRON_MODE) {
			// check receive gateway beacon reset Timer1 
			if((RptGlblVar.Tmr1DynaVal = ((RPT_TASK_TIMER_VAL) - TASK_CUR_RUNTIME(TIMER1))) <= (RPT_END_EARLY_TIMER))
			{
				RptGlblVar.Tmr1DynaVal += (RPT_END_EARLY_TIMER - RptGlblVar.Tmr1DynaVal);
				Tmr1Cnt = ((RPT_TASK_TIMER_VAL + RPT_END_EARLY_TIMER) - RptGlblVar.Tmr1DynaVal);
			}else{
				RptGlblVar.Tmr1DynaVal -= (RptGlblVar.Tmr1DynaVal - RPT_END_EARLY_TIMER);
				Tmr1Cnt = ((RPT_TASK_TIMER_VAL + RPT_END_EARLY_TIMER) - RptGlblVar.Tmr1DynaVal);
			}
			RstTimerValue(TIMER1, Tmr1Cnt);
			
			switch(pSyncBeacon->Action){
				case RF_AL_SYNC:
				case RF_AL_ALL_WU_TIME:
					mResult = TAG_MSG_ONLY_SYNC;
					break;
				case RF_AL_UPDATE_TAG:
				case RF_AL_GET_TAG_STATUS:
				case RF_AL_UPDATE_REPEATER_TAG:
					pSyncBeacon->Action = RF_AL_UPDATE_TAG;
					mResult = TAG_MSG_TO_RCV_TAG_LIST;
					break;		
				case RF_AL_HOP_TO_REPEATER:
				case RF_AL_HOP_TO_TAG:
					//setSynTimer(TagWakeUpInterval, 0);		//Sync time sec
					mhopcount = (pSyncBeacon->ChnCtrl) & 0xF;
					if(ghopcount == 0) {			//first time received hopping sync beacon
							ghopcount = mhopcount;
							gCHindex = ((pSyncBeacon->ChnCtrl) >> 4) & 0xF;
					}			
					mResult = TAG_MSG_HOPPING;
				 break;
				default:
					mResult = TAG_MSG_NO_HIT_ID;
					break;
			}	
			// record Gateway Sequence num
			RptGlblVar.gTxSeqNo = pSyncBeacon->BeaconSeq;
			// if the repeater replies sync, clear return power on mode counter.
			RptGlblVar.RtnPwrModeCnt = 0;
			
			//**************************************************************************
			//************************* A7127 RF ***************************************
			//**************************************************************************
			//Copy Repeater ID to head field
			/*memcpy(pSyncBeacon->RepeaterID, &RptDefSet.RptID[3], 2);
			
			// Encrypt Beacon packet
			if(RptGlblVar.cryptflag == 1){
				RptGlblVar.cryptflag = 0;
				
				AES128_Encrypt(SyncBuf, AES_KEY, EncryptData);
				A7127_XmitPkt_Proc(SYNC_LEN, EncryptData);
			}else{
				A7127_XmitPkt_Proc(SYNC_LEN, SyncBuf);
			}
			*/
			//**************************************************************************
			
		}else{					
			if(RptGlblVar.BindFlag == 1){
				// entry repeater task mode
				Delay1ms(RPT_TASK_TIMER_VAL - RPT_END_EARLY_TIMER);
				SetOneShotTimer1(RPT_TASK_TIMER_VAL);
				
				printf("Rpt Beacon Sync\r\n");
				RptGlblVar.PwrOnSyncBeacon = 1;
				
				RptGlblVar.gState = BEACON_STATE;
				RptGlblVar.gMode = RPT_TASK_MODE;//RPT_RX_MODE;
			}else{
				// bind upper layer transfer ID
				ret = check_array_zero(RptGlblVar.BindID, 2);
				if(ret == true){
					memcpy(RptGlblVar.BindID, pSyncBeacon->GatewayID, 2);
				}else{
					iRssi = RSSI_measurement();
					printf("RxID = %d, i RSSI = %d\r\n", pSyncBeacon->GatewayID[1], iRssi);
					if(iRssi > 80){
						ret = compare_array_date(RptGlblVar.BindID, pSyncBeacon->GatewayID, 2);
						if(ret == true){
							RptGlblVar.BindCnt++;
							if(RptGlblVar.BindCnt >= 3){
								RptGlblVar.BindFlag = 1;
							}
						}else{
							memcpy(RptGlblVar.BindID, pSyncBeacon->GatewayID, 2);
						}
					}
				}
			}
		}
		
		return mResult;
}

/*********************************************************************
* Rpt_RxPktProc
*********************************************************************/
uint8_t Rpt_RxPktProc(uint8_t FifoLen)
{
		//bool ret;
		//uint8_t mtag;
		uint8_t mhopcount;
		uint8_t mResult = 0, i = 0, payload = 0;
		uint8_t recv[64] = {0};
		
#if 0		
		A7127_SetFIFOLen(FifoLen);
		
		L01_CSN_LOW( );
		
		SPI_ExchangeByte(FIFO_REG);
	
		//printf("recv = ");
		for(i = 0; i < FifoLen; i++)	{
			recv[i] = RADIO->RX_DATA[i];
			//payload = RADIO->RX_DATA[i];
			SPI_ExchangeByte(recv[i]);
			//printf("%x", recv[i]);
		}
		L01_CSN_HIGH( );
		
		GIOSetTXMode();
		//while(GIO1); //wait transmit completed
#endif //0
		for(i = 0; i < FifoLen; i++)	{
			recv[i] = RADIO->RX_DATA[i];
			//printf("%x", recv[i]);
		}
		//printf("\r\n");
		//printf("Received packet. %x:%x:%x:%x:%x:%x:%x\r\n", recv[0],recv[1],recv[2], recv[3], recv[4],recv[5],recv[6]);
		
		/* Decrypt Beacon function */
		if(FifoLen == SYNC_LEN) {
			AES128_Decrypt(recv, AES_KEY, DecryptData);	
			
			if(DecryptData[1] == 0x01) {
				RptGlblVar.cryptflag = DecryptData[1];
				memcpy(recv, DecryptData, FifoLen);
				//memcpy(RptGlblVar.SyncBuf , recv, FifoLen);
			}else {
				//do nothing, because it is RAW data.
				//memcpy(RptGlblVar.SyncBuf , recv, FifoLen);
			}
		}
		
		switch(recv[1]){
			case RF_PT_SYNC_BEACON:
				mResult = Rpt_SyncType_Proc(recv);
				break;
				
			case RF_PT_TAG_STATUS:
			case RF_PT_TAG_UPDATE_LIST:		
			case RF_PT_ALWAYS_WAKE_BEACON:	
				mResult = Rpt_TagListType_Proc(recv);
			break;

			case RF_PT_HOPPING_BEACON:				
				//setSynTimer(recv[4], updateflag);		//Sync time sec
				mhopcount = recv[3] & 0xF;
			
				//first time received hopping sync beacon
				if(ghopcount == 0) {			
						ghopcount = mhopcount;
						gCHindex = (recv[3] >> 4) & 0xF;
				}
				//setSynTimer(recv[4], updateflag);		//Sync time sec
				mResult = TAG_MSG_HOPPING;
				break;
				
			case RF_PT_PACKET_HEADER:
			case RF_PT_DATA_PACKET:
				mResult = Rpt_UpdDataType_Proc(RF_PT_DATA_PACKET, recv);
				break;
			
			case RF_PT_TAG_UNBOUND:
			case RF_PT_CHANGE_BEACON_SRC:
				memcpy(RptGlblVar.TagLstBuf, recv, FifoLen); //Copy received Tag list data to buffer
				break;
				
			default:
#ifndef _NO_DBG_PRINTF
				DBGMSG("Rx dummy Pkt = ");
				for(i = 0; i < FifoLen; i++){
					DBGMSG("%x ", recv[i]);
				}
				DBGMSG("\r\n");
#endif //_NO_DBG_PRINTF
				break;
		}
		
		return mResult;
}

/**
  * @brief  Enables or disables the specified USART peripheral.
  * @param  FifoLen: set RF receive length. 
	*         mSec : wait rx packet timer(ms).
  * @param  NewState: new state of the Timer toggle.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
uint8_t RF_GetPkt1(uint8_t FifoLen, uint32_t mSec, FunctionalState NewState)
{
		uint8_t RxPktResult = 0, i = 0;
		uint8_t WTRFlag = 0;
	
		/* Set RF packet fifo length */
		SetFIFOLvl(FifoLen);
		/* RF entry RX state */
		StrobeCmd(CMD_RX);
		Oneshoutflag = 0;		//Time out flag
	
		//A7127_SetFIFOLen(FifoLen);
	
		if(NewState == ENABLE){
			/* set get packet timeout timer */
			SetTimer(TIMER0, mSec);		//Start receive timeout timer
		}
		
		while(1){
			if(WTRFlag == 0) {		//Wait CMD_RX action, WTR = 1.
				if((RADIO->STATUS & (1 << 7))){
					WTRFlag = 1; // if RF Rx ready toggle flag to Rx packet
				}
			}else {
				if((!(RADIO->STATUS & (1 << 7))) || (Oneshoutflag)) {
					if(NewState == ENABLE){
						Timer_StopTimer(TIMER0);
					}
					
					if(!Oneshoutflag){
						//memcpy((void*)0x20000000, (void*)0x50001500, 0x40);
						//LA_GPIO_HIGHT();
						//while(GIO1); //wait transmit completed
						//RxPktResult = Rpt_RxPktProc(FifoLen); //Rx packet
						L01_CSN_LOW( );
						
						SPI_ExchangeByte(FIFO_REG);
				
						for(i = 0; i < FifoLen; ++i){
							//RADIO->RX_DATA[i];
							SPI_ExchangeByte(RADIO->RX_DATA[i]);
						}
						//memcpy((void*)0x50002018, (void*)0x20000000, 0x40);
						
						L01_CSN_HIGH( );
						GIOSetTXMode();
						//LA_GPIO_LOW();
					}else {		//Time out
						StrobeCmd(CMD_STBY); //Back to Standby
						//printf("Get Pkt Timeout.\r\n");
						RxPktResult = TAG_MSG_RX_TIMEOUT;
					}
					break;
				}
			}
		}
		
		return RxPktResult;
}

#if 1
uint8_t RF_GetPkt(uint8_t FifoLen, uint32_t mSec, FunctionalState NewState)
{
		uint8_t RxPktResult = 0;
		uint8_t WTRFlag = 0;
	
		/* Set RF packet fifo length */
		SetFIFOLvl(FifoLen);
		/* RF entry RX state */
		StrobeCmd(CMD_RX);
		Oneshoutflag = 0;		//Time out flag
	
		if(NewState == ENABLE){
			/* set get packet timeout timer */
			SetTimer(TIMER0, mSec);		//Start receive timeout timer
		}
		
		while(1){
			if(WTRFlag == 0) {		//Wait CMD_RX action, WTR = 1.
				if((RADIO->STATUS & (1 << 7))){
					WTRFlag = 1; // if RF Rx ready toggle flag to Rx packet
				}
			}else {
				if((!(RADIO->STATUS & (1 << 7))) || (Oneshoutflag)) {
					if(NewState == ENABLE){
						Timer_StopTimer(TIMER0);
					}
					
					StrobeCmd(CMD_STBY); //Back to Standby
					
					if(!Oneshoutflag){	
						RxPktResult = Rpt_RxPktProc(FifoLen); //Rx packet
					}else {		//Time out
						//printf("Get Pkt Timeout.\r\n");
						RxPktResult = TAG_MSG_RX_TIMEOUT;
					}
					break;
				}
			}
		}
		
		return RxPktResult;
}
#endif //0

void Rpt_GetUpdData_Proc(int16_t mlen)
{
		uint8_t mReturn = 0;

		mindex = Oneshoutflag = 0;		//Time out flag

		switch(gDatarate){
			case RF_DATARATE_1M:
				SetTimer(TIMER0, RPT_1M_UPDTIMER);		//100ms
			break;
			case RF_DATARATE_500K:
				SetTimer(TIMER0, RPT_500K_UPDTIMER);		//135ms, Cover all data transmit time
			break;
			case RF_DATARATE_250K:
				SetTimer(TIMER0, RPT_250K_UPDTIMER);		//200ms
			break;	
		}

		while(mlen > 0) {
			mReturn = RF_GetPkt(PAYLOAD_LEN, 0, DISABLE);
			//mReturn = RF_GetPkt1(PAYLOAD_LEN, 0, DISABLE);
			
			if(Oneshoutflag) {
				StrobeCmd(CMD_STBY); //exit rx mode
				//printf("DATA_STATE___RX data timer out.\r\n");
				mReturn = TAG_MSG_PACKET_LOST;
				break;
			}else{
				mlen -= (PAYLOAD_LEN - 2);		//two bytes header
			}
		}
		
		Timer_StopTimer(TIMER0);
		
		//RptGlblVar.RxUpdBufPtrAddr += DataLen; 

		if((Oneshoutflag) || (mReturn == TAG_MSG_PACKET_LOST)) {	//Don't need update EPD because data error.
			//Receipt complete reply repeater status
			//Rpt_Rpy_RptStatus(mReturn);
			
			/*RptGlblVar.UpdCntTmp2--;
			memset(&TagID[0 + ((RptGlblVar.UpdCntTmp2) * 5)], 0, 5);
			//RptGlblVar.UpdCntTmp2++;
			RptGlblVar.UpdCntTmp1--;
			*/
			//clear update data
			//memset(&pEpdBuf[RptGlblVar.RxUpdBufPtrAddr - DataLen], 0, DataLen);
			mReturn = TAG_MSG_PACKET_LOST;
			printf("Data Lost\r\n");
		}else {			
			//Receipt complete reply repeater status
			printf("%d\r\n", RptGlblVar.RxUpdBufPtrAddr);
			mReturn = TAG_MSG_RX_COMPLETED;
		}
		
		RptGlblVar.UpdCntTmp2++;	
		//Rpt_Rpy_RptStatus(mReturn);
		
		/*if(RptGlblVar.UpdCntTmp2 != RptGlblVar.UpdCntTmp1){
			RptGlblVar.gState = HEADER_STATE;
		}else{
			RptGlblVar.UpdCntTmp2 = RptGlblVar.gState = 0;
		}	
		*/
		//RptGlblVar.UpdCntTmp2 = RptGlblVar.gState = 0;
}

void Rpt_RxTask_Proc(uint8_t TaskState)
{
		uint8_t mReturn = 0;
		uint16_t Tmr1Cnt = 0;
	
		switch(TaskState) {
			case PWRBEACON_STATE:
				mReturn = RF_GetPkt(SYNC_LEN, RPT_PWRON_BEACON_TIMER, ENABLE);      //FOR SPENSER.
				break;	
			
			case BEACON_STATE:
				mReturn = RF_GetPkt(SYNC_LEN, RPT_BEACON_TIMER, ENABLE);
				break;
		
			case TAG_LIST_STATE:
				mReturn = RF_GetPkt(PAYLOAD_LEN, PRT_RX_PKT_TIMER, ENABLE);      //FOR SPENSER.
				break;	

			case HEADER_STATE:	
				mReturn = RF_GetPkt(PAYLOAD_LEN, PRT_RX_PKT_TIMER, ENABLE); //3ms OK, 1ms fail //FOR SPENSER.
				break;
					
			case DATA_STATE:
				Rpt_GetUpdData_Proc(RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2]);
			
			/* Unused state */
			case HOPBEACON_STATE:
			case REPLYRESULT_STATE:
			case EPDDATA_STATE:
			case EPDREFRESH_STATE:
			case ENTERPM1_STATE:
			case HOPPM1_STATE:
			case FAIL_STATE:
			default:
				return;
		}
		
		switch(mReturn) {
			case TAG_MSG_HIT_ID:
				switch(TaskState){
					case TAG_LIST_STATE:
						RptGlblVar.gState = HEADER_STATE;
						break;
					case HEADER_STATE:
						RptGlblVar.gState = DATA_STATE;
						break;
				}
				break;
			
			case TAG_MSG_HOPPING:
				RptGlblVar.HotBcnFlag = 1;
			case TAG_MSG_NO_HIT_ID:
			case TAG_MSG_ONLY_SYNC:	
				RptGlblVar.gState = 0;
				break;
			
			case TAG_MSG_REPORT_STATUS_OK:
				break;
			
			case TAG_MSG_TO_RCV_TAG_LIST:
				RptGlblVar.gState = TAG_LIST_STATE;
				break;
			
			case TAG_MSG_RX_TIMEOUT:
				switch(TaskState) {
					case PWRBEACON_STATE:
						printf("Lost Beacon...PWRBEACON_STATE\r\n");
						RptGlblVar.RtnPwrOnMode = 1;
						break;
					
					case BEACON_STATE:
						RptGlblVar.Tmr1DynaVal = ((RPT_TASK_TIMER_VAL) - TASK_CUR_RUNTIME(TIMER1));
						Tmr1Cnt = (RPT_TASK_TIMER_VAL + RPT_END_EARLY_TIMER) - RptGlblVar.Tmr1DynaVal;
						RstTimerValue(TIMER1, Tmr1Cnt);
						
						printf("Lost Beacon...BEACON_STATE\r\n");
						RptGlblVar.gState = 0; //PWRBEACON_STATE; //BEACON_STATE
						RptGlblVar.RtnPwrOnMode = 1;
						break;
				}
				break;
				
			default:
				break;
		}
}

//*****************************************************************************************
