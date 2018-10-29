/***********************************************************************
 * RFpacket.c
 *
 ***********************************************************************/
#include "main.h"
#include <math.h>
#include "isr.h"

#include "RptProc.h"
#include "RptDataflash.h"
#include "RptComm.h"
#include "A7127RF_Drv.h"

uint8_t EpdRFData[EpdUpdateSize] = {0};
uint8_t *pEpdBuf = (uint8_t *)EpdRFData;

//uint8_t TagID[30] = {0}; // = {0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t GatewayID[5]; // = {0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t RepeaterID[5] = {0xFF, 0xFF, 0xFE, 0x00, 0x01};//{0x52, 0x00, 0x00, 0x00, 0x01};

uint8_t mindex; 		//Backup update tag head and data sequence number for check update packet lost 
uint8_t ghopcount;
uint8_t gCHindex;

/*
 * Jerry add
 */
extern uint8_t AES_KEY[];
extern void RstTimerValue(TIMER_Type *timer, uint32_t msec);
extern void Rpt_HandleBindGateway(P_SYNC_BEACON pSyncBeacon);

/**
  * @brief      Handle scanner packet
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used handle scanner packet.
  */
void Rpt_HandleSCNR_function(uint8_t *RxTaskBuf)
{
		uint8_t mReturn = 0;
		P_SCAN_PACKET_HEAD pScanPktHead = (P_SCAN_PACKET_HEAD)RxTaskBuf;
		
		YELLOW_LED_LOW(); //enter handle scanner, trigger yellow led
	
		memset(RxTaskBuf, 0, PAYLOAD_LEN);
	
		while(GIO1); //wait transmit completed
		mReturn = A7127RF_RecvPacket(PAYLOAD_LEN, RxTaskBuf, RPT_HANDLE_SCNR_TIMER, ENABLE); //wait receive scanner packet

		if(pScanPktHead->byPacketType != RF_PT_SCANNER_PACKET) {
			return ;
		}

		if(mReturn == TAG_MSG_RX_COMPLETED) {
			if(CompareData(&RptDefSet.RptID[3], pScanPktHead->byRepeaterID, 2)) {
				DBGMSG("SCNR %d\r\n", mReturn);
				M8107RF_SendPacket(PAYLOAD_LEN, RxTaskBuf); //transparent scanner packet to gateway
				mReturn = M8107RF_RecvPacket(PAYLOAD_LEN, RxTaskBuf, RPT_HANDLE_SCNR_TIMER, ENABLE); //receive gateway reply ack
				
				if(mReturn == TAG_MSG_RX_COMPLETED) {
					DBGMSG("ACK %d\r\n", mReturn);
					
					A7127RF_SendPacket(PAYLOAD_LEN, RxTaskBuf); //transparent ack to scanner
					while(GIO1); //wait transmit completed
				}
			}
		}

		YELLOW_LED_HIGH();
}

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
				
		RptRpyStatus->GwCHN = RptDefSet.Gchn;
		RptRpyStatus->TagCHN = RptDefSet.Tchn;
	
		RptRpyStatus->RptM8107RSSI = RSSI_measurement();
		RptRpyStatus->RptA7127RSSI = RSSI_measurement();
	
		RptRpyStatus->RptDataRate = RptDefSet.RptDataRate;
		RptRpyStatus->RptMode = RPT_ONERF_ONE_CHN_MODE;
		memcpy(RptRpyStatus->TagID, &RptGlblVar.TagID[RptGlblVar.MultiTagCnt], 5);

		M8107RF_SendPacket(PAYLOAD_LEN, RptGlblVar.TxBuffer);
			
		RptGlblVar.MultiTagCnt += 5;
}

