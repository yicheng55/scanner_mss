#include "main.h"
#include "RptProc.h"
#include "RptDataFlash.h"
#include "RptComm.h"

extern uint8_t AES_KEY[];

void Rpt_HandleScnrPacket(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t mSec)
{
		uint8_t RecvResult = 0;
		P_SCAN_PACKET_HEAD pScanPktHead = (P_SCAN_PACKET_HEAD)RecvBuf;
	
		memset(RecvBuf, 0, FifoLen);
		RecvResult = M8107RF_RecvPacket(FifoLen, RecvBuf, mSec, ENABLE);

		if(pScanPktHead->byPacketType != RF_PT_SCANNER_PACKET){
			return ;
		}
		
		if(RecvResult == TAG_MSG_RX_COMPLETED) {
			if(CompareData(&RptDefSet.RptID[3], pScanPktHead->byRepeaterID, 2)){
				printf("SNCR %d\r\n", RecvResult);
				M8107RF_SetCHAN(RptDefSet.Gchn); //Change channel of gateway
				M8107RF_SendPacket(FifoLen, RecvBuf); //Transparent scanner packet to gateway
			
				RecvResult = M8107RF_RecvPacket(FifoLen, RecvBuf, mSec, ENABLE);
			
				if(RecvResult == TAG_MSG_RX_COMPLETED){
					printf("ACK %d\r\n", RecvResult);
					
					M8107RF_SetCHAN(RptDefSet.Tchn); //Change channel of Gateway
					M8107RF_SendPacket(FifoLen, RecvBuf); //Transparent gateway ack to scanner
				}
			}
		}
}

/**
  * @brief      Repeater reply Tag combine status to Gateway
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used reply Tag status to Gateway.
  */
void Rpt_SendTagCombRslt(uint8_t result, uint8_t MultiTag, uint8_t *TagCombBuf)
{
		uint8_t i = 0;
		P_RPT_COMB_TAG_UPDRS RptCombTagUpdRs = (P_RPT_COMB_TAG_UPDRS)TagCombBuf;
		
		M8107RF_SetCHAN(RptDefSet.Gchn); //Change RF Channel of Gateway
	
		RptCombTagUpdRs->SeqNum = RptGlblVar.gTxSeqNo++;
		RptCombTagUpdRs->PckType = RF_PT_REPEATER_COMBTAG_RS;
		RptCombTagUpdRs->Result = result;
		RptCombTagUpdRs->MultiTag = MultiTag;
		RptCombTagUpdRs->ResultLen = sizeof(RPT_COMB_TAG_UPDRS);
	
		/*for(i = 0; i < PAYLOAD_LEN; i++){
			DBGMSG("%x ", TxBuf[i]);
		}
		DBGMSG("\r\n");
		*/

		for(i = 0; i < 5; i++){
			M8107RF_SendPacket(PAYLOAD_LEN, TagCombBuf);
			Delay1us(1450);
		}
}

