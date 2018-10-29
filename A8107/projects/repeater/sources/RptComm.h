#ifndef _RPT_COMMON_H_
#define _RPT_COMMON_H_

#include <stdbool.h>
#include <stdint.h>
#include "RptProc.h"
#include "RfCommunication.h"

//#define _DBG_PRINTF
#ifdef _DBG_PRINTF
#define DBGMSG(format, args...) printf("[%s:%d] "format, __FILE__, __LINE__, ##args)
#else
#define DBGMSG printf//(args...)
#endif //DBG_PRINTF

//#define _NO_DBG_PRINTF
#ifdef _NO_DBG_PRINTF
#define DBGMSG(args...)
#endif //__NO_DBG_PRINTF

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
//typedef enum {ERROR = 0, SUCCESS = !DISABLE} ErrorState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

#define COMPARE_RSSI_VALUE                          50//65

#define USE_BUTTON_INTERRUPT                        (GPIO0->DATA_PIN.P08)
#define KEY_ON                                      0
#define KEY_OFF                                     1

#define SYNC_LEN                                    16
#define PAYLOAD_LEN                                 64
//#define RPT_RX_MAX_BIGDATA                        3

/* Repeater runtime for each task */
#if defined (REPEATER_500MS_MODE)
#define RPT_TASK_TIMER_VAL                          500
#define RPT_PWRON_BEACON_TIMER                      497

#define RPT_RX_MAX_BIGDATA_HEAD                     3
#define RPT_RX_MAX_BIGDATA                          3
#elif defined (REPEATER_TRANSPARENT_MODE)
#define RPT_TASK_TIMER_VAL                          (1000 - RPT_TASK_STOP_EARLY)
#define RPT_PWRON_BEACON_TIMER                      (RPT_TASK_TIMER_VAL - 10)

#define RPT_RX_MAX_BIGDATA_HEAD                     6
#define RPT_RX_MAX_BIGDATA                          1
#endif

//#define RPT_TASK_TIMER_VAL                        500
#define RPT_TASK_STOP_EARLY                         20

#define TASK_CUR_RUNTIME(timer)                     (timer->VALUE / ((SystemCoreClock / 1000) - 3))
//#define RstTimerValue(timer, msec)                  (timer->VALUE / ((SystemCoreClock / 1000) - 3)) * msec

/* RF receive data timer and receive update data idle timer */
#define RPT_1M_RX_ONEPKT_TIMER                      1.629
#define RPT_500K_RX_ONEPKT_TIMER                    1.769 //Recv one packet timer = 1.629, receive packet idle timer = 0.14.
#define RPT_250K_RX_ONEPKT_TIMER                    1.629

/* RF get packet timer value */
//#define RPT_PWRON_BEACON_TIMER                    490
#define RPT_BEACON_TIMER                            60
#define PRT_RX_PKT_TIMER                            24
#define RPT_RX_UPDDATA_TIMER                        5

#define RPT_FREETIME_RECV_TIMER                     250
#define RPT_HANDLE_BIND_TIMER                       350
#define RPT_TAG_REPLY_TIMER                         6

#define RPT_1M_UPDTIMER                             100
#define RPT_500K_UPDTIMER                           130
#define RPT_250K_UPDTIMER                           200

/* Tag update buffer size */
#define BIGDATA_BUFSIZE                             4774 //4736 * 6

/* Repeater RF Channel */
#define GW_CHN                                      184 //First channel for Gateway
#define TAG_CHN                                     200 //Second channel for Tag

/* Repeater ID */
#define RPT_ID1                                     0xFF
#define RPT_ID2                                     0xFF
#define RPT_ID3                                     0xFE
#define RPT_ID4                                     0x00
#define RPT_ID5                                     0x06 // < 50, Tx mode delay more than ((80 *80) * 10us) / 1000 = Delay 64ms

