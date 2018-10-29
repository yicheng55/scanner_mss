#include "main.h"
#include "RptProc.h"
#include "RFpacket.h"
#include "RptDataFlash.h"
#include "RptComm.h"

extern uint8_t AES_KEY[];

extern void RstTimerValue(TIMER_Type *timer, uint32_t msec);

void Rpt_HandleFreeTimeRecvPkt(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t mSec)
{
    uint8_t RecvResult = 0;
    P_SCAN_PACKET_HEAD pScanPktHead;
    P_RPT_RECV_PKT_TYPE pFreeTimeRecv;

    Repeater_LED_Ctrl(TRIGGER_FOR_FREEBEACON);

    memset(RecvBuf, 0, FifoLen);

    RecvResult = M8107RF_RecvPacket(FifoLen, RecvBuf, mSec, ENABLE);

    if (RecvResult == TAG_MSG_RX_COMPLETED)
    {
        pFreeTimeRecv = (P_RPT_RECV_PKT_TYPE)RecvBuf;

        switch (pFreeTimeRecv->PacketType)
        {
            case RF_PT_SCANNER_PACKET:
                pScanPktHead = (P_SCAN_PACKET_HEAD)RecvBuf;

                if (CompareData(&RptDefSet.RptID[3], pScanPktHead->byRepeaterID, 2))
                {
                    //printf("RX SNCR Suc.\r\n");
                    M8107RF_SetCHAN(RptDefSet.Gchn);            //Change channel of gateway
                    M8107RF_SendPacket(FifoLen, RecvBuf); //Transparent scanner packet to gateway

                    RecvResult = M8107RF_RecvPacket(FifoLen, RecvBuf, mSec, ENABLE);

                    if (RecvResult == TAG_MSG_RX_COMPLETED)
                    {
                        M8107RF_SetCHAN(RptDefSet.Tchn); //Change channel of Gateway
                        M8107RF_SendPacket(FifoLen, RecvBuf); //Transparent gateway ack to scanner
                        printf("RX ACK Suc.\r\n");
                    }
                    else
                    {
                        printf("RX ACK fail.\r\n");
                    }
                }
                break;

            case RF_PT_TAG_BUTTON_NOTIFY:
            case RF_PT_TAG_BUTTON_NOTIFY_ACK:
                M8107RF_SetCHAN(RptDefSet.Gchn);            //Change channel of gateway
                M8107RF_SendPacket(FifoLen, RecvBuf); //Transparent scanner packet to gateway

                RecvResult = M8107RF_RecvPacket(FifoLen, RecvBuf, mSec, ENABLE);

                if (RecvResult == TAG_MSG_RX_COMPLETED)
                {
                    M8107RF_SetCHAN(RptDefSet.Tchn); //Change channel of Gateway
                    M8107RF_SendPacket(FifoLen, RecvBuf); //Transparent gateway ack to scanner
                }

                DBGMSG("ACK Tag BUTTON ");
                if (RecvResult == TAG_MSG_RX_COMPLETED)
                {
                    DBGMSG("Success...\r\n");
                }
                else if (RecvResult == TAG_MSG_RX_COMPLETED)
                {
                    DBGMSG("Timeout...\r\n");
                }
                break;
        }
    }

    //while(TASK_CUR_RUNTIME(TIMER1) > 30);
}

/**
  * @brief      Repeater reply Tag combine status to Gateway
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used reply Tag status to Gateway.
  */
void Rpt_SendTagCombRslt(uint8_t SeqNum, uint8_t result, uint8_t MultiTag, uint8_t *TagCombBuf)
{
    uint8_t i = 0;
    P_RPT_COMB_TAG_UPDRS RptCombTagUpdRs = (P_RPT_COMB_TAG_UPDRS)TagCombBuf;

    M8107RF_SetCHAN(RptDefSet.Gchn); //Change RF Channel of Gateway

    RptCombTagUpdRs->SeqNum = SeqNum++;
    RptCombTagUpdRs->PckType = RF_PT_REPEATER_COMBTAG_RS;
    RptCombTagUpdRs->Result = result;
    RptCombTagUpdRs->MultiTag = MultiTag;
    RptCombTagUpdRs->ResultLen = sizeof(RPT_COMB_TAG_UPDRS);

    /*for(i = 0; i < PAYLOAD_LEN; i++){
        DBGMSG("%x ", TxBuf[i]);
    }
    DBGMSG("\r\n");
    */

    for (i = 0; i < 10; i++)
    {
        M8107RF_SendPacket(PAYLOAD_LEN, TagCombBuf);
        Delay1us(125);
    }
}

