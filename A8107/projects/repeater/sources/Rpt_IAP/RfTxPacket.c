#include "main.h"
#include "RptProc.h"
#include "RptSCNR.h"
#include "RptDataFlash.h"

extern uint8_t AES_KEY[];

/**
  * @brief      Repeater transfer packet process
  * @param[out] None
  * @return     None
  * @details    This API is used transfer packet.
  */
void Rpt_TxPktProc(uint8_t length, uint8_t *buffer)
{
		uint8_t i = 0;
		
		SetFIFOLvl(length);
		
		for(i = 0; i < length; i++){
			RADIO->TX_DATA[i] = buffer[i];
			//printf("%x ", buffer[i]);
		}
		//printf("\r\n");
		
		StrobeCmd(CMD_TX); //entry tx & transmit
		Delay10us(1);
		while(RADIO->STATUS & (1 << 7)); 	//WTR
}

/**
  * @brief      Repeater reply Tag status to Gateway
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used reply Tag status to Gateway.
  */
void Rpt_RpyTagCombResult(uint8_t result, uint8_t MultiTag, uint8_t *TxBuf)
{
		//uint8_t i = 0;
		P_RPT_COMB_TAG_UPDRS RptCombTagUpdRs = (P_RPT_COMB_TAG_UPDRS)TxBuf;
		
		RptCombTagUpdRs->SeqNum = RptGlblVar.gTxSeqNo++;
		RptCombTagUpdRs->PckType = RF_PT_REPEATER_COMBTAG_RS;
		RptCombTagUpdRs->Result = result;
		RptCombTagUpdRs->MultiTag = MultiTag;
		RptCombTagUpdRs->ResultLen = sizeof(RPT_COMB_TAG_UPDRS);
		
		/*
		for(i = 0; i < PAYLOAD_LEN; i++){
			printf("%x ", TxBuf[i]);
		}
		printf("\r\n");
		*/
		
		Rpt_TxPktProc(PAYLOAD_LEN, TxBuf);
	
		RptGlblVar.RxCombTagRsCnt = 0;
}

void Rpt_CombTag_Result(uint8_t *TagRsltBuf)
{
		// copy Tag result to buffer
		memcpy(&RptGlblVar.TxBuffer[6 + RptGlblVar.AccuTagRsltLen], &TagRsltBuf[2], sizeof(RPT_COMB_TAG_STATUS));		
		RptGlblVar.AccuTagRsltLen += (sizeof(RPT_COMB_TAG_STATUS));
	
		RptGlblVar.RxTagUpdRslt = TAG_MSG_RX_COMPLETED; //Repeater receive Tag result complete.
		RptGlblVar.UpdTagCmpl = 1; //trigger reply combine Tag result flag
		RptGlblVar.RxCombTagRsCnt++; // count receive Tag result 
		RptGlblVar.UpdCntTmp1++; // update next Tag
	
		RptGlblVar.TxUpdDataLen = RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.RxCombTagRsCnt]; //copy update Tag length
		RptGlblVar.TxUpdBufPtrAddr += RptGlblVar.TxUpdDataLen; // move update data buffer adress
		
		RptGlblVar.TxUpdTagCnt--;
}

/**
  * @brief      Repeater receive Tag update result & Tag status
  * @param[out] None
  * @return     None
  * @details    This API is used receive Tag update result & Tag status.
  */
