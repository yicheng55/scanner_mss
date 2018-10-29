#include "main.h"

#include "M8107RTC.h"

#include "RptProc.h"
#include "RFpacket.h"
#include "RptComm.h"
#include "RptDataFlash.h"

/* Jerry add */
RPT_NVRAM_BASE_PARAMETER RptDefSet;

extern const uint8_t CHTable_NonWifiRang[];

/*
P_RPT_GLOBAL_VARIABLE ModuleName_SetParam(P_RPT_GLOBAL_VARIABLE gParam)
{
  //g_RptGlblVar.gMode = gParam->gMode;

  return gParam;
}

P_RPT_GLOBAL_VARIABLE ModuleName_GetParam(P_RPT_GLOBAL_VARIABLE gParam)
{
  //g_RptGlblVar.gMode = gParam->gMode;

  return gParam;
}
*/

/********** Handle Timer1 Interrupt ISR ***********/
static volatile uint8_t Tmr1OneShoutFlag = 0;

void SetTimer1IntISR(uint8_t tigger)
{
    Tmr1OneShoutFlag = tigger;
}

uint8_t GetTimer1IntISR()
{
    return Tmr1OneShoutFlag;
}

//******************************************************

#ifdef USE_A7127RF_MODULE
#include "A7127RF_Drv.h"

/**
  * @brief      Repeater second RF A7127 transmite packet.
  * @param[in]  None
  * @return     None
  * @details    This API is used A7127 RF transmit packet.
  */
void A7127RF_SendPacket(uint32_t FifoLen, uint8_t *Buffer)
{
    A7127RF_SetFIFOLen(FifoLen);

    A7127_WriteMultiReg(FIFO_REG, Buffer, FifoLen);

    GIOSetTXMode();
    Delay1us(10);

#if defined (A7127RF_MAIN_MODE) && defined (REPEATER_TRANSPARENT_MODE)
    while (GIO1); //wait transmit completed
#elif (defined (M8107RF_MAIN_MODE) || defined (A7127RF_MAIN_MODE)) && defined (REPEATER_500MS_MODE)
    while (GIO1); //wait transmit completed
#elif defined (M8107RF_MAIN_MODE) && defined (REPEATER_TRANSPARENT_MODE)

#endif
}

/**
  * @brief      Repeater A7127 RF receive packet.
  * @param[in]  None
  * @return     None
  * @details    This API is used A7127 RF receive packet.
  */
uint8_t A7127RF_RecvPacket(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t msec, FunctionalState RecvTimerState)
{
    uint8_t mResult = 0;
    uint8_t WTRFlag = 0;

    /* Enable ADCM for RSSI measurement */
    A7127_WriteReg(MODECTRL_REG, 0x63);

    A7127RF_SetFIFOLen(FifoLen);

    GIOSetRXMode();     //Enable RX
    Delay1us(10);

    /** Received Slave return packet and print out total packet **/
    if (RecvTimerState == ENABLE)
    {
        Oneshoutflag = 0;
        SetTimer(TIMER0, msec);
    }

    while (1)
    {
        if (WTRFlag == 0)  //Wait CMD_RX action, WTR = 1.
        {
            if (GIO2)
            {
                WTRFlag = 1; //if RF Rx ready toggle flag to Rx packet
            }
            else if (Oneshoutflag)
            {
                return TAG_MSG_RX_TIMEOUT;
            }
        }
        else
        {
            if ((!(GIO2)) || (Oneshoutflag))
            {
                if (RecvTimerState == ENABLE)
                {
                    Timer_StopTimer(TIMER0);
                }
                A7127RF_StrobeCmd(CMD_STBY);

                if (!Oneshoutflag)
                {
                    A7127_ReadMultiReg(FIFO_REG, FifoLen, RecvBuf);
                    mResult = TAG_MSG_RX_COMPLETED;
                }
                else
                {
                    mResult = TAG_MSG_RX_TIMEOUT; // Receive Tag update Result fake, create timeout packet to Gateway
                }
                break;
            }
        }
    }

    return mResult;
}
#endif //USE_A7127RF_MODULE

/**
  * @brief      change RF channel
  * @param[in]  Channel is wants to change the value of the RF channel
  * @return     None
  * @details    This API is used change RF channel.
  */
