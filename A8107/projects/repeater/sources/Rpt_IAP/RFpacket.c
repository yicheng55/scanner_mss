/***********************************************************************
 * RFpacket.c
 *
 ***********************************************************************/
#include "main.h"
#include "RptProc.h"
#include "isr.h"
#include <math.h>
#include "RptDataflash.h"

uint8_t TagID[30] = {0}; // = {0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t GatewayID[5]; // = {0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t RepeaterID[5] = {0xFF, 0xFF, 0xFE, 0x00, 0x01};//{0x52, 0x00, 0x00, 0x00, 0x01};

uint8_t mindex; //for packet lost check
uint8_t ghopcount;
uint8_t gCHindex;
//uint8_t gRxPktResult = 0;		//packet lost flag
uint8_t updateflag = 0;
uint8_t	LossCnt = 0;			//Lost Beacon count -- SPenser
uint8_t TagWakeUpInterval;
uint8_t RxTimeOutCnt = 0;
uint8_t gTagSeq = 0;
uint16_t UpdateCount = 0;
uint16_t RebootCount = 0;		
uint16_t UpdateFailCount = 0;	
uint32_t gRSSIvalue = 0;			//current RSSI value
int16_t DataLen = 0;

uint8_t *pEpdBuf = (uint8_t *)EpdRFData;

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
			
		RptRpyStatus->SeqNum = SeqNo++;
		RptRpyStatus->PckType = RF_PT_REPEATER_STATUS;
		RptRpyStatus->Result = result;
		RptRpyStatus->Reserved = 0;
	
		memcpy(&RptRpyStatus->GatewayID[0], &AgentID[3], 2);
		memcpy(&RptRpyStatus->RepeaterID[0], &RptDefSet.RptID[3], 2);
			
		RptRpyStatus->RptVerMajor = RptDefSet.RptVerMJR;
		RptRpyStatus->RptVerMinor = RptDefSet.RptVerMINR;
		RptRpyStatus->RptVerRelease = RptDefSet.RptVerRLSE;
		RptRpyStatus->RptVerBuild = RptDefSet.RptVerBLD;
		
		RptRpyStatus->RptRSSI = rssi_turn_dbm(RSSI_measurement());
		RptRpyStatus->RptCHN = RptDefSet.Gchn;
		RptRpyStatus->TagCHN = RptDefSet.Tchn;
	
		RptRpyStatus->RptStandbyTime = htons(RptDefSet.StartSec);
		RptRpyStatus->RptRsyncCNT = RptDefSet.RptReSync;
		memcpy(RptRpyStatus->TagID, &TagID[RptGlblVar.MultiTagCnt], 5);

		Rpt_TxPktProc(PAYLOAD_LEN, RptGlblVar.TxBuffer);
			
		RptGlblVar.MultiTagCnt += 5;
}

uint8_t Rpt_UpdDataType_Proc(uint8_t PktType, uint8_t *DataBuf)
{
		bool ret;
		uint8_t mResult = 0;
		P_RPT_UPD_TAG_HEAD pTagUpdHead;
	
		switch(PktType){
			case RF_PT_PACKET_HEADER:
				pTagUpdHead = (P_RPT_UPD_TAG_HEAD)DataBuf;
			
				ret = compare_array_date(&RptDefSet.RptID[3], pTagUpdHead->RepeaterID, 2);
				//ret = check_array_zero(pTagUpdHead->RepeaterID, 2);
				//ret = 1;
				if(ret == true){
					//mindex = recv[0];
					mindex = pTagUpdHead->Seqno;
					DataLen = 0;
					//memcpy(&DataLen, &recv[12], 2);
					DataLen = pTagUpdHead->PictureSize;
					//memcpy(&RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2], &DataLen, 2);
					RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2] = pTagUpdHead->PictureSize;
					pEpdBuf = &EpdRFData[RptGlblVar.RxUpdBufPtrAddr];
					
					mResult = TAG_MSG_HIT_ID;
					//RptGlblVar.gState = DATA_STATE;
				}else {
					mResult = TAG_MSG_RX_TIMEOUT;
				}
				break;
			
			case RF_PT_DATA_PACKET:
				if((DataBuf[0] - mindex) > 1) {
					printf("Lost some data...%x - %x\r\n", DataBuf[0], mindex);
					mResult = TAG_MSG_PACKET_LOST; //indicate packet lost
					//break;
				}else{
					mindex = DataBuf[0];	
					memcpy(pEpdBuf, &DataBuf[2], PAYLOAD_LEN - 2);		//62: -two bytes header, 64-2
					pEpdBuf += (PAYLOAD_LEN - 2);
				}
				break;
				
			default:
				break;
		}
	
		return mResult;
}