void Rpt_HdleReplyPacket(uint8_t Action, uint8_t mResult, uint8_t *RecvRsltBuf)
{
		uint8_t i = 0;
		P_RPT_COMB_TAG_STATUS pCombTagStatus = (P_RPT_COMB_TAG_STATUS)&RecvRsltBuf[2];
		P_TAG_UPDATE_LIST pTxTagList = (P_TAG_UPDATE_LIST)RptGlblVar.TagLstBuf;
		//P_RPT_UPD_TAG_HEAD pRptUpdHead;// = (P_RPT_UPD_TAG_HEAD)RptGlblVar.UpdHeadBuf[RptGlblVar.UpdCntTmp2];

		switch(Action) {
			case RF_AL_UPDATE_TAG:
			case RF_AL_UPDATE_REPEATER_TAG:
				// Rpt_CombTag_Result(mResult, TagRsltBuf);
				switch(mResult){ 
					case TAG_MSG_RX_TIMEOUT:
						pCombTagStatus->Result = TAG_MSG_RX_TIMEOUT;
						memcpy(pCombTagStatus->TAG_ID, &RptGlblVar.TagID[RptGlblVar.UpdCntTmp2 * 5], 5);
						break;
					case TAG_MSG_RX_COMPLETED:
						memcpy(&RecvRsltBuf[16], &RecvRsltBuf[17], 2); // Aligment Tag Result formate
						break;
				}
				
				// copy Tag result to combine buffer
				memcpy(&RptGlblVar.TxBuffer[6 + RptGlblVar.AccuTagRsltLen], &RecvRsltBuf[2], sizeof(RPT_COMB_TAG_STATUS));		
				RptGlblVar.AccuTagRsltLen += (sizeof(RPT_COMB_TAG_STATUS));
			
				RptGlblVar.RxTagUpdRslt = TAG_MSG_RX_COMPLETED; //Repeater receive Tag result complete.
				//RptGlblVar.UpdTagCmpl = 1; //trigger reply combine Tag result flag
				RptGlblVar.RxCombTagRsCnt++; // count receive Tag result 
				
				memset(RptGlblVar.UpdHeadBuf[RptGlblVar.UpdCntTmp2], 0, sizeof(RPT_UPD_TAG_HEAD));
				RptGlblVar.UpdCntTmp2++;
				//pRptUpdHead = (P_RPT_UPD_TAG_HEAD)RptGlblVar.UpdHeadBuf[RptGlblVar.UpdCntTmp2];
				//RptGlblVar.TxUpdDataLen = pRptUpdHead->PictureSize;
				//RptGlblVar.TxUpdDataLen = RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2]; //copy update Tag length
				//RptGlblVar.TxUpdBufPtrAddr += RptGlblVar.TxUpdDataLen; // move update data buffer adress
				
				//RptGlblVar.UpdCntTmp1++; 
				RptGlblVar.TxUpdTagCnt--;
				break;
			
			case RF_AL_GET_TAG_STATUS:
				switch(mResult){
					case TAG_MSG_RX_TIMEOUT:
						pCombTagStatus->Result = TAG_MSG_RX_TIMEOUT;
						memcpy(pCombTagStatus->TAG_ID, pTxTagList->TAGID, 5);
						break;
					case TAG_MSG_RX_COMPLETED:
						memcpy(&RecvRsltBuf[16], &RecvRsltBuf[17], 2); // Aligment Tag Result formate
						break;
				}
				memcpy(&RptGlblVar.TxBuffer[6], &RecvRsltBuf[2], sizeof(RPT_COMB_TAG_STATUS));	

				Rpt_SendTagCombRslt(mResult, RptGlblVar.TxUpdTagCnt, RptGlblVar.TxBuffer);
				break;
			
			case RF_AL_TAG_UNBOUND:
			case RF_AL_CHANGE_BEACON_SRC:
				M8107RF_SetCHAN(RptDefSet.Gchn); //Change Repeater Channel of Tag
			
				for(i = 0; i < 5; i++){
					M8107RF_SendPacket(PAYLOAD_LEN, RecvRsltBuf);
					Delay1us(1450);
				}
				break;
		}
}

/**
  * @brief      Repeater send update head & Data to Tag
  * @param[out] None
  * @return     None
  * @details    This API is used send update head & Data to Tag.
  */
void Rpt_SendUpdateData(int16_t UpdDataLen, uint8_t *UpdDataBuf)
{
		uint8_t DataSeqNo = 0;
		uint16_t j = 0;
		P_RPT_UPD_TAG_DATA pSendBigData = (P_RPT_UPD_TAG_DATA)UpdDataBuf;

		//Send update data to Tag
		while(UpdDataLen > 0) {
			pSendBigData->Seqno = DataSeqNo++;
			pSendBigData->PacketType = RF_PT_DATA_PACKET;
			//memcpy(pSendBigData->toTagData, &EpdRFData[RptGlblVar.TxUpdBufPtrAddr] + j, PAYLOAD_LEN - 2);
			memcpy(pSendBigData->toTagData, &RptGlblVar.UpdDataBuf[RptGlblVar.UpdCntTmp2][0 + j], PAYLOAD_LEN - 2);
			
			M8107RF_SendPacket(PAYLOAD_LEN, UpdDataBuf); //TX Packet
			
			UpdDataLen -= (PAYLOAD_LEN - 2);
			j += (PAYLOAD_LEN - 2);
			
			Delay1us(125); //wait slave receive
		}
}