void Rpt_RxTag_RpyResult(uint8_t Action, uint32_t msec)
{
		uint8_t i = 0, mResult = 0;
		uint8_t TagRsltBuf[64] = {0};
		P_RPT_COMB_TAG_STATUS pCombTagStatus = (P_RPT_COMB_TAG_STATUS)&TagRsltBuf[2];

		StrobeCmd(CMD_RX); //entry rx & receive
		/** Received Slave return packet and print out total packet **/	
		Oneshoutflag = 0;
		SetTimer(TIMER0, msec);
	
		while((RADIO->STATUS & (1 << 7)) && (Oneshoutflag == 0)); 	//WTR	
		
		if(Oneshoutflag){
			StrobeCmd(CMD_STBY);
			// Receive Tag update Result fake, create timeout packet to Gateway
			pCombTagStatus->Result = TAG_MSG_RX_TIMEOUT;
			memcpy(pCombTagStatus->TAG_ID, &TagID[RptGlblVar.UpdCntTmp1 * 5], 5);
				
			mResult = TAG_MSG_RX_TIMEOUT;
			printf("Tag Result Timeout..\r\n");
		}else {
			for(i = 0; i < PAYLOAD_LEN; i++){
				TagRsltBuf[i] = RADIO->RX_DATA[i];
				//printf("%x ", TagRsltBuf[i]);
			}
			//printf("\r\n");
			
			memcpy(&TagRsltBuf[16], &TagRsltBuf[17], 2); // Aligment Tag Result formate
			
			printf("Tag Result Complete..\r\n");
			mResult = TAG_MSG_RX_COMPLETED;
		}
		
		if((Action == RF_AL_UPDATE_TAG) || (Action == RF_AL_UPDATE_REPEATER_TAG)){
			Rpt_CombTag_Result(TagRsltBuf);
			while(Oneshoutflag != 1);
		}else if (Action == RF_AL_GET_TAG_STATUS){ // Reply Tag Status 
			Rpt_SetRF_Channel(RptDefSet.Gchn); //Change RF Channel of Gateway
			
			memcpy(&RptGlblVar.TxBuffer[6], &TagRsltBuf[2], sizeof(RPT_COMB_TAG_STATUS));	
			
			while(Oneshoutflag != 1);
			Rpt_RpyTagCombResult(mResult, RptGlblVar.TxUpdTagCnt, RptGlblVar.TxBuffer);
		}
}

/**
  * @brief      Repeater send update head & Data to Tag
  * @param[out] None
  * @return     None
  * @details    This API is used send update head & Data to Tag.
  */
void Rpt_TxUpdPack_Proc(void)
{
		uint8_t DataSeqNo = 0;
		uint16_t j = 0;
		uint8_t UpdBuf[64] = {0};
		P_RPT_UPD_TAG_HEAD pSendBigDataHead = (P_RPT_UPD_TAG_HEAD)UpdBuf;
		P_RPT_UPD_TAG_DATA pSendBigData;
		
		//Delay1us(3500);//Delay1us(4450);
		//send head to Tag
		pSendBigDataHead->Seqno = RptGlblVar.gTxSeqNo++;
		pSendBigDataHead->PacketType = RF_PT_PACKET_HEADER;
		memcpy(&pSendBigDataHead->TagID[0], &TagID[RptGlblVar.UpdCntTmp1 * 5], 5);
		memcpy(&pSendBigDataHead->GetwayID[0], &AgentID[0], 2);
		memcpy(&pSendBigDataHead->RepeaterID[0], &RptDefSet.RptID[3], 2);
		pSendBigDataHead->Compress = 0;
		pSendBigDataHead->PictureSize = RptGlblVar.TxUpdDataLen;

		Rpt_TxPktProc(PAYLOAD_LEN, UpdBuf); //TX Packet
	
		Delay1us(500);
		if(RptGlblVar.TxUpdDataLen == 0){
			// if update size zero, still wait update timer
			//printf("TxUpdDataLen = %d\r\n", RptGlblVar.TxUpdDataLen);
			switch(RptDefSet.RptDataRate){
				case RF_DATARATE_1M:
					Delay1ms(RPT_1M_UPDTIMER - 1);
				break;
				case RF_DATARATE_500K:
					Delay1ms(RPT_500K_UPDTIMER - 1);
				break;
				case RF_DATARATE_250K:
					Delay1ms(RPT_250K_UPDTIMER - 1);
				break;	
			}
		}else{		
			//Send update data to Tag
			pSendBigData = (P_RPT_UPD_TAG_DATA)UpdBuf;
			while(RptGlblVar.TxUpdDataLen > 0) {
				pSendBigData->Seqno = DataSeqNo++;
				pSendBigData->PacketType = RF_PT_DATA_PACKET;
				memcpy(pSendBigData->toTagData, &EpdRFData[RptGlblVar.TxUpdBufPtrAddr] + j, PAYLOAD_LEN - 2);
				
				Delay1us(131); //wait slave receive
				Rpt_TxPktProc(PAYLOAD_LEN, UpdBuf); //TX Packet
				
				RptGlblVar.TxUpdDataLen -= (PAYLOAD_LEN - 2);
				j += (PAYLOAD_LEN - 2);
			}
		}
}

