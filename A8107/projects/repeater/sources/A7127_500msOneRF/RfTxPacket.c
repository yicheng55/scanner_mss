#include "main.h"
#include "RptProc.h"
#include "RFpacket.h"
#include "RptDataFlash.h"
#include "RptComm.h"
#include "A7127RF_Drv.h"

extern uint8_t AES_KEY[];

void Rpt_HandleFreeTimeRecvPkt(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t mSec)
{
    uint8_t RecvResult = 0;
    P_SCAN_PACKET_HEAD pScanPktHead = (P_SCAN_PACKET_HEAD)RecvBuf;

    YELLOW_LED_HIGH();

    memset(RecvBuf, 0, FifoLen);
    //RecvResult = M8107RF_RecvPacket(FifoLen, RecvBuf, mSec, ENABLE);
    RecvResult = A7127RF_RecvPacket(FifoLen, RecvBuf, mSec, ENABLE);

    if (pScanPktHead->byPacketType != RF_PT_SCANNER_PACKET)
    {
        RecvResult = 0;
    }

    if (RecvResult == TAG_MSG_RX_COMPLETED)
    {
        if (CompareData(&RptDefSet.RptID[3], pScanPktHead->byRepeaterID, 2))
        {
            printf("SNCR Suc.\r\n");
            //M8107RF_SetCHAN(RptDefSet.Gchn);                  //Change channel of gateway
            //M8107RF_SendPacket(FifoLen, RecvBuf);         //Transparent scanner packet to gateway

            A7127RF_SetCHAN(RptDefSet.Gchn);                    //Change channel of gateway
            A7127RF_SendPacket(FifoLen, RecvBuf);       //Transparent scanner packet to gateway

            //RecvResult = M8107RF_RecvPacket(FifoLen, RecvBuf, mSec, ENABLE);
            RecvResult = A7127RF_RecvPacket(FifoLen, RecvBuf, mSec, ENABLE);

            if (RecvResult == TAG_MSG_RX_COMPLETED)
            {
                printf("ACK Suc.\r\n");

                //M8107RF_SetCHAN(RptDefSet.Tchn);          //Change channel of Gateway
                //M8107RF_SendPacket(FifoLen, RecvBuf); //Transparent gateway ack to scanner
                A7127RF_SetCHAN(RptDefSet.Tchn);                //Change channel of gateway
                A7127RF_SendPacket(FifoLen, RecvBuf);   //Transparent gateway ack to scanner
            }
        }
        else
        {
            printf("ACK fail.\r\n");
        }
    }

    while (TASK_CUR_RUNTIME(TIMER1) < 30);
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

    RptCombTagUpdRs->SeqNum = g_RptGlblVar.gTxSeqNo++;
    RptCombTagUpdRs->PckType = RF_PT_REPEATER_COMBTAG_RS;
    RptCombTagUpdRs->Result = result;
    RptCombTagUpdRs->MultiTag = MultiTag;
    RptCombTagUpdRs->ResultLen = sizeof(RPT_COMB_TAG_UPDRS);

    /*for(i = 0; i < PAYLOAD_LEN; i++){
        DBGMSG("%x ", TxBuf[i]);
    }
    DBGMSG("\r\n");
    */

    //M8107RF_SetCHAN(RptDefSet.Gchn);              //Change RF Channel of Gateway
    A7127RF_SetCHAN(RptDefSet.Gchn);                //Change channel of gateway

    for (i = 0; i < 5; i++)
    {
        //M8107RF_SendPacket(PAYLOAD_LEN, TagCombBuf);
        A7127RF_SendPacket(PAYLOAD_LEN, TagCombBuf);
        Delay1us(1450);
    }
}