/* Extra define of Repeater */
//#define REPEATER_STATUS                             0x31 //Repeater reply status of get update Tag data
//#define RPT_COMBTAG_RS                              0x32 //Repeater combine Tag result reply of gateway

/* define version control of repeater */
#define RPT_VERSION_MAJOR                           0x0
#define RPT_VERSION_MINOR                           0x0
#define RPT_VERSION_RELEASE                         0x0
#define RPT_VERSION_BUILD                           0x15

#define RPT_BUILD_PROJECT_DATE                      20180718

#define RPT_ONERF_ONE_CHN_MODE                      0
#define RPT_ONERF_TWO_CHN_MODE                      1
#define RPT_TWORF_TWO_CHN_MODE                      2

/* Define LED GPIO */
#define GREEN_LED_HIGH()                            GPIO0->OUTENABLECLR = (1 << 11)
#define GREEN_LED_LOW()                             GPIO0->OUTENABLESET = (1 << 11)

#define YELLOW_LED_HIGH()                           GPIO0->OUTENABLECLR = (1 << 15)
#define YELLOW_LED_LOW()                            GPIO0->OUTENABLESET = (1 << 15)

//#define LA_GPIO_HIGHT()                             GPIO0->OUTENABLECLR = (1 << 15)
//#define LA_GPIO_LOW()                               GPIO0->OUTENABLESET = (1 << 15)

/* Repeater runtime mode */
#define RPT_PWRON_MODE                              1
#define RPT_TASK_MODE                               2
#define RPT_RX_MODE                                 3
#define RPT_TX_MODE                                 4

/* Control LED State */
#define LED_TOTAL_TURN_ON                           1
#define LED_TOTAL_TURN_OFF                          2
#define TRIGGER_FOR_FREEBEACON                      3
#define TRIGGER_FOR_UPD_TAG                         4

typedef struct node {
    uint8_t MemCpybuf[PAYLOAD_LEN];
} MemCpyNode, *P_MemCpyNode;

#pragma pack(1)
typedef struct {
    uint8_t DelayFlag;
    uint8_t TxDelayTime;
    uint16_t RxSyncTime;
} DYNAMIC_DELAY_TX, *P_DYNAMIC_DELAY_TX;

typedef struct {
    uint8_t SyncPkt[SYNC_LEN];
    uint8_t DeviceListPkt[PAYLOAD_LEN];                                                 //
    uint8_t RawHeadBuf[RPT_RX_MAX_BIGDATA_HEAD][PAYLOAD_LEN];   //Receive Big Data Header Buffer
    uint8_t RawDataBuf[RPT_RX_MAX_BIGDATA][BIGDATA_BUFSIZE];    //Receive Big Data Buffer
} KEEP_RECEIVE_PACKET, *P_KEEP_RECEIVE_PACKET;

typedef struct {
    uint8_t UpdFirmwareFlag;                                //trigger flag for receive update repeater firmware data
    uint8_t KeepUpdFwSegIndex;
    uint32_t WrNewAppAddress;
} REPEATER_UPDATE_FIRMWARE, *P_REPEATER_UPDATE_FIRMWARE;

typedef struct {
    uint8_t RunTimeMode;                                    //Repeater power on runtime mode
    uint8_t RxPktState;                                     //Repeater receive packet state
    uint8_t RxPktLen;                                       //Repeater receive packet length

    uint8_t SeqIndex;                                       //Keep receive packet sequence number
    uint8_t RtnPwrModeflag;                                 //for lost sync, trigger flag for return power on mode resync
    uint8_t SyncCryptFlag;                                  //receive sync packet is crypt, trigger flag for crypt transparent packet to Tag

    uint8_t KeepRxDataCnt;
    uint8_t TxDataCnt;
    uint8_t KeepUpLayerID[5];

    uint16_t RxPktTime;                                     //Repeater receive packet timeout value, minisecond

    DYNAMIC_DELAY_TX DelayTxStrc;
    KEEP_RECEIVE_PACKET KeepRxPktStrc;
    REPEATER_UPDATE_FIRMWARE UpdRptFwStrc;
} RPT_RUNTIME_PARAMETER, *P_RPT_RUNTIME_PARAMETER;