/**
  * @brief      Repeater send update head & Data to Tag
  * @param[out] None
  * @return     None
  * @details    This API is used send update head & Data to Tag.
  */
void Rpt_SendUpdateData(P_RPT_RUNTIME_PARAMETER mRunTimePara, int16_t UpdDataLen, uint8_t *UpdDataBuf)
{
    uint16_t j = 0;
    P_RPT_UPD_TAG_DATA pSendBigData = (P_RPT_UPD_TAG_DATA)UpdDataBuf;

    mRunTimePara->SeqIndex = 0;

    while (UpdDataLen > 0)
    {
        pSendBigData->Seqno = mRunTimePara->SeqIndex++;
        pSendBigData->PacketType = RF_PT_DATA_PACKET;
        memcpy(pSendBigData->toTagData, &mRunTimePara->KeepRxPktStrc.RawDataBuf[mRunTimePara->TxDataCnt][0 + j], PAYLOAD_LEN - 2);

        M8107RF_SendPacket(PAYLOAD_LEN, UpdDataBuf); //TX Packet

        UpdDataLen -= (PAYLOAD_LEN - 2);
        j += (PAYLOAD_LEN - 2);

        Delay1us(125); //wait slave receive
    }
}

void Rpt_SendUpdateHead(uint8_t *UpdHeadBuf)
{
    P_RPT_UPD_TAG_HEAD pSendBigDataHead = (P_RPT_UPD_TAG_HEAD)UpdHeadBuf;

    //send head to Tag
    memcpy(&pSendBigDataHead->RepeaterID[0], &RptDefSet.RptID[3], 2);

    M8107RF_SendPacket(PAYLOAD_LEN, UpdHeadBuf);
}

