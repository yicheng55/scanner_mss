#ifndef _RF_TX_PACKET_H_
#define _RF_TX_PACKET_H_

#include <stdint.h>
#include "RptComm.h"

//void Rpt_TxModeTask(uint8_t *TxPktBuf);
void Rpt_TxModeTask(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *TxPktBuf);

#endif /* _RF_TX_PACKET_H_ */
