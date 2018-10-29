#include <math.h>
#include "main.h"

#include "RptProc.h"
#include "RFpacket.h"
#include "RfTxPacket.h"
#include "RptDataFlash.h"
#include "M8107RTC.h"
#include "RptComm.h"
#include "A7127RF_Drv.h"

extern void RstTimerValue(TIMER_Type *timer, uint32_t msec);

void Rpt_SendFakeSyncBeacon(void)
{
    P_SYNC_BEACON pTxSyncBeacon = (P_SYNC_BEACON)g_RptGlblVar.SyncBuf;

    // Repeater receive beacon fake, create beacon send to tag
    pTxSyncBeacon->BeaconSeq = g_RptGlblVar.gTxSeqNo++;
    pTxSyncBeacon->BeaconType = RF_PT_SYNC_BEACON;
    pTxSyncBeacon->Action = RF_AL_SYNC;
    pTxSyncBeacon->ChnCtrl = 0;
    pTxSyncBeacon->WakeUpTime = 1;
    memcpy(pTxSyncBeacon->GatewayID, &g_RptGlblVar.GatewayID[0], 2);
    memcpy(pTxSyncBeacon->RepeaterID, &RptDefSet.RptID[3], 2);

    //M8107RF_SendPacket(SYNC_LEN, g_RptGlblVar.SyncBuf);
    A7127RF_SendPacket(SYNC_LEN, g_RptGlblVar.SyncBuf);
}

void Rpt_OneRF_OneCHAN_Task(void)
{
    uint8_t DelayBeacon = RptDefSet.RptID[4];
    uint16_t Tmr1Cnt = 0;

    //********************************************************************
    //*************************** Rpt RX Mode ****************************
    //********************************************************************
    while (g_RptGlblVar.Tmr1OneShoutFlag != 1);
    g_RptGlblVar.Tmr1OneShoutFlag = 0;

    do
    {
        Rpt_RxModeTask(g_RptGlblVar.gState); //receive Gateway RF packet
        //}while(g_RptGlblVar.Tmr1OneShoutFlag != 1);
    }
    while (TASK_CUR_RUNTIME(TIMER1) > 5);
    //********************************************************************

    //********************************************************************
    //*************************** Rpt TX Mode ****************************
    //********************************************************************
    while (g_RptGlblVar.Tmr1OneShoutFlag != 1);
    g_RptGlblVar.Tmr1OneShoutFlag = 0; //clear Timer1 interrupt flag

    Tmr1Cnt = ((RPT_TASK_TIMER_VAL - RPT_TASK_STOP_EARLY - 1) -
               abs(RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1)));
    RstTimerValue(TIMER1, Tmr1Cnt); // Recount Timer1 of Tx mode

    /* Confirm completion of receiving updates, trigger flag */
    if (g_RptGlblVar.GlblTempVar.tUpdCntTmp1 != 0)
    {
        g_RptGlblVar.RxUpdDataDone = 1;
    }

    //clear temporarily variable
    memset(&g_RptGlblVar.GlblTempVar, 0, sizeof(RPT_GLBLTEMP_VARIABLE));

    //Change RF Channel of Tag
    //M8107RF_SetCHAN(RptDefSet.Tchn);
    A7127RF_SetCHAN(RptDefSet.Tchn);

    //Delay send beacon, 1ms + (RepeaterID * 10 us)
    Delay1ms(1);
    Delay10us(DelayBeacon * 2);

    //Repeater Tx Task process
    Rpt_TxModeTask();

    //Tx task end, change RF channel of gateway
    //M8107RF_SetCHAN(RptDefSet.Gchn);
    A7127RF_SetCHAN(RptDefSet.Gchn);

    //clear temporarily variable
    memset(&g_RptGlblVar.GlblTempVar, 0, sizeof(RPT_GLBLTEMP_VARIABLE));

    /* Return Task mode or power on mode */
    Rpt_ResetRxTaskState(g_RptGlblVar.RtnPwrOnMode);

    //********************************************************************
}

/**
  * @brief      Repeater main process 2.
  * @param[in]  None
  * @return     None
  * @details    This API is used perform repeater actions.
  */
void Rpt_Process(void)
{
    switch (g_RptGlblVar.gMode)
    {
        case RPT_PWRON_MODE:
            do
            {
                Rpt_RxModeTask(g_RptGlblVar.gState);

                // 500ms send fake Sync Beacon to tag
                if (g_RptGlblVar.Tmr1OneShoutFlag == 1)
                {
                    g_RptGlblVar.Tmr1OneShoutFlag = 0;

                    Rpt_SendFakeSyncBeacon();
                }

            }
            while (g_RptGlblVar.PwrOnSyncBeacon != 1);
            g_RptGlblVar.PwrOnSyncBeacon = 0; //clear power on beacon sync flag
            break;

        case RPT_TASK_MODE:
            Rpt_OneRF_OneCHAN_Task();
            break;
    }
}

#if 0
void Rpt_GlobalVar_Init(void)
{
    uint8_t mflag = 0, i = 0;

    //Read DataFlash parameter
    mflag = Rpt_Read_DataFlash();
    if (mflag != 0)
    {
        Rpt_DefaultSetting(); //DataFlash zero, read repeater default setup
        Rpt_Write_DataFlash(); //write default parameter to dataflash
    }

    gDatarate = RptDefSet.RptDataRate; //RF data rate
    //M8107RF_SetCHAN(RptDefSet.Gchn); //RF channel
    A7127RF_Enable(); //Eanble A7127 RF

    A7127RF_SetCHAN(RptDefSet.Gchn); //RF channel

    printf("Rpt ID = ");
    for (i = 0; i < 5; i++)
    {
        DBGMSG("%x ", RptDefSet.RptID[i]);
    }
    printf(", Ver : %x.%x.%x.%x, Date : %d\r\n",
           RptDefSet.RptVerMJR, RptDefSet.RptVerMINR,
           RptDefSet.RptVerRLSE, RptDefSet.RptVerBLD,
           RPT_BUILD_PROJECT_DATE);

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

    // init runtime value
    memset(&g_RptGlblVar, 0, sizeof (RPT_GLOBAL_VARIABLE));

    g_RptGlblVar.gMode = RPT_PWRON_MODE;//RPT_RX_MODE;//RptPowerOnMode;
    g_RptGlblVar.gState = PWRBEACON_STATE;//BEACON_STATE;

    g_RptGlblVar.RecvLen = SYNC_LEN;
    g_RptGlblVar.RecvTimer = RPT_PWRON_BEACON_TIMER;

    SetOneShotTimer1(RPT_TASK_TIMER_VAL + RPT_TASK_STOP_EARLY);
}
#endif //0