void Rpt_SycnAction_Proc(uint8_t Action)
{
		uint8_t i = 0;
		uint16_t UpdStartTime = 0, UpdDelayTime = 0;
		P_TAG_UPDATE_LIST pTxTagList = (P_TAG_UPDATE_LIST)RptGlblVar.TagLstBuf;
				
		Delay1us(1000);
	
		switch(Action){
			case RF_AL_SYNC:
				YELLOW_LED_LOW();
				// Only Sync Beacon , entry handle scanner packet
				Rpt_ScnrProg_Proc(PAYLOAD_LEN, RPT_HANDLE_SCNR_TIMER);
			
				YELLOW_LED_HIGH();
				break;
			case RF_AL_GET_TAG_STATUS:
				/* Send Tag list to Tag */
				//memcpy(pTxTagList->TAGID, TagID, RptGlblVar.TxUpdTagCnt * 5);
				
				Rpt_TxPktProc(PAYLOAD_LEN, RptGlblVar.TagLstBuf); //RF Tx proc
			
				Rpt_RxTag_RpyResult(RF_AL_GET_TAG_STATUS, 6); //Rx Tag reply status
				break;
			case RF_AL_TAG_ALWAYS_WAKE_UP:
			case RF_AL_GET_REPEATER_STATUS:
				break;
			case RF_AL_HOP_TO_TAG:
			case RF_AL_HOP_TO_REPEATER:
				//gState = 	HOPPM1_STATE;
				printf("ghopcount = %d\r\n", ghopcount);
				if(ghopcount == 1){		//Hopping
					printf("Hop to channel = %d\r\n", CHTable[gCHindex]);
					//StrobeCmd(CMD_STBY);  
					//SetCH(CHTable[gCHindex]); //freq 2450MHz CH100
					RptDefSet.Tchn = RptDefSet.Gchn = CHTable[gCHindex];
					Rpt_SetRF_Channel(RptDefSet.Tchn);
					
					RptGlblVar.HotBcnFlag = ghopcount = 0;
					Rpt_Write_DataFlash();
					
					RptGlblVar.RtnPwrOnMode = 1;
				}/*else if(ghopcount > 0){
					ghopcount--;
				}*/
				break;
			case RF_AL_UPDATE_TAG:
			case RF_AL_UPDATE_REPEATER_TAG:
				if(RptGlblVar.RxUpdDataDone == 1)  // have update
				{				
					/* Send Tag list to Tag */
					memcpy(pTxTagList->TAGID, TagID, RptGlblVar.TxUpdTagCnt * 5);
					Rpt_TxPktProc(PAYLOAD_LEN, RptGlblVar.TagLstBuf);
					
					// Clear some variable
					RptGlblVar.RxUpdDataDone = RptGlblVar.TxUpdBufPtrAddr =
						RptGlblVar.TxUpdDataLen = RptGlblVar.AccuTagRsltLen = 
							RptGlblVar.UpdCntTmp1 = 0;
					
					//copy the first update data length
					RptGlblVar.TxUpdDataLen = RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2];
					
					//start send update Tag head & raw data
					while(RptGlblVar.TxUpdTagCnt > 0){
						//Keep update start time
						UpdStartTime = TASK_CUR_RUNTIME(TIMER1);
						
						Delay1us(1450); //Delay 1.45ms for send Head
						
						switch(gDatarate) {
							case RF_DATARATE_1M:
								Rpt_TxUpdPack_Proc();
								break;
							case RF_DATARATE_500K:
								Rpt_TxUpdPack_Proc();
								break;
							case RF_DATARATE_250K:
								Rpt_TxUpdPack_Proc();
								break;
						}
						//Receive Tag reply result.
						Rpt_RxTag_RpyResult(RF_AL_UPDATE_REPEATER_TAG, 6); 			
						
						//Delayed until the one Tag update time
						if(RptGlblVar.TxUpdTagCnt != 0){
							switch(gDatarate){
								case RF_DATARATE_1M:
									UpdDelayTime = RPT_1M_UPDTIMER - (UpdStartTime -= TASK_CUR_RUNTIME(TIMER1));
								break;
								case RF_DATARATE_500K:
									UpdDelayTime = RPT_500K_UPDTIMER - (UpdStartTime -= TASK_CUR_RUNTIME(TIMER1));
								break;
								case RF_DATARATE_250K:
									UpdDelayTime = RPT_250K_UPDTIMER - (UpdStartTime -= TASK_CUR_RUNTIME(TIMER1));
								break;	
							}
							Delay1ms(UpdDelayTime);
						}
					}

					Rpt_SetRF_Channel(RptDefSet.Gchn); //Change RF Channel of Gateway
	
					// Reply Combine Tag Status
					Rpt_RpyTagCombResult(RptGlblVar.RxTagUpdRslt, RptGlblVar.RxCombTagRsCnt, RptGlblVar.TxBuffer); // Reply Combine Tag Status
					/*if(RptGlblVar.UpdTagCmpl == 1){
						RptGlblVar.UpdTagCmpl = 0; //clear flag
						Rpt_RpyTagCombResult(RptGlblVar.RxTagUpdRslt, RptGlblVar.TxBuffer); // Reply Combine Tag Status
					}else{
						//Change RF Channel have send some data
						memset(RptGlblVar.TxBuffer, 0, PAYLOAD_LEN);
						Rpt_RpyTagCombResult(RptGlblVar.RxTagUpdRslt, RptGlblVar.TxBuffer);
					}
					*/
				}
				break;
		}
		
		//Hop Beacon change Channel count
		if(RptGlblVar.HotBcnFlag == 1){
			RptGlblVar.HotBcnFlag = 0;
			if(ghopcount > 0){
				ghopcount--;
			}
		}
}

