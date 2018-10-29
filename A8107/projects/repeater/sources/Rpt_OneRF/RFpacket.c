/***********************************************************************
 * RFpacket.c
 *
 ***********************************************************************/
#include "main.h"
#include <math.h>
#include "flash.h"
#include "isr.h"

#include "RptProc.h"
#include "RptComm.h"
#include "RptDataflash.h"
#include "RptDataFlash.h"

uint8_t mindex; //for updata packet lost check
uint8_t ghopcount;
uint8_t gCHindex;
//uint8_t EpdRFData[EpdUpdateSize] = {0};
//uint8_t *pEpdBuf = (uint8_t *)EpdRFData;
//uint8_t *pEpdBuf;
//uint8_t TagID[30] = {0}; // = {0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t GatewayID[5]; // = {0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t RepeaterID[5] = {0xFF, 0xFF, 0xFE, 0x00, 0x01};//{0x52, 0x00, 0x00, 0x00, 0x01};
//uint8_t gRxPktResult = 0;	//packet lost flag
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
extern uint8_t Rpt_HandleBindGateway(P_SYNC_BEACON pSyncBeacon);
extern void OTA_Init(void);

/**
  * @brief      Send repeater stauts to gatway
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used send repeater status to gateway.
  */
void Rpt_Status(uint8_t result)
{	
		P_RPT_STATUS_REPORT RptRpyStatus = (P_RPT_STATUS_REPORT)RptGlblVar.TxBuffer;
			
		RptRpyStatus->SeqNum = RptGlblVar.gTxSeqNo++;
		RptRpyStatus->PckType = RF_PT_REPEATER_STATUS;
		RptRpyStatus->Result = result;
	
		memcpy(&RptRpyStatus->GatewayID[0], &RptGlblVar.GatewayID[3], 2);
		memcpy(&RptRpyStatus->RepeaterID[0], &RptDefSet.RptID[3], 2);
			
		RptRpyStatus->RptM8107RSSI = RSSI_measurement();
		RptRpyStatus->GwCHN = RptDefSet.Gchn;
		RptRpyStatus->TagCHN = RptDefSet.Tchn;
		RptRpyStatus->RptDataRate = RptDefSet.RptDataRate;
		RptRpyStatus->RptMode = RPT_ONERF_ONE_CHN_MODE;
		memcpy(RptRpyStatus->TagID, &RptGlblVar.TagID[RptGlblVar.MultiTagCnt], 5);

		M8107RF_SendPacket(PAYLOAD_LEN, RptGlblVar.TxBuffer);
			
		RptGlblVar.MultiTagCnt += 5;
}

/*uint8_t Repeater_replyOTAresult(uint8_t Result, uint8_t *AckBuf)
{
	P_DEVICE_OTA_REPLY_RESULT pDeviceOTAresult = (P_DEVICE_OTA_REPLY_RESULT)AckBuf;
	
	pDeviceOTAresult->Seq = mindex++;
	pDeviceOTAresult->Result = Result;
	pDeviceOTAresult->SegmentIndex = 
}*/

uint8_t HandleRecvUpdFirmwareHead(uint8_t *OtaBuf)
{
		uint8_t mResult = 0, OtaCounter = 0;
		P_DEVICE_OTA_HEAD pUpdFirmwareHead = (P_DEVICE_OTA_HEAD)OtaBuf;
	
		mindex = pUpdFirmwareHead->Seqno; //backup sequence number
			
		if(CompareData(&RptGlblVar.BckupSegmentIndex, &pUpdFirmwareHead->SegmentIndex, sizeof(pUpdFirmwareHead->SegmentIndex)))
		{
			RptGlblVar.BckupSegmentIndex = pUpdFirmwareHead->SegmentIndex;
			
			if(((OtaCounter = pUpdFirmwareHead->SeqmentSize % FLASH_PAGE_SIZE)) != 0){
				OtaCounter = (pUpdFirmwareHead->SeqmentSize / FLASH_PAGE_SIZE) + 1;
			}else{
				OtaCounter = (pUpdFirmwareHead->SeqmentSize / FLASH_PAGE_SIZE);
			}
			RptGlblVar.WrNewAppAddress = NEW_APP_START_ADDR + (pUpdFirmwareHead->SegmentIndex * FLASH_PAGE_SIZE * OtaCounter);
		}else{
			// ACK receive head status
			mResult = TAG_MSG_OTA_PACKET_REPEATED;
			//Repeater_replyOTAresult(mResult, OtaBuf);
		}
		
		
		
		//mResult = TAG_MSG_HIT_ID;	
		//mResult = TAG_MSG_NO_HIT_ID;
		
		return mResult;
}