void M8107RF_SetCHAN(uint8_t Channel)
{
    StrobeCmd(CMD_STBY);
    SetCH(Channel);
    Delay10us(1);
}

/**
  * @brief      Repeater transfer packet process
  * @param[out] None
  * @return     None
  * @details    This API is used transfer packet.
  */
void M8107RF_SendPacket(uint32_t FifoLen, uint8_t *buffer)
{
    uint8_t i = 0;

    M8107RF_SetFIFOLvl(FifoLen);

    for (i = 0; i < FifoLen; i++)
    {
        RADIO->TX_DATA[i] = buffer[i];
        //printf("%x ", buffer[i]);
    }
    //printf("\r\n");

    StrobeCmd(CMD_TX); //entry tx & transmit
    Delay1us(10);

#if defined (A7127RF_MAIN_MODE) && defined (REPEATER_TRANSPARENT_MODE)

#elif (defined (M8107RF_MAIN_MODE) || defined (A7127RF_MAIN_MODE)) && defined (REPEATER_500MS_MODE)
    while (RADIO->STATUS & (1 << 7));   //WTR
#elif defined (M8107RF_MAIN_MODE) && defined (REPEATER_TRANSPARENT_MODE)
    while (RADIO->STATUS & (1 << 7));   //WTR
#endif
}

/**
  * @brief      Repeater receive Tag update result & Tag status
  * @param[out] None
  * @return     None
  * @details    This API is used receive Tag update result & Tag status.
  */
uint8_t M8107RF_RecvPacket(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t msec, FunctionalState RecvTimerState)
{
    uint8_t i = 0, mResult = 0;
    uint8_t WTRFlag = 0;

    M8107RF_SetFIFOLvl(FifoLen);

    /** Received Slave return packet and print out total packet **/
    StrobeCmd(CMD_RX); //entry rx & receive
    Delay1us(10);

    if (RecvTimerState == ENABLE)
    {
        Oneshoutflag = 0;
        SetTimer(TIMER0, msec);
    }

    while (1)
    {
        if (WTRFlag == 0)  //Wait CMD_RX action, WTR = 1.
        {
            if ((RADIO->STATUS & (1 << 7)))
            {
                WTRFlag = 1; //if RF Rx ready toggle flag to Rx packet
            }
            else if (Oneshoutflag)
            {
                return TAG_MSG_RX_TIMEOUT;
            }
        }
        else
        {
            if ((!(RADIO->STATUS & (1 << 7))) || (Oneshoutflag))
            {
                if (RecvTimerState == ENABLE)
                {
                    Timer_StopTimer(TIMER0);
                }
                StrobeCmd(CMD_STBY); //Back to Standby

                if (!Oneshoutflag)
                {
                    for (i = 0; i < FifoLen; i++)
                    {
                        RecvBuf[i] = RADIO->RX_DATA[i];
                    }
                    //*(P_MemCpyNode)RecvBuf = *(P_MemCpyNode)RADIO->RX_DATA;

                    mResult = TAG_MSG_RX_COMPLETED;
                }
                else
                {
                    mResult = TAG_MSG_RX_TIMEOUT;
                }
                break;
            }
        }
    }

    return mResult;
}

/**
  * @brief      negative number turn  positive number
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used negative number turn  positive number.
  */
int n_turn_p(int num)
{
    if (num < 0)
    {
        num = ~(num - 1);
    }
    else if (num > 0)
    {
        num = ~(num + 1);
    }

    return num;
}

/**
  * @brief      Compare packet data
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used array data is the same of Agent default Information.
  */
bool CompareData(uint8_t *src_array, uint8_t *dest_array, unsigned int array_size)
{
    uint8_t i;

    if (dest_array != NULL)
    {
        for (i = 0; i < array_size; i++)
        {
            if (src_array[i] != dest_array[i])
            {
                return false;
            }
        }
    }

    return true;
}

/**
  * @brief      Check the Array data is zero
  * @param[out] ProtocolHeader
  * @return     true or false
  * @details    This API is used array data is zero, if zero return true.
  */
bool CheckZero(uint8_t *array, unsigned int array_size)
{
    uint8_t i;

    if (array != NULL)
    {
        for (i = 0; i < array_size; i++)
        {
            if (array[i] != 0)
            {
                return false;
            }
        }
    }

    return true;
}