void Rpt_TransUpdTagData(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint32_t FifoLen, uint8_t *TaskBuf)
{
    uint8_t RecvResult = 0, TxBigDataCnt = 0, RxDeviceRslt = 0, KeepRsltLen = 0;
    uint8_t KeepRsltBuf[PAYLOAD_LEN] = {0};
    int16_t UpdStartTime = 0, UpdDelayTime = 0, UpdTimeout = 0;
    int16_t TxBigDataLen = 0;

    P_TAG_UPDATE_LIST pKeepTagList = (P_TAG_UPDATE_LIST)mRunTimePara->KeepRxPktStrc.DeviceListPkt;

    P_RPT_UPD_TAG_HEAD pRptUpdHead;
    P_TAG_STATUS rTagStatus;
    P_RPT_COMB_TAG_STATUS pCombTagStatus;

    TxBigDataCnt = pKeepTagList->UpdTagCnt;

    (*mRunTimePara).TxDataCnt = 0; //clear transparent counter

    //start send update Tag head & raw data
    while (pKeepTagList->UpdTagCnt > 0)
    {
        pRptUpdHead = (P_RPT_UPD_TAG_HEAD)mRunTimePara->KeepRxPktStrc.RawHeadBuf[mRunTimePara->TxDataCnt];
        TxBigDataLen = pRptUpdHead->PictureSize;

        UpdStartTime = TASK_CUR_RUNTIME(TIMER1); //Keep starting update time

        switch (RptDefSet.RptDataRate)
        {
            case RF_DATARATE_1M:
                if (TxBigDataLen != 0)
                {
                    Delay1us(2750); //Delay 2.75ms for send Head
                    Rpt_SendUpdateHead(mRunTimePara->KeepRxPktStrc.RawHeadBuf[mRunTimePara->TxDataCnt]);
                    Delay1us(125);  //Delay 0.125ms for send Data
                    Rpt_SendUpdateData(mRunTimePara, TxBigDataLen, TaskBuf);
                }
                else
                {
                    DBGMSG("No Data Upd\r\n");
                    Delay1ms(RPT_1M_UPDTIMER - 12);
                }
                UpdTimeout = RPT_1M_UPDTIMER;
                break;

            case RF_DATARATE_500K:
                if (TxBigDataLen != 0)
                {
                    Delay1us(1450); //Delay 1.45ms for send Head
                    Rpt_SendUpdateHead(mRunTimePara->KeepRxPktStrc.RawHeadBuf[mRunTimePara->TxDataCnt]);

                    Delay1us(500); //Delay 0.05ms for send Data
                    Rpt_SendUpdateData(mRunTimePara, TxBigDataLen, TaskBuf);
                }
                else
                {
                    DBGMSG("No Data Upd\r\n");
                    Delay1ms(RPT_500K_UPDTIMER - 12);
                }
                UpdTimeout = RPT_500K_UPDTIMER;
                break;

            case RF_DATARATE_250K:
                if (TxBigDataLen != 0)
                {
                    Delay1us(2400); //Delay 2.4ms for send Head
                    Rpt_SendUpdateHead(mRunTimePara->KeepRxPktStrc.RawHeadBuf[mRunTimePara->TxDataCnt]);
                    Delay1us(2000); //Delay 2.0ms for send Data
                    Rpt_SendUpdateData(mRunTimePara, TxBigDataLen, TaskBuf);
                }
                else
                {
                    DBGMSG("No Data Upd\r\n");
                    Delay1ms(RPT_250K_UPDTIMER - 12);
                }
                UpdTimeout = RPT_250K_UPDTIMER;
                break;
        }
        pKeepTagList->UpdTagCnt--;
        /* receive tag reply status */
        RecvResult = M8107RF_RecvPacket(FifoLen, TaskBuf, RPT_TAG_REPLY_TIMER, ENABLE);

        pCombTagStatus = (P_RPT_COMB_TAG_STATUS)&TaskBuf[2];
        switch (RecvResult)
        {
            case TAG_MSG_RX_TIMEOUT:
                pCombTagStatus->Result = TAG_MSG_RX_TIMEOUT;
                memcpy(pCombTagStatus->TAG_ID, pRptUpdHead->TagID, 5);
                break;
            case TAG_MSG_RX_COMPLETED:
                memcpy(&TaskBuf[16], &TaskBuf[17], 2); // Aligment Tag Result formate
                break;
        }

        memcpy(&KeepRsltBuf[sizeof(RPT_COMB_TAG_UPDRS) + KeepRsltLen], &TaskBuf[2], sizeof(RPT_COMB_TAG_STATUS));
        KeepRsltLen += (sizeof(RPT_COMB_TAG_STATUS));

        (*mRunTimePara).TxDataCnt++; //counter trans big data

        rTagStatus = (P_TAG_STATUS)&TaskBuf[3];
        DBGMSG("%02X:%02X update ", rTagStatus->TAG_ID[3], rTagStatus->TAG_ID[4]);
        if (RecvResult == TAG_MSG_RX_TIMEOUT)
        {
            DBGMSG("faild...");
        }
        else if (RecvResult == TAG_MSG_RX_COMPLETED)
        {
            RxDeviceRslt = TAG_MSG_RX_COMPLETED;
            DBGMSG("OK...");
        }
        DBGMSG("%d\r\n", RSSI_measurement());

        /* Waiting for a tag update time */
        UpdDelayTime = (UpdTimeout - abs(UpdStartTime - TASK_CUR_RUNTIME(TIMER1)));
        if (UpdDelayTime > 0)
        {
            Delay1ms(UpdDelayTime);
        }
    }

    /* Wait for tx task timer < 30, reply tag combo status to gateway */
    while (TASK_CUR_RUNTIME(TIMER1) > 30);

    Rpt_SendTagCombRslt(mRunTimePara->SeqIndex, RxDeviceRslt, TxBigDataCnt, KeepRsltBuf);
}

