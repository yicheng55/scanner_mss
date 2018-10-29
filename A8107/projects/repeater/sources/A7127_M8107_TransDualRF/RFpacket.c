/***********************************************************************
 * RFpacket.c
 *
 ***********************************************************************/
#include "main.h"
#include <math.h>
#include "isr.h"

#include "RptProc.h"
#include "RFpacket.h"
#include "RptDataflash.h"
#include "RptComm.h"
#include "A7127RF_Drv.h"

uint8_t EpdRFData[BIGDATA_BUFSIZE] = {0};
uint8_t *pEpdBuf = (uint8_t *)EpdRFData;

//uint8_t TagID[30] = {0};  //= {0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t GatewayID[5];         //= {0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t RepeaterID[5]         //= {0xFF, 0xFF, 0xFE, 0x00, 0x01};//{0x52, 0x00, 0x00, 0x00, 0x01};

//uint8_t g_RptGlblVar.mindex;      //Backup update tag head and data sequence number for check update packet lost
//uint8_t g_RptGlblVar.ghopcount;
//uint8_t g_RptGlblVar.gCHindex;

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

    YELLOW_LED_HIGH();

    memset(RxTaskBuf, 0, PAYLOAD_LEN);

    StrobeCmd(CMD_STBY);
    while (GIO1); //wait transmit completed
    mReturn = A7127RF_RecvPacket(PAYLOAD_LEN, RxTaskBuf, RPT_FREETIME_RECV_TIMER, ENABLE); //wait receive scanner packet

    if (pScanPktHead->byPacketType != RF_PT_SCANNER_PACKET)
    {
        return ;
    }

    if (mReturn == TAG_MSG_RX_COMPLETED)
    {
        if (CompareData(&RptDefSet.RptID[3], pScanPktHead->byRepeaterID, 2))
        {
            DBGMSG("SCNR %d\r\n", mReturn);
            A7127RF_StrobeCmd(CMD_STBY);
            M8107RF_SendPacket(PAYLOAD_LEN, RxTaskBuf); //transparent scanner packet to gateway
            mReturn = M8107RF_RecvPacket(PAYLOAD_LEN, RxTaskBuf, RPT_FREETIME_RECV_TIMER, ENABLE); //receive gateway reply ack

            if (mReturn == TAG_MSG_RX_COMPLETED)
            {
                DBGMSG("ACK %d\r\n", mReturn);

                StrobeCmd(CMD_STBY);
                A7127RF_SendPacket(PAYLOAD_LEN, RxTaskBuf); //transparent ack to scanner
                while (GIO1); //wait transmit completed
            }
        }
    }
}

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

    RptRpyStatus->GwCHN = RptDefSet.Gchn;
    RptRpyStatus->TagCHN = RptDefSet.Tchn;

    RptRpyStatus->RptM8107RSSI = RSSI_measurement();
    RptRpyStatus->RptA7127RSSI = RSSI_measurement();

    RptRpyStatus->RptDataRate = RptDefSet.RptDataRate;
    RptRpyStatus->RptMode = RPT_ONERF_ONE_CHN_MODE;
    memcpy(RptRpyStatus->TagID, &g_RptGlblVar.TagID[g_RptGlblVar.GlblTempVar.tMultiTagCnt], 5);

    //A7127RF_StrobeCmd(CMD_STBY);
    //M8107RF_SendPacket(PAYLOAD_LEN, g_RptGlblVar.TxBuffer);
    A7127RF_SendPacket(PAYLOAD_LEN, g_RptGlblVar.TxBuffer);

    g_RptGlblVar.GlblTempVar.tMultiTagCnt += 5;
}