void Rpt_SendUpdateHead(uint8_t *UpdHeadBuf)
{
		//uint8_t DataSeqNo = 0;
		P_RPT_UPD_TAG_HEAD pSendBigDataHead = (P_RPT_UPD_TAG_HEAD)UpdHeadBuf;
		
		//send head to Tag
		/*pSendBigDataHead->Seqno = RptGlblVar.gTxSeqNo++;
		pSendBigDataHead->PacketType = RF_PT_PACKET_HEADER;
		memcpy(&pSendBigDataHead->TagID[0], &TagID[RptGlblVar.UpdCntTmp1 * 5], 5);
		memcpy(&pSendBigDataHead->GetwayID[0], &GatewayID[0], 2);
		memcpy(&pSendBigDataHead->RepeaterID[0], &RptDefSet.RptID[3], 2);
		pSendBigDataHead->Compress = 0;
		pSendBigDataHead->PictureSize = RptGlblVar.TxUpdDataLen;
		*/
		memcpy(&pSendBigDataHead->RepeaterID[0], &RptDefSet.RptID[3], 2);
	
		M8107RF_SendPacket(PAYLOAD_LEN, UpdHeadBuf); //TX Packet
}

void Rpt_TransUpdTagData(uint8_t Action, uint32_t FifoLen, uint8_t *TaskBuf)
{
		uint8_t RecvResult = 0;
		int16_t UpdStartTime = 0, UpdDelayTime = 0, UpdTimeout = 0;
		P_RPT_UPD_TAG_HEAD pRptUpdHead;
	
		// Clear some variable
		/*RptGlblVar.RxUpdDataDone = RptGlblVar.TxUpdBufPtrAddr = 
			RptGlblVar.TxUpdDataLen = RptGlblVar.AccuTagRsltLen = */
				//RptGlblVar.UpdCntTmp1 = 
		RptGlblVar.AccuTagRsltLen = RptGlblVar.TxUpdDataLen = RptGlblVar.UpdCntTmp2 = 0;

		//copy the first update data length
		//RptGlblVar.TxUpdDataLen = RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2];

		//start send update Tag head & raw data
		while(RptGlblVar.TxUpdTagCnt > 0) {
			//copy the first update data length
			pRptUpdHead = (P_RPT_UPD_TAG_HEAD)RptGlblVar.UpdHeadBuf[RptGlblVar.UpdCntTmp2];
			RptGlblVar.TxUpdDataLen = pRptUpdHead->PictureSize;

			//Calculate when to trans updates
			/*if(RptGlblVar.TxUpdDataLen != 0){
				UpdTimeout = Rpt_CalcRecvUpd_Timeout(RptGlblVar.TxUpdDataLen);	
			}
			*/
			UpdStartTime = TASK_CUR_RUNTIME(TIMER1); //Keep starting update time
			
			switch(RptDefSet.RptDataRate) {
				case RF_DATARATE_1M:
					if(RptGlblVar.TxUpdDataLen != 0){
						Delay1us(2750); //Delay 2.75ms for send Head
						Rpt_SendUpdateHead(TaskBuf);
						Delay1us(125); //Delay 0.125ms for send Data
						Rpt_SendUpdateData(pRptUpdHead->PictureSize, TaskBuf);
					}else{
						DBGMSG("No Data Upd\r\n");
						Delay1ms(RPT_1M_UPDTIMER - 12);
						UpdTimeout = RPT_1M_UPDTIMER;
					}
					break;
				
				case RF_DATARATE_500K:
					if(RptGlblVar.TxUpdDataLen != 0){
						Delay1us(1450); //Delay 1.45ms for send Head
						Rpt_SendUpdateHead(RptGlblVar.UpdHeadBuf[RptGlblVar.UpdCntTmp2]);
						Delay1us(500); //Delay 0.05ms for send Data
						Rpt_SendUpdateData(RptGlblVar.TxUpdDataLen, TaskBuf);
					}else{
						DBGMSG("No Data Upd\r\n");
						Delay1ms(RPT_500K_UPDTIMER - 12);
						UpdTimeout = RPT_500K_UPDTIMER;
					}
					break;
				
				case RF_DATARATE_250K:
					if(RptGlblVar.TxUpdDataLen != 0){
						Delay1us(2400); //Delay 2.4ms for send Head
						Rpt_SendUpdateHead(TaskBuf);
						Delay1us(2000); //Delay 2.0ms for send Data
						Rpt_SendUpdateData(RptGlblVar.TxUpdDataLen, TaskBuf);
					}else{
						DBGMSG("No Data Upd\r\n");
						Delay1ms(RPT_250K_UPDTIMER - 12);
						UpdTimeout = RPT_250K_UPDTIMER;
					}
					break;
			}
			
			RecvResult = M8107RF_RecvPacket(FifoLen, TaskBuf, 6, ENABLE);
			Rpt_HdleReplyPacket(Action, RecvResult, TaskBuf);
			DBGMSG("Tag Result = %d %d\r\n", RecvResult, RptGlblVar.TxUpdTagCnt);
			
			UpdDelayTime = (UpdTimeout - (UpdStartTime - TASK_CUR_RUNTIME(TIMER1)));
			if(UpdDelayTime > 0) {
				Delay1ms(UpdDelayTime);
			}
		}

		while(TASK_CUR_RUNTIME(TIMER1) < 30); // wait Timer1 value < 30
		Rpt_SendTagCombRslt(RptGlblVar.RxTagUpdRslt, RptGlblVar.RxCombTagRsCnt, RptGlblVar.TxBuffer); // Reply Combine Tag Status to Gateway
}