/**
  * @brief      Repeater receive gateway beacon lose, send fake beacon.
  * @param[out] None
  * @return     None
  * @details    This API is used send fake beacon packet.
  */
uint8_t Rpt_TxBeacon_Proc(void)
{
		P_SYNC_BEACON pTxSyncBeacon = (P_SYNC_BEACON)RptGlblVar.SyncBuf;
	
		if(RptGlblVar.RtnPwrOnMode == 1){
			/* Repeater rx beacon fake, create beacon send to tag */
			pTxSyncBeacon->BeaconSeq = RptGlblVar.gTxSeqNo++;
			pTxSyncBeacon->BeaconType = RF_PT_SYNC_BEACON;
			pTxSyncBeacon->Action = RF_AL_SYNC;
			pTxSyncBeacon->ChnCtrl = 0;
			pTxSyncBeacon->WakeUpTime = 1;
		}else{
			// Rpt rx update data fail, change action type to Sync
			if((RptGlblVar.RxUpdDataDone != 1) && 
					((pTxSyncBeacon->Action == RF_AL_UPDATE_TAG) || 
						(pTxSyncBeacon->Action == RF_AL_UPDATE_REPEATER_TAG)))
			{
				pTxSyncBeacon->Action = RF_AL_SYNC;
			}
		}
		// Change Beacon Gateway ID to Repeater ID
		memcpy(pTxSyncBeacon->GatewayID, &RptDefSet.RptID[3], 2);
		//memcpy(pTxSyncBeacon->RptID, &RptDefSet.RptID[3], 2);
		
		/* Encrypt Beacon packet */
		if(RptGlblVar.cryptflag == 1){
			AES128_Encrypt(RptGlblVar.SyncBuf, AES_KEY, EncryptData);
			Rpt_TxPktProc(SYNC_LEN, EncryptData);
			RptGlblVar.cryptflag = 0;
		}else{
			Rpt_TxPktProc(SYNC_LEN, RptGlblVar.SyncBuf);
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
void Rpt_TxTask_Proc(void)
{
		uint8_t action = 0;
		
		//************** Tx Beacon **************************
		action = Rpt_TxBeacon_Proc();
		//*****************************************************
		
		//handle Beacon action
		Rpt_SycnAction_Proc(action);
				
		/* Return Task mode or power on mode*/
		if(RptGlblVar.RtnPwrOnMode == 1){
			RptGlblVar.RtnPwrOnMode = 0;
			
			if(RptGlblVar.RtnPwrModeCnt >= 3){
				RptGlblVar.RtnPwrModeCnt = 0; //clear resync counter
				//RptDefSet.RptReSync++;
				RptDefSet.StartSec = GetStandbyTimeInSecond();
				Rpt_Write_DataFlash();
				
				Rpt_GlobalVar_Init();
			}else{
				RptGlblVar.gState = PWRBEACON_STATE;//PWRBEACON_STATE;
				RptGlblVar.gMode = RPT_TASK_MODE;//RPT_RX_MODE;
			}
		}else{
			RptGlblVar.gState = BEACON_STATE; //BEACON_STATE;//PWRBEACON_STATE;
			RptGlblVar.gMode = RPT_TASK_MODE;//RPT_RX_MODE;
		}	
}