uint8_t Rpt_RecvUpdData(uint8_t PktType, uint8_t *UpdDataBuf)
{
    bool ret;
    uint8_t mResult = 0;
    P_RPT_UPD_TAG_HEAD pTagUpdHead;

    switch (PktType)
    {
        case RF_PT_PACKET_HEADER:
            pTagUpdHead = (P_RPT_UPD_TAG_HEAD)UpdDataBuf;

            //ret = CompareData(&RptDefSet.RptID[3], &pTagUpdHead->RepeaterID[0], 2);
            //ret = CheckZero(pTagUpdHead->RepeaterID, 2);
            ret = 1;
            if (ret == true)
            {
                g_RptGlblVar.mindex = pTagUpdHead->Seqno; //copy sequence number
                g_RptGlblVar.RxTtlUpdDataLenBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2] = pTagUpdHead->PictureSize;

                //********************** Transparent ***************************
                //StrobeCmd(CMD_STBY);
                //while(GIO1); //wait transmit completed
                //A7127RF_SendPacket(PAYLOAD_LEN, UpdDataBuf);
                while (RADIO->STATUS & (1 << 7));   //WTR
                M8107RF_SendPacket(PAYLOAD_LEN, UpdDataBuf);
                //***************************************************************

                mResult = TAG_MSG_HIT_ID;
            }
            else
            {
                mResult = TAG_MSG_NO_HIT_ID;
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

                //********************** Transparent ***************************
                //StrobeCmd(CMD_STBY);
                //A7127RF_SendPacket(PAYLOAD_LEN, UpdDataBuf);
                while (RADIO->STATUS & (1 << 7));   //WTR
                M8107RF_SendPacket(PAYLOAD_LEN, UpdDataBuf);
                //***************************************************************

                g_RptGlblVar.mindex = UpdDataBuf[0];
                /*memcpy(pEpdBuf, &UpdDataBuf[2], PAYLOAD_LEN - 2);     //62: -two bytes header, 64-2
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

    memset(g_RptGlblVar.TagID, 0, sizeof(g_RptGlblVar.TagID));
    memset(g_RptGlblVar.RxTtlUpdDataLenBuf, 0, sizeof(g_RptGlblVar.RxTtlUpdDataLenBuf));

    switch (PktType)
    {
        case RF_PT_TAG_UPDATE_LIST:
            if (ret == true)
            {
                //g_RptGlblVar.TxUpdTagCnt =
                g_RptGlblVar.GlblTempVar.tUpdCntTmp1 = mtag = (pTagUpdateList->PktCtrl) & 0x7;      //want update number

                //memcpy(&GatewayID[3], pTagUpdateList->GatewayID, 2);
                memcpy(&g_RptGlblVar.TagID[0], &pTagUpdateList->TAGID[0], mtag * 5);


                //********************** Transparent ***************************
                //StrobeCmd(CMD_STBY);
                //while(GIO1); //wait transmit completed
                //A7127RF_SendPacket(PAYLOAD_LEN, TagListBuf);
                while (RADIO->STATUS & (1 << 7));   //WTR
                M8107RF_SendPacket(PAYLOAD_LEN, TagListBuf);
                //***************************************************************

                mResult = TAG_MSG_HIT_ID;  //Match update
            }
            else
            {
                mResult = TAG_MSG_NO_HIT_ID;
            }
            break;

        case RF_PT_TAG_STATUS:
            memcpy(g_RptGlblVar.TagLstBuf, TagListBuf, PAYLOAD_LEN); //Copy received Tag list data to buffer

            //************************ Transparent *************************
            //StrobeCmd(CMD_STBY);
            //while(GIO1); //wait transmit completed
            //A7127RF_SendPacket(PAYLOAD_LEN, TagListBuf);
            while (RADIO->STATUS & (1 << 7));
            M8107RF_SendPacket(PAYLOAD_LEN, TagListBuf);
            //***************************************************************

            mResult = M8107RF_RecvPacket(PAYLOAD_LEN, TagListBuf,   RPT_TAG_REPLY_TIMER, ENABLE);

            if (mResult == TAG_MSG_RX_TIMEOUT)
            {
                printf("Get Tag Status timeout...\r\n");
            }
            else if (mResult == TAG_MSG_RX_COMPLETED)
            {
                M8107RF_SendPacket(PAYLOAD_LEN, TagListBuf);
                printf("Get Tag Status success...\r\n");
            }
            mResult = TAG_MSG_NO_HIT_ID;
            break;

        case RF_PT_REPEATER_STATUS:
            if (ret == true)
            {
                Rpt_Status(TAG_MSG_REPORT_STATUS_OK);
                mResult = TAG_MSG_NO_HIT_ID;
            }
            break;

        case RF_PT_TAG_UNBOUND:
        case RF_PT_CHANGE_BEACON_SRC:
            //StrobeCmd(CMD_STBY);
            //while(GIO1); //wait A7127 TX Complete
            //A7127RF_SendPacket(PAYLOAD_LEN, TagListBuf); // Trans Tag List Packet to Tag
            //while(GIO1);
            while (RADIO->STATUS & (1 << 7));
            M8107RF_SendPacket(PAYLOAD_LEN, TagListBuf);

            //mResult = A7127RF_RecvPacket(PAYLOAD_LEN, TagListBuf, RPT_HANDLE_BIND_TIMER, ENABLE); //Receive Tag reply status
            mResult = M8107RF_RecvPacket(PAYLOAD_LEN, TagListBuf, RPT_HANDLE_BIND_TIMER, ENABLE); //Receive Tag reply status

            if (mResult == TAG_MSG_RX_COMPLETED)
            {
                //printf("ChgBCN %d\r\n", mResult);
                //A7127RF_StrobeCmd(CMD_STBY);
                //M8107RF_SendPacket(PAYLOAD_LEN, TagListBuf); //Transparent Tag status to Gateway
                A7127RF_SendPacket(PAYLOAD_LEN, TagListBuf); //Transparent Tag status to Gateway
            }

            if (PktType == RF_AL_TAG_UNBOUND)
            {
                if (mResult == TAG_MSG_RX_TIMEOUT)
                {
                    DBGMSG("Unbind Timeout...\r\n");
                }
                else if (mResult == TAG_MSG_RX_COMPLETED)
                {
                    DBGMSG("Unbind %x:%x\r\n", g_RptGlblVar.TagLstBuf[11], g_RptGlblVar.TagLstBuf[12]);
                }
            }
            else if (PktType == RF_AL_CHANGE_BEACON_SRC)
            {
                if (mResult == TAG_MSG_RX_TIMEOUT)
                {
                    DBGMSG("ChgBCN Timeout...\r\n");
                }
                else if (mResult == TAG_MSG_RX_COMPLETED)
                {
                    DBGMSG("ChgBCN %x:%x\r\n", g_RptGlblVar.TagLstBuf[11], g_RptGlblVar.TagLstBuf[12]);
                }
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
    uint32_t RstTmr1 = 0, TmpTimer = 0;
    P_SYNC_BEACON pSyncBeacon = (P_SYNC_BEACON)SyncBuf;

    if (g_RptGlblVar.gMode != RPT_PWRON_MODE)
    {
        // check receive gateway beacon reset timer
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


            case RF_AL_TAG_UNBOUND:
            case RF_AL_CHANGE_BEACON_SRC:

            case RF_AL_GET_TAG_STATUS:
            case RF_AL_GET_REPEATER_STATUS:

            case RF_AL_UPDATE_TAG:
            case RF_AL_UPDATE_REPEATER_TAG:
                if (pSyncBeacon->Action == RF_AL_UPDATE_REPEATER_TAG)
                {
                    pSyncBeacon->Action = RF_AL_UPDATE_TAG;
                }
                mResult = TAG_MSG_TO_RCV_TAG_LIST;
                break;

            case RF_AL_HOP_TO_REPEATER:
            case RF_AL_HOP_TO_TAG:
                mhopcount = (pSyncBeacon->ChnCtrl) & 0xF;

                //first time received hopping sync beacon
                if (g_RptGlblVar.ghopcount == 0)
                {
                    g_RptGlblVar.ghopcount = mhopcount;
                    g_RptGlblVar.gCHindex = ((pSyncBeacon->ChnCtrl) >> 4) & 0xF;
                }
                mResult = TAG_MSG_HOPPING;
                break;

            default:
                mResult = TAG_MSG_NO_HIT_ID;
                break;
        }

        g_RptGlblVar.RtnPwrModeCnt = 0;

        //***************************************************************
        //**************** A7127 RF Module Transparent ******************
        //***************************************************************
        memcpy(pSyncBeacon->RepeaterID, &RptDefSet.RptID[3], 2);

        // Encrypt Beacon packet
        if (g_RptGlblVar.cryptflag == 1)
        {
            g_RptGlblVar.cryptflag = 0; //clear crypt flag

            AES128_Encrypt(SyncBuf, AES_KEY, EncryptData);
            M8107RF_SendPacket(SYNC_LEN, EncryptData);
        }
        else
        {
            M8107RF_SendPacket(SYNC_LEN, SyncBuf);
        }
        //****************************************************************

    }
    else
    {
        ret = CheckZero(pSyncBeacon->RepeaterID, 2);
        if (ret == false)
        {
            DBGMSG("RptID non zero...%x:%x\r\n", pSyncBeacon->RepeaterID[0], pSyncBeacon->RepeaterID[1]);
            return TAG_MSG_RX_TIMEOUT;
        }

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

    /*for(i = 0; i < FifoLen; i++) {
        PktBuf[i] = RADIO->RX_DATA[i];
    }
    */

    /* Decrypt Beacon function */
    if (FifoLen == SYNC_LEN)
    {
        AES128_Decrypt(PktBuf, AES_KEY, DecryptData);

        if (DecryptData[1] == 0x01)
        {
            g_RptGlblVar.cryptflag = DecryptData[1];
            memcpy(PktBuf, DecryptData, FifoLen);
            //memcpy(g_RptGlblVar.SyncBuf , PktBuf, FifoLen);
        }
        else
        {
            //do nothing, because it is RAW data.
            //memcpy(g_RptGlblVar.SyncBuf , PktBuf, FifoLen);
        }
    }

    /* Processing packet types */
    switch (pRecvPktType->PacketType)
    {
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
            //setSynTimer(recv[4], updateflag);     //Sync time sec
            mhopcount = PktBuf[3] & 0xF;

            //first time received hopping sync beacon
            if (g_RptGlblVar.ghopcount == 0)
            {
                g_RptGlblVar.ghopcount = mhopcount;
                g_RptGlblVar.gCHindex = (PktBuf[3] >> 4) & 0xF;
            }
            //setSynTimer(recv[4], updateflag);     //Sync time sec
            mResult = TAG_MSG_HOPPING;
            break;

        case RF_PT_PACKET_HEADER:
        case RF_PT_DATA_PACKET:
            mResult = Rpt_RecvUpdData(pRecvPktType->PacketType, PktBuf);
            break;

        default:
#ifndef _NO_DBG_PRINTF
            DBGMSG("Rx dummy Pkt = ");
            for (i = 0; i < FifoLen; i++)
            {
                DBGMSG("%x ", PktBuf[i]);
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

    A7127_WriteReg(MODECTRL_REG, 0x63); //Enable ADCM for RSSI measurement

    A7127RF_SetFIFOLen(FifoLen);

    GIOSetRXMode();                     //Enable RX

    if (TimerState == ENABLE)
    {
        Oneshoutflag = 0;               //Time out flag
        SetTimer(TIMER0, mSec); //set get packet timeout timer
    }

    while (1)
    {
        if (WTRFlag == 0)  //Wait CMD_RX action, WTR = 1.
        {
            if (GIO2)
            {
                WTRFlag = 1; //if RF Rx ready toggle flag to Rx packet
            }
        }
        else
        {
            if ((!(GIO2)) || (Oneshoutflag))
            {
                if (TimerState == ENABLE)
                {
                    Timer_StopTimer(TIMER0);
                }
                A7127RF_StrobeCmd(CMD_STBY);

                if (!Oneshoutflag)
                {
                    A7127_ReadMultiReg(FIFO_REG, FifoLen, RecvBuf);
                    RxPktResult = Rpt_HandleRecvPktType(FifoLen, RecvBuf); //handle rx packet
                }
                else
                {
                    RxPktResult = TAG_MSG_RX_TIMEOUT;
                }
                break;
            }
        }
    }
    /*while((GIO2) && (Oneshoutflag == 0)); //WTR
    if(TimerState == ENABLE) {
        Timer_StopTimer(TIMER0);
    }
    A7127RF_StrobeCmd(CMD_STBY);

    if(Oneshoutflag) {
        RxPktResult = TAG_MSG_RX_TIMEOUT;
    }else {
        RxPktResult = Rpt_HandleRecvPktType(FifoLen, RecvBuf);
    }
    */
    return RxPktResult;
}

uint8_t Rpt_TransUpdTagPacket(uint32_t FifoLen)
{
    uint8_t RxPktResult = 0, i = 0, tUpdData = 0;
    uint8_t WTRFlag = 0;

    GIOSetRXMode();         //Enable RX

    while (1)
    {
        if (WTRFlag == 0)  //Wait CMD_RX action, WTR = 1.
        {
            if ((GIO2) && (RADIO->STATUS & (1 << 7)))
            {
                StrobeCmd(CMD_STBY);
                WTRFlag = 1; //if RF Rx ready toggle flag to Rx packet
            }
        }
        else
        {
            if ((!(GIO2)) || (Oneshoutflag))
            {

                if (!Oneshoutflag)
                {
                    __disable_irq();

                    L01_CSN_LOW( );

                    SPI_ExchangeByte(R_REGISTER | FIFO_REG);

                    for (i = 0; i < FifoLen; i++)
                    {
                        RADIO->TX_DATA[i] = tUpdData = SPI_ExchangeByte(0x00);

                        if (i == 0)
                        {
                            if ((tUpdData - g_RptGlblVar.mindex) > 1)
                            {
                                DBGMSG("Lost data = %x : %x\r\n", tUpdData, g_RptGlblVar.mindex);
                                RxPktResult = TAG_MSG_PACKET_LOST;
                            }
                            else
                            {
                                g_RptGlblVar.mindex = tUpdData;
                                RxPktResult = TAG_MSG_RX_COMPLETED;
                            }
                        }
                    }
                    L01_CSN_HIGH();

                    __enable_irq();

                    StrobeCmd(CMD_TX); //entry tx & transmit
                    //Delay1us(10);
                    //while(RADIO->STATUS & (1 << 7));
                }
                else
                {
                    RxPktResult = TAG_MSG_PACKET_LOST;
                }
                break;
            }
        }
    }
    /*while((GIO2) && (Oneshoutflag == 0) && (RADIO->STATUS & (1 << 7)));
    StrobeCmd(CMD_STBY);
    //A7127RF_StrobeCmd(CMD_STBY);

    if(Oneshoutflag){
        RxPktResult = TAG_MSG_PACKET_LOST;
    }else {
        __disable_irq();

        L01_CSN_LOW( );

        SPI_ExchangeByte(R_REGISTER | FIFO_REG);

        for(i = 0; i < FifoLen; i++) {
            RADIO->TX_DATA[i] = tUpdData = SPI_ExchangeByte(0x00);

            if(i == 0){
                if((tUpdData - g_RptGlblVar.mindex) > 1){
                    DBGMSG("Lost data = %x : %x\r\n", tUpdData, g_RptGlblVar.mindex);
                    RxPktResult = TAG_MSG_PACKET_LOST;
                }else{
                    g_RptGlblVar.mindex = tUpdData;
                    RxPktResult = TAG_MSG_RX_COMPLETED;
                }
            }
        }
        L01_CSN_HIGH();

        __enable_irq();

        StrobeCmd(CMD_TX); //entry tx & transmit
        Delay1us(10);
        //while(RADIO->STATUS & (1 << 7));
    }
    */
    return RxPktResult;
}

uint8_t Rpt_HandleRecvUpdTagData(int16_t mlen, uint8_t *RecvUpdBuf)
{
    uint8_t mReturn = 0;
    //uint16_t UpdTimeOut = 0;

    g_RptGlblVar.mindex = 0;            //clear sequence num
    Oneshoutflag = 0;   //Timer0 timeout flag

    M8107RF_SetFIFOLvl(PAYLOAD_LEN); //Set M8107 RF packet fifo length
    A7127RF_SetFIFOLen(PAYLOAD_LEN); //Set A7127 RF packet fifo length

    //UpdTimeOut = Rpt_CalcRecvUpd_Timeout(mlen); //Calculate when to receive updates
    //SetTimer(TIMER0, UpdTimeOut); //set receive update data timeout
    switch (RptDefSet.RptDataRate)
    {
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

    while (mlen > 0)
    {
        mReturn = Rpt_TransUpdTagPacket(PAYLOAD_LEN);

        if (Oneshoutflag)
        {
            mReturn = TAG_MSG_PACKET_LOST;
            break;
        }

        mlen -= (PAYLOAD_LEN - 2);  //two bytes header
    }
    Timer_StopTimer(TIMER0);
    StrobeCmd(CMD_STBY);

    if ((Oneshoutflag) || (mReturn == TAG_MSG_PACKET_LOST))     //Don't need update EPD because data error.
    {
        printf("DATA_STATE RX data timeout or lost.\r\n");
    }
    else
    {
        while (RADIO->STATUS & (1 << 7));
        mReturn = M8107RF_RecvPacket(PAYLOAD_LEN, RecvUpdBuf, RPT_TAG_REPLY_TIMER, ENABLE);
        //while(GIO1);  //wait A7127 transmit completed
        //mReturn = A7127RF_RecvPacket(PAYLOAD_LEN, RecvUpdBuf, RPT_TAG_REPLY_TIMER, ENABLE); // A7127 receive Tag result

        if (mReturn == TAG_MSG_RX_COMPLETED)
        {
            //A7127RF_StrobeCmd(CMD_STBY);
            //M8107RF_SendPacket(PAYLOAD_LEN, RecvUpdBuf); //transparent A7127 receive packet
            A7127RF_SendPacket(PAYLOAD_LEN, RecvUpdBuf); //transparent A7127 receive packet
        }
        DBGMSG("%d %d\r\n", g_RptGlblVar.GlblTempVar.tUpdCntTmp2, g_RptGlblVar.GlblTempVar.tUpdCntTmp1);
    }

    g_RptGlblVar.GlblTempVar.tUpdCntTmp2++;
    if (g_RptGlblVar.GlblTempVar.tUpdCntTmp2 < g_RptGlblVar.GlblTempVar.tUpdCntTmp1)
    {
        //g_RptGlblVar.gState = HEADER_STATE;
        mReturn = TAG_MSG_HIT_ID;
    }
    else
    {
        g_RptGlblVar.GlblTempVar.tUpdCntTmp2 = g_RptGlblVar.GlblTempVar.tUpdCntTmp1 = g_RptGlblVar.gState = 0;
        mReturn = TAG_MSG_NO_HIT_ID;
    }

    return mReturn;
}

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
            mReturn = Rpt_HandleRecvUpdTagData(g_RptGlblVar.RxTtlUpdDataLenBuf[g_RptGlblVar.GlblTempVar.tUpdCntTmp2], RxTaskBuf);
            break;

        default:
            mReturn = rpt_rfpacket_receive(g_RptGlblVar.RecvLen, RxTaskBuf, g_RptGlblVar.RecvTimer, ENABLE);
            break;
    }

    /* Handle return runtime message */
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

//**********************************************************************************