uint8_t Rpt_TagListType_Proc(uint8_t *DataBuf)
{
		bool ret;
		uint8_t mtag = 0, mResult = 0;
		P_TAG_UPDATE_LIST pTagUpdateList = (P_TAG_UPDATE_LIST)DataBuf;
		
		ret = compare_array_date(pTagUpdateList->RepeaterID, &RptDefSet.RptID[3], 2);
		//ret = check_array_zero(pTagUpdateList->RepeaterID, 2);
		//ret = 1;
		if(ret == true){
			memset(TagID, 0, sizeof(TagID));
			memset(RptGlblVar.RxTtlUpdDataLenBuf, 0, sizeof(RptGlblVar.RxTtlUpdDataLenBuf));
		
			RptGlblVar.TxUpdTagCnt = RptGlblVar.UpdCntTmp1 = mtag = (pTagUpdateList->PktCtrl) & 0x7;		//want update number
			
			if(pTagUpdateList->PacketType == RF_PT_TAG_UPDATE_LIST){
				// Repeater cannot receive more than 3 updates
				if(mtag > RPT_RX_MAX_UPDATE){
					mtag = RptGlblVar.TxUpdTagCnt = RptGlblVar.UpdCntTmp1 = RPT_RX_MAX_UPDATE;
				}
				
				if(mtag > 0){ //have update
					//updateflag = 1;
					mResult = TAG_MSG_HIT_ID;  //Match update
					memcpy(&AgentID[3], pTagUpdateList->GatewayID, 2);
					memcpy(&TagID[0], &pTagUpdateList->TAGID[0], mtag * 5);
				}
			}
			memcpy(RptGlblVar.TagLstBuf, DataBuf, PAYLOAD_LEN); //Copy received Tag list data to buffer
		}else{
			mResult = TAG_MSG_NO_HIT_ID;  //Match update
		}
		
		return mResult;
}