void Rpt_HdleReplyPacket(uint8_t Action, uint8_t mResult, uint8_t *RecvRsltBuf)
{
    uint8_t i = 0;
    static uint8_t tAccuTagRsltLen = 0;
    P_RPT_COMB_TAG_STATUS pCombTagStatus = (P_RPT_COMB_TAG_STATUS)&RecvRsltBuf[2];
    P_TAG_UPDATE_LIST pTxTagList = (P_TAG_UPDATE_LIST)g_RptGlblVar.TagLstBuf;
    //P_RPT_UPD_TAG_HEAD pRptUpdHead;// = (P_RPT_UPD_TAG_HEAD)g_RptGlblVar.UpdHeadBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2];

    switch (Action)
    {
        case RF_AL_UPDATE_TAG:
        case RF_AL_UPDATE_REPEATER_TAG:
            // Rpt_CombTag_Result(mResult, TagRsltBuf);
            switch (mResult)
            {
                case TAG_MSG_RX_TIMEOUT:
                    pCombTagStatus->Result = TAG_MSG_RX_TIMEOUT;
                    memcpy(pCombTagStatus->TAG_ID, &g_RptGlblVar.TagID[g_RptGlblVar.GlblTempVar.tUpdCntTmp2 * 5], 5);
                    break;
                case TAG_MSG_RX_COMPLETED:
                    memcpy(&RecvRsltBuf[16], &RecvRsltBuf[17], 2); // Aligment Tag Result formate
                    break;
            }

            // copy Tag result to combine buffer
            //memcpy(&g_RptGlblVar.TxBuffer[sizeof(RPT_COMB_TAG_UPDRS) +  g_RptGlblVar.GlblTempVar.tAccuTagRsltLen], &RecvRsltBuf[2], sizeof(RPT_COMB_TAG_STATUS));
            //g_RptGlblVar.GlblTempVar.tAccuTagRsltLen += (sizeof(RPT_COMB_TAG_STATUS));
            memcpy(&g_RptGlblVar.TxBuffer[sizeof(RPT_COMB_TAG_UPDRS) + tAccuTagRsltLen], &RecvRsltBuf[2], sizeof(RPT_COMB_TAG_STATUS));
            tAccuTagRsltLen += (sizeof(RPT_COMB_TAG_STATUS));

            g_RptGlblVar.RxTagUpdRslt = TAG_MSG_RX_COMPLETED; //Repeater receive Tag result complete.
            //g_RptGlblVar.UpdTagCmpl = 1; //trigger reply combine Tag result flag
            g_RptGlblVar.GlblTempVar.tRxCombTagRsCnt++; // count receive Tag result

            memset(g_RptGlblVar.UpdHeadBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2], 0, sizeof(RPT_UPD_TAG_HEAD));
            g_RptGlblVar.GlblTempVar.tUpdCntTmp2++;
            //pRptUpdHead = (P_RPT_UPD_TAG_HEAD)g_RptGlblVar.UpdHeadBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2];
            //g_RptGlblVar.GlblTempVar.tTxUpdDataLen = pRptUpdHead->PictureSize;
            //g_RptGlblVar.GlblTempVar.tTxUpdDataLen = g_RptGlblVar.RxTtlUpdDataLenBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2]; //copy update Tag length
            //g_RptGlblVar.TxUpdBufPtrAddr += g_RptGlblVar.GlblTempVar.tTxUpdDataLen; // move update data buffer adress

            //g_RptGlblVar.GlblTempVar.tUpdCntTmp1++;
            g_RptGlblVar.TxUpdTagCnt--;

            if (g_RptGlblVar.TxUpdTagCnt == 0)
            {
                tAccuTagRsltLen = 0;
            }
            break;

        case RF_AL_GET_TAG_STATUS:
            switch (mResult)
            {
                case TAG_MSG_RX_TIMEOUT:
                    pCombTagStatus->Result = TAG_MSG_RX_TIMEOUT;
                    memcpy(pCombTagStatus->TAG_ID, pTxTagList->TAGID, 5);
                    break;
                case TAG_MSG_RX_COMPLETED:
                    memcpy(&RecvRsltBuf[16], &RecvRsltBuf[17], 2); // Aligment Tag Result formate
                    break;
            }
            memcpy(&g_RptGlblVar.TxBuffer[sizeof(RPT_COMB_TAG_UPDRS)], &RecvRsltBuf[2], sizeof(RPT_COMB_TAG_STATUS));

            Rpt_SendTagCombRslt(mResult, g_RptGlblVar.TxUpdTagCnt, g_RptGlblVar.TxBuffer);
            break;

        case RF_AL_TAG_UNBOUND:
        case RF_AL_CHANGE_BEACON_SRC:
            //M8107RF_SetCHAN(RptDefSet.Gchn); //Change Repeater Channel of Tag
            A7127RF_SetCHAN(RptDefSet.Gchn); //Change Repeater Channel of Tag

            for (i = 0; i < 5; i++)
            {
                //M8107RF_SendPacket(PAYLOAD_LEN, RecvRsltBuf);
                A7127RF_SendPacket(PAYLOAD_LEN, RecvRsltBuf);
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

    while (UpdDataLen > 0)
    {
        pSendBigData->Seqno = DataSeqNo++;
        pSendBigData->PacketType = RF_PT_DATA_PACKET;
        //memcpy(pSendBigData->toTagData, &EpdRFData[g_RptGlblVar.TxUpdBufPtrAddr] + j, PAYLOAD_LEN - 2);
        memcpy(pSendBigData->toTagData, &g_RptGlblVar.UpdDataBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2][0 + j], PAYLOAD_LEN - 2);

        //M8107RF_SendPacket(PAYLOAD_LEN, UpdDataBuf); //TX Packet
        A7127RF_SendPacket(PAYLOAD_LEN, UpdDataBuf); //TX Packet

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
    /*pSendBigDataHead->Seqno = g_RptGlblVar.gTxSeqNo++;
    pSendBigDataHead->PacketType = RF_PT_PACKET_HEADER;
    memcpy(&pSendBigDataHead->TagID[0], &TagID[g_RptGlblVar.GlblTempVar.tUpdCntTmp1 * 5], 5);
    memcpy(&pSendBigDataHead->GetwayID[0], &GatewayID[0], 2);
    memcpy(&pSendBigDataHead->RepeaterID[0], &RptDefSet.RptID[3], 2);
    pSendBigDataHead->Compress = 0;
    pSendBigDataHead->PictureSize = g_RptGlblVar.GlblTempVar.tTxUpdDataLen;
    */
    memcpy(&pSendBigDataHead->RepeaterID[0], &RptDefSet.RptID[3], 2);

    //M8107RF_SendPacket(PAYLOAD_LEN, UpdHeadBuf); //TX Packet
    A7127RF_SendPacket(PAYLOAD_LEN, UpdHeadBuf); //TX Packet
}

void Rpt_TransUpdTagData(uint8_t Action, uint32_t FifoLen, uint8_t *TaskBuf)
{
    uint8_t RecvResult = 0;
    int16_t UpdStartTime = 0, UpdDelayTime = 0, UpdTimeout = 0;
    P_RPT_UPD_TAG_HEAD pRptUpdHead;
    P_TAG_STATUS rTagStatus;

    // Clear some variable
    /*g_RptGlblVar.RxUpdDataDone = g_RptGlblVar.TxUpdBufPtrAddr =
        g_RptGlblVar.GlblTempVar.tTxUpdDataLen =  g_RptGlblVar.GlblTempVar.tAccuTagRsltLen = */
    //g_RptGlblVar.GlblTempVar.tUpdCntTmp1 =
    //g_RptGlblVar.GlblTempVar.tAccuTagRsltLen = g_RptGlblVar.GlblTempVar.tTxUpdDataLen = g_RptGlblVar.GlblTempVar.tUpdCntTmp2 = 0;

    //copy the first update data length
    //g_RptGlblVar.GlblTempVar.tTxUpdDataLen = g_RptGlblVar.RxTtlUpdDataLenBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2];

    //start send update Tag head & raw data
    while (g_RptGlblVar.TxUpdTagCnt > 0)
    {
        //copy the first update data length
        pRptUpdHead = (P_RPT_UPD_TAG_HEAD)g_RptGlblVar.UpdHeadBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2];
        g_RptGlblVar.GlblTempVar.tTxUpdDataLen = pRptUpdHead->PictureSize;

        //Calculate when to trans updates
        /*if(g_RptGlblVar.GlblTempVar.tTxUpdDataLen != 0){
            UpdTimeout = Rpt_CalcRecvUpd_Timeout(g_RptGlblVar.GlblTempVar.tTxUpdDataLen);
        }
        */
        UpdStartTime = TASK_CUR_RUNTIME(TIMER1); //Keep starting update time

        switch (RptDefSet.RptDataRate)
        {
            case RF_DATARATE_1M:
                if (g_RptGlblVar.GlblTempVar.tTxUpdDataLen != 0)
                {
                    Delay1us(2750); //Delay 2.75ms for send Head
                    Rpt_SendUpdateHead(TaskBuf);
                    Delay1us(125);  //Delay 0.125ms for send Data
                    Rpt_SendUpdateData(pRptUpdHead->PictureSize, TaskBuf);
                }
                else
                {
                    DBGMSG("No Data Upd\r\n");
                    Delay1ms(RPT_1M_UPDTIMER - 12);
                }
                UpdTimeout = RPT_1M_UPDTIMER;
                break;

            case RF_DATARATE_500K:
                if (g_RptGlblVar.GlblTempVar.tTxUpdDataLen != 0)
                {
                    Delay1us(1450); //Delay 1.45ms for send Head
                    Rpt_SendUpdateHead(g_RptGlblVar.UpdHeadBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2]);
                    Delay1us(500); //Delay 0.05ms for send Data
                    Rpt_SendUpdateData(g_RptGlblVar.GlblTempVar.tTxUpdDataLen, TaskBuf);
                }
                else
                {
                    DBGMSG("No Data Upd\r\n");
                    Delay1ms(RPT_500K_UPDTIMER - 12);
                }
                UpdTimeout = RPT_500K_UPDTIMER;
                break;

            case RF_DATARATE_250K:
                if (g_RptGlblVar.GlblTempVar.tTxUpdDataLen != 0)
                {
                    Delay1us(2400); //Delay 2.4ms for send Head
                    Rpt_SendUpdateHead(TaskBuf);
                    Delay1us(2000); //Delay 2.0ms for send Data
                    Rpt_SendUpdateData(g_RptGlblVar.GlblTempVar.tTxUpdDataLen, TaskBuf);
                }
                else
                {
                    DBGMSG("No Data Upd\r\n");
                    Delay1ms(RPT_250K_UPDTIMER - 12);
                }
                UpdTimeout = RPT_250K_UPDTIMER;
                break;
        }
        /* receive tag reply status */
        //RecvResult = M8107RF_RecvPacket(FifoLen, TaskBuf, 6, ENABLE);
        A7127_WriteReg(MODECTRL_REG, 0x63); //Enable ADCM for RSSI measurement
        RecvResult = A7127RF_RecvPacket(FifoLen, TaskBuf, 6, ENABLE);
        Rpt_HdleReplyPacket(Action, RecvResult, TaskBuf);

        if (RecvResult == TAG_MSG_RX_TIMEOUT)
        {
            DBGMSG("Tag reply timeout");
        }
        else if (RecvResult == TAG_MSG_RX_COMPLETED)
        {
            rTagStatus = (P_TAG_STATUS)&TaskBuf[3];
            DBGMSG("%x:%x reply suc.", rTagStatus->TAG_ID[3], rTagStatus->TAG_ID[4]);
        }
        DBGMSG(", rssi = %d\r\n", A7127_ReadReg(RSSI_REG));

        /* Waiting for a tag update time */
        UpdDelayTime = (UpdTimeout - abs(UpdStartTime - TASK_CUR_RUNTIME(TIMER1)));
        if (UpdDelayTime > 0)
        {
            Delay1ms(UpdDelayTime);
        }
    }

    /* Wait for tx task timer < 30, reply tag combo status to gateway */
    while (TASK_CUR_RUNTIME(TIMER1) < 30);
    Rpt_SendTagCombRslt(g_RptGlblVar.RxTagUpdRslt, g_RptGlblVar.GlblTempVar.tRxCombTagRsCnt, g_RptGlblVar.TxBuffer);
}

