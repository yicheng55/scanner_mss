#ifndef _RPT_SCNR_H_
#define _RPT_SCNR_H_

#include <stdint.h>

#pragma pack(1)

typedef struct _SCAN_PACKET_HEAD {
    //--- Common Header ---
    uint8_t bySequenceNumber;                                         // Sequence Number
    uint8_t byPacketType;                                            // Packet Type: RF_PT_SCANNER_PACKET
    uint8_t byCommand;                                                     // Command: RF_CMD_SCANNER_PAIRING, RF_CMD_SCANNER_UNPAIRING, RF_CMD_CONFIRMED_ACK, etc...
    uint8_t byDeviceID[2];                                           // Scanner ID/Tag ID/etc...
    uint8_t byGatewayID[2];                                        // Gateway ID
    uint8_t byRepeaterID[2];                                       // Repeater ID
    //uint8_t Reserverd[55];
} SCAN_PACKET_HEAD, *P_SCAN_PACKET_HEAD;

#pragma pack()

void Rpt_TransScnrPacket(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t mSec);

#endif /* _RPT_SCNR_H_ */