uint8_t Rpt_SyncType_Proc(uint8_t *DataBuf)
{
		bool ret;
		uint8_t iRssi = 0, mResult = 0, mhopcount;
		uint16_t Tmr1Cnt = 0;
		P_SYNC_BEACON pSyncBeacon = (P_SYNC_BEACON)DataBuf;
		
		if(RptGlblVar.gMode != RPT_PWRON_MODE) {
			// check receive gateway beacon reset Timer1 
			//printf("%d ", RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1));
			if((RptGlblVar.Tmr1DynaVal = (RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1))) < (RPT_END_EARLY_TIMER))
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
				case RF_AL_GET_REPEATER_STATUS:

					break;
				case RF_AL_UPDATE_TAG:
				case RF_AL_GET_TAG_STATUS:
				case RF_AL_UPDATE_REPEATER_TAG:
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
		}else{					
			if(RptGlblVar.BindFlag == 1){
				// entry repeater task mode
				Delay1ms(RPT_TASK_TIMER_VAL - RPT_END_EARLY_TIMER);
				SetOneShotTimer1(RPT_TASK_TIMER_VAL);
				
				printf("Rpt Beacon Sync Bind ID = %x\r\n", RptGlblVar.BindID[1]);
				RptGlblVar.PwrOnSyncBeacon = 1;
				
				RptGlblVar.gState = BEACON_STATE;
				RptGlblVar.gMode = RPT_TASK_MODE;//RPT_RX_MODE;
				YELLOW_LED_HIGH();
			}else{
				// bind upper layer transfer ID
				ret = check_array_zero(RptGlblVar.BindID, 2);
				if(ret == true){
					memcpy(RptGlblVar.BindID, pSyncBeacon->GatewayID, 2);
				}else{
					//iRssi = rssi_turn_dbm(RSSI_measurement());
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

		// if the repeater replies sync, clear return power on mode counter.
		RptGlblVar.RtnPwrModeCnt = 0;
		
		return mResult;
}

/*********************************************************************
* Rpt_RxPktProc
*********************************************************************/
uint8_t Rpt_RxPktProc(uint8_t FifoLen)
{
		bool ret;
		//uint8_t mtag;
		uint8_t mhopcount;
		uint8_t mResult = 0, i = 0;
		//uint8_t updateflag = 0; //check Beacon
		//uint8_t payloadLength = PAYLOAD_LEN;
		uint8_t recv[64] = {0};
	
		//if((RptGlblVar.gState == PWRBEACON_STATE) || (RptGlblVar.gState == BEACON_STATE)){
			//payloadLength = SYNC_LEN;
		//}
		
		//printf("recv = ");
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
				
			default:
				printf("Rx dummy Pkt = ");
				for(i = 0; i < FifoLen; i++){
					printf("%x ", recv[i]);
				}
				printf("\r\n");
				break;
		}
		
		return mResult;
}
//**********************************************************************************

/**
  * @brief  Enables or disables the specified USART peripheral.
  * @param  FifoLen: set RF receive length. 
	*         mSec : wait rx packet timer(ms).
  * @param  NewState: new state of the Timer toggle.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
uint8_t Rpt_RxState_GetPkt_Proc(uint32_t FifoLen, uint32_t mSec, FunctionalState TimerState)
{
		uint8_t RxPktResult = 0;
		uint8_t WTRFlag = 0;

		/* Set RF packet fifo length */
		SetFIFOLvl(FifoLen);
		/* RF entry RX state */
		StrobeCmd(CMD_RX);
		Oneshoutflag = 0;		//Time out flag
	
		if(TimerState == ENABLE){
			//set get packet timeout timer
			SetTimer(TIMER0, mSec);
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
						//printf("Get Pkt Timeout.\r\n");
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
		uint8_t i = 0;
		uint8_t mReturn = 0;
		
		mindex = 0; //clear sequence num
		Oneshoutflag = 0;		//Time out flag

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
		
		StrobeCmd(CMD_RX);
		Delay10us(1);
		
		while(mlen > 0) {
			mReturn = Rpt_RxState_GetPkt_Proc(PAYLOAD_LEN, 0, DISABLE);
			
			if((Oneshoutflag) || (mReturn == TAG_MSG_PACKET_LOST)) {
				StrobeCmd(CMD_STBY); //exit rx mode
				//printf("DATA_STATE___RX data timer out.\r\n");
				mReturn = TAG_MSG_PACKET_LOST;
				break;
			}else{
				mlen -= (PAYLOAD_LEN - 2);		//two bytes header
			}
		}
		
		Timer_StopTimer(TIMER0);
		
		RptGlblVar.UpdCntTmp2++;	
		RptGlblVar.RxUpdBufPtrAddr += DataLen; 
		
		if((Oneshoutflag) || (mReturn == TAG_MSG_PACKET_LOST)) {	//Don't need update EPD because data error.
			//Receipt complete reply repeater status
			Rpt_Rpy_RptStatus(mReturn);
			
			/*RptGlblVar.UpdCntTmp2--;
			memset(&TagID[0 + ((RptGlblVar.UpdCntTmp2) * 5)], 0, 5);
			//RptGlblVar.UpdCntTmp2++;
			RptGlblVar.UpdCntTmp1--;
			*/
			memset(&RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2 - 1], 0, 2);
			//memset(&pEpdBuf[RptGlblVar.RxUpdBufPtrAddr - DataLen], 0, DataLen); //clear update data
			printf("Data Lost = %d, %d\r\n", RptGlblVar.UpdCntTmp2, RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2 - 1]);
		}else {
			//Receipt complete reply repeater status
			Rpt_Rpy_RptStatus(TAG_MSG_RX_COMPLETED);
			
			printf("%d\r\n", RptGlblVar.RxUpdBufPtrAddr);
		}
		
		//printf("%d %d\r\n", RptGlblVar.UpdCntTmp2, RptGlblVar.UpdCntTmp1);
		if(RptGlblVar.UpdCntTmp2 != RptGlblVar.UpdCntTmp1){
			RptGlblVar.gState = HEADER_STATE;
		}else{
			/*for(i = 0 ; i < 3; i++){
				printf("%d ", RptGlblVar.RxTtlUpdDataLenBuf[i]);
			}
			printf("\r\n");
			*/
			RptGlblVar.UpdCntTmp2 = RptGlblVar.gState = 0;
		}	
}