void Rpt_HandleBeacon_Action(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint32_t FifoLen, uint8_t *TmpTxBuf)
{
    uint8_t RecvResult = 0, i = 0, HopChnflag = 0;
    uint8_t TxRsltBuf[64] = {0};
    static uint8_t HoppingCnt = 0, ChnIndex = 0;
    P_SYNC_BEACON pTxSyncBeacon = (P_SYNC_BEACON)mRunTimePara->KeepRxPktStrc.SyncPkt;

    P_TAG_UPDATE_LIST pKeepTagList = (P_TAG_UPDATE_LIST)mRunTimePara->KeepRxPktStrc.DeviceListPkt;

    P_TAG_UPDATE_LIST pTagListData;
    P_TAG_STATUS gTagStatus;
    P_RPT_COMB_TAG_STATUS pCombTagStatus;


    Delay1us(125);

    switch (pTxSyncBeacon->Action)
    {
        case RF_AL_SYNC:
        case RF_AL_ALL_WU_TIME:
            ChnIndex = HoppingCnt = 0; //clear hopping counter & channel index for hopping to change channel
            Rpt_HandleFreeTimeRecvPkt(FifoLen, TmpTxBuf, RPT_FREETIME_RECV_TIMER); //freetime enter handle scanner
            break;

        case RF_AL_GET_TAG_STATUS:
        case RF_AL_TAG_ALWAYS_WAKE_UP:
            pTagListData = (P_TAG_UPDATE_LIST)mRunTimePara->KeepRxPktStrc.DeviceListPkt;

            M8107RF_SendPacket(FifoLen, mRunTimePara->KeepRxPktStrc.DeviceListPkt);
            Delay1us(125);

            RecvResult = M8107RF_RecvPacket(FifoLen, TmpTxBuf, RPT_HANDLE_BIND_TIMER, ENABLE);

            pCombTagStatus = (P_RPT_COMB_TAG_STATUS)&TmpTxBuf[2];

            switch (RecvResult)
            {
                case TAG_MSG_RX_TIMEOUT:
                    pCombTagStatus->Result = TAG_MSG_RX_TIMEOUT;
                    memcpy(pCombTagStatus->TAG_ID, pKeepTagList->TAGID, 5);
                    break;
                case TAG_MSG_RX_COMPLETED:
                    memcpy(&TmpTxBuf[16], &TmpTxBuf[17], 2); // Aligment Tag Result formate
                    break;
            }

            memcpy(&TxRsltBuf[sizeof(RPT_COMB_TAG_UPDRS)], &TmpTxBuf[2], sizeof(RPT_COMB_TAG_STATUS));

            Rpt_SendTagCombRslt(pKeepTagList->Seq, RecvResult, pKeepTagList->UpdTagCnt, TxRsltBuf);
            break;

        case RF_AL_TAG_UNBOUND:
        case RF_AL_CHANGE_BEACON_SRC:
            pTagListData = (P_TAG_UPDATE_LIST)mRunTimePara->KeepRxPktStrc.DeviceListPkt;

            M8107RF_SendPacket(FifoLen, mRunTimePara->KeepRxPktStrc.DeviceListPkt);
            Delay1us(125);

            RecvResult = M8107RF_RecvPacket(FifoLen, TmpTxBuf, RPT_HANDLE_BIND_TIMER, ENABLE);

            if (RecvResult == TAG_MSG_RX_COMPLETED)
            {
                gTagStatus = (P_TAG_STATUS)&TmpTxBuf[3];

                if (CompareData(&gTagStatus->REPEATER_ID[0], &RptDefSet.RptID[3], 2))
                {
                    M8107RF_SetCHAN(RptDefSet.Gchn); //Change Repeater Channel of Tag

                    for (i = 0; i < 10; i++)
                    {
                        M8107RF_SendPacket(PAYLOAD_LEN, TmpTxBuf);
                        Delay1us(125);
                    }
                }
            }

            if (pTxSyncBeacon->Action == RF_AL_TAG_UNBOUND)
            {
                DBGMSG("Unbind ");
                if (RecvResult == TAG_MSG_RX_TIMEOUT)
                {
                    DBGMSG("Timeout...\r\n");
                }
                else if (RecvResult == TAG_MSG_RX_COMPLETED)
                {
                    DBGMSG("%x:%x Success...\r\n", pTagListData->TAGID[3], pTagListData->TAGID[4]);
                }
            }
            else if (pTxSyncBeacon->Action == RF_AL_CHANGE_BEACON_SRC)
            {
                DBGMSG("Change Beacon ");
                if (RecvResult == TAG_MSG_RX_TIMEOUT)
                {
                    DBGMSG("Timeout...\r\n");
                }
                else if (RecvResult == TAG_MSG_RX_COMPLETED)
                {
                    DBGMSG("%x:%x Success...\r\n", pTagListData->TAGID[3], pTagListData->TAGID[4]);
                }
            }
            break;

        case RF_AL_HOP_TO_TAG:
        case RF_AL_HOP_TO_REPEATER:
            HopChnflag = 1;
            if (HoppingCnt == 0)
            {
                HoppingCnt = (pTxSyncBeacon->ChnCtrl) & 0xF;
                ChnIndex = (pTxSyncBeacon->ChnCtrl >> 4) & 0xF;
            }
            else if (HoppingCnt == 1)
            {
                printf("Hop to channel = %d\r\n", CHTable[ChnIndex]);
                StrobeCmd(CMD_STBY);
                //SetCH(CHTable[g_RptGlblVar.gCHindex]); //freq 2450MHz CH100
                RptDefSet.Tchn = RptDefSet.Gchn = CHTable[ChnIndex];
                M8107RF_SetCHAN(RptDefSet.Tchn);

                //g_RptGlblVar.HotBcnFlag =
                ChnIndex = HoppingCnt = 0;
                Rpt_Write_DataFlash();

                printf("GwChn = %d, TagChn = %d\r\n", RptDefSet.Gchn, RptDefSet.Tchn);
                (*mRunTimePara).RtnPwrModeflag = 1;
            }
            break;

        case RF_AL_UPDATE_TAG:
        case RF_AL_UPDATE_REPEATER_TAG:
            pTagListData = (P_TAG_UPDATE_LIST)mRunTimePara->KeepRxPktStrc.DeviceListPkt;

            Repeater_LED_Ctrl(TRIGGER_FOR_UPD_TAG); //Enter update Tag

            M8107RF_SendPacket(FifoLen, mRunTimePara->KeepRxPktStrc.DeviceListPkt);
            Delay1us(125);

            Rpt_TransUpdTagData(mRunTimePara, FifoLen, TmpTxBuf);
            break;
    }

    //Hop Beacon change Channel count
    if (HopChnflag == 1)
    {
        if (HoppingCnt > 0)
        {
            HoppingCnt--;
        }
    }
}