void Rpt_HandleBeacon_Action(uint8_t Action, uint32_t FifoLen, uint8_t *TaskBuf)
{
		uint8_t RecvResult = 0;
		
		switch(Action){
			case RF_AL_SYNC:
				YELLOW_LED_LOW(); 
				Rpt_HandleScnrPacket(FifoLen, TaskBuf, RPT_HANDLE_SCNR_TIMER); // Only Sync Beacon , entry handle scanner packet
				break;
			
			case RF_AL_TAG_UNBOUND:
			case RF_AL_CHANGE_BEACON_SRC:
				
			case RF_AL_GET_TAG_STATUS:
			case RF_AL_TAG_ALWAYS_WAKE_UP:
				M8107RF_SendPacket(FifoLen, RptGlblVar.TagLstBuf); //Send Tag list to Tag
				
				RecvResult = M8107RF_RecvPacket(FifoLen, TaskBuf, 250, ENABLE);
				if(Action == RF_AL_TAG_UNBOUND){
					DBGMSG("Bind %d\r\n", RecvResult);
				}else if(Action == RF_AL_CHANGE_BEACON_SRC){
					DBGMSG("ChgBCN %d %x:%x\r\n", RecvResult, RptGlblVar.TagLstBuf[11], RptGlblVar.TagLstBuf[12]);
				}
				
				if(RecvResult == TAG_MSG_RX_COMPLETED){
					Rpt_HdleReplyPacket(Action, RecvResult, TaskBuf);
				}
				break;
			
			case RF_AL_HOP_TO_TAG:
			case RF_AL_HOP_TO_REPEATER:
				DBGMSG("ghopcount = %d\r\n", ghopcount);
				if(ghopcount == 1){		//Hopping
					DBGMSG("Hop to channel = %d\r\n", CHTable[gCHindex]);
					//StrobeCmd(CMD_STBY);  
					//SetCH(CHTable[gCHindex]); //freq 2450MHz CH100
					RptDefSet.Tchn = RptDefSet.Gchn = CHTable[gCHindex];
					M8107RF_SetCHAN(RptDefSet.Tchn);
					
					RptGlblVar.HotBcnFlag = ghopcount = 0;
					Rpt_Write_DataFlash();
					
					RptGlblVar.RtnPwrOnMode = 1;
				}/*else if(ghopcount > 0){
					ghopcount--;
				}*/
				break;
				
			case RF_AL_UPDATE_TAG:
			case RF_AL_UPDATE_REPEATER_TAG:
				if(RptGlblVar.RxUpdDataDone == 1) {  // have update
					RptGlblVar.RxUpdDataDone = 0; //Clear Repeater Update Tag Flag
					
					//memcpy(pTxTagList->TAGID, TagID, RptGlblVar.TxUpdTagCnt * 5);
					M8107RF_SendPacket(FifoLen, RptGlblVar.TagLstBuf); //Send Tag list to Tag
					
					Rpt_TransUpdTagData(Action, FifoLen, TaskBuf);
				}
				break;
				
			case RF_AL_GET_REPEATER_STATUS:
				break;
		}
		
		//Hop Beacon change Channel count
		if(RptGlblVar.HotBcnFlag == 1){
			RptGlblVar.HotBcnFlag = 0;
			if(ghopcount > 0){
				ghopcount--;
			}
		}

		YELLOW_LED_HIGH();
}