void Rpt_RxState_TagMsg_Proc(uint8_t TagMsg)
{
		uint16_t Tmr1Cnt = 0;
	
		switch(TagMsg) {
			case TAG_MSG_HIT_ID:
				switch(RptGlblVar.gState) {
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
				//printf("TAG_MSG_TO_RCV_TAG_LIST\r\n");
				RptGlblVar.gState = TAG_LIST_STATE;
				break;
			case TAG_MSG_RX_TIMEOUT:
				if(RptGlblVar.gState == PWRBEACON_STATE){
					printf("Lost Beacon...PWRBEACON_STATE\r\n");
					//RptGlblVar.gState = PWRBEACON_STATE;
				}else if(RptGlblVar.gState == BEACON_STATE){
					//printf("%d ", TASK_CUR_RUNTIME(TIMER1));
					//RptGlblVar.Tmr1DynaVal = ((RPT_TASK_TIMER_VAL) - (TIMER1->VALUE / ((SystemCoreClock / 1000) - 3)));
					//Tmr1Cnt = (RPT_TASK_TIMER_VAL + (RPT_END_EARLY_TIMER + 1)) - RptGlblVar.Tmr1DynaVal;
					Tmr1Cnt = (RPT_TASK_TIMER_VAL + RPT_END_EARLY_TIMER + 1) - (RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1));
					//printf("%d ", Tmr1Cnt);
					RstTimerValue(TIMER1, Tmr1Cnt);
					
					printf("Lost Beacon...BEACON_STATE\r\n");
					RptGlblVar.gState = 0; //PWRBEACON_STATE; //BEACON_STATE
					RptGlblVar.RtnPwrModeCnt += RptGlblVar.RtnPwrOnMode = 1;
				}
				break;
			default:
				break;
		}
}