uint8_t HandleRecvUpdTagHead(P_RPT_UPD_TAG_HEAD pTagUpdHead)
{
		uint8_t mResult = 0;
		
		if(CompareData(&RptDefSet.RptID[3], &pTagUpdHead->RepeaterID[0], 2)) {
			mindex = pTagUpdHead->Seqno; //backup sequence number
			
			RptGlblVar.TxUpdDataLen = 0; //clear receive update update data length counter
			memcpy(RptGlblVar.UpdHeadBuf[RptGlblVar.UpdCntTmp2], &pTagUpdHead[0], sizeof(RPT_UPD_TAG_HEAD)); //backup update Tag head
			
			mResult = TAG_MSG_HIT_ID;
		}else {
			mResult = TAG_MSG_NO_HIT_ID;
		}
		
		return mResult;
}

/**
  * @brief      Receive Update Tag Data
  * @param[out] ProtocolHeader
  * @return     receive result
  * @details    This API is used receive update Tag data.
  */
uint8_t Rpt_RecvUpdData(uint8_t PktType, uint8_t *UpdDataBuf)
{
		uint8_t mResult = 0;
		uint8_t *pEpdBuf; //= (uint8_t *)RptGlblVar.UpdDataBuf[RptGlblVar.UpdCntTmp2];//[RptGlblVar.TxUpdDataLen];
		//P_RPT_UPD_TAG_HEAD pTagUpdHead;
	
		switch(PktType){
			case RF_PT_PACKET_HEADER:
				if(RptGlblVar.UpdFirmwareFlag != 1){
					//For transparent update Tag
					mResult = HandleRecvUpdTagHead((P_RPT_UPD_TAG_HEAD)UpdDataBuf);
				}else{
					//For OTA
					mResult = HandleRecvUpdFirmwareHead(UpdDataBuf);
				}
				break;
			
			case RF_PT_DATA_PACKET:
				if((UpdDataBuf[0] - mindex) > 1) {
					//DBGMSG("Lost some data...%x - %x\r\n", UpdDataBuf[0], mindex);
					mResult = TAG_MSG_PACKET_LOST; //indicate packet lost
					break;
				}else{
					pEpdBuf = (uint8_t *)&RptGlblVar.UpdDataBuf[RptGlblVar.UpdCntTmp2][RptGlblVar.TxUpdDataLen];
					mindex = UpdDataBuf[0];	//backup sequence number
					memcpy(pEpdBuf, &UpdDataBuf[2], PAYLOAD_LEN - 2); //62 : -two bytes header, 64-2
					RptGlblVar.TxUpdDataLen += (PAYLOAD_LEN - 2);
				}
				break;
		}
	
		return mResult;
}

/**
  * @brief      Receive Tag list packet.
  * @param[out] ProtocolHeader
  * @return     receive result
  * @details    This API is used receive Tag List packet for next action.
  */