uint8_t Rpt_RecvUpdData(uint8_t PktType, uint8_t *UpdDataBuf)
{
		bool ret;
		uint8_t mResult = 0;
		P_RPT_UPD_TAG_HEAD pTagUpdHead;
	
		switch(PktType){
			case RF_PT_PACKET_HEADER:
				pTagUpdHead = (P_RPT_UPD_TAG_HEAD)UpdDataBuf;
	
				//ret = CompareData(&RptDefSet.RptID[3], &pTagUpdHead->RepeaterID[0], 2);
				//ret = CheckZero(pTagUpdHead->RepeaterID, 2);
				ret = 1;
				if(ret == true){
					mindex = pTagUpdHead->Seqno; //copy sequence number
					RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2] = pTagUpdHead->PictureSize;
					
					//***************************************************************
					//********************** A7127 RF Module ************************
					//***************************************************************
					while(GIO1); //wait transmit completed
					A7127RF_SendPacket(PAYLOAD_LEN, UpdDataBuf);
					//***************************************************************
					
					mResult = TAG_MSG_HIT_ID;
				}else {
					mResult = TAG_MSG_NO_HIT_ID;
				}
				break;
			
			case RF_PT_DATA_PACKET:
				if((UpdDataBuf[0] - mindex) > 1) {
					//DBGMSG("Lost some data...%x - %x\r\n", UpdDataBuf[0], mindex);
					mResult = TAG_MSG_PACKET_LOST; //indicate packet lost
					break;
				}else{
					//***************************************************************
					//********************** A7127 RF Module ************************
					//***************************************************************
					A7127RF_SendPacket(PAYLOAD_LEN, UpdDataBuf);
					//***************************************************************
					
					mindex = UpdDataBuf[0];	
					/*memcpy(pEpdBuf, &UpdDataBuf[2], PAYLOAD_LEN - 2);		//62: -two bytes header, 64-2
					pEpdBuf += (PAYLOAD_LEN - 2);
					*/
				}
				break;
		}
	
		return mResult;
}