/**
  * @brief      change timer value.
    * @param[in]  timer is the base address of TIMER module.
  * @param[in]  msec is the change current use timer value.
  * @return     None
  * @details    This API is used reset timer value.
  */
void RstTimerValue(TIMER_Type *timer, uint32_t msec)
{
    Timer_StopTimer(TIMER1);
    timer->RELOAD = ((SystemCoreClock / 1000) - 3) * msec;
    timer->VALUE = ((SystemCoreClock / 1000) - 3) * msec;
    Timer_StartTimer(TIMER1);
}

/**
  * @brief      Settup TIMER1.
  * @param[in]  msec is the set use timer value.
  * @return     None
  * @details    This API is used settup timer1.
  */
void SetOneShotTimer1(uint32_t msec)
{
    NVIC_DisableIRQ(TIMER1_IRQn); /* Disable Timer Interrupt */

    SetTimer1IntISR(RESET);

    TIMER1->CTRL = (0 << 3) | (0 << 0); //timer enable & interrupt

    TIMER1->INTCLEAR = 1; //clear timer interrupt
    TIMER1->RELOAD = ((SystemCoreClock / 1000) - 3) * msec;
    TIMER1->VALUE = ((SystemCoreClock / 1000) - 3) * msec; //1ms for Xtal = 16MHz

    TIMER1->CTRL = (1 << 3) | (1 << 0); //timer enable & interrupt

    NVIC_EnableIRQ(TIMER1_IRQn);  /* Enable Timer Interrupt */
}

void Rpt_DefaultSetting(void)
{
    RPT_NVRAM_BASE_PARAMETER _RptDefSet;

    _RptDefSet.Gchn = GW_CHN;
    _RptDefSet.Tchn = TAG_CHN;
    _RptDefSet.RptDataRate = RF_DATARATE_500K;
    _RptDefSet.RptBatLv = 100;

    _RptDefSet.RptVerMJR = RPT_VERSION_MAJOR;
    _RptDefSet.RptVerMINR = RPT_VERSION_MINOR;
    _RptDefSet.RptVerRLSE = RPT_VERSION_RELEASE;
    _RptDefSet.RptVerBLD = RPT_VERSION_BUILD;

    //_RptDefSet.RptReSync = 0;
    //_RptDefSet.StartSec = GetStandbyTimeInSecond();
    _RptDefSet.RptHD_Spec = RPT_ONERF_ONE_CHN_MODE;

    _RptDefSet.RptID[0] = RPT_ID1;
    _RptDefSet.RptID[1] = RPT_ID2;
    _RptDefSet.RptID[2] = RPT_ID3;
    _RptDefSet.RptID[3] = RPT_ID4;
    _RptDefSet.RptID[4] = RPT_ID5;
#ifdef TEST_RF_CHANNEL
    _RptDefSet.ChanCnt = 0;
#endif //

    memcpy(&RptDefSet, &_RptDefSet, sizeof(RPT_NVRAM_BASE_PARAMETER));
}

void Repeater_LED_Ctrl(uint8_t State)
{
    switch (State)
    {
        case LED_TOTAL_TURN_ON:
            YELLOW_LED_HIGH();
            GREEN_LED_HIGH();
            break;
        case LED_TOTAL_TURN_OFF:
            YELLOW_LED_LOW();
            GREEN_LED_LOW();
            break;
        case TRIGGER_FOR_FREEBEACON:
            YELLOW_LED_HIGH();
            break;
        case TRIGGER_FOR_UPD_TAG:
            GREEN_LED_HIGH();
            break;
    }
}

