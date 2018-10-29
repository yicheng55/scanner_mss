#include <math.h>
#include "main.h"

#include "RptProc.h"
#include "RFpacket.h"
#include "RfTxPacket.h"
#include "RptDataFlash.h"
#include "M8107RTC.h"
#include "RptComm.h"

extern uint8_t AES_KEY[];

static RPT_RUNTIME_PARAMETER RunTimePara =
{
    // uint8_t variable
    .RunTimeMode = RPT_PWRON_MODE,              //set repeater runtime mode
    .RxPktState = PWRBEACON_STATE,              //set repeater receive packet state
    .RxPktLen = SYNC_LEN,                                   //set rx mode receive packet length
    .SeqIndex = 0,
    .RtnPwrModeflag = 0,

    .KeepRxDataCnt = 0,                                     //record receive big data counter
    .TxDataCnt = 0,                                             //record transparent big data counter
    //.KeepSyncPkt = {0},                                       //Keep receive Beacon packet
    //.KeepTagListPkt = {0},                                //Keep receive TagList packet
    //.KeepBigDataHead = {0},                           //Keep receive big data head
    //.KeepBigData = {0},                                   //Keep receive big data

    //uint16_t variable
    .RxPktTime = RPT_PWRON_BEACON_TIMER,    //set rx mode receive packet timer
};

extern void RstTimerValue(TIMER_Type *timer, uint32_t msec);
extern uint8_t Rpt_HandleBindGateway(uint8_t *RptRunTimeMode, P_SYNC_BEACON pSyncBeacon);

/**
  * @brief      Repeater main process 2.
  * @param[in]  None
  * @return     None
  * @details    This API is used perform repeater actions.
  */
void Rpt_Process(void)
{
    uint8_t RxResult = 0;
    uint8_t PktBuf[64] = {0};
    P_RPT_RUNTIME_PARAMETER pRunTimePara = &RunTimePara;

    switch (pRunTimePara->RunTimeMode)
    {
        case RPT_PWRON_MODE:
            while (GetTimer1IntISR() == RESET);
            SetTimer1IntISR(RESET);
            // 500ms send fake Sync Beacon to tag
            Rpt_SendFakeSyncBeacon(pRunTimePara, &pRunTimePara->KeepUpLayerID[3], PktBuf);

            RxResult = M8107RF_RecvPacket(pRunTimePara->RxPktLen, PktBuf, pRunTimePara->RxPktTime, ENABLE);
            if (RxResult == TAG_MSG_RX_COMPLETED)
            {
                //Decrypt sync packet
                AES128_Decrypt(PktBuf, AES_KEY, DecryptData);
                if (DecryptData[1] == 0x01)
                {
                    //*(P_MemCpyNode)PktBuf = *(P_MemCpyNode)DecryptData;
                    memcpy(PktBuf, DecryptData, pRunTimePara->RxPktLen);
                }
                Rpt_HandleBindGateway(&pRunTimePara->RunTimeMode, (P_SYNC_BEACON)PktBuf);
            }
            break;

        case RPT_TASK_MODE:
            while (GetTimer1IntISR() == RESET);
            SetTimer1IntISR(RESET);

            //Repeater RX Task process
            do
            {
                pRunTimePara = Rpt_RxModeTask(pRunTimePara, PktBuf);
            }
            while (TASK_CUR_RUNTIME(TIMER1) > 5);

            //Change RF Channel of Tag
            M8107RF_SetCHAN(RptDefSet.Tchn);

            while (GetTimer1IntISR() == RESET);
            SetTimer1IntISR(RESET);

            //Repeater TX Task process
            Rpt_TxModeTask(pRunTimePara, PktBuf);

            //Tx task end, change RF channel of gateway
            M8107RF_SetCHAN(RptDefSet.Gchn);

            memset((*pRunTimePara).KeepRxPktStrc.SyncPkt, 0, SYNC_LEN);
            //memset(&(*pRunTimePara).KeepRxPktStrc, 0, sizeof((*pRunTimePara).KeepRxPktStrc));
            break;
    }

    /* Return Task mode or power on mode */
    pRunTimePara = Rpt_ResetRxTaskState(pRunTimePara);
}

