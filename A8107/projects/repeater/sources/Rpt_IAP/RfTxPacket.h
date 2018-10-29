#ifndef _RF_TX_PACKET_H_
#define _RF_TX_PACKET_H_

#include <stdint.h>

void Rpt_TxTask_Proc(void);
void Rpt_TxPktProc(uint8_t length, uint8_t *buffer);

#endif /* _RF_TX_PACKET_H_ */