/**
  * @brief      Repeater receive gateway beacon lose, send fake beacon.
  * @param[out] None
  * @return     None
  * @details    This API is used send fake beacon packet.
  */
P_RPT_RUNTIME_PARAMETER Rpt_SendBeaconPacket(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint32_t FifoLen)
{
    P_SYNC_BEACON pTxSyncBeacon = (P_SYNC_BEACON)mRunTimePara->KeepRxPktStrc.SyncPkt;
    P_TAG_UPDATE_LIST pKeepTagList = (P_TAG_UPDATE_LIST)mRunTimePara->KeepRxPktStrc.DeviceListPkt;

    if (mRunTimePara->RtnPwrModeflag == 1)
    {
        // Repeater rx beacon fake, create beacon send to tag
        pTxSyncBeacon->BeaconSeq = pTxSyncBeacon->BeaconSeq++;
        pTxSyncBeacon->BeaconType = RF_PT_SYNC_BEACON;
        pTxSyncBeacon->Action = RF_AL_SYNC;
        pTxSyncBeacon->ChnCtrl = 0;
        pTxSyncBeacon->WakeUpTime = 1;
    }
    else
    {
        switch (pTxSyncBeacon->Action)
        {
            case RF_AL_UPDATE_TAG:
            case RF_AL_UPDATE_REPEATER_TAG:
                pTxSyncBeacon->Action = RF_AL_UPDATE_TAG;
                if (!CompareData(pKeepTagList->RepeaterID, &RptDefSet.RptID[3], 2))
                {
                    //DGBMSG("Not updating tags through this repeater\r\n");
                    pTxSyncBeacon->Action = RF_AL_SYNC;
                }
                break;

            case RF_AL_WRITE_TAG_NVM_DATA:
            case RF_AL_WRITE_TAG_ORIGINAL_NVM_DATA:
            case RF_AL_GET_REPEATER_STATUS:
                pTxSyncBeacon->Action = RF_AL_SYNC;
                break;
        }
    }

    /* copy Repeater ID to SyncBeacon RepeaterID field */
    memcpy(pTxSyncBeacon->RepeaterID, &RptDefSet.RptID[3], 2);

    /* Encrypt Beacon packet */
    if (mRunTimePara->SyncCryptFlag == 1)
    {
        (*mRunTimePara).SyncCryptFlag = 0; //clear crypt flag
        AES128_Encrypt(mRunTimePara->KeepRxPktStrc.SyncPkt, AES_KEY, EncryptData);
        M8107RF_SendPacket(FifoLen, EncryptData);
    }
    else
    {
        M8107RF_SendPacket(FifoLen, mRunTimePara->KeepRxPktStrc.SyncPkt);
    }

    return mRunTimePara;
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
void Rpt_TxModeTask(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *TxPktBuf)
{
    int DelayBeacon = (RptDefSet.RptID[4] * RptDefSet.RptID[4]) * 10;

    RstTimerValue(TIMER1, RPT_TASK_TIMER_VAL - (RPT_TASK_STOP_EARLY - 1));

    //Delay send beacon, Dealy microsecond = (RepeaterID[4] * RepeaterID[4] * 10)
    while ((DelayBeacon--) > 0)
    {
        __nop();
    }
    //Delay1us(DelayBeacon);

    //Tx Beacon
    mRunTimePara = Rpt_SendBeaconPacket(mRunTimePara, SYNC_LEN);

    //handle next action
    Rpt_HandleBeacon_Action(mRunTimePara, PAYLOAD_LEN, TxPktBuf);
}