uint8_t ScanButtonAction(void)
{
    uint8_t mflag = 0;
    uint32_t tCNT = 0;

    Repeater_LED_Ctrl(LED_TOTAL_TURN_OFF);

    if (!USE_BUTTON_INTERRUPT)
    {
        Oneshoutflag = 1;       //Timer0 timeout flag

        while (!USE_BUTTON_INTERRUPT)
        {
            if (Oneshoutflag == 1)
            {
                Oneshoutflag = 0;
                SetTimer(TIMER0, 999);
                tCNT++;
                printf("tCNT = %d\r\n", tCNT);
            }

            if ((tCNT > 3) && (tCNT < 10))
            {
                YELLOW_LED_HIGH();
            }
            else if (tCNT >= 10)
            {
                GREEN_LED_HIGH();
                Delay1ms(500);
            }

            if ((tCNT >= 3) && (tCNT < 10) && (USE_BUTTON_INTERRUPT))
            {
                printf("Software reset repeater...\r\n");
                Delay1ms(10);

                __disable_irq();
                NVIC_SystemReset();
            }
            else   if (tCNT >= 10)
            {
                printf("Restore NVRAM..\r\n");
                mflag = Rpt_Read_DataFlash();
                if (mflag == 0)
                {
                    RptDefSet.Gchn = GW_CHN;
                    RptDefSet.Tchn = TAG_CHN;//CHTable_NonWifiRang[RptDefSet.ChanCnt];
                    RptDefSet.RptDataRate = RF_DATARATE_500K;

                    Rpt_Write_DataFlash(); //write default parameter to dataflash
                    Delay1ms(10);

                    __disable_irq();
                    NVIC_SystemReset();
                }
            }
        }
        Timer_StopTimer(TIMER0);

        return KEY_OFF;
    }
    else
    {
        return KEY_OFF;
    }
}

void Rpt_SendFakeSyncBeacon(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *GwID, uint8_t *TxPktBuf)
{
    P_SYNC_BEACON pTxSyncBeacon = (P_SYNC_BEACON)TxPktBuf;

#if defined (M8107RF_MAIN_MODE) && defined (REPEATER_500MS_MODE) && defined (M8107RF_ONLY_ONE)
    M8107RF_SetCHAN(RptDefSet.Tchn);
#elif defined (M8107RF_MAIN_MODE) && defined (REPEATER_TRANSPARENT_MODE) && defined (USE_A7127RF_MODULE)

#endif //

    /* Repeater receive beacon fake, create beacon send to tag */
    pTxSyncBeacon->BeaconSeq = (*mRunTimePara).SeqIndex++;
    pTxSyncBeacon->BeaconType = RF_PT_SYNC_BEACON;
    pTxSyncBeacon->Action = RF_AL_SYNC;
    pTxSyncBeacon->ChnCtrl = 0;
    pTxSyncBeacon->WakeUpTime = 1;
    memcpy(pTxSyncBeacon->GatewayID, GwID, 2);
    memcpy(pTxSyncBeacon->RepeaterID, &RptDefSet.RptID[3], 2);

#if defined (M8107RF_MAIN_MODE) && defined (REPEATER_500MS_MODE) && defined (M8107RF_ONLY_ONE)
    M8107RF_SendPacket(SYNC_LEN, TxPktBuf);
    M8107RF_SetCHAN(RptDefSet.Gchn);
#elif defined (M8107RF_MAIN_MODE) && defined (REPEATER_TRANSPARENT_MODE) && defined (USE_A7127RF_MODULE)
    A7127RF_SendPacket(SYNC_LEN, TxPktBuf);
#endif //
}

/**
  * @brief      set state and mode of return rx task.
  * @param[in]  None
  * @return     None
  * @details    This API is used return rx task state and mode.
  */
P_RPT_RUNTIME_PARAMETER Rpt_ResetRxTaskState(P_RPT_RUNTIME_PARAMETER mRunTimePara)
{
    static uint8_t CntRtnPwrMode = 0;

    /* Scan Button Action */
    while ((ScanButtonAction()) == KEY_ON);

    switch (mRunTimePara->RunTimeMode)
    {
        case RPT_PWRON_MODE:
            break;

        case RPT_TASK_MODE:
            if (mRunTimePara->RtnPwrModeflag == 1)
            {
                mRunTimePara->RtnPwrModeflag = 0; //Bug here
                if (CntRtnPwrMode++ >= 5)
                {
                    CntRtnPwrMode = 0;
                    Rpt_GlobalVar_Init(); //return repeater variable init

                    mRunTimePara->RunTimeMode = RPT_PWRON_MODE;
                }
                mRunTimePara->RxPktState = PWRBEACON_STATE;
                mRunTimePara->RxPktTime = RPT_PWRON_BEACON_TIMER;
            }
            else
            {
                mRunTimePara->RtnPwrModeflag = 0;
                CntRtnPwrMode = 0;

                mRunTimePara->RxPktState = BEACON_STATE; //BEACON_STATE; //PWRBEACON_STATE;
                mRunTimePara->RxPktTime = RPT_BEACON_TIMER;
            }

            mRunTimePara->RxPktLen = SYNC_LEN;
            //printTime(); //show M8107 RTC Time

            if (TASK_CUR_RUNTIME(TIMER1) > RPT_TASK_TIMER_VAL)
            {
                printf("Timer1 = %d\r\n", TASK_CUR_RUNTIME(TIMER1));
                RstTimerValue(TIMER1, RPT_TASK_TIMER_VAL);
            }
            break;
    }

    return mRunTimePara;
}