void Rpt_HandleBeacon_Action(uint8_t Action, uint32_t FifoLen, uint8_t *TaskBuf)
{
    uint8_t RecvResult = 0;

    Delay1us(125);

    switch (Action)
    {
        case RF_AL_SYNC:
            Rpt_HandleFreeTimeRecvPkt(FifoLen, TaskBuf, RPT_FREETIME_RECV_TIMER); //freetime enter handle scanner
            break;

        case RF_AL_TAG_UNBOUND:
        case RF_AL_CHANGE_BEACON_SRC:

        case RF_AL_GET_TAG_STATUS:
        case RF_AL_TAG_ALWAYS_WAKE_UP:
            //M8107RF_SendPacket(FifoLen, g_RptGlblVar.TagLstBuf);
            A7127RF_SendPacket(FifoLen, g_RptGlblVar.TagLstBuf);

            //RecvResult = M8107RF_RecvPacket(FifoLen, TaskBuf, RPT_HANDLE_BIND_TIMER, ENABLE);
            RecvResult = A7127RF_RecvPacket(FifoLen, TaskBuf, RPT_HANDLE_BIND_TIMER, ENABLE);

            if (RecvResult == TAG_MSG_RX_COMPLETED)
            {
                Rpt_HdleReplyPacket(Action, RecvResult, TaskBuf);
            }

            if (Action == RF_AL_TAG_UNBOUND)
            {
                if (RecvResult == TAG_MSG_RX_TIMEOUT)
                {
                    DBGMSG("Unbind Timeout...\r\n");
                }
                else if (RecvResult == TAG_MSG_RX_COMPLETED)
                {
                    DBGMSG("Unbind %x:%x\r\n", g_RptGlblVar.TagLstBuf[11], g_RptGlblVar.TagLstBuf[12]);
                }
            }
            else if (Action == RF_AL_CHANGE_BEACON_SRC)
            {
                if (RecvResult == TAG_MSG_RX_TIMEOUT)
                {
                    DBGMSG("ChgBCN Timeout...\r\n");
                }
                else if (RecvResult == TAG_MSG_RX_COMPLETED)
                {
                    DBGMSG("ChgBCN %x:%x\r\n", g_RptGlblVar.TagLstBuf[11], g_RptGlblVar.TagLstBuf[12]);
                }
            }
            break;

        case RF_AL_HOP_TO_TAG:
        case RF_AL_HOP_TO_REPEATER:
            printf("g_RptGlblVar.ghopcount = %d\r\n", g_RptGlblVar.ghopcount);
            if (g_RptGlblVar.ghopcount == 1)        //Hopping
            {
                printf("Hop to channel = %d\r\n", CHTable[g_RptGlblVar.gCHindex]);
                StrobeCmd(CMD_STBY);
                //SetCH(CHTable[g_RptGlblVar.gCHindex]); //freq 2450MHz CH100
                RptDefSet.Tchn = RptDefSet.Gchn = CHTable[g_RptGlblVar.gCHindex];
                //M8107RF_SetCHAN(RptDefSet.Tchn);
                A7127RF_SetCHAN(RptDefSet.Tchn);

                g_RptGlblVar.HotBcnFlag = g_RptGlblVar.ghopcount = 0;
                Rpt_Write_DataFlash();

                g_RptGlblVar.RtnPwrOnMode = 1;
            }/*else if(g_RptGlblVar.ghopcount > 0){
                    g_RptGlblVar.ghopcount--;
                }*/
            break;

        case RF_AL_UPDATE_TAG:
        case RF_AL_UPDATE_REPEATER_TAG:
            if (g_RptGlblVar.RxUpdDataDone == 1)
            {
                g_RptGlblVar.RxUpdDataDone = 0;

                //memcpy(pTxTagList->TAGID, TagID, g_RptGlblVar.TxUpdTagCnt * 5);
                //M8107RF_SendPacket(FifoLen, g_RptGlblVar.TagLstBuf);
                A7127RF_SendPacket(FifoLen, g_RptGlblVar.TagLstBuf);

                Rpt_TransUpdTagData(Action, FifoLen, TaskBuf);
            }
            break;

        case RF_AL_GET_REPEATER_STATUS:
            break;
    }

    //Hop Beacon change Channel count
    if (g_RptGlblVar.HotBcnFlag == 1)
    {
        g_RptGlblVar.HotBcnFlag = 0;
        if (g_RptGlblVar.ghopcount > 0)
        {
            g_RptGlblVar.ghopcount--;
        }
    }
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

    if (g_RptGlblVar.RtnPwrOnMode == 1)
    {
        // Repeater rx beacon fake, create beacon send to tag
        pTxSyncBeacon->BeaconSeq = g_RptGlblVar.gTxSeqNo++;
        pTxSyncBeacon->BeaconType = RF_PT_SYNC_BEACON;
        pTxSyncBeacon->Action = RF_AL_SYNC;
        pTxSyncBeacon->ChnCtrl = 0;
        pTxSyncBeacon->WakeUpTime = 1;
    }
    else
    {
        // Repeater recv update data fail, change action type to Sync
        if ((g_RptGlblVar.RxUpdDataDone != 1) &&
                ((pTxSyncBeacon->Action == RF_AL_UPDATE_TAG) ||
                 (pTxSyncBeacon->Action == RF_AL_UPDATE_REPEATER_TAG)))
        {
            pTxSyncBeacon->Action = RF_AL_SYNC;
        }
        else
        {
            switch (pTxSyncBeacon->Action)
            {
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
    if (g_RptGlblVar.cryptflag == 1)
    {
        g_RptGlblVar.cryptflag = 0; //clear crypt flag
        A7127_WriteReg(KEYC_REG, 0x09);     //Enable A7127 RF Hardware AES128
        //AES128_Encrypt(SyncBuf, AES_KEY, EncryptData);
        //M8107RF_SendPacket(FifoLen, EncryptData);
    }/*else{
            //M8107RF_SendPacket(FifoLen, SyncBuf);
        }*/

    A7127RF_SendPacket(FifoLen, SyncBuf);
    A7127_WriteReg(KEYC_REG, 0x08);     //Disable A7127 RF Hardware AES128

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
    action = Rpt_SendBeaconPacket(SYNC_LEN, g_RptGlblVar.SyncBuf);

    //handle next action
    Rpt_HandleBeacon_Action(action, PAYLOAD_LEN, TxTaskBuf);
}
