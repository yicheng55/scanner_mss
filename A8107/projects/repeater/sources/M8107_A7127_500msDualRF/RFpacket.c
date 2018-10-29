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
#include "RptOTA.h"
#include "RptDataflash.h"
#include "RptDataFlash.h"

//uint8_t mindex; //for updata packet lost check
//uint8_t g_RptGlblVar.ghopcount;
//uint8_t g_RptGlblVar.gCHindex;

/*
 * Jerry add
*/
extern uint8_t AES_KEY[];
extern void RstTimerValue(TIMER_Type *timer, uint32_t msec);
extern void Rpt_HandleBindGateway(P_SYNC_BEACON pSyncBeacon);
extern void OTA_Init(void);

/**
  * @brief      Send repeater stauts to gatway
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used send repeater status to gateway.
  */
void Rpt_Status(uint8_t result)
{
    P_RPT_STATUS_REPORT RptRpyStatus = (P_RPT_STATUS_REPORT)g_RptGlblVar.TxBuffer;

    RptRpyStatus->SeqNum = g_RptGlblVar.gTxSeqNo++;
    RptRpyStatus->PckType = RF_PT_REPEATER_STATUS;
    RptRpyStatus->Result = result;

    memcpy(&RptRpyStatus->GatewayID[0], &g_RptGlblVar.GatewayID[3], 2);
    memcpy(&RptRpyStatus->RepeaterID[0], &RptDefSet.RptID[3], 2);

    RptRpyStatus->RptM8107RSSI = RSSI_measurement();
    RptRpyStatus->GwCHN = RptDefSet.Gchn;
    RptRpyStatus->TagCHN = RptDefSet.Tchn;
    RptRpyStatus->RptDataRate = RptDefSet.RptDataRate;
    RptRpyStatus->RptMode = RPT_ONERF_ONE_CHN_MODE;
    memcpy(RptRpyStatus->TagID, &g_RptGlblVar.TagID[g_RptGlblVar.GlblTempVar.tMultiTagCnt], 5);

    M8107RF_SendPacket(PAYLOAD_LEN, g_RptGlblVar.TxBuffer);

    g_RptGlblVar.GlblTempVar.tMultiTagCnt += 5;
}