/* Repeater Default Setting Struct */
typedef struct {
    uint8_t Gchn;           //Gateway to Repeater channel
    uint8_t Tchn;           //Repeater to Tag channel
    uint8_t RptDataRate;    //RF Data rate
    uint8_t RptBatLv;       //Repeater Battery Level

    uint8_t RptVerMJR;      //Repeater Version Major
    uint8_t RptVerMINR;     //Repeater Version Minor
    uint8_t RptVerRLSE;     //Repeater Version Release
    uint8_t RptVerBLD;      //Repeater Version Build

    //uint16_t RptReSync;     //Repeater ReSync Beacon counter
    //uint32_t StartSec;      //Repeater power on timer

    uint8_t RptID[5];       //Repeater ID
    uint8_t RptHD_Spec;     //Repeater Hardware Spec

    int ChanCnt;            //For Test Channel
} RPT_NVRAM_BASE_PARAMETER, *P_RPT_NVRAM_BASE_PARAMETER;

typedef struct {
    uint8_t FirstCHN;       //Gateway to Repeater channel
    uint8_t SecdCHN;        //Repeater to Tag channel
    uint8_t RptDataRate;    //RF Data rate
    //uint8_t RptBatLv;       //Repeater Battery Level

    uint32_t RptVerMJR;     //Repeater Version Major
    uint32_t RptVerMINR;    //Repeater Version Minor
    //uint8_t RptVerRLSE;     //Repeater Version Release
    //uint8_t RptVerBLD;      //Repeater Version Build

    //uint16_t RptReSync;     //Repeater ReSync Beacon counter
    //uint32_t StartSec;      //Repeater power on timer

    uint8_t RptID[5];       //Repeater ID
    //uint8_t RptHDSpec;      //Repeater Hardware Spec
} RPT_NVRAM_PARAMETER, *P_RPT_NVRAM_PARAMETER;

typedef struct {
    uint8_t RptRxState;
    uint8_t RptRxLen;
    uint16_t RptRxTime;
} RPT_PROC_STATE, *P_RPT_PROC_STATE;

#pragma pack()

extern RPT_NVRAM_BASE_PARAMETER RptDefSet;

void SetTimer1IntISR(uint8_t tigger);
uint8_t GetTimer1IntISR(void);

void Rpt_SendFakeSyncBeacon(P_RPT_RUNTIME_PARAMETER mRunTimePara, uint8_t *GwID, uint8_t *TxPktBuf);
uint16_t Rpt_CalcRecvUpd_Timeout(int16_t UpdLen);
void SetOneShotTimer1(uint32_t msec);
void Rpt_DefaultSetting(void);

P_RPT_RUNTIME_PARAMETER Rpt_ResetRxTaskState(P_RPT_RUNTIME_PARAMETER RunTimePara);

void Repeater_LED_Ctrl(uint8_t State);
uint8_t ScanButtonAction(void);
void Rpt_GlobalVar_Init(void);

int n_turn_p(int num);
bool CompareData(uint8_t *src_array, uint8_t *dest_array, unsigned int array_size);
bool CheckZero(uint8_t *array, unsigned int array_size);

void A7127RF_SendPacket(uint32_t FifoLen, uint8_t *Buffer);
uint8_t A7127RF_RecvPacket(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t msec, FunctionalState RecvTimerState);

void M8107RF_SetCHAN(uint8_t Channel);
void M8107RF_SendPacket(uint32_t FifoLen, uint8_t *buffer);
uint8_t M8107RF_RecvPacket(uint32_t FifoLen, uint8_t *RecvBuf, uint32_t msec, FunctionalState RecvTimerState);

#endif /* _RPT_COMMON_H_ */