uint8_t Rpt_HandleBindGateway(P_RPT_RUNTIME_PARAMETER mRunTimePara, P_SYNC_BEACON pSyncBeacon)
{
    uint8_t mResult = 0;
    static uint8_t BindGwFlag = 0;
    static uint8_t RxSyncCnt = 0;
    static uint32_t LastRxRssi = 0;
    uint32_t iRssi = 0;

    if (!CheckZero(pSyncBeacon->RepeaterID, 2))
    {
        return TAG_MSG_RX_TIMEOUT;
    }

    if (BindGwFlag == 1)
    {
#if defined (REPEATER_500MS_MODE)
        Delay1ms(RPT_TASK_TIMER_VAL);
#elif defined (REPEATER_TRANSPARENT_MODE)
        //Delay1ms(RPT_TASK_STOP_EARLY + 1);
#endif //

        SetTimer1IntISR(RESET);
        SetOneShotTimer1(RPT_TASK_TIMER_VAL - (RPT_TASK_STOP_EARLY - 1)); //set Timer1 timer

        (*mRunTimePara).RunTimeMode = RPT_TASK_MODE;

        memcpy(&(*mRunTimePara).KeepUpLayerID[3], pSyncBeacon->GatewayID, 2);
        printf("Rpt Beacon Sync Bind ID = %02X:%02X\r\n", mRunTimePara->KeepUpLayerID[0], mRunTimePara->KeepUpLayerID[1]);

        BindGwFlag = RxSyncCnt = 0; //binding up layer device, clear variable for next use

        mResult = TAG_MSG_NO_HIT_ID;
    }
    else
    {
        // bind upper layer transfer ID
        if (CheckZero(mRunTimePara->KeepUpLayerID, 2))
        {
            memcpy(mRunTimePara->KeepUpLayerID, pSyncBeacon->GatewayID, 2);
        }
        else
        {

#if defined (M8107RF_MAIN_MODE)
            iRssi = RSSI_measurement();
#elif defined (A7127RF_MAIN_MODE)
            iRssi = A7127_ReadReg(RSSI_REG);
#endif
            printf("GwID = %d, RptID = %d, RSSI = %d\r\n", pSyncBeacon->GatewayID[1], pSyncBeacon->RepeaterID[1], iRssi);
            if (iRssi > COMPARE_RSSI_VALUE)
            {
                if (CompareData(mRunTimePara->KeepUpLayerID, pSyncBeacon->GatewayID, 2))
                {
                    LastRxRssi = iRssi;
                    RxSyncCnt++;
                    if (RxSyncCnt >= 1)
                    {
                        BindGwFlag = 1;
                    }
                }
                else
                {
                    if (iRssi > LastRxRssi)
                    {
                        memcpy(mRunTimePara->KeepUpLayerID, pSyncBeacon->GatewayID, 2);
                    }
                }
            }
        }
    }

    return mResult;
}

#if 0
/**
  * @brief      Calculate Repeater receive update data timeout.
    * @param[in]  UpdLen is the receive packet length.
  * @return     TotalTiemr is the update data timeout.
  * @details    This API is used calculate update data timeout.
  */
uint16_t Rpt_CalcRecvUpd_Timeout(int16_t UpdLen)
{
    uint16_t TotalTimer = 0;

    //The remainder is greater than 0
    if ((UpdLen % PAYLOAD_LEN) != 0)
    {
        UpdLen = ((UpdLen / PAYLOAD_LEN) + 1);
    }
    else
    {
        UpdLen = (UpdLen / PAYLOAD_LEN);
    }

    //Calculate receive update data Timeout
    switch (RptDefSet.RptDataRate)
    {
        case RF_DATARATE_1M:
            TotalTimer = (UpdLen * RPT_1M_RX_ONEPKT_TIMER);
            break;
        case RF_DATARATE_500K:
            TotalTimer = (UpdLen * RPT_500K_RX_ONEPKT_TIMER);
            break;
        case RF_DATARATE_250K:
            TotalTimer = (UpdLen * RPT_250K_RX_ONEPKT_TIMER);
            break;
    }

    return TotalTimer;
}
#endif //0