uint8_t Rpt_RecvTagList(uint8_t PktType, uint8_t *TagListBuf)
{
		bool ret;
		uint8_t mtag = 0, mResult = 0;
		P_TAG_UPDATE_LIST pTagUpdateList = (P_TAG_UPDATE_LIST)TagListBuf;

		//ret = CompareData(pTagUpdateList->RepeaterID, &RptDefSet.RptID[3], 2);
		//ret = CheckZero(pTagUpdateList->RepeaterID, 2);
		ret = 1;

		memset(RptGlblVar.TagID, 0, sizeof(RptGlblVar.TagID));
		memset(RptGlblVar.RxTtlUpdDataLenBuf, 0, sizeof(RptGlblVar.RxTtlUpdDataLenBuf));
	
		switch(PktType){
			case RF_PT_TAG_UPDATE_LIST:
				if(ret == true) {					
					//RptGlblVar.TxUpdTagCnt = 
					RptGlblVar.UpdCntTmp1 = mtag = (pTagUpdateList->PktCtrl) & 0x7;		//want update number
					
					//memcpy(&GatewayID[3], pTagUpdateList->GatewayID, 2);
					memcpy(&RptGlblVar.TagID[0], &pTagUpdateList->TAGID[0], mtag * 5);
					
					//***************************************************************
					//********************** A7127 RF Module ************************
					//***************************************************************
					while(GIO1); //wait transmit completed
					A7127RF_SendPacket(PAYLOAD_LEN, TagListBuf);
					//***************************************************************
					
					mResult = TAG_MSG_HIT_ID;  //Match update
				}else{
					mResult = TAG_MSG_NO_HIT_ID;
				}
				break;
				
			case RF_PT_TAG_STATUS:
				memcpy(RptGlblVar.TagLstBuf, TagListBuf, PAYLOAD_LEN); //Copy received Tag list data to buffer
				mResult = TAG_MSG_NO_HIT_ID;
				break;
				
			case RF_PT_REPEATER_STATUS:
				if(ret == true){
					Rpt_Status(TAG_MSG_REPORT_STATUS_OK);
					mResult = TAG_MSG_NO_HIT_ID;
				}
				break;			
			
			case RF_PT_TAG_UNBOUND:
			case RF_PT_CHANGE_BEACON_SRC:
				while(GIO1); //wait A7127 TX Complete
				A7127RF_SendPacket(PAYLOAD_LEN, TagListBuf); // Trans Tag List Packet to Tag
				while(GIO1);
			
				mResult = A7127RF_RecvPacket(PAYLOAD_LEN, TagListBuf, RPT_HANDLE_BIND_TIMER, ENABLE); //Receive Tag reply status
	
				if(mResult == TAG_MSG_RX_COMPLETED){
					printf("ChgBCN %d\r\n", mResult);	
					M8107RF_SendPacket(PAYLOAD_LEN, TagListBuf); //Transparent Tag status to Gateway
				}
				
				mResult = TAG_MSG_NO_HIT_ID;
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
		if(ret == false){
			DBGMSG("RptID non zero...%x:%x\r\n", pSyncBeacon->RepeaterID[0], pSyncBeacon->RepeaterID[1]);
			return TAG_MSG_RX_TIMEOUT;
		}
			
		if(RptGlblVar.gMode != RPT_PWRON_MODE) {
			// check receive gateway beacon reset timer
			if((RptGlblVar.Tmr1DynaVal = (RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1)))
				<= (RPT_TASK_STOP_EARLY))
			{
				RptGlblVar.Tmr1DynaVal += (RPT_TASK_STOP_EARLY - RptGlblVar.Tmr1DynaVal);
				Tmr1Cnt = ((RPT_TASK_TIMER_VAL + RPT_TASK_STOP_EARLY) - RptGlblVar.Tmr1DynaVal);
			}else{
				RptGlblVar.Tmr1DynaVal -= (RptGlblVar.Tmr1DynaVal - RPT_TASK_STOP_EARLY);
				Tmr1Cnt = ((RPT_TASK_TIMER_VAL + RPT_TASK_STOP_EARLY) - RptGlblVar.Tmr1DynaVal);
			}
			RstTimerValue(TIMER1, Tmr1Cnt);
			
			switch(pSyncBeacon->Action){
				case RF_AL_SYNC:
				case RF_AL_ALL_WU_TIME:
					mResult = TAG_MSG_ONLY_SYNC;
					break;
				
				
				case RF_AL_TAG_UNBOUND:
				case RF_AL_CHANGE_BEACON_SRC:
					
				case RF_AL_GET_TAG_STATUS:				
				case RF_AL_GET_REPEATER_STATUS:
					
				case RF_AL_UPDATE_TAG:		
				case RF_AL_UPDATE_REPEATER_TAG:
					if(pSyncBeacon->Action == RF_AL_UPDATE_REPEATER_TAG){
						pSyncBeacon->Action = RF_AL_UPDATE_TAG;
					}
					mResult = TAG_MSG_TO_RCV_TAG_LIST;
					break;		
				
				case RF_AL_HOP_TO_REPEATER:
				case RF_AL_HOP_TO_TAG:
					mhopcount = (pSyncBeacon->ChnCtrl) & 0xF;
				
					//first time received hopping sync beacon
					if(ghopcount == 0) {			
							ghopcount = mhopcount;
							gCHindex = ((pSyncBeacon->ChnCtrl) >> 4) & 0xF;
					}			
					mResult = TAG_MSG_HOPPING;
				 break;
					
				default:
					mResult = TAG_MSG_NO_HIT_ID;
					break;
			}	
			
			RptGlblVar.RtnPwrModeCnt = 0; 
			
			//***************************************************************
			//**************** A7127 RF Module Transparent ******************
			//***************************************************************
			memcpy(pSyncBeacon->RepeaterID, &RptDefSet.RptID[3], 2);

			if(RptGlblVar.cryptflag){
				/* Encrypt Beacon packet */
				A7127_WriteReg(KEYC_REG, 0x09);	//Enable A7127 RF Hardware AES128
				A7127RF_SendPacket(SYNC_LEN, SyncBuf);
				
				A7127_WriteReg(KEYC_REG, 0x08); //Disable A7127 RF Hardware AES128
				RptGlblVar.cryptflag = 0;
			}else{
				A7127RF_SendPacket(SYNC_LEN, SyncBuf);
			}
			//****************************************************************
			
		}else{
			Rpt_HandleBindGateway((P_SYNC_BEACON)SyncBuf);
		}
		
		return mResult;
}

