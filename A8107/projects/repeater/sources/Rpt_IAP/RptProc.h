#ifndef _RPT_PROC_H_
#define _RPT_PROC_H_

#include <stdbool.h>
#include <stdint.h>

#define SYNC_LEN									16
#define PAYLOAD_LEN								64
#define RPT_RX_MAX_UPDATE					3

/* Repeater runtime for each task */
#define RPT_TASK_TIMER_VAL				500
#define RPT_END_EARLY_TIMER				15

#define TASK_CUR_RUNTIME(timer)		(timer->VALUE / ((SystemCoreClock / 1000) - 3))

/* RF get packet timer value */
#define RPT_PWRON_BEACON_TIMER		249
#define RPT_BEACON_TIMER					40
#define PRT_RX_PKT_TIMER					10
#define RPT_RX_UPDDATA_TIMER			5
#define RPT_HANDLE_SCNR_TIMER 		150

#define RPT_1M_UPDTIMER 					100
#define RPT_500K_UPDTIMER 				130
#define RPT_250K_UPDTIMER 				200

/* Tag update buffer size */
#define EpdUpdateSize 						4736 * 6

/* Repeater RF Channel */
#define GW_CHN										200 //First channel for Gateway 
#define TAG_CHN										200 //Second channel for Tag

/* Repeater ID */
#define RPT_ID1										0xFF
#define RPT_ID2										0xFF
#define RPT_ID3										0xFE
#define RPT_ID4										0x00
#define RPT_ID5										0x06

/* Extra define of Repeater */
#define REPEATER_STATUS						0x31 //Repeater reply status of get update Tag data
#define RPT_COMBTAG_RS						0x32 //Repeater combine Tag result reply of gateway

/* define version control of repeater */
#define	RPT_VERSION_MAJOR	     		0x0
#define	RPT_VERSION_MINOR	      	0x0
#define	RPT_VERSION_RELEASE	    	0x0
#define	RPT_VERSION_BUILD	      	0x11

/* define LED control */
#define GREEN_LED_HIGH()					GPIO0->OUTENABLECLR = (1 << 11)
#define GREEN_LED_LOW()						GPIO0->OUTENABLESET = (1 << 11)

#define YELLOW_LED_HIGH()					GPIO0->OUTENABLECLR = (1 << 15)
#define YELLOW_LED_LOW()					GPIO0->OUTENABLESET = (1 << 15)

/* Repeater runtime mode */
#define RPT_PWRON_MODE 						1
#define RPT_TASK_MODE 						2
#define RPT_RX_MODE		 						3
#define RPT_TX_MODE		 						4

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))
//typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

/* Repeater Task Struct */
typedef struct {
	uint8_t isRun; //program run flag, 0 = Not running, 1 = running
	uint16_t TimerSlice; //Timer
	uint16_t SliceNumber; //Mission interval ??????,?TimerSlice?0?,?????TimerSlice????
	void (*TaskPointer)(void* parameter); //Task function to run
}Task_Struct, *Task_Struct_Pointer;
extern Task_Struct tasks[];

/* Repeater Global variable Struct */
typedef struct {
	uint8_t gMode; //Repeater runtime mode
	uint8_t gState; //Repeater Rx packet state
	uint8_t PwrOnSyncBeacon; //Repeater Power on Sync Beacon State
	uint8_t Tmr1DynaVal;
	
	uint8_t RxUpdDataDone; //Repeater confirms receipt of updated tag data
	uint8_t RxTagUpdRslt; //Repeater RX Tag update result 
	
	uint8_t UpdCntTmp1; //check receive update Tag counter
	uint8_t UpdCntTmp2; //
		
	uint16_t RxUpdBufPtrAddr; //
	uint16_t RxTtlUpdDataLenBuf[RPT_RX_MAX_UPDATE]; //
	
	uint8_t gTxSeqNo; //Repeater Tx packet sequence number
	uint8_t MultiTagCnt;
	uint8_t UpdTagCmpl;
	uint8_t TxUpdTagCnt;
	int16_t TxUpdDataLen;
	uint16_t TxUpdBufPtrAddr; //Repeater Tx Update Data Buffer start address
	uint16_t AccuTagRsltLen;
	uint8_t RxCombTagRsCnt;
	
	uint8_t RtnPwrOnMode; //Repeater return poweron mode recalibrate beacon
	uint8_t RtnPwrModeCnt; //Repeater lost beacon counter
	uint16_t ReSycCNT; //records repeater resync 
	
	volatile uint8_t Tmr1OneShoutFlag; //Timer oneshout interrupt trigger flag
	
	uint8_t ScnrState; //For handle scanner packet state
	
	uint8_t SyncBuf[SYNC_LEN]; //receive sync buffer
	uint8_t TagLstBuf[PAYLOAD_LEN]; //receive Tag update list buffer
	uint8_t TxBuffer[PAYLOAD_LEN]; //
	
	uint8_t cryptflag;
	
	// for bind ID 
	uint8_t BindFlag;
	uint8_t BindCnt;
	uint8_t BindID[2];
	uint8_t HotBcnFlag;
	uint8_t BindRssi;
}RPT_GLOBAL_VARIABLE, *P_RPT_GLOBAL_VARIABLE;
extern RPT_GLOBAL_VARIABLE RptGlblVar;

/* Repeater Default Setting Struct */
typedef struct {
	uint8_t Gchn; //Gateway channel
	uint8_t Tchn; //Tag channel
	uint8_t RptDataRate; //RF Data rate
	uint8_t RptBatLv; //Repeater Battery 
		
	uint8_t RptVerMJR; //Repeater Version Major
	uint8_t RptVerMINR; //Repeater Version Minor
	uint8_t RptVerRLSE; //Repeater Version Release
	uint8_t RptVerBLD; //Repeater Version Build
	
	uint16_t RptReSync; //Repeater ReSync beacon counter
	uint32_t StartSec; //Repeater power on timer
	
	uint8_t RptID[5]; //Repeater ID
	
}RPT_DEFAULT_SETTING, *P_RPT_DEFAULT_SETTING;
extern RPT_DEFAULT_SETTING RptDefSet;

void Rpt_GlobalVar_Init(void);
void TaskRemarks(void);
void Rpt_Process1(void);
void Rpt_Process2(void);

void Rpt_SetRF_Channel(uint8_t Channel);
int n_turn_p(int num);
uint8_t rssi_turn_dbm(uint8_t Rssi);
bool compare_array_date(uint8_t *src_array, uint8_t *dest_array, unsigned int array_size);
bool check_array_zero(uint8_t *array, unsigned int array_size);

#endif /* _RPT_PROC_H_ */
