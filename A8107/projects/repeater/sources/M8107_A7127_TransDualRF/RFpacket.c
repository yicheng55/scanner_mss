/***********************************************************************
 * RFpacket.c
 *
 ***********************************************************************/
#include "main.h"
#include <math.h>
#include "flash.h"
#include "isr.h"

#include "RptProc.h"
#include "RFpacket.h"
#include "RptComm.h"
#include "RptDataflash.h"
#include "RptOTA.h"
#include "RptDataFlash.h"

#include "A7127RF_Drv.h"

/*
 * Jerry add
*/
extern uint8_t AES_KEY[];
extern void RstTimerValue(TIMER_Type *timer, uint32_t msec);
extern void OTA_Init(void);

void A7127RF_TransPkt(uint16_t TransLen, uint8_t *TansBuf)
{
    //StrobeCmd(CMD_STBY); //M8107 RF entry standyby mode
    while (GIO1);                //wait transmit completed
    A7127RF_SendPacket(TransLen, TansBuf);
}

/**
  * @brief      Send repeater stauts to gatway
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used send repeater status to gateway.
  */
void ReplyRptStatus(uint8_t SeqNum, uint8_t MultiTag, uint8_t result, uint8_t *GwID, uint8_t *TagID, uint8_t *RpyBuf)
{
    P_RPT_STATUS_REPORT RptRpyStatus = (P_RPT_STATUS_REPORT)RpyBuf;

    RptRpyStatus->SeqNum = SeqNum++;
    RptRpyStatus->PckType = RF_PT_REPEATER_STATUS;
    RptRpyStatus->Result = result;

    memcpy(&RptRpyStatus->GatewayID[0], GwID, 2);
    memcpy(&RptRpyStatus->RepeaterID[0], &RptDefSet.RptID[3], 2);

    RptRpyStatus->RptM8107RSSI = RSSI_measurement();
    RptRpyStatus->GwCHN = RptDefSet.Gchn;
    RptRpyStatus->TagCHN = RptDefSet.Tchn;
    RptRpyStatus->RptDataRate = RptDefSet.RptDataRate;
    RptRpyStatus->RptMode = RPT_ONERF_ONE_CHN_MODE;
    memcpy(RptRpyStatus->TagID, &TagID, 5);

    M8107RF_SendPacket(PAYLOAD_LEN, RpyBuf);
}

uint8_t HandleRecvUpdTagHead(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *pDataHead)
{
    uint8_t mResult = 0, i = 0;
    P_TAG_UPDATE_LIST pTagListPkt = (P_TAG_UPDATE_LIST)mRunTimePara->KeepRxPktStrc.DeviceListPkt;
    P_RPT_UPD_TAG_HEAD pBigDataHead = (P_RPT_UPD_TAG_HEAD)pDataHead;

    //if(CompareData(&RptDefSet.RptID[3], &pBigDataHead->RepeaterID[0], 2))
    {
        /*for(i = 0; i < pTagListPkt->UpdTagCnt; i++){
            if(CompareData(pBigDataHead->TagID, &pTagListPkt->TAGID[i * 5], sizeof(pBigDataHead->TagID))){
                (*mRunTimePara).KeepRxDataCnt = i;
                break;
            }
        }*/
        //***************************************************************
        //************** Transparent recive data to Tag *****************
        //***************************************************************
        A7127RF_TransPkt(mRunTimePara->RxPktLen, pDataHead);
        //***************************************************************

        *(P_MemCpyNode)&(*mRunTimePara).KeepRxPktStrc.RawHeadBuf[mRunTimePara->KeepRxDataCnt] = *(P_MemCpyNode)pDataHead;

        mResult = TAG_MSG_HIT_ID;
    }/*else {
            mResult = TAG_MSG_NO_HIT_ID;
        }*/

    return mResult;
}

/**
  * @brief      Receive Update Tag Data
  * @param[out] ProtocolHeader
  * @return     receive result
  * @details    This API is used receive update Tag data.
  */