/*********************************************************************
* Rpt_RecvPktType_Proc
*********************************************************************/
uint8_t Rpt_HandleRecvPktType(uint32_t FifoLen, uint8_t *PktBuf)
{
		uint8_t mhopcount;
		uint8_t mResult = 0, i = 0;
		P_RPT_RECV_PKT_TYPE pRecvPktType = (P_RPT_RECV_PKT_TYPE)PktBuf;
		
		for(i = 0; i < FifoLen; i++) {
			PktBuf[i] = RADIO->RX_DATA[i];
		}

		/* Decrypt Beacon function */
		if(FifoLen == SYNC_LEN) {
			AES128_Decrypt(PktBuf, AES_KEY, DecryptData);	
			
			if(DecryptData[1] == 0x01) {
				RptGlblVar.cryptflag = DecryptData[1];
				memcpy(PktBuf, DecryptData, FifoLen);
				//memcpy(RptGlblVar.SyncBuf , PktBuf, FifoLen);
			}else {
				//do nothing, because it is RAW data.
				//memcpy(RptGlblVar.SyncBuf , PktBuf, FifoLen);
			}
		}
		
		/* Processing packet types */
		switch(pRecvPktType->PacketType){
			case RF_PT_SYNC_BEACON:
				mResult = Rpt_RecvBeacon(PktBuf);
				break;
			
			case RF_PT_TAG_UNBOUND:
			case RF_PT_CHANGE_BEACON_SRC:
				
			case RF_PT_TAG_STATUS:
			case RF_PT_REPEATER_STATUS:
				
			case RF_PT_TAG_UPDATE_LIST:
			case RF_PT_ALWAYS_WAKE_BEACON:
				mResult = Rpt_RecvTagList(pRecvPktType->PacketType, PktBuf);
				break;

			case RF_PT_HOPPING_BEACON:				
				//setSynTimer(recv[4], updateflag);		//Sync time sec
				mhopcount = PktBuf[3] & 0xF;
			
				//first time received hopping sync beacon
				if(ghopcount == 0) {			
						ghopcount = mhopcount;
						gCHindex = (PktBuf[3] >> 4) & 0xF;
				}
				//setSynTimer(recv[4], updateflag);		//Sync time sec
				mResult = TAG_MSG_HOPPING;
				break;
				
			case RF_PT_PACKET_HEADER:
			case RF_PT_DATA_PACKET:
				mResult = Rpt_RecvUpdData(pRecvPktType->PacketType, PktBuf);
				break;
		}
		
		return mResult;
}
//**********************************************************************************

uint8_t Rpt_HandleRecvPacket(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t mSec, FunctionalState TimerState)
{
		uint8_t RxPktResult = 0;
		uint8_t WTRFlag = 0;

		/* Set RF packet fifo length */
		M8107_SetFIFOLvl(FifoLen);

		/* RF entry RX state */
		StrobeCmd(CMD_RX);
		Oneshoutflag = 0;		//clear Timer0 timeout flag
	
		if(TimerState == ENABLE) {
			SetTimer(TIMER0, mSec); //set get packet timeout timer
		}
		
		while(1){
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
						RxPktResult = TAG_MSG_RX_TIMEOUT;
					}
					break;
				}
			}
		}
		
		return RxPktResult;
}