uint8_t Rpt_RecvTagList(uint8_t PktType, uint8_t *TagListBuf)
{
		//bool ret;
		uint8_t mtag = 0, mResult = 0;
		P_TAG_UPDATE_LIST pTagUpdateList = (P_TAG_UPDATE_LIST)TagListBuf;
		P_DEVICE_OTA_TAG_LIST pDevieOtaList;
	
		memset(RptGlblVar.TagID, 0, sizeof(RptGlblVar.TagID)); //Clear backup TagID buffer
		//ret = CompareData(pTagUpdateList->RepeaterID, &RptDefSet.RptID[3], sizeof(pTagUpdateList->RepeaterID)); //Compare packet repeater field 
	
		switch(PktType){
			case RF_PT_TAG_UPDATE_LIST:
				//if(ret == true) {
				if(CompareData(pTagUpdateList->RepeaterID, &RptDefSet.RptID[3], sizeof(pTagUpdateList->RepeaterID))) {
					/* Repeater cannot receive more than 3 tag updates data */
					if((pTagUpdateList->PktCtrl & 0x7) > RPT_RX_MAX_UPDATE){
						RptGlblVar.TxUpdTagCnt = RptGlblVar.UpdCntTmp1 = mtag = RPT_RX_MAX_UPDATE;
					}else{
						RptGlblVar.TxUpdTagCnt = RptGlblVar.UpdCntTmp1 = mtag = (pTagUpdateList->PktCtrl & 0x7);		//want update number
					}

					memcpy(&RptGlblVar.TagID[0], &pTagUpdateList->TAGID[0], mtag * 5); //backup update Tag ID
					memcpy(RptGlblVar.TagLstBuf, TagListBuf, PAYLOAD_LEN); //backup received Tag list packet to buffer
					
					mResult = TAG_MSG_HIT_ID; //Match update
				}else{
					mResult = TAG_MSG_NO_HIT_ID;
				}
				break;
				
			case RF_PT_TAG_STATUS:
				memcpy(RptGlblVar.TagLstBuf, TagListBuf, PAYLOAD_LEN); //Copy received Tag list data to buffer
				mResult = TAG_MSG_NO_HIT_ID;
				break;
				
			case RF_PT_REPEATER_STATUS:
				//if(ret == true){
				if(CompareData(pTagUpdateList->RepeaterID, &RptDefSet.RptID[3], sizeof(pTagUpdateList->RepeaterID))){
					Rpt_Status(TAG_MSG_REPORT_STATUS_OK); //reply repeater status
				}
				mResult = TAG_MSG_NO_HIT_ID;
				break;
				
			case RF_PT_TAG_UNBOUND:
			case RF_PT_CHANGE_BEACON_SRC:	
				memcpy(RptGlblVar.TagLstBuf, TagListBuf, PAYLOAD_LEN); //backup received Tag list data to buffer
				printf("Bind & Change Src\r\n");
				mResult = TAG_MSG_NO_HIT_ID;
				break;			
			
			case RF_PT_OVER_THE_AIR:
				pDevieOtaList = (P_DEVICE_OTA_TAG_LIST)TagListBuf;
			
				if((CompareData(pDevieOtaList->RepeaterID, &RptDefSet.RptID[3], sizeof(pDevieOtaList->RepeaterID)))
						&& (pDevieOtaList->Attribution == Attribution_OTA_Repeater))
				{
					RptGlblVar.UpdFirmwareFlag = 1;
					mResult = TAG_MSG_HIT_ID;
				}else{
					mResult = TAG_MSG_NO_HIT_ID;
				}
				break;
		}
		
		return mResult;
}

