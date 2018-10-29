#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "main.h"
#include "flash.h"

#include "RptProc.h"
#include "RFpacket.h"
#include "RptComm.h"
#include "RptDataFlash.h"
#include "RptOTA.h"

//extern uint8_t mindex;

extern uint8_t rpt_rfpacket_receive(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t mSec, FunctionalState TimerState);
extern void OTA_Init(void);

void WriteNewAppBin(uint32_t BackAppStartAddr, uint8_t *NewAppBinBuf, uint32_t NewAppBinSize)
{
		uint32_t i = 0;
		uint32_t u32Addr = BackAppStartAddr;	//write address
		uint32_t EraseCounter = 0; 						//erase counter
		uint32_t NbrOfPage = 0;							 	//Record erase counter
		uint32_t u32Data[32] = {0}; 					//write data

		__disable_irq();

		//calculation erase page range
		if(((NbrOfPage = NewAppBinSize % FLASH_PAGE_SIZE)) != 0){
			NbrOfPage = (NewAppBinSize / FLASH_PAGE_SIZE) + 1;
		}else{
			NbrOfPage = (NewAppBinSize / FLASH_PAGE_SIZE);
		}

		// erase flash
		for(EraseCounter = 0; (EraseCounter < NbrOfPage); EraseCounter++) {
			Flash_PageErase(u32Addr + (FLASH_PAGE_SIZE * EraseCounter));
		}

		// write new app .bin to Flash
		for(i = 0; i < NewAppBinSize; i += FLASH_PAGE_SIZE){
			memcpy(&u32Data, NewAppBinBuf + i, FLASH_PAGE_SIZE);
			Flash_Write_U32(u32Addr + i, &u32Data[0], 32);
			//u32Addr += FLASH_PAGE_SIZE;
		}

		__enable_irq();
}

//void HandleReplyOtaResult(uint8_t Result, uint8_t SegmentIndex, uint8_t *AckBuf)
void HandleReplyOtaResult(uint8_t SeqNum, uint8_t Result, uint8_t SegmentIndex, uint8_t *AckBuf)
{
		uint8_t i = 0;
		P_DEVICE_OTA_REPLY_RESULT pDeviceOTAresult = (P_DEVICE_OTA_REPLY_RESULT)AckBuf;

		//pDeviceOTAresult->Seq = g_RptGlblVar.mindex++;
		pDeviceOTAresult->Seq = SeqNum++;
		pDeviceOTAresult->PacketType = RF_PT_OVER_THE_AIR;
		pDeviceOTAresult->SegmentIndex = SegmentIndex;
		pDeviceOTAresult->Result = Result;

		for(i =  0; i < 5; i++){
			M8107RF_SendPacket(PAYLOAD_LEN, AckBuf);
		}
}

uint8_t HandleRecvOtaHead(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *HeadBuf)
{
		uint8_t mResult = 0;
		P_DEVICE_OTA_HEAD pUpdOtaHead = (P_DEVICE_OTA_HEAD)HeadBuf;

		/* Gateway OTA retry 10 fail, send Beacon return normal state */
		if(pUpdOtaHead->PacketType == RF_PT_SYNC_BEACON){
			mResult = TAG_MSG_NO_HIT_ID;
			//g_RptGlblVar.RtnPwrOnMode = 1;
			(*mRunTimePara).RtnPwrModeflag = 1;
		}

		//g_RptGlblVar.mindex = pUpdOtaHead->Seqno; //backup sequence number

		//if(g_RptGlblVar.BckupSegmentIndex == pUpdOtaHead->SegmentIndex) {
		if(mRunTimePara->UpdRptFwStrc.KeepUpdFwSegIndex == pUpdOtaHead->SegmentIndex) {
			//memcpy(g_RptGlblVar.UpdHeadBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2], &OtaBuf[0], sizeof(DEVICE_OTA_HEAD)); //backup OTA head
			//*(P_MemCpyNode)&(*pRunTimePara).KeepBigDataHead[g_RptGlblVar.GlblTempVar.tUpdCntTmp2] = *(P_MemCpyNode)HeadBuf;
			//*(P_MemCpyNode)&(*mRunTimePara).KeepBigDataHead[mRunTimePara->KeepRxDataCnt] = *(P_MemCpyNode)HeadBuf;
			*(P_MemCpyNode)&(*mRunTimePara).KeepRxPktStrc.RawHeadBuf[mRunTimePara->KeepRxDataCnt] = *(P_MemCpyNode)HeadBuf;
		}

		mResult = TAG_MSG_HIT_ID;

		return mResult;
}