uint8_t Rpt_TransUpdTagPacket(uint32_t FifoLen)
{
		uint8_t RxPktResult = 0, i = 0;
			
		StrobeCmd(CMD_RX); 	//M8107 RF entry RX state
		Oneshoutflag = 0;		//clear Timer0 timeout flag
		
		while((RADIO->STATUS & (1 << 7)) && (Oneshoutflag == 0) && (GIO1)); 	//WTR	
		StrobeCmd(CMD_STBY);
		
		if(Oneshoutflag){
			RxPktResult = TAG_MSG_PACKET_LOST;
		}else {
			if((RADIO->RX_DATA[0] - mindex) > 1){
				DBGMSG("Lost data = %x : %x\r\n", RADIO->RX_DATA[0], mindex);
				RxPktResult = TAG_MSG_PACKET_LOST;
			}else{			
				__disable_irq();
				
				L01_CSN_LOW( );
							
				SPI_ExchangeByte(FIFO_REG);

				for(i = 0; i < FifoLen; i++){
					SPI_ExchangeByte(RADIO->RX_DATA[i]);
				}
				
				L01_CSN_HIGH( );
				
				__enable_irq();
				
				GIOSetTXMode();
				
				mindex = RADIO->RX_DATA[0];
				RxPktResult = TAG_MSG_RX_COMPLETED;
			}
		}

		return RxPktResult;
}