uint8_t Rpt_RecvBeacon(uint8_t *SyncBuf)
{
		bool ret;
		uint8_t mResult = 0, mhopcount;
		uint16_t Tmr1Cnt = 0;
		P_SYNC_BEACON pSyncBeacon = (P_SYNC_BEACON)SyncBuf;
						
		ret = CheckZero(pSyncBeacon->RepeaterID, 2);
		if(ret == false) {
			DBGMSG("RptID non zero...\r\n");
			return 0;
		}
		
		if(RptGlblVar.gMode != RPT_PWRON_MODE) {
			// check receive gateway beacon reset Timer1 
			//DBGMSG("%d ", RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1));
			if((RptGlblVar.Tmr1DynaVal = (RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1))) <= (RPT_TASK_STOP_EARLY))
			{
				RptGlblVar.Tmr1DynaVal += (RPT_TASK_STOP_EARLY - RptGlblVar.Tmr1DynaVal);
				Tmr1Cnt = ((RPT_TASK_TIMER_VAL + RPT_TASK_STOP_EARLY) - RptGlblVar.Tmr1DynaVal);
			}else{
				RptGlblVar.Tmr1DynaVal -= (RptGlblVar.Tmr1DynaVal - RPT_TASK_STOP_EARLY);
				Tmr1Cnt = ((RPT_TASK_TIMER_VAL + RPT_TASK_STOP_EARLY) - RptGlblVar.Tmr1DynaVal);
			}
			RstTimerValue(TIMER1, Tmr1Cnt);
			//DBGMSG("%x ", pSyncBeacon->Action);
			switch(pSyncBeacon->Action){
				case RF_AL_SYNC:
				case RF_AL_ALL_WU_TIME:
					mResult = TAG_MSG_ONLY_SYNC;
					break;
				
				case RF_AL_OVER_THE_AIR:
					
				case RF_AL_TAG_UNBOUND:
				case RF_AL_CHANGE_BEACON_SRC:
				
				case RF_AL_GET_TAG_STATUS:
				case RF_AL_GET_REPEATER_STATUS:
					
				case RF_AL_UPDATE_TAG:
				case RF_AL_UPDATE_REPEATER_TAG:
					mResult = TAG_MSG_TO_RCV_TAG_LIST;
					break;		
				
				case RF_AL_HOP_TO_REPEATER:
				case RF_AL_HOP_TO_TAG:
					//setSynTimer(TagWakeUpInterval, 0);//Sync time sec
					mhopcount = (pSyncBeacon->ChnCtrl) & 0xF;
				
					//first time received hopping sync beacon
					if(ghopcount == 0) {
							ghopcount = mhopcount;
							gCHindex = ((pSyncBeacon->ChnCtrl) >> 4) & 0xF;
					}			
					
					mResult = TAG_MSG_HOPPING;
				 break;
					
				default:
					mResult = TAG_MSG_RX_TIMEOUT; //TAG_MSG_NO_HIT_ID;
					break;
			}	
			
			RptGlblVar.gTxSeqNo = pSyncBeacon->BeaconSeq; // record Gateway Sequence num
			RptGlblVar.RtnPwrModeCnt = 0; // if the repeater replies sync, clear return power on mode counter.
			
		}else{
			Rpt_HandleBindGateway((P_SYNC_BEACON)SyncBuf);
		}
		
		return mResult;
}