uint8_t Rpt_RecvUpdData(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *UpdDataBuf)
{
    uint8_t mResult = 0;
    static uint16_t RxBigDataCnt = 0;

    P_RPT_RECV_PKT_TYPE pBigHead = (P_RPT_RECV_PKT_TYPE)UpdDataBuf;
    P_RPT_UPD_TAG_DATA pBigData;

    switch (pBigHead->PacketType)
    {
        case RF_PT_PACKET_HEADER:
            RxBigDataCnt = 0; //clear last keep length

            if (mRunTimePara->UpdRptFwStrc.UpdFirmwareFlag != 1)
            {
                mResult = HandleRecvUpdTagHead(mRunTimePara, UpdDataBuf); //For transparent update tag data
            }
            else
            {
                mResult = HandleRecvOtaHead(mRunTimePara, UpdDataBuf);      //For OTA repeater
            }
            break;

        case RF_PT_DATA_PACKET:
        case RF_PT_OVER_THE_AIR:
            pBigData = (P_RPT_UPD_TAG_DATA)UpdDataBuf;

            if ((pBigData->Seqno - mRunTimePara->SeqIndex) > 1)
            {
                DBGMSG("Lost some data...%x - %x\r\n", UpdDataBuf[0], mRunTimePara->SeqIndex);
                mResult = TAG_MSG_PACKET_LOST; //indicate packet lost
                break;
            }
            else
            {
                (*mRunTimePara).SeqIndex = pBigData->Seqno;

                //***************************************************************
                //************** Transparent recive data to Tag *****************
                //***************************************************************
                A7127RF_TransPkt(mRunTimePara->RxPktLen, UpdDataBuf);
                //***************************************************************

                //*(P_MemCpyNode)&(*mRunTimePara).KeepRxPktStr.RawDataBuf[mRunTimePara->KeepRxDataCnt][RxBigDataCnt] = *(P_MemCpyNode)pBigData->toTagData;
                RxBigDataCnt += (PAYLOAD_LEN - 2);
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
uint8_t Rpt_RecvTagList(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *TagListBuf)
{
    //bool ret;
    //uint8_t mtag = 0,
    uint8_t mResult = 0;
    uint8_t RptBdID[4] = {0x00, 0x00, 0xFF, 0xFF}; //Repeater Backdoor ID for engineering
    P_TAG_UPDATE_LIST pTagUpdateList = (P_TAG_UPDATE_LIST)TagListBuf;
    P_RPT_NVRAM_BASE_PARAMETER pRptNvRam;
    P_DEVICE_OTA_TAG_LIST pDevieOtaList;

    switch (pTagUpdateList->PacketType)
    {
        case RF_PT_TAG_UPDATE_LIST:
            //if(CompareData(pTagUpdateList->RepeaterID, &RptDefSet.RptID[3], sizeof(pTagUpdateList->RepeaterID)))
        {
            //***************************************************************
            //************** Transparent recive data to Tag *****************
            //***************************************************************
            Repeater_LED_Ctrl(TRIGGER_FOR_UPD_TAG);
            A7127RF_TransPkt(mRunTimePara->RxPktLen, TagListBuf);
            //***************************************************************

            //pTagUpdateList = (P_TAG_UPDATE_LIST)mRunTimePara->KeepRxPktStr.DeviceListPkt;
            /* Repeater cannot receive more than 3 tag updates data */
            /*if((pTagUpdateList->UpdTagCnt & 0x7) > RPT_RX_MAX_BIGDATA){
                pTagUpdateList->UpdTagCnt = RPT_RX_MAX_BIGDATA;
            }else{
                pTagUpdateList->UpdTagCnt = (pTagUpdateList->UpdTagCnt & 0x7);
            }*/

            mResult = TAG_MSG_HIT_ID; //Match update
            }/*else{
                    mResult = TAG_MSG_NO_HIT_ID;
                }*/
        break;

        case RF_PT_TAG_STATUS:
            mResult = TAG_MSG_NO_HIT_ID;
            break;

        case RF_PT_REPEATER_STATUS:
            if (CompareData(pTagUpdateList->RepeaterID, &RptDefSet.RptID[3], sizeof(pTagUpdateList->RepeaterID)) ||
                    CompareData(pTagUpdateList->RepeaterID, &RptBdID[0], sizeof(pTagUpdateList->RepeaterID)) ||
                    CompareData(pTagUpdateList->RepeaterID, &RptBdID[3], sizeof(pTagUpdateList->RepeaterID)))
            {
                ReplyRptStatus(pTagUpdateList->Seq, pTagUpdateList->UpdTagCnt,
                               TAG_MSG_REPORT_STATUS_OK, pTagUpdateList->GatewayID, pTagUpdateList->TAGID,  TagListBuf); //reply repeater status
            }
            mResult = TAG_MSG_NO_HIT_ID;
            break;

        case RF_PT_TAG_UNBOUND:
        case RF_PT_CHANGE_BEACON_SRC:
            mResult = TAG_MSG_NO_HIT_ID;
            break;

        case RF_PT_OTA_UPDATE_LIST:
            pDevieOtaList = (P_DEVICE_OTA_TAG_LIST)pTagUpdateList;

            if (((CompareData(pDevieOtaList->RepeaterID, &RptDefSet.RptID[3], sizeof(pDevieOtaList->RepeaterID))) ||
                    (CompareData(pDevieOtaList->RepeaterID, &RptBdID[0], sizeof(pDevieOtaList->RepeaterID))) ||
                    (CompareData(pDevieOtaList->RepeaterID, &RptBdID[3], sizeof(pDevieOtaList->RepeaterID))))
                    && (pDevieOtaList->Attribution == Attribution_OTA_Repeater))
            {
                (*mRunTimePara).UpdRptFwStrc.WrNewAppAddress = NEW_APP_START_ADDR;
                (*mRunTimePara).UpdRptFwStrc.KeepUpdFwSegIndex = 0;
                (*mRunTimePara).UpdRptFwStrc.UpdFirmwareFlag = 1;

                mResult = TAG_MSG_HIT_ID;
            }
            else
            {
                mResult = TAG_MSG_NO_HIT_ID;
            }
            break;

        case RF_PT_WRITE_TAG_NVM_DATA:
            printf("RF_PT_WRITE_TAG_NVM_DATA\r\n");
            if (CompareData(pTagUpdateList->RepeaterID, &RptDefSet.RptID[3], sizeof(pTagUpdateList->RepeaterID)) ||
                    CompareData(pTagUpdateList->RepeaterID, &RptBdID[0], sizeof(pTagUpdateList->RepeaterID)) ||
                    CompareData(pTagUpdateList->RepeaterID, &RptBdID[3], sizeof(pTagUpdateList->RepeaterID)))
            {
                pRptNvRam = (P_RPT_NVRAM_BASE_PARAMETER)pTagUpdateList;

                Rpt_Read_DataFlash();
                memcpy(&RptDefSet, &pRptNvRam, sizeof(RPT_NVRAM_BASE_PARAMETER));

                Rpt_Write_DataFlash(); //write default parameter to dataflash
                Delay1ms(10);

                __disable_irq();
                NVIC_SystemReset();
                mResult = TAG_MSG_HIT_ID;
            }
            else
            {
                mResult = TAG_MSG_NO_HIT_ID;
            }
            break;
    }

    return mResult;
}

//Test SNCR
void FakeSncrProductPkt(uint8_t *ProductBuf)
{
    PESL_BARCODE_PACKET pSncrProduct = (PESL_BARCODE_PACKET)ProductBuf;
    char *ProfilePtr = (char *)&pSncrProduct->stBarcode.byProduct[0];

    pSncrProduct->bySequenceNumber = 0;
    pSncrProduct->byPacketType = RF_PT_SCANNER_PACKET;
    pSncrProduct->byCommand = 0x86;

    memcpy(pSncrProduct->byRepeaterID, &RptDefSet.RptID[3], 2);

    pSncrProduct->stBarcode.nLength = 5;
    pSncrProduct->stBarcode.nFormat = 0x06;
    sprintf(ProfilePtr, "%s", "00001");

    M8107RF_SetFIFOLvl(64);
    M8107RF_SendPacket(64, ProductBuf);
}

uint8_t Rpt_RecvBeacon(uint8_t cryptflag, uint8_t *SyncBuf)
{
    uint8_t mResult = 0;
    P_SYNC_BEACON pSyncBeacon = (P_SYNC_BEACON)SyncBuf;
    uint32_t RstTmr1 = 0, TmpTimer = 0;

    if (!CheckZero(pSyncBeacon->RepeaterID, 2))
    {
        //DBGMSG("RptID non zero...\r\n");
        return TAG_MSG_RX_TIMEOUT;
    }

    /* check receive gateway beacon reset Timer1 */
    //DBGMSG("%d,%d ", RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1), TASK_CUR_RUNTIME(TIMER1));
    if ((TmpTimer = abs(RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1))) <= (RPT_TASK_STOP_EARLY) )
    {
        TmpTimer += abs((RPT_TASK_STOP_EARLY) - TmpTimer);
        RstTmr1 = ((RPT_TASK_TIMER_VAL + RPT_TASK_STOP_EARLY) - TmpTimer);
    }
    else
    {
        TmpTimer -= abs(TmpTimer - RPT_TASK_STOP_EARLY);
        RstTmr1 = ((RPT_TASK_TIMER_VAL + RPT_TASK_STOP_EARLY) - TmpTimer);
    }
    RstTimerValue(TIMER1, RstTmr1);


    //***************************************************************
    //************** Transparent recive data to Tag *****************
    //***************************************************************
    memcpy(pSyncBeacon->RepeaterID, &RptDefSet.RptID[3], 2);
    /* Encrypt Beacon packet */
    if (cryptflag == 1)
    {
        A7127_WriteReg(KEYC_REG, 0x09); //Enable A7127 RF Hardware AES128
    }
    A7127RF_TransPkt(SYNC_LEN, SyncBuf);

    A7127_WriteReg(KEYC_REG, 0x08); //Disable A7127 RF Hardware AES128
    //****************************************************************

    switch (pSyncBeacon->Action)
    {
        case RF_AL_SYNC:
        case RF_AL_ALL_WU_TIME:
            /*Delay1ms(10);
            if(g_RptGlblVar.SimualSncrFlag == 0){ //Test SNCR
                g_RptGlblVar.SimualSncrFlag++;
                FakeSncrProductPkt(SyncBuf);
            }else{
                g_RptGlblVar.SimualSncrFlag++;
                if(g_RptGlblVar.SimualSncrFlag == 30){
                    g_RptGlblVar.SimualSncrFlag = 0;
                    //FakeSncrProductPkt(SyncBuf);
                }
            }*/
            mResult = TAG_MSG_ONLY_SYNC;
            break;

        case RF_AL_WRITE_TAG_NVM_DATA:
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
            //printf("RF_AL_HOP_TO_TAG\r\n");
            //mhopcount = (pSyncBeacon->ChnCtrl) & 0xF;
            /*first time received hopping sync beacon */
            /*if(g_RptGlblVar.ghopcount == 0) {
                g_RptGlblVar.ghopcount = mhopcount;
                g_RptGlblVar.gCHindex = ((pSyncBeacon->ChnCtrl) >> 4) & 0xF;
            }
            */
            //mResult = TAG_MSG_HOPPING;
            mResult = TAG_MSG_NO_HIT_ID;
            break;

        default:
            mResult = TAG_MSG_RX_TIMEOUT; //TAG_MSG_NO_HIT_ID;
            break;
    }

    return mResult;
}

/*********************************************************************
* Rpt_RecvPktType_Proc
*********************************************************************/
uint8_t Rpt_HandleRecvPktType(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *TmpBuf)
{
    //uint8_t mhopcount;
    uint8_t cryptflag = 0;
    uint8_t i = 0;
    uint8_t mResult = 0;
    P_RPT_RECV_PKT_TYPE pRecvPktHead = (P_RPT_RECV_PKT_TYPE)TmpBuf;

    /*for(i = 0; i < FifoLen; i++)  {
        recv[i] = RADIO->RX_DATA[i];
    }
    */
    //*(P_MemCpyNode)recv = *(P_MemCpyNode)RADIO->RX_DATA;
    (*mRunTimePara).SeqIndex = pRecvPktHead->Seqno; //keep receive packet sequence number

    /* Processing packet types */
    switch (pRecvPktHead->PacketType)
    {
        case RF_PT_SYNC_BEACON:
            //if(pRunTimePara->RxPktLen == SYNC_LEN) {
            AES128_Decrypt(TmpBuf, AES_KEY, DecryptData);

            if (DecryptData[1] == 0x01)
            {
                cryptflag = 1;
                //(*mRunTimePara).SyncCryptFlag = DecryptData[1];
                //g_RptGlblVar.cryptflag = DecryptData[1];
                //memcpy(recv, DecryptData, FifoLen);
                *(P_MemCpyNode)TmpBuf = *(P_MemCpyNode)DecryptData;
            }/*else {
                        //do nothing, because it is RAW data.
                    }*/
            //memcpy(g_RptGlblVar.SyncBuf , recv, FifoLen);
            //*(P_MemCpyNode)g_RptGlblVar.SyncBuf = *(P_MemCpyNode)recv;
            //*(P_MemCpyNode)&(*mRunTimePara).KeepSyncPkt = *(P_MemCpyNode)recv;
            //*(P_MemCpyNode)&(*mRunTimePara).KeepRxPktStr.SyncPkt = *(P_MemCpyNode)TmpBuf;
            //}

            mResult = Rpt_RecvBeacon(cryptflag, TmpBuf);
            break;

        case RF_PT_WRITE_TAG_NVM_DATA:
        case RF_PT_OVER_THE_AIR:

        case RF_PT_TAG_UNBOUND:
        case RF_PT_CHANGE_BEACON_SRC:

        case RF_PT_TAG_STATUS:
        case RF_PT_REPEATER_STATUS:

        case RF_PT_TAG_UPDATE_LIST:
        case RF_PT_ALWAYS_WAKE_BEACON:
            (*mRunTimePara).KeepRxDataCnt = 0; //clear keep receive big data counter
            *(P_MemCpyNode)&(*mRunTimePara).KeepRxPktStrc.DeviceListPkt = *(P_MemCpyNode)TmpBuf;

            mResult = Rpt_RecvTagList(mRunTimePara, TmpBuf);
            break;

        case RF_PT_HOPPING_BEACON:
            //printf("RF_PT_HOPPING_BEACON\r\n");
            //mhopcount = recv[3] & 0xF;

            //first time received hopping sync beacon
            /*if(g_RptGlblVar.ghopcount == 0) {
                    g_RptGlblVar.ghopcount = mhopcount;
                    g_RptGlblVar.gCHindex = (recv[3] >> 4) & 0xF;
            }*/
            //setSynTimer(recv[4], updateflag);     //Sync time sec

            //mResult = TAG_MSG_HOPPING;
            mResult = TAG_MSG_NO_HIT_ID;
            break;

        case RF_PT_PACKET_HEADER:
        case RF_PT_DATA_PACKET:
            mResult = Rpt_RecvUpdData(mRunTimePara, TmpBuf);
            break;

        default:
#ifndef _NO_DBG_PRINTF
            DBGMSG("Rx dummy Pkt = ");
            for (i = 0; i < mRunTimePara->RxPktLen; i++)
            {
                DBGMSG("%x ", TmpBuf[i]);
            }
            DBGMSG("\r\n");
#endif //_NO_DBG_PRINTF
            break;
    }

    return mResult;
}
//**********************************************************************************

/*uint8_t rpt_rfpacket_receive(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t mSec, FunctionalState TimerState)
{
        uint8_t RxPktResult = 0;
        uint8_t WTRFlag = 0;

        M8107RF_SetFIFOLvl(FifoLen); // Set RF receive packet fifo length

        StrobeCmd(CMD_RX); //RF entry RX state
        Oneshoutflag = 0;   //Timer0 timeout flag

        if(TimerState == ENABLE) {
            SetTimer(TIMER0, mSec); //set receive packet timeout
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
                        RxPktResult = Rpt_HandleRecvPktType(FifoLen, RecvBuf); //handle rx packet
                    }else {
                        RxPktResult = TAG_MSG_RX_TIMEOUT;
                    }
                    break;
                }
            }
        }

        return RxPktResult;
}
*/

uint8_t Rpt_RxMode_RecvUpdData(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *RxTmpBuf)
{
    uint8_t mReturn = 0, i = 0;
    int16_t mlen = 0;
    uint32_t iRSSI = 0;
    P_TAG_UPDATE_LIST pKeepTagList = (P_TAG_UPDATE_LIST)mRunTimePara->KeepRxPktStrc.DeviceListPkt;
    P_RPT_UPD_TAG_HEAD pRptUpdHead = (P_RPT_UPD_TAG_HEAD)mRunTimePara->KeepRxPktStrc.RawHeadBuf[mRunTimePara->KeepRxDataCnt];

    mlen = pRptUpdHead->PictureSize;

    (*mRunTimePara).SeqIndex = 0; //clear sequence num
    Oneshoutflag = 0;   //Timer0 timeout flag

    M8107RF_SetFIFOLvl(PAYLOAD_LEN); //Set M8107 RF packet fifo length
    A7127RF_SetFIFOLen(PAYLOAD_LEN); //Set A7127 RF packet fifo length

    switch (RptDefSet.RptDataRate)
    {
        case RF_DATARATE_1M:
            SetTimer(TIMER0, RPT_1M_UPDTIMER);      //100ms
            break;
        case RF_DATARATE_500K:
            SetTimer(TIMER0, RPT_500K_UPDTIMER);        //135ms, Cover all data transmit time
            break;
        case RF_DATARATE_250K:
            SetTimer(TIMER0, RPT_250K_UPDTIMER);        //200ms
            break;
    }

    while (mlen > 0)
    {
        mReturn = M8107RF_RecvPacket(mRunTimePara->RxPktLen, RxTmpBuf, mRunTimePara->RxPktTime, ENABLE);
        if (mReturn == TAG_MSG_RX_COMPLETED)
        {
            mReturn = Rpt_HandleRecvPktType(mRunTimePara, RxTmpBuf);
        }

        if (Oneshoutflag)
        {
            //DBGMSG("DATA_STATE___RX data timer out.\r\n");
            mReturn = TAG_MSG_PACKET_LOST;
            break;
        }
        else
        {
            mlen -= (PAYLOAD_LEN - 2);      //two bytes header
        }
    }
    Timer_StopTimer(TIMER0); //stop Timer0 of receive complete
    StrobeCmd(CMD_STBY); //exit rx mode

    if ((Oneshoutflag) || (mReturn == TAG_MSG_PACKET_LOST))     //Don't need update EPD because data error.
    {
        memset(pRptUpdHead, 0, sizeof(RPT_UPD_TAG_HEAD) - 3);
        mReturn = TAG_MSG_RX_TIMEOUT;
    }
    else
    {
        //mReturn = TAG_MSG_RX_COMPLETED;
        while (GIO1);   //wait A7127 transmit completed
        mReturn = A7127RF_RecvPacket(PAYLOAD_LEN, RxTmpBuf, /*RPT_TAG_REPLY_TIMER*/ 5, ENABLE); // A7127 receive Tag result
        A7127RF_StrobeCmd(CMD_STBY);

        if (mReturn == TAG_MSG_RX_COMPLETED)
        {
            for (i = 0; i < 5; i++)
            {
                M8107RF_SendPacket(PAYLOAD_LEN, RxTmpBuf); //transparent A7127 receive packet
                Delay1us(125);
            }
        }
    }

    //Receipt complete, reply repeater status
    //ReplyRptStatus(mRunTimePara->RxPktTime, mRunTimePara->KeepRxDataCnt,
    //mReturn, &mRunTimePara->KeepUpLayerID[3], pRptUpdHead->TagID, RxTmpBuf); //Receipt complete, reply repeater status

#ifndef _NO_DBG_PRINTF
    iRSSI = A7127_ReadReg(RSSI_REG);
    DBGMSG("%02X:%02X update ", pRptUpdHead->TagID[3], pRptUpdHead->TagID[4]);
    if (mReturn == TAG_MSG_RX_TIMEOUT)
    {
        DBGMSG("Faild");
    }
    else
    {
        DBGMSG("OK");
    }
    DBGMSG(", %d\r\n", iRSSI);
#endif // _NO_DBG_PRINTF

    (*mRunTimePara).KeepRxDataCnt++;
    if (mRunTimePara->KeepRxDataCnt < pKeepTagList->UpdTagCnt)
    {
        mReturn = TAG_MSG_HIT_ID;
    }
    else
    {
        (*mRunTimePara).KeepRxDataCnt = 0;
        mReturn = TAG_MSG_NO_HIT_ID;
    }

    return mReturn;
}

P_RPT_RUNTIME_PARAMETER Rpt_RxModeTask(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *RxTmpBuf)
{
    uint8_t mReturn = 0;
    uint16_t Tmr1Cnt = 0;

    if (mRunTimePara->RxPktState == 0)
    {
        return mRunTimePara;
    }

    switch (mRunTimePara->RxPktState)
    {
        case DATA_STATE:
            if (mRunTimePara->UpdRptFwStrc.UpdFirmwareFlag != 1)
            {
                mReturn = Rpt_RxMode_RecvUpdData(mRunTimePara, RxTmpBuf);
            }
            else
            {
                mReturn = HandleRecvOtaData(mRunTimePara, RxTmpBuf);
            }
            break;

        default:
            mReturn = M8107RF_RecvPacket(mRunTimePara->RxPktLen, RxTmpBuf, mRunTimePara->RxPktTime, ENABLE);
            if (mReturn == TAG_MSG_RX_COMPLETED)
            {
                mReturn = Rpt_HandleRecvPktType(mRunTimePara, RxTmpBuf);
            }
            break;
    }

    /* Handle return runtime message */
    switch (mReturn)
    {
        case TAG_MSG_HIT_ID:
            if ((mRunTimePara->RxPktState == TAG_LIST_STATE) || (mRunTimePara->RxPktState == DATA_STATE))
            {
                mRunTimePara->RxPktState = HEADER_STATE;
            }
            else if (mRunTimePara->RxPktState == HEADER_STATE)
            {
                mRunTimePara->RxPktState = DATA_STATE;
            }
            break;

        case TAG_MSG_HOPPING:
        case TAG_MSG_NO_HIT_ID:
        case TAG_MSG_ONLY_SYNC:
            mRunTimePara->RxPktState = 0;
            break;

        case TAG_MSG_REPORT_STATUS_OK:
        case TAG_MSG_TO_RCV_TAG_LIST:
            mRunTimePara->RxPktState = TAG_LIST_STATE;

            mRunTimePara->RxPktLen = PAYLOAD_LEN;
            mRunTimePara->RxPktTime = PRT_RX_PKT_TIMER;
            break;

        case TAG_MSG_RX_TIMEOUT:
            if (mRunTimePara->RunTimeMode != RPT_PWRON_MODE)
            {
                if ((mRunTimePara->RxPktState == BEACON_STATE) || (mRunTimePara->RxPktState == PWRBEACON_STATE))
                {
                    Tmr1Cnt = (RPT_TASK_TIMER_VAL + RPT_TASK_STOP_EARLY) - abs(RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1));
                    RstTimerValue(TIMER1, Tmr1Cnt);

                    mRunTimePara->RxPktState = 0; //PWRBEACON_STATE; //BEACON_STATE
                    mRunTimePara->RtnPwrModeflag = 1;

                    printf("Lost Beacon...%d\r\n", Tmr1Cnt);
                }
                else
                {
                    if (TASK_CUR_RUNTIME(TIMER1) < (PRT_RX_PKT_TIMER * 2))
                    {
                        mRunTimePara->RxPktState = 0;
                    }
                }
            }
            break;
    }

    return mRunTimePara;
}

//**********************************************************************************