uint8_t Rpt_HandleRecvUpdTagData(int16_t mlen, uint8_t *RecvUpdBuf)
{
		uint8_t mReturn = 0;
		//uint16_t UpdTimeOut = 0;
		
		mindex = 0; 			//clear sequence num
		Oneshoutflag = 0;	//Timer0 timeout flag

		M8107_SetFIFOLvl(PAYLOAD_LEN); //Set M8107 RF packet fifo length
		A7127_SetFIFOLen(PAYLOAD_LEN); //Set A7127 RF packet fifo length
	
		//UpdTimeOut = Rpt_CalcRecvUpd_Timeout(mlen); //Calculate when to receive updates
		//SetTimer(TIMER0, UpdTimeOut); //set receive update data timeout
		switch(RptDefSet.RptDataRate) {
			case RF_DATARATE_1M:
				SetTimer(TIMER0, RPT_1M_UPDTIMER);
			break;
			case RF_DATARATE_500K:
				SetTimer(TIMER0, RPT_500K_UPDTIMER);
			break;
			case RF_DATARATE_250K:
				SetTimer(TIMER0, RPT_250K_UPDTIMER);
			break;	
		}
		
		while(mlen > 0) {
			mReturn = Rpt_TransUpdTagPacket(PAYLOAD_LEN);
			
			if(Oneshoutflag) {
				mReturn = TAG_MSG_PACKET_LOST;
				break;
			}else{
				mlen -= (PAYLOAD_LEN - 2);	//two bytes header
			}
		}
		Timer_StopTimer(TIMER0);
		StrobeCmd(CMD_STBY);
		
		if((Oneshoutflag) || (mReturn == TAG_MSG_PACKET_LOST)) {	//Don't need update EPD because data error.
			printf("DATA_STATE RX data timeout or lost.\r\n");
		}else {
			while(GIO1);	//wait A7127 transmit completed
			mReturn = A7127RF_RecvPacket(PAYLOAD_LEN, RecvUpdBuf, RPT_TAG_REPLY_TIMER, ENABLE); // A7127 receive Tag result
			
			if(mReturn == TAG_MSG_RX_COMPLETED){ 
				M8107RF_SendPacket(PAYLOAD_LEN, RecvUpdBuf); //transparent A7127 receive packet
			}
			DBGMSG("%d %d\r\n", RptGlblVar.UpdCntTmp2, RptGlblVar.UpdCntTmp1);
		}

		RptGlblVar.UpdCntTmp2++;
		if(RptGlblVar.UpdCntTmp2 < RptGlblVar.UpdCntTmp1){
			//RptGlblVar.gState = HEADER_STATE;
			mReturn = TAG_MSG_HIT_ID;
		}else{
			RptGlblVar.UpdCntTmp2 = RptGlblVar.UpdCntTmp1 = RptGlblVar.gState = 0;
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
				mReturn = Rpt_HandleRecvUpdTagData(RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2], RxTaskBuf);
				break;
			
			default:
				mReturn = Rpt_HandleRecvPacket(RptGlblVar.RecvLen, RxTaskBuf, RptGlblVar.RecvTimer, ENABLE);
				break;
		}

		/* Handle return runtime message */
		switch(mReturn) {
			case TAG_MSG_HIT_ID:
				if((TaskState == TAG_LIST_STATE) || (TaskState == DATA_STATE)) {
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
					
					printf("Lost Beacon...\r\n");
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
				mReturn = Rpt_RecvPacket(SYNC_LEN, RxTaskBuf, RPT_PWRON_BEACON_TIMER, ENABLE);
				break;	
			
			case BEACON_STATE:
				mReturn = Rpt_RecvPacket(SYNC_LEN, RxTaskBuf, RPT_BEACON_TIMER, ENABLE);
				break;
		
			case TAG_LIST_STATE:
				mReturn = Rpt_RecvPacket(PAYLOAD_LEN, RxTaskBuf, PRT_RX_PKT_TIMER, ENABLE);
				break;	

			case HEADER_STATE:
				mReturn = Rpt_RecvPacket(PAYLOAD_LEN, RxTaskBuf, PRT_RX_PKT_TIMER, ENABLE);
				break;
					
			case DATA_STATE:
				Rpt_RxMode_RecvUpdData(RptGlblVar.RxTtlUpdDataLenBuf[RptGlblVar.UpdCntTmp2], RxTaskBuf);
				return ;
			
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
				/*switch(TaskState) {
					case TAG_LIST_STATE:
						RptGlblVar.gState = HEADER_STATE;
						break;
					case HEADER_STATE:
						RptGlblVar.gState = DATA_STATE;
						break;
				}*/
				if(TaskState == TAG_LIST_STATE){
					RptGlblVar.gState = HEADER_STATE;
				}else if(TaskState == TAG_LIST_STATE){
					RptGlblVar.gState = DATA_STATE;
				}
				break;
			
			case TAG_MSG_ONLY_SYNC:	
					Rpt_HandleSCNR_function(RxTaskBuf);
					RptGlblVar.gState = 0;
				break;
					
			case TAG_MSG_HOPPING:
				RptGlblVar.HotBcnFlag = 1;
			case TAG_MSG_NO_HIT_ID:
				RptGlblVar.gState = 0;
				break;
			
			case TAG_MSG_REPORT_STATUS_OK:
			case TAG_MSG_TO_RCV_TAG_LIST:
				RptGlblVar.gState = TAG_LIST_STATE;
				break;
			
			case TAG_MSG_RX_TIMEOUT:
				//switch(TaskState){
					//case PWRBEACON_STATE:
					//case BEACON_STATE:
				
						if((RptGlblVar.gMode != RPT_PWRON_MODE) && 
							((TaskState == PWRBEACON_STATE) || (TaskState == BEACON_STATE))){
							Tmr1Cnt = (RPT_TASK_TIMER_VAL + RPT_END_EARLY_TIMER) - (RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1));
							RstTimerValue(TIMER1, Tmr1Cnt);
							printf("Lost Beacon...\r\n");
							RptGlblVar.gState = 0; //PWRBEACON_STATE; //BEACON_STATE
							RptGlblVar.RtnPwrOnMode = 1;
						}
						//break;
				//}
				break;
		}
}
#endif //0

//**********************************************************************************
