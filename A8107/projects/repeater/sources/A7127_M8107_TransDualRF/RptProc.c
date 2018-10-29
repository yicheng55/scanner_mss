#include <math.h>
#include "main.h"

#include "RptProc.h"
#include "RFpacket.h"
#include "RfTxPacket.h"
#include "RptDataFlash.h"
#include "M8107RTC.h"
#include "RptComm.h"
#include "A7127RF_Drv.h"

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
            }
            while (g_RptGlblVar.PwrOnSyncBeacon != 1);

            g_RptGlblVar.PwrOnSyncBeacon = 0;
            break;

        case RPT_TASK_MODE:
            while (g_RptGlblVar.Tmr1OneShoutFlag != 1);
            g_RptGlblVar.Tmr1OneShoutFlag = 0;

            do
            {
                Rpt_RxModeTask(g_RptGlblVar.gState);
            }
            while (TASK_CUR_RUNTIME(TIMER1) > 5);

            Rpt_ResetRxTaskState(g_RptGlblVar.RtnPwrOnMode);

            //A7127RF_StrobeCmd(CMD_STBY);
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

    A7127RF_Enable(); //Eanble A7127 RF

    M8107RF_SetCHAN(RptDefSet.Tchn); //RF channel
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