void Rpt_GlobalVar_Init(void)
{
    uint8_t mflag = 0, i = 0;

    //Read DataFlash parameter
    //mflag = Rpt_Read_DataFlash();
    //if(mflag != 0)
    {
        Rpt_DefaultSetting(); //DataFlash zero, read repeater default setup
        Rpt_Write_DataFlash(); //write default parameter to dataflash
    }
#ifdef TEST_RF_CHANNEL
    else
    {
        RptDefSet.ChanCnt++;
        RptDefSet.Tchn = CHTable_NonWifiRang[RptDefSet.ChanCnt];
        Rpt_Write_DataFlash(); //write default parameter to dataflash

        Rpt_Read_DataFlash();
    }
#endif // TEST_RF_CHANNEL

    gDatarate = RptDefSet.RptDataRate; //Set RF Datarate

#ifdef USE_A7127RF_MODULE
    A7127RF_Enable(); //Eanble A7127 RF
#endif //USE_A7127RF_MODULE

    /* Set RF Channel */
#if defined (M8107RF_MAIN_MODE) && defined (REPEATER_500MS_MODE) && defined (M8107RF_ONLY_ONE)
    M8107RF_SetCHAN(RptDefSet.Gchn);
#elif defined (M8107RF_MAIN_MODE) && defined (REPEATER_500MS_MODE) && !defined (M8107RF_ONLY_ONE)
    M8107RF_SetCHAN(RptDefSet.Gchn);
    A7127RF_SetCHAN(RptDefSet.Tchn);
#elif defined (M8107RF_MAIN_MODE) && defined (REPEATER_TRANSPARENT_MODE) && defined (USE_A7127RF_MODULE)
    M8107RF_SetCHAN(RptDefSet.Gchn);
    A7127RF_SetCHAN(RptDefSet.Tchn);
#elif defined (A7127RF_MAIN_MODE) && defined (REPEATER_500MS_MODE) && defined (A7127RF_ONLY_ONE)
    A7127RF_SetCHAN(RptDefSet.Gchn);
    StrobeCmd(CMD_STBY);
#elif defined (A7127RF_MAIN_MODE) && defined (REPEATER_500MS_MODE) && !defined (A7127RF_ONLY_ONE)
    M8107RF_SetCHAN(RptDefSet.Tchn);
    A7127RF_SetCHAN(RptDefSet.Gchn);
#elif defined (A7127RF_MAIN_MODE) && defined (REPEATER_TRANSPARENT_MODE)
    M8107RF_SetCHAN(RptDefSet.Tchn);
    A7127RF_SetCHAN(RptDefSet.Gchn);
#endif

    printf("Rpt ID = ");
    for (i = 0; i < 5; i++)
    {
        DBGMSG("%x ", RptDefSet.RptID[i]);
    }
    //printf(", Ver : %x.%x.%x.%x, Date : %d\r\n",
    DBGMSG(", Ver : %x.%x.%x.%x, Date : %s\r\n",
           RptDefSet.RptVerMJR, RptDefSet.RptVerMINR,
           RptDefSet.RptVerRLSE, RptDefSet.RptVerBLD,
           /*RPT_BUILD_PROJECT_DATE*/__DATE__);

    switch (RptDefSet.RptDataRate)
    {
        case RF_DATARATE_1M:
            printf("RF DataRate 1M, ");
            break;
        case RF_DATARATE_500K:
            printf("RF DataRate 500K, ");
            break;
        case RF_DATARATE_250K:
            printf("RF DataRate 250K, ");
            break;
    }
    printf("Gw CHN = %d, Tag CHN = %d\r\n", RptDefSet.Gchn, RptDefSet.Tchn);

    SetOneShotTimer1(RPT_TASK_TIMER_VAL);

    Repeater_LED_Ctrl(LED_TOTAL_TURN_OFF);
}