uint8_t HandleRecvOtaData(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *DataBuf)
{
		uint8_t mReturn = 0;
		//P_DEVICE_OTA_HEAD pRptOtaHead = (P_DEVICE_OTA_HEAD)g_RptGlblVar.UpdHeadBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2];
		//P_DEVICE_OTA_HEAD pRptOtaHead = (P_DEVICE_OTA_HEAD)RunTimePara->KeepBigDataHead[g_RptGlblVar.GlblTempVar.tUpdCntTmp2];
		//P_DEVICE_OTA_TAG_LIST pDevieOtaList = (P_DEVICE_OTA_TAG_LIST)mRunTimePara->KeepTagListPkt;
		P_DEVICE_OTA_TAG_LIST pDevieOtaList = (P_DEVICE_OTA_TAG_LIST)mRunTimePara->KeepRxPktStrc.DeviceListPkt;
		//P_DEVICE_OTA_HEAD pRptOtaHead = (P_DEVICE_OTA_HEAD)mRunTimePara->KeepBigDataHead[mRunTimePara->KeepRxDataCnt];
		P_DEVICE_OTA_HEAD pRptOtaHead = (P_DEVICE_OTA_HEAD)mRunTimePara->KeepRxPktStrc.RawHeadBuf[mRunTimePara->KeepRxDataCnt];
		int16_t mlen = pRptOtaHead->SeqmentSize;
		int16_t SegmentSize = pRptOtaHead->SeqmentSize;

		//g_RptGlblVar.mindex = 0; 			//clear sequence num
		mRunTimePara->SeqIndex = 0;
		Oneshoutflag = 0;	//Timer0 timeout flag

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
			//mReturn = rpt_rfpacket_receive(PAYLOAD_LEN, RecvBuf, 0, DISABLE);
			mReturn = M8107RF_RecvPacket(mRunTimePara->RxPktLen, DataBuf, mRunTimePara->RxPktTime, ENABLE);
			if(mReturn == TAG_MSG_RX_COMPLETED){
				mReturn = Rpt_HandleRecvPktType(mRunTimePara, DataBuf);
			}

			if(Oneshoutflag) {
				mReturn = TAG_MSG_PACKET_LOST;
				break;
			}else{
				mlen -= (PAYLOAD_LEN - 2);	//two bytes header
			}
		}
		Timer_StopTimer(TIMER0);	//stop Timer0 of receive complete
		StrobeCmd(CMD_STBY); 			//exit rx mode

		if((Oneshoutflag) || (mReturn == TAG_MSG_PACKET_LOST)) {
			//memset(&g_RptGlblVar.UpdHeadBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2], 0, sizeof(RPT_UPD_TAG_HEAD));	 //receive update data fail, clear update head buffer
			mReturn = TAG_MSG_PACKET_LOST;
		}else {
			mReturn = TAG_MSG_RX_COMPLETED;
		}

		//if((g_RptGlblVar.BckupSegmentIndex == pRptOtaHead->SegmentIndex) && (mReturn == TAG_MSG_RX_COMPLETED)) {
		if((mRunTimePara->UpdRptFwStrc.KeepUpdFwSegIndex == pRptOtaHead->SegmentIndex) && (mReturn == TAG_MSG_RX_COMPLETED)) {
			// ACK updata firmware Status, contain reply segment index
			//HandleReplyOtaResult(mReturn, pRptOtaHead->SegmentIndex, DataBuf);
			HandleReplyOtaResult(mRunTimePara->SeqIndex, mReturn, pRptOtaHead->SegmentIndex, DataBuf);

			printf("Suc. Index = %x %x, SegmentSize = %d, WrAddr = %x...\r\n",
								//g_RptGlblVar.BckupSegmentIndex, pRptOtaHead->SegmentIndex, SegmentSize, g_RptGlblVar.WrNewAppAddress);
								//mRunTimePara->BckupSegmentIndex, pRptOtaHead->SegmentIndex, SegmentSize, g_RptGlblVar.WrNewAppAddress);
								//mRunTimePara->UpdRptFwStrc.KeepUpdFwSegIndex, pRptOtaHead->SegmentIndex, SegmentSize, mRunTimePara->WrNewAppAddress);
								mRunTimePara->UpdRptFwStrc.KeepUpdFwSegIndex, pRptOtaHead->SegmentIndex, SegmentSize, mRunTimePara->UpdRptFwStrc.WrNewAppAddress);

			/* Write new app binary file to flash */
			//WriteNewAppBin(g_RptGlblVar.WrNewAppAddress, g_RptGlblVar.UpdDataBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2], SegmentSize);
			//WriteNewAppBin(g_RptGlblVar.WrNewAppAddress, RunTimePara->KeepBigData[g_RptGlblVar.GlblTempVar.tUpdCntTmp2], SegmentSize);
			//WriteNewAppBin(g_RptGlblVar.WrNewAppAddress, mRunTimePara->KeepBigData[mRunTimePara->KeepRxDataCnt], SegmentSize);
			//WriteNewAppBin(mRunTimePara->WrNewAppAddress, mRunTimePara->KeepBigData[mRunTimePara->KeepRxDataCnt], SegmentSize);
			//WriteNewAppBin(mRunTimePara->WrNewAppAddress, mRunTimePara->KeepRxPktStrc.RawDataBuf[mRunTimePara->KeepRxDataCnt], SegmentSize);
			WriteNewAppBin(mRunTimePara->UpdRptFwStrc.WrNewAppAddress, mRunTimePara->KeepRxPktStrc.RawDataBuf[mRunTimePara->KeepRxDataCnt], SegmentSize);

			//g_RptGlblVar.WrNewAppAddress += SegmentSize;
			//(*mRunTimePara).WrNewAppAddress += SegmentSize;
			(*mRunTimePara).UpdRptFwStrc.WrNewAppAddress += SegmentSize;

			//g_RptGlblVar.BckupSegmentIndex++;
			(*mRunTimePara).UpdRptFwStrc.KeepUpdFwSegIndex++;
		}else{
			mReturn = TAG_MSG_OTA_PACKET_REPEATED;
			HandleReplyOtaResult(mRunTimePara->SeqIndex, mReturn, pRptOtaHead->SegmentIndex, DataBuf);

			//printf("fail Index = %x %x...\r\n", g_RptGlblVar.BckupSegmentIndex, pRptOtaHead->SegmentIndex);
			printf("fail Index = %x %x...\r\n", mRunTimePara->UpdRptFwStrc.KeepUpdFwSegIndex, pRptOtaHead->SegmentIndex);
		}

		//g_RptGlblVar.GlblTempVar.tTxUpdDataLen = 0;	//clear receive update data length
		//g_RptGlblVar.GlblTempVar.tUpdCntTmp2 = 0;

		//if(g_RptGlblVar.UpdFirmwareCount != g_RptGlblVar.BckupSegmentIndex) {
		//if(mRunTimePara->BckupSegmentIndex != g_RptGlblVar.UpdFirmwareCount) {
		if(mRunTimePara->UpdRptFwStrc.KeepUpdFwSegIndex != pDevieOtaList->TotalOfPacket) {
			mReturn = TAG_MSG_HIT_ID;
		}else{
			OTA_Init();

			Delay1ms(0x2000);
			printf("Reset().. \r\n");
			Delay1ms(0x2000);

			__disable_irq();

			NVIC_SystemReset();
		}

		return mReturn;
}
