#ifndef _RPT_PROC_H_
#define _RPT_PROC_H_

#include <stdbool.h>
#include <stdint.h>

#ifndef RPT_500MS_MODE
#define RPT_500MS_MODE
#endif //RPT_500MS_MODE

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
//typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

#define SYNC_LEN												16
#define PAYLOAD_LEN											64
#define RPT_RX_MAX_UPDATE								3

/* Repeater runtime for each task */
#define RPT_TASK_TIMER_VAL							500
#define RPT_TASK_STOP_EARLY							20

#define TASK_CUR_RUNTIME(timer)					(timer->VALUE / ((SystemCoreClock / 1000) - 3))

/* RF receive data timer and receive update data idle timer */
#define RPT_1M_RX_ONEPKT_TIMER					1.629
#define RPT_500K_RX_ONEPKT_TIMER				1.769 //Recv one packet timer = 1.629, receive packet idle timer = 0.14.
#define RPT_250K_RX_ONEPKT_TIMER				1.629

/* RF get packet timer value */
#define RPT_PWRON_BEACON_TIMER					497
#define RPT_BEACON_TIMER								60
#define PRT_RX_PKT_TIMER								20
#define RPT_RX_UPDDATA_TIMER						5

#define RPT_HANDLE_SCNR_TIMER 					100

#define RPT_1M_UPDTIMER 								100
#define RPT_500K_UPDTIMER 							130
#define RPT_250K_UPDTIMER 							200

/* Tag update buffer size */
#define EpdUpdateSize 									4736// * 6

/* Repeater RF Channel */	
#define GW_CHN													184 //First channel for Gateway 
#define TAG_CHN													184 //Second channel for Tag

/* Repeater ID */
#define RPT_ID1													0xFF
#define RPT_ID2													0xFF
#define RPT_ID3													0xFE
#define RPT_ID4													0x00
#define RPT_ID5													0x06

/* Extra define of Repeater */
//#define REPEATER_STATUS								0x31 //Repeater reply status of get update Tag data
//#define RPT_COMBTAG_RS								0x32 //Repeater combine Tag result reply of gateway

/* define version control of repeater */
#define	RPT_VERSION_MAJOR	     					0x0
#define	RPT_VERSION_MINOR	      				0x0
#define	RPT_VERSION_RELEASE	    				0x0
#define	RPT_VERSION_BUILD	      				0x14

#define	RPT_BUILD_PROJECT_DATE					20180718

#define RPT_ONERF_ONE_CHN_MODE					0
#define RPT_ONERF_TWO_CHN_MODE					1
#define RPT_TWORF_TWO_CHN_MODE					2

/* define LED control */
#define GREEN_LED_HIGH()								GPIO0->OUTENABLECLR = (1 << 11)
#define GREEN_LED_LOW()									GPIO0->OUTENABLESET = (1 << 11)

#define YELLOW_LED_HIGH()								GPIO0->OUTENABLECLR = (1 << 15)
#define YELLOW_LED_LOW()								GPIO0->OUTENABLESET = (1 << 15)

#define LA_GPIO_HIGHT()									GPIO0->OUTENABLECLR = (1 << 15)
#define LA_GPIO_LOW()										GPIO0->OUTENABLESET = (1 << 15)

/* Repeater runtime mode */
#define RPT_PWRON_MODE 									1
#define RPT_TASK_MODE 									2
#define RPT_RX_MODE		 									3
#define RPT_TX_MODE		 									4

/* Repeater Global variable Struct */
typedef struct {
	uint8_t gMode; 												//Repeater runtime mode
	uint8_t gState; 											//Repeater Rx packet state
	uint8_t PwrOnSyncBeacon; 							//Repeater Power on Sync Beacon State
	uint8_t Tmr1DynaVal;

	uint8_t RxUpdDataDone; 								//Repeater confirms receipt of updated tag data
	uint8_t RxTagUpdRslt; 								//Repeater RX Tag update result 
	
	uint8_t UpdCntTmp1; 									//check receive update Tag counter
	uint8_t UpdCntTmp2; 									//
	
	uint8_t gTxSeqNo; 										//Repeater Tx packet sequence number
	uint8_t MultiTagCnt;      						//
	uint8_t UpdTagCmpl;       						//
	uint8_t TxUpdTagCnt;      						//
	uint8_t RtnPwrOnMode; 								//Repeater return poweron mode recalibrate beacon
	uint8_t RtnPwrModeCnt; 								//Repeater lost beacon counter
	uint8_t ScnrState; 										//For handle scanner packet state
	uint8_t RxCombTagRsCnt;   						//
	uint8_t cryptflag;        						//
	
	uint8_t RecvLen;
	
	// for bind ID 
	uint8_t BindFlag;
	uint8_t BindCnt;
	uint8_t HotBcnFlag;
	uint8_t BindRssi;
	volatile uint8_t Tmr1OneShoutFlag;		//Timer oneshout interrupt trigger flag
	
	uint8_t BindID[2];
	uint8_t SyncBuf[SYNC_LEN]; 						//receive sync buffer
	uint8_t TagLstBuf[PAYLOAD_LEN]; 			//receive Tag update list buffer
	uint8_t UpdHeadBuf[RPT_RX_MAX_UPDATE][PAYLOAD_LEN];
	uint8_t UpdDataBuf[RPT_RX_MAX_UPDATE][EpdUpdateSize];
	uint8_t TxBuffer[PAYLOAD_LEN]; 				//
	
	uint8_t GatewayID[5];
	uint8_t TagID[RPT_RX_MAX_UPDATE * 5];
	
	uint8_t  UpdFirmwareFlag;
	uint8_t  UpdFirmwareCount;
	uint8_t  BckupSegmentIndex;
	uint32_t WrNewAppAddress;
	
	uint16_t RxUpdBufPtrAddr; 						//
	uint16_t TxUpdBufPtrAddr; 						//Repeater Tx Update Data Buffer start address
	uint16_t AccuTagRsltLen;
	
	uint16_t ReSycCNT; 										//records repeater resync 
	
	uint16_t RecvTimer;
	
	int16_t TxUpdDataLen;	
	int16_t RxTtlUpdDataLenBuf[RPT_RX_MAX_UPDATE]; //	
	
}RPT_GLOBAL_VARIABLE, *P_RPT_GLOBAL_VARIABLE;

extern RPT_GLOBAL_VARIABLE RptGlblVar;

void Rpt_GlobalVar_Init(void);
void Rpt_Process(void);

#endif /* _RPT_PROC_H_ */
