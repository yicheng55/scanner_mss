#include "main.h"
#include "RptSCNR.h"
#include "RptProc.h"
#include "RptComm.h"

void Rpt_TransScnrPacket(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t mSec)
{
    uint8_t RecvResult = 0;
    P_SCAN_PACKET_HEAD pScanPktHead = (P_SCAN_PACKET_HEAD)RecvBuf;

    memset(RecvBuf, 0, FifoLen);
    RecvResult = M8107RF_RecvPacket(FifoLen, RecvBuf, mSec, ENABLE);

    if (pScanPktHead->byPacketType != RF_PT_SCANNER_PACKET)
    {
        return ;
    }

    if (RecvResult == TAG_MSG_RX_COMPLETED)
    {
        if (CompareData(&RptDefSet.RptID[3], pScanPktHead->byRepeaterID, 2))
        {
            printf("SNCR %d\r\n", RecvResult);
            M8107RF_SetCHAN(RptDefSet.Gchn); //Change channel of gateway
            M8107RF_SendPacket(FifoLen, RecvBuf); //Transparent scanner packet to gateway

            RecvResult = M8107RF_RecvPacket(FifoLen, RecvBuf, mSec, ENABLE);

            if (RecvResult == TAG_MSG_RX_COMPLETED)
            {
                printf("ACK %d\r\n", RecvResult);

                M8107RF_SetCHAN(RptDefSet.Tchn); //Change channel of Gateway
                M8107RF_SendPacket(FifoLen, RecvBuf); //Transparent gateway ack to scanner
            }
        }
    }
}
