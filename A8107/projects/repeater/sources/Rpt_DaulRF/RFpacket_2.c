/***********************************************************************
 * RFpacket.c
 *
 ***********************************************************************/
#include <math.h>
#include "main.h"
#include "RptProc.h"
#include "isr.h"
#include "RptDataflash.h"
#include "A7127RF_Drv.h"

uint8_t EpdRFData[EpdUpdateSize] = {0};
uint8_t *pEpdBuf = (uint8_t *)EpdRFData;

uint8_t TagID[30] = {0}; // = {0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t AgentID[5]; // = {0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t RepeaterID[5] = {0xFF, 0xFF, 0xFE, 0x00, 0x01};//{0x52, 0x00, 0x00, 0x00, 0x01};

uint8_t mindex; //for updata packet lost check
uint8_t ghopcount;
uint8_t gCHindex;

/*
 * Jerry add
*/
extern uint8_t AES_KEY[];
extern void RstTimerValue(TIMER_Type *timer, uint32_t msec);

/**
  * @brief      Send repeater stauts to gatway
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used send repeater status to gateway.
  */
void Rpt_Rpy_RptStatus(uint8_t result)
{	
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
		uint8_t mResult = 0;
		P_RPT_UPD_TAG_HEAD pTagUpdHead;
		//int16_t UpdLen = 0;
	
		switch(PktType){
			case RF_PT_PACKET_HEADER:
				pTagUpdHead = (P_RPT_UPD_TAG_HEAD)UpdDataBuf;
	
				//ret = compare_array_date(&RptDefSet.RptID[3], &pTagUpdHead->RepeaterID[0], 2);
				//ret = check_array_zero(pTagUpdHead->RepeaterID, 2);
				ret = 1;
				if(ret == true){
					mindex = pTagUpdHead->Seqno;
					
					//DataLen = 0;
					//DataLen = pTagUpdHead->PictureSize;
					
					RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2] = pTagUpdHead->PictureSize;
					//pEpdBuf = &EpdRFData[RptGlblVar.RxUpdBufPtrAddr];
					mResult = TAG_MSG_HIT_ID;

					A7127_XmitPkt_Proc(PAYLOAD_LEN, UpdDataBuf);

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
					//memcpy(pEpdBuf, &UpdDataBuf[2], PAYLOAD_LEN - 2);		//62: -two bytes header, 64-2
					//pEpdBuf += (PAYLOAD_LEN - 2);
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
					if(mtag > RPT_RX_MAX_UPDATE){
						mtag = RptGlblVar.TxUpdTagCnt = RptGlblVar.UpdCntTmp1 = RPT_RX_MAX_UPDATE;
					}
					
					/*if(mtag > 0){ //have update
						memcpy(&AgentID[3], pTagUpdateList->GatewayID, 2);
						memcpy(&TagID[0], &pTagUpdateList->TAGID[0], mtag * 5);
					}
					memcpy(RptGlblVar.TagLstBuf, TagListBuf, PAYLOAD_LEN); //Copy received Tag list data to buffer
					*/
					A7127_XmitPkt_Proc(PAYLOAD_LEN, TagListBuf);
					
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
						
		ret = check_array_zero(pSyncBeacon->RepeaterID, 2);
		if(ret == false){
			DBGMSG("Rpt ID non zero...\r\n");
			return 0;
		}
		
		if(RptGlblVar.gMode != RPT_PWRON_MODE) {
			// check receive gateway beacon reset Timer1 
			//DBGMSG("%d ", RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1));
			if((RptGlblVar.Tmr1DynaVal = (RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1))) <= (RPT_END_EARLY_TIMER))
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
				
				case RF_AL_GET_TAG_STATUS:
				case RF_AL_GET_REPEATER_STATUS:
				
				case RF_AL_TAG_UNBOUND:
				case RF_AL_CHANGE_BEACON_SRC:
					
				case RF_AL_UPDATE_TAG:
				case RF_AL_UPDATE_REPEATER_TAG:
					if(pSyncBeacon->Action == RF_AL_UPDATE_REPEATER_TAG){
						pSyncBeacon->Action = RF_AL_UPDATE_TAG; 
					}
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
			
			//**************************************************************************
			//************************* A7127 RF ***************************************
			//**************************************************************************
			memcpy(pSyncBeacon->RepeaterID, &RptDefSet.RptID[3], 2); //Copy Repeater ID to head field
			
			if(RptGlblVar.cryptflag == 1){ // Encrypt Beacon packet
				RptGlblVar.cryptflag = 0;
				
				AES128_Encrypt(SyncBuf, AES_KEY, EncryptData);
				A7127_XmitPkt_Proc(SYNC_LEN, EncryptData);
			}else{
				A7127_XmitPkt_Proc(SYNC_LEN, SyncBuf);
			}
			
			//**************************************************************************
			
			RptGlblVar.gTxSeqNo = pSyncBeacon->BeaconSeq; // record Gateway Sequence num
			RptGlblVar.RtnPwrModeCnt = 0; // if the repeater replies sync, clear return power on mode counter.
		}else{					
			if(RptGlblVar.BindFlag == 1){
				// entry repeater task mode
				RptGlblVar.Tmr1OneShoutFlag = 0;
				//Delay1ms(RPT_TASK_TIMER_VAL - RPT_END_EARLY_TIMER);
				SetOneShotTimer1(RPT_TASK_TIMER_VAL);
				
				printf("Rpt Beacon Sync Bind ID = %x\r\n", RptGlblVar.BindID[1]);
				RptGlblVar.PwrOnSyncBeacon = 1;
				
				RptGlblVar.gState = BEACON_STATE;
				RptGlblVar.gMode = RPT_TASK_MODE;//RPT_RX_MODE;
				
				YELLOW_LED_HIGH();
			}else{				
				/* bind upper layer transfer ID */
				ret = check_array_zero(RptGlblVar.BindID, 2);
				if(ret == true){
					memcpy(RptGlblVar.BindID, pSyncBeacon->GatewayID, 2);
				}else{
					iRssi = RSSI_measurement();
					//ret = check_array_zero(pSyncBeacon->RptID, 2);
					ret = 1;
					printf("GwID = %d, RptID = %d, i RSSI = %d\r\n", pSyncBeacon->GatewayID[1], pSyncBeacon->RepeaterID[1], iRssi);
					if((iRssi > COMPARE_RSSI_VALUE) && (ret == true)){
					//if(iRssi > 100){
						ret = compare_array_date(RptGlblVar.BindID, pSyncBeacon->GatewayID, 2);
						if(ret == true){
							RptGlblVar.BindRssi = iRssi;
							RptGlblVar.BindCnt++;
							if(RptGlblVar.BindCnt >= 3){
								RptGlblVar.BindFlag = 1;
							}
						}else{
							if(iRssi > RptGlblVar.BindRssi){
								memcpy(RptGlblVar.BindID, pSyncBeacon->GatewayID, 2);
							}
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
		bool ret;
		uint8_t mhopcount;
		uint8_t mResult = 0, i = 0;
		uint8_t recv[64] = {0};

		//DBGMSG("recv = ");
		for(i = 0; i < FifoLen; i++)	{
			recv[i] = RADIO->RX_DATA[i];
			//DBGMSG("%x ", recv[i]);
		}
		//DBGMSG("\r\n");
		//DBGMSG("Received packet. %x:%x:%x:%x:%x:%x:%x\r\n", recv[0],recv[1],recv[2], recv[3], recv[4],recv[5],recv[6]);
		
		/* Decrypt Beacon function */
		if(FifoLen == SYNC_LEN) {
			AES128_Decrypt(recv, AES_KEY, DecryptData);	
			
			if(DecryptData[1] == 0x01) {
				RptGlblVar.cryptflag = DecryptData[1];
				memcpy(recv, DecryptData, FifoLen);
				memcpy(RptGlblVar.SyncBuf , recv, FifoLen);
			}else {
				//do nothing, because it is RAW data.
				memcpy(RptGlblVar.SyncBuf , recv, FifoLen);
			}
		}
		
		/* Processing packet types */
		switch(recv[1]){
			case RF_PT_SYNC_BEACON:
				mResult = Rpt_SyncType_Proc(recv);
				break;
			
			case RF_PT_TAG_STATUS:
			case RF_PT_REPEATER_STATUS:
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
				mResult = Rpt_UpdDataType_Proc(RF_PT_PACKET_HEADER, recv);
				break;
				
			case RF_PT_DATA_PACKET:
				mResult = Rpt_UpdDataType_Proc(RF_PT_DATA_PACKET, recv);
				break;
			
			case RF_PT_TAG_UNBOUND:
			case RF_PT_CHANGE_BEACON_SRC:
				memcpy(RptGlblVar.TagLstBuf, recv, PAYLOAD_LEN); //Copy received Tag list data to buffer
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
//**********************************************************************************

uint8_t Rpt_RxState_GetPkt_Proc1(uint32_t FifoLen, uint32_t mSec, FunctionalState TimerState)
{
		uint8_t RxPktResult = 0, i = 0;
		uint8_t WTRFlag = 0;

		/* Set RF packet fifo length */
		SetFIFOLvl(FifoLen);
		A7127_SetFIFOLen(FifoLen);
		/* RF entry RX state */
		StrobeCmd(CMD_RX);
	
		if(TimerState == ENABLE){
			Oneshoutflag = 0;		//Time out flag
			SetTimer(TIMER0, mSec); //set recv packet timeout
		}
		
		while(1){
			if(WTRFlag == 0) {		//Wait CMD_RX action, WTR = 1.
				if((RADIO->STATUS & (1 << 7))){
					WTRFlag = 1; // if RF Rx ready toggle flag to Rx packet
				}
			}else {
				if((!(RADIO->STATUS & (1 << 7))) || (Oneshoutflag)) {
					if(TimerState == ENABLE){
						Timer_StopTimer(TIMER0);
					}
					StrobeCmd(CMD_STBY); //Back to Standby
					
					if(!Oneshoutflag){	
						LA_GPIO_HIGHT();
						//RxPktResult = Rpt_RxPktProc(FifoLen); //Rx packet
						L01_CSN_LOW( );
						
						SPI_ExchangeByte(FIFO_REG);
				
						for(i = 0; i < FifoLen; ++i){
							//RADIO->RX_DATA[i];
							SPI_ExchangeByte(RADIO->RX_DATA[i]);
						}
											
						L01_CSN_HIGH();
						GIOSetTXMode();
						
						LA_GPIO_LOW();
					}else {		//Time out
						//DBGMSG("Get Pkt Timeout.\r\n");
						RxPktResult = TAG_MSG_RX_TIMEOUT;
					}
					break;
				}
			}
		}
		
		return RxPktResult;
}

uint8_t Rpt_RxState_GetPkt_Proc(uint32_t FifoLen, uint32_t mSec, FunctionalState TimerState)
{
		uint8_t RxPktResult = 0;
		uint8_t WTRFlag = 0;

		/* Set RF packet fifo length */
		SetFIFOLvl(FifoLen);
		/* RF entry RX state */
		StrobeCmd(CMD_RX);
	
		if(TimerState == ENABLE){
			Oneshoutflag = 0;		//Time out flag
			SetTimer(TIMER0, mSec); //set recv packet timeout
		}
		
		while(1){
			if(WTRFlag == 0) {		//Wait CMD_RX action, WTR = 1.
				if((RADIO->STATUS & (1 << 7))){
					WTRFlag = 1; // if RF Rx ready toggle flag to Rx packet
				}
			}else {
				if((!(RADIO->STATUS & (1 << 7))) || (Oneshoutflag)) {
					if(TimerState == ENABLE){
						Timer_StopTimer(TIMER0);
					}
					StrobeCmd(CMD_STBY); //Back to Standby
					
					if(!Oneshoutflag){	
						RxPktResult = Rpt_RxPktProc(FifoLen); //Rx packet
					}else {		//Time out
						//DBGMSG("Get Pkt Timeout.\r\n");
						RxPktResult = TAG_MSG_RX_TIMEOUT;
					}
					break;
				}
			}
		}
		
		return RxPktResult;
}

void Rpt_RxState_UpdData_Proc(int16_t mlen)
{
		uint8_t mReturn = 0;
		uint16_t UpdTimeOut = 0;
	
		mindex = 0; //clear sequence num
		Oneshoutflag = 0;		//Time out flag
	
		//UpdTimeOut = Rpt_CalcRecvUpd_Timeout(mlen); //Calculate when to receive updates
		//SetTimer(TIMER0, UpdTimeOut); //set receive update data timeout
		SetTimer(TIMER0, 300); //set receive update data timeout
		/*switch(gDatarate){
			case RF_DATARATE_1M:
				SetTimer(TIMER0, RPT_1M_UPDTIMER); //100ms
			break;
			case RF_DATARATE_500K:
				SetTimer(TIMER0, RPT_500K_UPDTIMER); //135ms, Cover all data transmit time
			break;
			case RF_DATARATE_250K:
				SetTimer(TIMER0, RPT_250K_UPDTIMER); //200ms
			break;	
		}
		*/

		//LA_GPIO_HIGHT();
		/* receive update data */
		while(mlen > 0) {
			//mReturn = Rpt_RxState_GetPkt_Proc(PAYLOAD_LEN, 0, DISABLE);
			mReturn = Rpt_RxState_GetPkt_Proc1(PAYLOAD_LEN, 0, DISABLE);
			
			/*if(Oneshoutflag) {
				StrobeCmd(CMD_STBY); //exit rx mode
				//DBGMSG("DATA_STATE___RX data timer out.\r\n");
				mReturn = TAG_MSG_PACKET_LOST;
				break;
			}*/
			mlen = mlen - (PAYLOAD_LEN - 2);		//two bytes header
		}
		//LA_GPIO_LOW();
		
		Timer_StopTimer(TIMER0); //stop Timer0 of receive timeout
		//RptGlblVar.RxUpdBufPtrAddr += DataLen; //calculate next receive update data address
		RptGlblVar.RxUpdBufPtrAddr += RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2];
		
		/*if((Oneshoutflag) || (mReturn == TAG_MSG_PACKET_LOST)) {	//Don't need update EPD because data error.		
			memset(&RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2], 0, 2); //clear update counter
			//memset(&pEpdBuf[RptGlblVar.RxUpdBufPtrAddr - DataLen], 0, DataLen); //clear update data
			
			DBGMSG("Data Lost = %d, %d\r\n", RptGlblVar.UpdCntTmp2, RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2]);
			mReturn = TAG_MSG_RX_TIMEOUT;
		}else {
			*/
			DBGMSG("%d\r\n", RptGlblVar.RxUpdBufPtrAddr);
			//RptGlblVar.RxUpdBufPtrAddr = 0;
			mReturn = TAG_MSG_RX_COMPLETED;
		//}
		RptGlblVar.UpdCntTmp2++;
		Rpt_Rpy_RptStatus(mReturn); //Receipt complete reply repeater status
	
		if(RptGlblVar.UpdCntTmp2 != RptGlblVar.UpdCntTmp1){
			RptGlblVar.gState = HEADER_STATE;
		}else{
			RptGlblVar.RxUpdBufPtrAddr = RptGlblVar.UpdCntTmp1 = RptGlblVar.UpdCntTmp2 = RptGlblVar.gState = 0;
		}
}

void Rpt_RxTask_Proc(uint8_t TaskState)
{
		uint8_t mReturn = 0;
		uint16_t Tmr1Cnt = 0;
	
		switch(TaskState) {
			case PWRBEACON_STATE:
				mReturn = Rpt_RxState_GetPkt_Proc(SYNC_LEN, RPT_PWRON_BEACON_TIMER, ENABLE);
				break;	
			
			case BEACON_STATE:
				mReturn = Rpt_RxState_GetPkt_Proc(SYNC_LEN, RPT_BEACON_TIMER, ENABLE);
				break;
		
			case TAG_LIST_STATE:
				mReturn = Rpt_RxState_GetPkt_Proc(PAYLOAD_LEN, PRT_RX_PKT_TIMER, ENABLE);
				break;	

			case HEADER_STATE:
				mReturn = Rpt_RxState_GetPkt_Proc(PAYLOAD_LEN, PRT_RX_PKT_TIMER, ENABLE);
				break;
					
			case DATA_STATE:
				Rpt_RxState_UpdData_Proc(RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2]);
				return;
			
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
				switch(TaskState) {
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
			case TAG_MSG_TO_RCV_TAG_LIST:
				RptGlblVar.gState = TAG_LIST_STATE;
				break;
			
			case TAG_MSG_RX_TIMEOUT:
				switch(TaskState){
					case PWRBEACON_STATE:
						printf("PwrOn Lost Beacon...\r\n");
						break;
					case BEACON_STATE:
						Tmr1Cnt = (RPT_TASK_TIMER_VAL + RPT_END_EARLY_TIMER) - (RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1));
						RstTimerValue(TIMER1, Tmr1Cnt);

						printf("Task Lost Beacon...\r\n");
						RptGlblVar.gState = 0; //PWRBEACON_STATE; //BEACON_STATE
						RptGlblVar.RtnPwrOnMode = 1;
						break;
				}
				break;
				
			default:
				break;
		}
}

//**********************************************************************************