uint8_t HandleRecvUpdTagHead(P_RPT_UPD_TAG_HEAD pTagUpdHead)
{
    uint8_t mResult = 0;

    if (CompareData(&RptDefSet.RptID[3], &pTagUpdHead->RepeaterID[0], 2))
    {
        g_RptGlblVar.mindex = pTagUpdHead->Seqno; //backup sequence number

        g_RptGlblVar.GlblTempVar.tTxUpdDataLen = 0; //clear receive update update data length counter
        memcpy(g_RptGlblVar.UpdHeadBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2], &pTagUpdHead[0], sizeof(RPT_UPD_TAG_HEAD)); //backup update Tag head

        mResult = TAG_MSG_HIT_ID;
    }
    else
    {
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
    uint8_t *pEpdBuf; //= (uint8_t *)g_RptGlblVar.UpdDataBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2];//[g_RptGlblVar.GlblTempVar.tTxUpdDataLen];
    //P_RPT_UPD_TAG_HEAD pTagUpdHead;

    switch (PktType)
    {
        case RF_PT_PACKET_HEADER:
            if (g_RptGlblVar.UpdFirmwareFlag != 1)
            {
                mResult = HandleRecvUpdTagHead((P_RPT_UPD_TAG_HEAD)UpdDataBuf);
            }
            else
            {
                mResult = HandleRecvOtaHead(UpdDataBuf);
            }
            break;

        case RF_PT_DATA_PACKET:
            if ((UpdDataBuf[0] - g_RptGlblVar.mindex) > 1)
            {
                //DBGMSG("Lost some data...%x - %x\r\n", UpdDataBuf[0], g_RptGlblVar.mindex);
                mResult = TAG_MSG_PACKET_LOST; //indicate packet lost
                break;
            }
            else
            {
                pEpdBuf = (uint8_t *)&g_RptGlblVar.UpdDataBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2][g_RptGlblVar.GlblTempVar.tTxUpdDataLen];
                g_RptGlblVar.mindex = UpdDataBuf[0];    //backup sequence number
                memcpy(pEpdBuf, &UpdDataBuf[2], PAYLOAD_LEN - 2); //62 : -two bytes header, 64-2
                g_RptGlblVar.GlblTempVar.tTxUpdDataLen += (PAYLOAD_LEN - 2);
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

    memset(g_RptGlblVar.TagID, 0, sizeof(g_RptGlblVar.TagID)); //Clear backup TagID buffer
    //ret = CompareData(pTagUpdateList->RepeaterID, &RptDefSet.RptID[3], sizeof(pTagUpdateList->RepeaterID)); //Compare packet repeater field

    switch (PktType)
    {
        case RF_PT_TAG_UPDATE_LIST:
            //if(ret == true) {
            if (CompareData(pTagUpdateList->RepeaterID, &RptDefSet.RptID[3], sizeof(pTagUpdateList->RepeaterID)))
            {
                /* Repeater cannot receive more than 3 tag updates data */
                if ((pTagUpdateList->PktCtrl & 0x7) > RPT_RX_MAX_BIGDATA)
                {
                    g_RptGlblVar.TxUpdTagCnt = g_RptGlblVar.GlblTempVar.tUpdCntTmp1 = mtag = RPT_RX_MAX_BIGDATA;
                }
                else
                {
                    g_RptGlblVar.TxUpdTagCnt = g_RptGlblVar.GlblTempVar.tUpdCntTmp1 = mtag = (pTagUpdateList->PktCtrl & 0x7);       //want update number
                }

                memcpy(&g_RptGlblVar.TagID[0], &pTagUpdateList->TAGID[0], mtag * 5); //backup update Tag ID
                memcpy(g_RptGlblVar.TagLstBuf, TagListBuf, PAYLOAD_LEN); //backup received Tag list packet to buffer

                mResult = TAG_MSG_HIT_ID; //Match update
            }
            else
            {
                mResult = TAG_MSG_NO_HIT_ID;
            }
            break;

        case RF_PT_TAG_STATUS:
            memcpy(g_RptGlblVar.TagLstBuf, TagListBuf, PAYLOAD_LEN); //Copy received Tag list data to buffer
            mResult = TAG_MSG_NO_HIT_ID;
            break;

        case RF_PT_REPEATER_STATUS:
            //if(ret == true){
            if (CompareData(pTagUpdateList->RepeaterID, &RptDefSet.RptID[3], sizeof(pTagUpdateList->RepeaterID)))
            {
                Rpt_Status(TAG_MSG_REPORT_STATUS_OK); //reply repeater status
            }
            mResult = TAG_MSG_NO_HIT_ID;
            break;

        case RF_PT_TAG_UNBOUND:
        case RF_PT_CHANGE_BEACON_SRC:
            memcpy(g_RptGlblVar.TagLstBuf, TagListBuf, PAYLOAD_LEN); //backup received Tag list data to buffer
            mResult = TAG_MSG_NO_HIT_ID;
            break;

        case RF_PT_OVER_THE_AIR:
            pDevieOtaList = (P_TAG_UPDATE_LIST)TagListBuf;

            if ((CompareData(pDevieOtaList->RepeaterID, &RptDefSet.RptID[3], sizeof(pDevieOtaList->RepeaterID)))
                    && (pDevieOtaList->Attribution == Attribution_OTA_Repeater))
            {
                g_RptGlblVar.UpdFirmwareCount = pDevieOtaList->TotalOfPacket;
                g_RptGlblVar.WrNewAppAddress = NEW_APP_START_ADDR;

                g_RptGlblVar.BckupSegmentIndex = 0;
                g_RptGlblVar.UpdFirmwareFlag = 1;

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

uint8_t Rpt_RecvBeacon(uint8_t *SyncBuf)
{
    bool ret;
    uint8_t mResult = 0, mhopcount;
    uint32_t RstTmr1 = 0, TmpTimer = 0;
    P_SYNC_BEACON pSyncBeacon = (P_SYNC_BEACON)SyncBuf;

    ret = CheckZero(pSyncBeacon->RepeaterID, 2);
    if (ret == false)
    {
        DBGMSG("RptID non zero...\r\n");
        return 0;
    }

    if (g_RptGlblVar.gMode != RPT_PWRON_MODE)
    {
        // check receive gateway beacon reset Timer1
        //DBGMSG("%d ", RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1));
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

        switch (pSyncBeacon->Action)
        {
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

            case RF_AL_HOP_TO_TAG:
            case RF_AL_HOP_TO_REPEATER:
                printf("hot to Tag or Rpt\r\n");
                mhopcount = (pSyncBeacon->ChnCtrl) & 0xF;

                if (g_RptGlblVar.ghopcount == 0)
                {
                    //first time received hopping sync beacon
                    g_RptGlblVar.ghopcount = mhopcount;
                    g_RptGlblVar.gCHindex = ((pSyncBeacon->ChnCtrl) >> 4) & 0xF;
                }

                mResult = TAG_MSG_HOPPING;
                break;

            default:
                mResult = TAG_MSG_RX_TIMEOUT; //TAG_MSG_NO_HIT_ID;
                break;
        }

        g_RptGlblVar.gTxSeqNo = pSyncBeacon->BeaconSeq; // record Gateway Sequence num
        g_RptGlblVar.RtnPwrModeCnt = 0; // if the repeater replies sync, clear return power on mode counter.

    }
    else
    {
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

    for (i = 0; i < FifoLen; i++)
    {
        recv[i] = RADIO->RX_DATA[i];
        //printf("%x ", recv[i]);
    }
    //printf("\r\n");

    if (FifoLen == SYNC_LEN)
    {
        AES128_Decrypt(recv, AES_KEY, DecryptData);

        if (DecryptData[1] == 0x01)
        {
            g_RptGlblVar.cryptflag = DecryptData[1];
            memcpy(recv, DecryptData, FifoLen);
        }/*else {
                //do nothing, because it is RAW data.
            }*/
        memcpy(g_RptGlblVar.SyncBuf , recv, FifoLen);
    }

    /* Processing packet types */
    switch (pRecvPktHead->PacketType)
    {
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
            mhopcount = recv[3] & 0xF;

            if (g_RptGlblVar.ghopcount == 0)  //first time received hopping sync beacon
            {
                g_RptGlblVar.ghopcount = mhopcount;
                g_RptGlblVar.gCHindex = (recv[3] >> 4) & 0xF;
            }

            mResult = TAG_MSG_HOPPING;
            break;

        case RF_PT_PACKET_HEADER:
        case RF_PT_DATA_PACKET:
            mResult = Rpt_RecvUpdData(pRecvPktHead->PacketType, recv);
            break;

        default:
#ifndef _NO_DBG_PRINTF
            DBGMSG("Rx dummy Pkt = ");
            for (i = 0; i < FifoLen; i++)
            {
                DBGMSG("%x ", recv[i]);
            }
            DBGMSG("\r\n");
#endif //_NO_DBG_PRINTF
            break;
    }

    return mResult;
}
//**********************************************************************************

uint8_t rpt_rfpacket_receive(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t mSec, FunctionalState TimerState)
{
    uint8_t RxPktResult = 0;
    uint8_t WTRFlag = 0;

    M8107RF_SetFIFOLvl(FifoLen); // Set RF packet fifo length

    StrobeCmd(CMD_RX); //RF entry RX state
    Oneshoutflag = 0;   //Time out flag

    if (TimerState == ENABLE)
    {
        SetTimer(TIMER0, mSec); //set get packet timeout timer
    }

    while (1)
    {
        if (WTRFlag == 0)  //Wait CMD_RX action, WTR = 1.
        {
            if ((RADIO->STATUS & (1 << 7)))
            {
                WTRFlag = 1; //if RF Rx ready toggle flag to Rx packet
            }
        }
        else
        {
            if ((!(RADIO->STATUS & (1 << 7))) || (Oneshoutflag))
            {
                if (TimerState == ENABLE)
                {
                    Timer_StopTimer(TIMER0);
                }
                StrobeCmd(CMD_STBY); //Back to Standby

                if (!Oneshoutflag)
                {
                    RxPktResult = Rpt_HandleRecvPktType(FifoLen, RecvBuf); //Rx packet
                }
                else        //Time out
                {
                    //DBGMSG("Get Pkt Timeout.\r\n");
                    RxPktResult = TAG_MSG_RX_TIMEOUT;
                }
                break;
            }
        }
    }

    return RxPktResult;
}

uint8_t Rpt_RxMode_RecvUpdData(uint8_t *RecvBuf)
{
    uint8_t mReturn = 0;
    //uint16_t UpdTimeOut = 0;
    P_RPT_UPD_TAG_HEAD pRptUpdHead = (P_RPT_UPD_TAG_HEAD)g_RptGlblVar.UpdHeadBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2];
    int16_t mlen = pRptUpdHead->PictureSize;

    g_RptGlblVar.mindex = 0; //clear sequence num
    Oneshoutflag = 0;   //Timer0 timeout flag

    /*if(mlen != 0) {
        UpdTimeOut = Rpt_CalcRecvUpd_Timeout(mlen); //Calculate when to receive updates
        SetTimer(TIMER0, UpdTimeOut); //set receive update data timeout
    }else {
    */
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
    //}

    /* receive update data */
    while (mlen > 0)
    {
        mReturn = rpt_rfpacket_receive(PAYLOAD_LEN, RecvBuf, 0, DISABLE);

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
        //receive update data fail, clear update head, keep update size for TX mode use
        memset(&g_RptGlblVar.UpdHeadBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2], 0, sizeof(RPT_UPD_TAG_HEAD) - 2);
        mReturn = TAG_MSG_RX_TIMEOUT;
    }
    else
    {
        mReturn = TAG_MSG_RX_COMPLETED;
    }
    Rpt_Status(mReturn); //Receipt complete reply repeater status

#ifndef _NO_DBG_PRINTF
    if (mReturn == TAG_MSG_RX_TIMEOUT)
    {
        DBGMSG("Data Lost = %d, %d\r\n", g_RptGlblVar.GlblTempVar.tUpdCntTmp2, g_RptGlblVar.GlblTempVar.tTxUpdDataLen);
    }
    else
    {
        DBGMSG("Recv Cmpl %d %d\r\n", g_RptGlblVar.GlblTempVar.tUpdCntTmp2, g_RptGlblVar.GlblTempVar.tTxUpdDataLen);
    }
#endif // _NO_DBG_PRINTF

    g_RptGlblVar.GlblTempVar.tTxUpdDataLen = 0; //clear receive update data counter
    g_RptGlblVar.GlblTempVar.tUpdCntTmp2++;

    if (g_RptGlblVar.GlblTempVar.tUpdCntTmp2 < g_RptGlblVar.GlblTempVar.tUpdCntTmp1)
    {
        //g_RptGlblVar.gState = HEADER_STATE;
        mReturn = TAG_MSG_HIT_ID;
    }
    else
    {
        g_RptGlblVar.GlblTempVar.tUpdCntTmp2 = g_RptGlblVar.gState = 0;
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

    if (TaskState == 0)
    {
        return ;
    }

    switch (TaskState)
    {
        case DATA_STATE:
            if (g_RptGlblVar.UpdFirmwareFlag != 1)
            {
                mReturn = Rpt_RxMode_RecvUpdData(RxTaskBuf);
            }
            else
            {
                mReturn = HandleRecvOtaData(RxTaskBuf);
            }
            break;

        default:
            mReturn = rpt_rfpacket_receive(g_RptGlblVar.RecvLen, RxTaskBuf, g_RptGlblVar.RecvTimer, ENABLE);
            break;
    }

    /* Handle return message */
    switch (mReturn)
    {
        case TAG_MSG_HIT_ID:
            if ((TaskState == TAG_LIST_STATE) || (TaskState == DATA_STATE))
            {
                g_RptGlblVar.gState = HEADER_STATE;
            }
            else if (TaskState == HEADER_STATE)
            {
                g_RptGlblVar.gState = DATA_STATE;
            }

            g_RptGlblVar.RecvLen = PAYLOAD_LEN;
            g_RptGlblVar.RecvTimer = PRT_RX_PKT_TIMER;
            break;

        case TAG_MSG_HOPPING:
            g_RptGlblVar.HotBcnFlag = 1;

        case TAG_MSG_NO_HIT_ID:
        case TAG_MSG_ONLY_SYNC:
            g_RptGlblVar.gState = 0;
            break;

        case TAG_MSG_REPORT_STATUS_OK:
        case TAG_MSG_TO_RCV_TAG_LIST:
            g_RptGlblVar.gState = TAG_LIST_STATE;

            //set receive timer & packet length
            g_RptGlblVar.RecvLen = PAYLOAD_LEN;
            g_RptGlblVar.RecvTimer = PRT_RX_PKT_TIMER;
            break;

        case TAG_MSG_RX_TIMEOUT:
            if (g_RptGlblVar.gMode != RPT_PWRON_MODE)
            {
                if ((TaskState == BEACON_STATE) || (TaskState == PWRBEACON_STATE))
                {
                    Tmr1Cnt = (RPT_TASK_TIMER_VAL + RPT_TASK_STOP_EARLY) - abs(RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1));
                    RstTimerValue(TIMER1, Tmr1Cnt);

                    g_RptGlblVar.gState = 0; //PWRBEACON_STATE; //BEACON_STATE
                    g_RptGlblVar.RtnPwrOnMode = 1;

                    printf("Lost Beacon...%d\r\n", Tmr1Cnt);
                }
                else
                {
                    if (TASK_CUR_RUNTIME(TIMER1) < (PRT_RX_PKT_TIMER * 2))
                    {
                        g_RptGlblVar.gState = 0;
                    }
                }
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

    switch (TaskState)
    {
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
            if (g_RptGlblVar.UpdFirmwareFlag != 1)
            {
                mReturn = Rpt_RxMode_RecvUpdData(RxTaskBuf);
            }
            else
            {
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

    switch (mReturn)
    {
        case TAG_MSG_HIT_ID:
            switch (TaskState)
            {
                //case DATA_STATE:
                case TAG_LIST_STATE:
                    g_RptGlblVar.gState = HEADER_STATE;
                    break;
                case HEADER_STATE:
                    g_RptGlblVar.gState = DATA_STATE;
                    break;
            }
            break;

        case TAG_MSG_HOPPING:
            g_RptGlblVar.HotBcnFlag = 1;
        case TAG_MSG_NO_HIT_ID:
        case TAG_MSG_ONLY_SYNC:
            g_RptGlblVar.gState = 0;
            break;

        case TAG_MSG_REPORT_STATUS_OK:
        case TAG_MSG_TO_RCV_TAG_LIST:
            g_RptGlblVar.gState = TAG_LIST_STATE;

            g_RptGlblVar.RecvLen = PAYLOAD_LEN;
            g_RptGlblVar.RecvTimer = PRT_RX_PKT_TIMER;
            break;

        case TAG_MSG_RX_TIMEOUT:
            switch (TaskState)
            {
                case PWRBEACON_STATE:
                case BEACON_STATE:
                    if (g_RptGlblVar.gMode != RPT_PWRON_MODE)
                    {

                        Tmr1Cnt = (RPT_TASK_TIMER_VAL + RPT_END_EARLY_TIMER) - abs(RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1));
                        RstTimerValue(TIMER1, Tmr1Cnt);

                        g_RptGlblVar.gState = 0; //PWRBEACON_STATE; //BEACON_STATE
                        g_RptGlblVar.RtnPwrOnMode = 1;

                        printf("Lost Beacon...\r\n");
                    }
                    break;
            }
            break;
    }
}
#endif //0

//**********************************************************************************