#if 1
void Rpt_RxTask_Proc(uint8_t TaskState)
{
		uint8_t mReturn = 0;
	
		switch(TaskState) {
			case PWRBEACON_STATE:
				mReturn = Rpt_RxState_GetPkt_Proc(SYNC_LEN, RPT_PWRON_BEACON_TIMER, ENABLE);      //FOR SPENSER.
				break;	
			
			case BEACON_STATE:
				mReturn = Rpt_RxState_GetPkt_Proc(SYNC_LEN, RPT_BEACON_TIMER, ENABLE);
				break;
		
			case TAG_LIST_STATE:
				mReturn = Rpt_RxState_GetPkt_Proc(PAYLOAD_LEN, PRT_RX_PKT_TIMER, ENABLE);      //FOR SPENSER.
				break;	

			case HEADER_STATE:
				mReturn = Rpt_RxState_GetPkt_Proc(PAYLOAD_LEN, PRT_RX_PKT_TIMER, ENABLE); //3ms OK, 1ms fail //FOR SPENSER.
				break;
					
			case DATA_STATE:
				Rpt_RxState_UpdData_Proc(DataLen);
				break;
			
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
		
		if(RptGlblVar.State != DATA_STATE){
			Rpt_RxState_TagMsg_Proc(mReturn);
		}

}
#endif //0

#if 0
void Rpt_RxTask_Proc(uint8_t TaskState)
{
		//bool ret;
		uint8_t mReturn = 0;
		uint16_t Tmr1Cnt = 0;
		int16_t mlen = 0;
	
		switch(RptGlblVar.gState) {
			case PWRBEACON_STATE:
				
				mReturn = Rpt_RxState_GetPkt_Proc(SYNC_LEN, RPT_PWRON_BEACON_TIMER, ENABLE);      //FOR SPENSER.
				
				/*if(Oneshoutflag) {		//Rx Beacon time out.			
					printf("Lost Beacon...PWRBEACON_STATE\r\n");
					RptGlblVar.gState = PWRBEACON_STATE;
				}else {
				*/
					switch(mReturn) {
						case TAG_MSG_HIT_ID:
						case TAG_MSG_NO_HIT_ID:
						case TAG_MSG_ONLY_SYNC:	
							RptGlblVar.gState = 0;
							break;
						case TAG_MSG_HOPPING:
							RptGlblVar.HotBcnFlag = 1;
							RptGlblVar.gState = 0;
							break;
						case TAG_MSG_REPORT_STATUS_OK:
						case TAG_MSG_TO_RCV_TAG_LIST:
							break;
						case TAG_MSG_RX_TIMEOUT:
							printf("Lost Beacon...PWRBEACON_STATE\r\n");
							RptGlblVar.gState = PWRBEACON_STATE;
							break;
						default:
							break;
					}
				//}
				break;
				
			case BEACON_STATE:
				mReturn = Rpt_RxState_GetPkt_Proc(SYNC_LEN, RPT_BEACON_TIMER, ENABLE);

				/*if(Oneshoutflag){		//Rx Beacon time out.
					RptGlblVar.Tmr1DynaVal = ((RPT_TASK_TIMER_VAL) - (TIMER1->VALUE / ((SystemCoreClock / 1000) - 3)));
					Tmr1Cnt = (RPT_TASK_TIMER_VAL + RPT_END_EARLY_TIMER) - RptGlblVar.Tmr1DynaVal;
					RstTimerValue(TIMER1, Tmr1Cnt);
					
					printf("Lost Beacon...BEACON_STATE\r\n");
					RptGlblVar.gState = 0; //PWRBEACON_STATE; //BEACON_STATE
					RptGlblVar.RtnPwrModeCnt += RptGlblVar.RtnPwrOnMode = 1;
				}else {
				*/
					switch(mReturn) {
						case TAG_MSG_NO_HIT_ID:
						case TAG_MSG_ONLY_SYNC:
							RptGlblVar.gState = 0;
							break;
						case TAG_MSG_HOPPING:
							//gState = 	HOPPM1_STATE;
							/*printf("ghopcount = %d\r\n", ghopcount);
							if(ghopcount == 1){		//Hopping
								printf("Hop to channel = %d\r\n", CHTable[gCHindex]);
								StrobeCmd(CMD_STBY);  
								SetCH(CHTable[gCHindex]); //freq 2450MHz CH100
								RptDefSet.Tchn = RptDefSet.Gchn = CHTable[gCHindex];
								Rpt_Write_DataFlash();
								ghopcount = 0;
							}
							else if(ghopcount > 0)
							{
								ghopcount--;
							}*/
							//ghopcount--;
							RptGlblVar.HotBcnFlag = 1;
							RptGlblVar.gState = 0;
							break;
						case TAG_MSG_REPORT_STATUS_OK:
						case TAG_MSG_TO_RCV_TAG_LIST:
							RptGlblVar.gState = TAG_LIST_STATE;
							break;		
						case TAG_MSG_RX_TIMEOUT:
							RptGlblVar.Tmr1DynaVal = ((RPT_TASK_TIMER_VAL) - (TIMER1->VALUE / ((SystemCoreClock / 1000) - 3)));
							Tmr1Cnt = (RPT_TASK_TIMER_VAL + RPT_END_EARLY_TIMER) - RptGlblVar.Tmr1DynaVal;
							RstTimerValue(TIMER1, Tmr1Cnt);
							
							printf("Lost Beacon...BEACON_STATE\r\n");
							RptGlblVar.gState = 0; //PWRBEACON_STATE; //BEACON_STATE
							RptGlblVar.RtnPwrModeCnt += RptGlblVar.RtnPwrOnMode = 1;
							break;
						default:
							RptGlblVar.gState = 0;
							break;
					}
				//}
				break;
				
			case TAG_LIST_STATE:
				mReturn = Rpt_RxState_GetPkt_Proc(PAYLOAD_LEN, PRT_RX_PKT_TIMER, ENABLE);      //FOR SPENSER.

				/*if(Oneshoutflag) {		//Rx Beacon time out.	
					printf("Lost Beacon...TAG_LIST_STATE\r\n");
					RptGlblVar.gState = TAG_LIST_STATE;
				}else {
				*/
					switch(mReturn) {
						case TAG_MSG_HIT_ID:
							RptGlblVar.gState = HEADER_STATE;
							break;
						case TAG_MSG_REPORT_STATUS_OK:
							break;	
						case TAG_MSG_NO_HIT_ID:
						case TAG_MSG_RX_TIMEOUT:
						default:
							RptGlblVar.gState = 0;
							break;
					}
				//}
				break;	

			case HEADER_STATE:	
				mReturn = Rpt_RxState_GetPkt_Proc(PAYLOAD_LEN, PRT_RX_PKT_TIMER, ENABLE); //3ms OK, 1ms fail //FOR SPENSER.

				/*if(Oneshoutflag) {		//Rx Beacon time out.	
					//printf("Lost HEADER_STATE...\r\n");
					RptGlblVar.gState = HEADER_STATE;
				}else {*/
					switch(mReturn){
						case TAG_MSG_HIT_ID:
							RptGlblVar.gState = DATA_STATE;
							break;
						case TAG_MSG_RX_TIMEOUT:
						default:
							break;
					}
 				//}
				break;
					
			case DATA_STATE:
				Rpt_RxState_UpdData_Proc(DataLen);
				break;
		}
}
#endif //0
//**********************************************************************************
