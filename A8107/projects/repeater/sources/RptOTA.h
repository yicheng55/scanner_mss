#ifndef _RPT_OTA_UPDATE_FIRMWARE_H_
#define _RPT_OTA_UPDATE_FIRMWARE_H_

#include <stdint.h>
#include "RptComm.h"

#define SSL_MAX_LEN 16

#define RPT_CHECK_UPDATE_FLAG_VALUE                         0xEEEE
#define RPT_CHECK_OTA_UPD_FLAG_ADDRESS                  0x0010000
#define RPT_OTA_UPDATE_FIRMWARE_ADDRESS                 0x0010001

typedef struct {
    uint16_t rom_statue;
    uint32_t rom_size;
    uint8_t  ssl_data[SSL_MAX_LEN];
} OTA_UPDATE_FRIMWRAR_DEF, *P_OTA_UPDATE_FRIMWRAR_DEF;

uint8_t Rpt_CheckOTA_Update(void);

//uint8_t HandleRecvOtaHead(uint8_t *OtaBuf);
uint8_t HandleRecvOtaHead(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *HeadBuf);
//uint8_t HandleRecvOtaData(uint8_t *RecvBuf);
uint8_t HandleRecvOtaData(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *DataBuf);

#endif /* _RPT_OTA_UPDATE_FIRMWARE_H_ */