/**
  * @brief      Repeater receive gateway beacon lose, send fake beacon.
  * @param[out] None
  * @return     None
  * @details    This API is used send fake beacon packet.
  */
uint8_t Rpt_SendBeaconPacket(uint32_t FifoLen, uint8_t *SyncBuf)
{
		P_SYNC_BEACON pTxSyncBeacon = (P_SYNC_BEACON)SyncBuf;
	
		if(RptGlblVar.RtnPwrOnMode == 1){
			// Repeater rx beacon fake, create beacon send to tag 
			pTxSyncBeacon->BeaconSeq = RptGlblVar.gTxSeqNo++;
			pTxSyncBeacon->BeaconType = RF_PT_SYNC_BEACON;
			pTxSyncBeacon->Action = RF_AL_SYNC;
			pTxSyncBeacon->ChnCtrl = 0;
			pTxSyncBeacon->WakeUpTime = 1;
		}else{
			// Repeater recv update data fail, change action type to Sync
			if((RptGlblVar.RxUpdDataDone != 1) && 
					((pTxSyncBeacon->Action == RF_AL_UPDATE_TAG) || 
						(pTxSyncBeacon->Action == RF_AL_UPDATE_REPEATER_TAG)))
			{
				pTxSyncBeacon->Action = RF_AL_SYNC;
			}else{
				switch(pTxSyncBeacon->Action){
					case RF_AL_UPDATE_REPEATER_TAG:
						pTxSyncBeacon->Action = RF_AL_UPDATE_TAG;
						break;
					
					case RF_AL_GET_REPEATER_STATUS:
						pTxSyncBeacon->Action = RF_AL_SYNC;
						break;
				}
			}
		}
		// copy Repeater ID to SyncBeacon RepeaterID field
		memcpy(pTxSyncBeacon->RepeaterID, &RptDefSet.RptID[3], 2);
		
		// Encrypt Beacon packet
		if(RptGlblVar.cryptflag == 1){
			RptGlblVar.cryptflag = 0; //clear crypt flag
			
			AES128_Encrypt(SyncBuf, AES_KEY, EncryptData);
			M8107RF_SendPacket(FifoLen, EncryptData);
		}else{
			M8107RF_SendPacket(FifoLen, SyncBuf);
		}
		
		return pTxSyncBeacon->Action;
}

/*****************************************************************************
 * Rpt_TxTask_Proc()
 * Packet format:
	1. SeqNo: Sequence number of Beacon packet.
	2. Type: Packet Type
		* #define SYNCBEACON          1
		* #define HEADERPACKET        2
		* #define DATAPACKET          3
		* #define RXDATARESULT        4

	3. PktCtl: Packet content control
		* b[2:0]: update number. 0 = No update
		* 

	4. CHCtl: Hopping control
		* b[3:0]: Channel Index
		* b[7:4]: Change after count

	5. WuTime: Sleep time = 1, 2, 5, 10, 20, 40 sec
	6. SAID: Source ID
	7. DAID0: Destination ID0
	8. Len0: Data length of DAID0
	9. DAID1: Destination ID1
	10. Len1: Data length of DAID1
	11. DAID & Len pair max. = 5
 *	
 *****************************************************************************/
void Rpt_TxModeTask(void)
{
		uint8_t action = 0;
		uint8_t TxTaskBuf[64] = {0};
		
		//Tx Beacon
		action = Rpt_SendBeaconPacket(SYNC_LEN, RptGlblVar.SyncBuf);
	
		Delay1us(125);
		
		//handle next action
		Rpt_HandleBeacon_Action(action, PAYLOAD_LEN, TxTaskBuf);
}