/*********************************************************************
* Rpt_RecvPktType_Proc
*********************************************************************/
uint8_t Rpt_HandleRecvPktType(uint32_t FifoLen, uint8_t *recv)
{
		uint8_t mhopcount;
		uint8_t mResult = 0, i = 0;
		P_RPT_RECV_PKT_TYPE pRecvPktHead = (P_RPT_RECV_PKT_TYPE)recv;

		for(i = 0; i < FifoLen; i++)	{
			recv[i] = RADIO->RX_DATA[i];
		}
	
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
		switch(pRecvPktHead->PacketType){
			case RF_PT_SYNC_BEACON:
				mResult = Rpt_RecvBeacon(recv);
				break;
			
			case RF_PT_OVER_THE_AIR:
			case RF_PT_TAG_UNBOUND:
			case RF_PT_CHANGE_BEACON_SRC:
				
			case RF_PT_TAG_STATUS:
			case RF_PT_REPEATER_STATUS:
				
			case RF_PT_TAG_UPDATE_LIST:
			case RF_PT_ALWAYS_WAKE_BEACON:
				mResult = Rpt_RecvTagList(pRecvPktHead->PacketType, recv);
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
				mResult = Rpt_RecvUpdData(pRecvPktHead->PacketType, recv);
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

uint8_t Rpt_HandleRecvPacket(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t mSec, FunctionalState TimerState)
{
		uint8_t RxPktResult = 0;
		uint8_t WTRFlag = 0;

		M8107_SetFIFOLvl(FifoLen); // Set RF packet fifo length
	
		StrobeCmd(CMD_RX); //RF entry RX state
		Oneshoutflag = 0;	//Time out flag
	
		if(TimerState == ENABLE) {
			SetTimer(TIMER0, mSec); //set get packet timeout timer
		}
		
		while(1) {
			if(WTRFlag == 0) { //Wait CMD_RX action, WTR = 1.
				if((RADIO->STATUS & (1 << 7))){
					WTRFlag = 1; //if RF Rx ready toggle flag to Rx packet
				}
			}else {
				if((!(RADIO->STATUS & (1 << 7))) || (Oneshoutflag)) {
					if(TimerState == ENABLE) {
						Timer_StopTimer(TIMER0);
					}
					StrobeCmd(CMD_STBY); //Back to Standby
					
					if(!Oneshoutflag) {	
						RxPktResult = Rpt_HandleRecvPktType(FifoLen, RecvBuf); //Rx packet
					}else {
						//DBGMSG("Get Pkt Timeout.\r\n");
						RxPktResult = TAG_MSG_RX_TIMEOUT;
					}
					break;
				}
			}
		}
		
		return RxPktResult;
}

uint8_t Rpt_RxMode_RecvOver_the_Air(uint8_t *RecvBuf)
{
		uint8_t mReturn = 0;
		//uint16_t UpdTimeOut = 0;
		P_RPT_UPD_TAG_HEAD pRptUpdHead = (P_RPT_UPD_TAG_HEAD)RptGlblVar.UpdHeadBuf[RptGlblVar.UpdCntTmp2];
		int16_t mlen = pRptUpdHead->PictureSize;
		int16_t SegmentSize = pRptUpdHead->PictureSize;
	
		mindex = 0; //clear sequence num
		Oneshoutflag = 0;	//Timer0 timeout flag

		/*if(mlen != 0) {
			UpdTimeOut = Rpt_CalcRecvUpd_Timeout(mlen); //Calculate when to receive updates
			SetTimer(TIMER0, UpdTimeOut); //set receive update data timeout
		}else {
		*/
		switch(RptDefSet.RptDataRate) {
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
		//}
			
		/* receive update data */
		while(mlen > 0) {
			mReturn = Rpt_HandleRecvPacket(PAYLOAD_LEN, RecvBuf, 0, DISABLE);
			
			if(Oneshoutflag) {
				//DBGMSG("DATA_STATE___RX data timer out.\r\n");
				mReturn = TAG_MSG_PACKET_LOST;
				break;
			}else{
				mlen -= (PAYLOAD_LEN - 2);		//two bytes header
			}
		}
		Timer_StopTimer(TIMER0); //stop Timer0 of receive complete
		StrobeCmd(CMD_STBY); //exit rx mode

		if((Oneshoutflag) || (mReturn == TAG_MSG_PACKET_LOST)) {	//Don't need update EPD because data error.		
			//receive update data fail, clear update head, keep update size for TX mode use
			memset(&RptGlblVar.UpdHeadBuf[RptGlblVar.UpdCntTmp2], 0, sizeof(RPT_UPD_TAG_HEAD) - 2);	
			mReturn = TAG_MSG_RX_TIMEOUT;
		}else {
			//if(RptGlblVar.UpdFirmwareFlag == 1){				
				WriteNewAppBin(RptGlblVar.WrNewAppAddress, RptGlblVar.UpdDataBuf[RptGlblVar.UpdCntTmp2], SegmentSize);
				
				LA_GPIO_LOW();
				//OTA_Init();
				//__disable_irq();
				//NVIC_SystemReset();
			
				//RptGlblVar.BckFirmwareIndex++;
				// ACK updata firmware Status, contain reply segment index
				
			//}
			mReturn = TAG_MSG_RX_COMPLETED;
		}
		Rpt_Status(mReturn); //Receipt complete reply repeater status
		
		RptGlblVar.TxUpdDataLen = 0; //clear receive update data counter
		RptGlblVar.UpdCntTmp2++; 
		
		//if(RptGlblVar.UpdFirmwareFlag != 1){
			if(RptGlblVar.UpdCntTmp2 < RptGlblVar.UpdCntTmp1){
				//RptGlblVar.gState = HEADER_STATE;
				//RptGlblVar.RecvLen = PAYLOAD_LEN;
				//RptGlblVar.RecvTimer = PRT_RX_PKT_TIMER;			
				mReturn = TAG_MSG_HIT_ID;
			}else{
				RptGlblVar.UpdCntTmp2 = RptGlblVar.gState = 0;
				//mReturn = TAG_MSG_NO_HIT_ID;
			}	
		/*}else {
			RptGlblVar.UpdCntTmp2 = 0;
			if(RptGlblVar.UpdFirmwareCount != RptGlblVar.BckFirmwareIndex){
				mReturn = TAG_MSG_HIT_ID;
			}else{
				OTA_Init();		
				__disable_irq();
				NVIC_SystemReset();
			}
		}
		*/
		
		return mReturn;
}

uint8_t Rpt_RxMode_RecvUpdData(uint8_t *RecvBuf)
{
		uint8_t mReturn = 0;
		//uint16_t UpdTimeOut = 0;
		P_RPT_UPD_TAG_HEAD pRptUpdHead = (P_RPT_UPD_TAG_HEAD)RptGlblVar.UpdHeadBuf[RptGlblVar.UpdCntTmp2];
		int16_t mlen = pRptUpdHead->PictureSize;
	
		mindex = 0; //clear sequence num
		Oneshoutflag = 0;	//Timer0 timeout flag

		/*if(mlen != 0) {
			UpdTimeOut = Rpt_CalcRecvUpd_Timeout(mlen); //Calculate when to receive updates
			SetTimer(TIMER0, UpdTimeOut); //set receive update data timeout
		}else {
		*/
		switch(RptDefSet.RptDataRate) {
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
		//}
			
		/* receive update data */
		while(mlen > 0) {
			mReturn = Rpt_HandleRecvPacket(PAYLOAD_LEN, RecvBuf, 0, DISABLE);
			
			if(Oneshoutflag) {
				//DBGMSG("DATA_STATE___RX data timer out.\r\n");
				mReturn = TAG_MSG_PACKET_LOST;
				break;
			}else{
				mlen -= (PAYLOAD_LEN - 2);		//two bytes header
			}
		}
		Timer_StopTimer(TIMER0); //stop Timer0 of receive complete
		StrobeCmd(CMD_STBY); //exit rx mode

		if((Oneshoutflag) || (mReturn == TAG_MSG_PACKET_LOST)) {	//Don't need update EPD because data error.		
			//receive update data fail, clear update head, keep update size for TX mode use
			memset(&RptGlblVar.UpdHeadBuf[RptGlblVar.UpdCntTmp2], 0, sizeof(RPT_UPD_TAG_HEAD) - 2);	
			mReturn = TAG_MSG_RX_TIMEOUT;
		}else {
			mReturn = TAG_MSG_RX_COMPLETED;
		}
		Rpt_Status(mReturn); //Receipt complete reply repeater status
		
#ifndef _NO_DBG_PRINTF		
		if(mReturn == TAG_MSG_RX_TIMEOUT){
			DBGMSG("Data Lost = %d, %d\r\n", RptGlblVar.UpdCntTmp2, RptGlblVar.TxUpdDataLen);
		}else{
			DBGMSG("Recv Cmpl %d %d\r\n", RptGlblVar.UpdCntTmp2, RptGlblVar.TxUpdDataLen);
		}
#endif // _NO_DBG_PRINTF
		
		RptGlblVar.TxUpdDataLen = 0; //clear receive update data counter
		RptGlblVar.UpdCntTmp2++; 
	
		if(RptGlblVar.UpdCntTmp2 < RptGlblVar.UpdCntTmp1){
			//RptGlblVar.gState = HEADER_STATE;
			//RptGlblVar.RecvLen = PAYLOAD_LEN;
			//RptGlblVar.RecvTimer = PRT_RX_PKT_TIMER;
			mReturn = TAG_MSG_HIT_ID;
		}else{
			RptGlblVar.UpdCntTmp2 = RptGlblVar.gState = 0;
			mReturn = TAG_MSG_NO_HIT_ID;
		}	
		
		return mReturn;
}

#if 1
void Rpt_RxModeTask(uint8_t TaskState)
{
		uint8_t mReturn = 0;
		uint8_t RxTaskBuf[64] = {0};
		uint16_t Tmr1Cnt = 0;
		
		if(TaskState == 0) {
			return ;
		}
		
		switch(TaskState) {
			case DATA_STATE:
				if(RptGlblVar.UpdFirmwareFlag != 1){
					mReturn = Rpt_RxMode_RecvUpdData(RxTaskBuf);
				}else{
					mReturn = Rpt_RxMode_RecvOver_the_Air(RxTaskBuf);
				}
				break;
			
			default:
				mReturn = Rpt_HandleRecvPacket(RptGlblVar.RecvLen, RxTaskBuf, RptGlblVar.RecvTimer, ENABLE);
				break;
		}

		/* Handle return runtime message */
		switch(mReturn) {
			case TAG_MSG_HIT_ID:
				if((TaskState == TAG_LIST_STATE) || (TaskState == DATA_STATE)) 
				{
					RptGlblVar.gState = HEADER_STATE;
				}else if(TaskState == HEADER_STATE){
					RptGlblVar.gState = DATA_STATE;
				}
				
				//set receive timer & packet length
				RptGlblVar.RecvLen = PAYLOAD_LEN;
				RptGlblVar.RecvTimer = PRT_RX_PKT_TIMER;
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
			
				RptGlblVar.RecvLen = PAYLOAD_LEN;
				RptGlblVar.RecvTimer = PRT_RX_PKT_TIMER;	
				break;
			
			case TAG_MSG_RX_TIMEOUT:
				if(((TaskState == BEACON_STATE) || (TaskState == PWRBEACON_STATE)) 
					&& (RptGlblVar.gMode != RPT_PWRON_MODE))
				{
					Tmr1Cnt = (RPT_TASK_TIMER_VAL + RPT_TASK_STOP_EARLY) - (RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1));
					RstTimerValue(TIMER1, Tmr1Cnt);
					
					RptGlblVar.gState = 0; //PWRBEACON_STATE; //BEACON_STATE
					RptGlblVar.RtnPwrOnMode = 1;
					
					printf("Lost Beacon...%d\r\n", Tmr1Cnt);
				}
				break;
		}
}
#endif //0

#if 0
void Rpt_RxModeTask(uint8_t TaskState)
{
		uint8_t mReturn = 0;
		uint8_t RxTaskBuf[64] = {0};
		uint16_t Tmr1Cnt = 0;

		switch(TaskState) {
			case PWRBEACON_STATE:
				mReturn = Rpt_RxMode_RecvPacket(SYNC_LEN, RxTaskBuf, RPT_PWRON_BEACON_TIMER, ENABLE);
				break;	
			
			case BEACON_STATE:
				mReturn = Rpt_RxMode_RecvPacket(SYNC_LEN, RxTaskBuf, RPT_BEACON_TIMER, ENABLE);
				break;
		
			case TAG_LIST_STATE:
				mReturn = Rpt_RxMode_RecvPacket(PAYLOAD_LEN, RxTaskBuf, PRT_RX_PKT_TIMER, ENABLE);
				break;	

			case HEADER_STATE:
				mReturn = Rpt_RxMode_RecvPacket(PAYLOAD_LEN, RxTaskBuf, PRT_RX_PKT_TIMER, ENABLE);
				break;
					
			case DATA_STATE:
				if(RptGlblVar.UpdFirmwareFlag != 1){
					mReturn = Rpt_RxMode_RecvUpdData(RxTaskBuf);
				}else{
					mReturn = Rpt_RxMode_RecvOver_the_Air(RxTaskBuf);
				}
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
				break;
		}
				
		switch(mReturn) {
			case TAG_MSG_HIT_ID:
				switch(TaskState) {
					//case DATA_STATE:
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
				
				RptGlblVar.RecvLen = PAYLOAD_LEN;
				RptGlblVar.RecvTimer = PRT_RX_PKT_TIMER;
				break;
			
			case TAG_MSG_RX_TIMEOUT:
				switch(TaskState){
					case PWRBEACON_STATE:
					case BEACON_STATE:
						if(RptGlblVar.gMode != RPT_PWRON_MODE){
							Tmr1Cnt = (RPT_TASK_TIMER_VAL + RPT_END_EARLY_TIMER) - (RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1));
							RstTimerValue(TIMER1, Tmr1Cnt);
							
							RptGlblVar.gState = 0; //PWRBEACON_STATE; //BEACON_STATE
							RptGlblVar.RtnPwrOnMode = 1;
							
							printf("Lost Beacon...%d\r\n", Tmr1Cnt);
						}
						break;
				}
				break;
		}
}
#endif //0

//**********************************************************************************
