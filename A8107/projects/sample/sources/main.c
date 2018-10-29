/*************************************************************
 * main.c
 *	Amiccom 2.4G RF Master role for test.
 *	V0.2 - 2017/9/6
 *		1. Add Beacon synchronization and powerdown/wakeup.
 *		2. update 4736 bytes time = 87ms.
 *	V0.3 - 2017/9/12
 *		1. wait RX timeout Timer. Uses TIMER0
 *		2. reply result
 *		3. SLPTIMER_Initial(SLPTIMER1, (32768 - 68), 0, 1);
 *		4. #define BEACONTIMEOUT		2
 *
 *	V0.4 - 2017/9/15
 *		1. Add Beacon field: Heart beat period.
 *		2. Add sleep timer option = 1, 2, 5, 10, 20,40 sec
 *    3. Emulate EPD update time 5s.
 *
 *	V0.5 - 2017/9/22
 *		1. Update Beacon format for Multi-Tag update and Hopping.
 *		
 *	V0.6 - 2017/9/27
 *		1. Add DataRate setting. 1Mbps, 500Kbps & 250Kbps.
 *
 *	V0.7 - 2017/10/2
 *		1. Add AES128 security
 *		2. Add RSSI measurement @SYNCBEACON.
 *
 *************************************************************/
#include "main.h"
#include "EPaperSSD1608.h"
#include "tagdefine.h"
#include "Flash.h"
#include "Board.h"
#include "ProjectConfig.h"

uint8_t TimeoutFlag;
uint16_t	    RxCnt;
uint32_t		Err_ByteCnt;
uint32_t  	Err_BitCnt;
uint8_t		LossCnt = 0;			//Lost Beacon count -- SPenser
uint8_t		tmpbuf[256];
//uint8_t RebootCnt[2];
uint32_t gBeaconwait = RF_TIMEOUT_BEACON;		//Receive Beacon timeout ms.
uint8_t gDatarate = 1;
uint32_t altfunset, outenset, pun; //IO Setting Info.

const uint8_t BitCount_Tab[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
const uint8_t ID_Tab[8]={0x44,0x61,0x76,0x69,0x63,0x6F,0x6D,0x5F}; //ID code
const uint8_t PageTab[8]={0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70};//page select
uint8_t OriginalData[16] = {0xAA,0x1,0,0,2,3,0x99,0,0,0,0,0,0,0,0,0};
const uint8_t AES_KEY[] =
{
  	0x4C, 0x68, 0x38, 0x41,
 	 	0x39, 0xF5, 0x74, 0xD8,
  	0x36, 0xBC, 0xF3, 0x4E,
  	0x9D, 0xFB, 0x01, 0xBF
};
uint8_t EncryptData[16];
uint8_t DecryptData[16];

const uint8_t CHTable[16] = {184, 200, 95, 94, 201, 250, 150, 145,
							139, 135, 136, 140, 142, 144, 160, 168};

uint8_t EpdRFData[4736];
uint8_t StatusInFlash[64];							
extern uint8_t gState;
extern uint8_t gTagDonotSleep;
extern uint8_t Oneshoutflag;
extern uint16_t DataLen;

extern uint8_t ghopcount;
extern uint8_t gCHindex;
extern uint32_t gRSSIvalue;;

extern void board_init(void);
extern void printTime(void);
extern void WriteLUT(void);
extern void ePaper_Init_03(void);

extern uint16_t UpdateCount;				
extern uint16_t RebootCount;		
extern uint16_t UpdateFailCount;	
extern uint8_t TAG_UPDATED;
/* Spenser added */
uint8_t gTagSeq = 0;
extern uint8_t mindex;

/************************************************************************
** UserRegister
************************************************************************/
void UserRegister(void)
{
	int i;
	
	RADIO->DATARATE = gDatarate;		//DateRate=500Kbps
	RADIO->FIFOCTRL 	= 0x0000003F;//fifo control register
	RADIO->CODE 		= 0x01280011;//Code1 register	//Enable CRC
	for (i=0; i<4; i++)
	{
		RADIO->SYNCB_ID[3-i] = ID_Tab[i];//ID Bytes		
	}
}
#if 0
void UserRegister(void)
{	
	uint8_t i;

	for (i=0; i<4; i++)
		RADIO->SYNCB_ID[3-i] = ID_Tab[i];//ID Bytes

	POWER->BATTERY 		= 0x00000000;//Battery detect register
	POWER->POWERCTRL 	= 0x0000C001;//Power control1 register
	POWER->POWERCTRL2 	= 0x000000D0;//Power control2 register
	//RADIO->RESET=;//Reset register
	RADIO->FIFOCTRL 	= 0x0000003F;//fifo control register
	RADIO->THRESHOLD 	= 0x00000032;//RSSI threshold register
	//RADIO->CODE 		= 0x01280000;//Code1 register
	RADIO->CODE 		= 0x01280011;//Code1 register	//Enable CRC
	RADIO->CODE2 		= 0x0000002A;//Code2 threshold register
	ADC12B->Control 	= 0x000061FE;//ADC control1 register
	ADC12B->IOSEL 		= 0x00000000;//ADC channel register
}
#endif

void ResetReadPointer(void)
{
	RADIO->RESET = 0x20;
}

void BackupIOSetting(void)
{
	altfunset = GPIO0->ALTFUNCSET;
	outenset  = GPIO0->OUTENABLESET;
	pun       = GPIO0->PUN;
}

void RestoreIOSetting(void)
{
	GPIO0->ALTFUNCSET   = altfunset;        // restore ALTFUNCSET
	GPIO0->OUTENABLESET = outenset;         // restore OUTENABLESET
	GPIO0->PUN          = pun;              // restore PUN
}

void ClearIOSetting()
{
//	GPIO0->OUTENABLESET = 0;
	GPIO0->ALTFUNCCLR   = 0xFFFFFFFF;       // clean all IO function
	GPIO0->OUTENABLECLR = 0xFFFFFFFF;       // set all IO to input
	GPIO0->PUN          = 0; 								// set all IO to pull-high
	
	GPIO0->OUTENABLESET = (1<<0) | (1<<14);  // set IO to output
	GPIO0->PUN_PIN.P00 = 1;
	GPIO0->PUN_PIN.P14 = 1;

	GPIO0->OUTENABLECLR = (1<<12);
	GPIO0->PUN_PIN.P12 = 1; //without pull-up

}
#if 0 //backup
void ClearIOSetting()
{
	GPIO0->OUTENABLESET = 0;
	GPIO0->ALTFUNCCLR   = 0xFFFFFFFF;       // clean all IO function
	GPIO0->OUTENABLECLR = 0xFFFFFFFF;       // set all IO to input
	GPIO0->PUN          = 0; 								// set all IO to pull-high
}
#endif
int main(void)
{
	uint8_t mReturn;
	uint8_t RxTimeOutCnt = 0;
	int mlen;
	gDatarate = RF_DATARATE_500K;//DATARATE500K;//DATARATE1M;			//Assign the data rate.
	
	BoardInit();		//Initial hardware system
	EPaperInit(EPAPER_MODE_DEFAULT, false);
	initRF(); //init RF
	//TurnOn_External_RTC_Crystal();
	UserRegister();		//M8107 registers setting
  StrobeCmd(CMD_STBY);
  SetCH(RF_CHN); //freq 2450MHz CH100 JAMES TEST is 184 or JERRY is 200
	TurnOn_External_RTC_Crystal();
	BoardInitRTC();
	NVIC_EnableIRQ(SLPTIMER1_IRQn);
//	printf("002\r\n");
	BackupIOSetting();
	gState = PWRBEACON_STATE;
//	
//	do
//	{
//		AES128_Encrypt(OriginalData,AES_KEY,EncryptData);
//				for(Z=0; Z<16; Z++)
//				{
//					printf("%02x ",EncryptData[Z]);
//				}	
//		AES128_Decrypt(EncryptData,AES_KEY,DecryptData);
//								for(Z=0; Z<16; Z++)
//				{
//					printf("%02x ",DecryptData[Z]);
//				}	
//				
//	}while(0);
	
	while(1) {
		switch(gState) {
		case PWRBEACON_STATE:
			SetFIFOLvl(16);
			setSynTimer(1, 0);
				StrobeCmd(CMD_RX);
				//Spenser
				mReturn = getpacket(900);      //FOR SPENSER.
				//mReturn = getpacket(30);		//FOR JERRY OR LONG DIATANCE.//2ms OK,  1ms fail
				
				if(Oneshoutflag) {		//Rx Beacon time out.			

						printf("Lost Beacon...PWRBEACON_STATE\r\n");
						LossCnt++;
						gState = ENTERPM1_STATE;
						
				}else {
					switch(mReturn) {
					case TAG_MSG_HIT_ID:
//							/* Spenser - multitags update */
//							StrobeCmd(CMD_STBY);
//							//printf("gTagSeq = %d\n", gTagSeq);
//							if(gDatarate == DATARATE500K) {
//								
//								if(gTagSeq > 0){

//									Delay1ms(gTagSeq*128);		//(120.3: 127ms) (132 : 138)
//									Delay1us(gTagSeq*850);		//(500) 850
//								}
//							}
//							/*-----------------------------*/
//							gState = TAG_LIST_STATE;
//							break;
					case TAG_MSG_NO_HIT_ID:
						case TAG_MSG_ONLY_SYNC:	
							gState = ENTERPM1_STATE;
							break;
					case TAG_MSG_HOPPING:
							//gState = 	HOPPM1_STATE;
							printf("ghopcount = %d\r\n", ghopcount);
							if(ghopcount == 1){		//Hopping
								printf("Hop to channel = %d\r\n", CHTable[gCHindex]);
								StrobeCmd(CMD_STBY);  
							SetCH(CHTable[gCHindex]); //freq 2450MHz CH100
							//Delay1ms(1);
							}
							gState = ENTERPM1_STATE;
							break;
					case TAG_MSG_RX_TIMEOUT:
							RxTimeOutCnt++;
							if(RxTimeOutCnt >= EPD_RETRY_LIMIT)
							{
								ReadAndWriteRebootCount();
//									NVIC_SystemReset();
							}
							gState = 	ENTERPM1_STATE;
							break;
	
					case TAG_MSG_REPORT_STATUS_OK:
							gState = ENTERPM1_STATE;
							break;	
						case TAG_MSG_TO_RCV_TAG_LIST:
							gState = TAG_LIST_STATE;
							break;
						default:
							gState = ENTERPM1_STATE;
							break;
					}
				}

				break;			
//			case PWRBEACON_STATE:
//				setSynTimer(1, 0);
//				StrobeCmd(CMD_RX);
//				mReturn= getpacket(900);		//200ms//900
//				
//				if(Oneshoutflag) {		//Rx Beacon time out.						
//					printf("Lost Beacon...PWRBEACON_STATE\r\n");
////					mReturn = RXTIMEOUT;
//						LossCnt++;
//						gState = ENTERPM1_STATE;
//					}
//					
//				switch(mReturn) {
//					case HITID:
//						/* Spenser - multitags update */
//						StrobeCmd(CMD_STBY);
//						if(gDatarate == DATARATE500K) {
//							if(gTagSeq > 0){
//								
//								Delay1ms(gTagSeq*128);		//(120.3: 127ms) (132 : 138)
//								Delay1us(gTagSeq*850);		//(500)
//							}
//						}
//						/*-----------------------------*/
//						gState = HEADER_STATE;
//						break;
//					case NOHITID:
//						gState = 	ENTERPM1_STATE;
//						break;
//					case HOPPING:
//						printf("Hopping\r\n");
//						gState = PWRBEACON_STATE;
//						break;
////					case STATENOTRIGHT:	
//					case RXTIMEOUT:
//						RxTimeOutCnt++;
//						if(RxTimeOutCnt >= EPD_RETRY_LIMIT)
//						{
//							ReadAndWriteRebootCount();
////							NVIC_SystemReset();
//						}
//						gState = 	ENTERPM1_STATE;
//						break;
//					case REPORTSTATUSOK:
//						gState = ENTERPM1_STATE;
//						break;	

//					default:
//						gState = PWRBEACON_STATE;
//						break;
//				}

//				
//				break;
				
			case BEACON_STATE:
					SetFIFOLvl(16);
					StrobeCmd(CMD_RX);
					//Spenser
					mReturn = getpacket(10);      //FOR SPENSER.
					//mReturn = getpacket(30);		//FOR JERRY OR LONG DIATANCE.//2ms OK,  1ms fail
					
					if(Oneshoutflag) {		//Rx Beacon time out.			

							printf("Lost Beacon...BEACON_STATE\r\n");
							LossCnt++;
							gState = ENTERPM1_STATE;
							
					}else {
						switch(mReturn) {
//							case HITID:
//								/* Spenser - multitags update */
//								StrobeCmd(CMD_STBY);
//								printf("gTagSeq = %d\n", gTagSeq);
//								if(gDatarate == DATARATE500K) {
//									
//									if(gTagSeq > 0){

//										Delay1ms(gTagSeq*128);		//(120.3: 127ms) (132 : 138)
//										Delay1us(gTagSeq*850);		//(500) 850
//									}
//								}
//								/*-----------------------------*/
//								gState = TAG_LIST_STATE;
//								break;
							case TAG_MSG_NO_HIT_ID:
							case TAG_MSG_ONLY_SYNC:	
								gState = ENTERPM1_STATE;
								break;
							case TAG_MSG_HOPPING:
								//gState = 	HOPPM1_STATE;
								printf("ghopcount = %d\r\n", ghopcount);
								if(ghopcount == 1){		//Hopping
									printf("Hop to channel = %d\r\n", CHTable[gCHindex]);
									StrobeCmd(CMD_STBY);  
				  				SetCH(CHTable[gCHindex]); //freq 2450MHz CH100
				  				//Delay1ms(1);
								}
								gState = ENTERPM1_STATE;
								break;
							case TAG_MSG_RX_TIMEOUT:
								RxTimeOutCnt++;
								if(RxTimeOutCnt >= EPD_RETRY_LIMIT)
								{
									ReadAndWriteRebootCount();
//									NVIC_SystemReset();
								}
								gState = 	ENTERPM1_STATE;
								break;
		
							case TAG_MSG_REPORT_STATUS_OK:
								gState = ENTERPM1_STATE;
								break;		
							case TAG_MSG_TO_RCV_TAG_LIST:
//								printf("TORCVTAGLIST...!!!!!!.........\r\n");
								gState = TAG_LIST_STATE;
								break;		
							default:
								gState = ENTERPM1_STATE;
								break;
						}
					}

				break;

			case HEADER_STATE:
				SetFIFOLvl(64);
//				printf("HEADER_STATE...\r\n");
				StrobeCmd(CMD_RX);
				//Spenser
				mReturn = getpacket(10);	      //FOR SPENSER
				//mReturn = getpacket(50);		//FOR JERRY OR LONG DIATANCE.//3ms OK, 1ms fail
				switch(mReturn){
					case TAG_MSG_HIT_ID:
						gState = DATA_STATE;
						break;
					
					case TAG_MSG_RX_TIMEOUT:
						printf("Receive Header timer out.\r\n");
						replypacket(mReturn);
						UpdateFailCount++;
						ReadAndWriteNonVolatileData(0,1);
						gState = ENTERPM1_STATE;
						break;
#if 0 	//Remarked by Spenser				
					default:
						gState = ENTERPM1_STATE;
						break;
#endif								
				}
				
				break;
					
			case DATA_STATE:
				mlen = (int)DataLen;
				/* Spenser added */
				mindex = 0;
				Oneshoutflag = 0;		//TIme out flag
				switch(gDatarate){
						case RF_DATARATE_1M:
							SetTimer(TIMER0, 100);		//100ms
							
							break;
						case RF_DATARATE_500K:
							SetTimer(TIMER0, 200);		//135ms, Cover all data transmit time
							
							break;
						case RF_DATARATE_250K:
							SetTimer(TIMER0, 200);		//200ms
							
							break;	
				}
				while(mlen > 0) {
					StrobeCmd(CMD_RX);
					getdatapacket();
					

					if(Oneshoutflag) {
						printf("DATA_STATE___RX data timer out.\r\n");
						mReturn = TAG_MSG_PACKET_LOST;
//						replypacket(mReturn);
						break;
					}
					//RxPacket();
					mlen -= 62;		//two bytes header						
				}
				Timer_StopTimer(TIMER0);
				/*-----------------------------------------------------*/

#if 0				
				while(mlen > 0) {
					StrobeCmd(CMD_RX);
					//Delay10us(1);
					switch(gDatarate){
						case RF_DATARATE_1M:
							mReturn = getpacket(100);		//100ms
							break;
						case RF_DATARATE_500K:
							mReturn = getpacket(250);		//150ms
							break;
						case RF_DATARATE_250K:
							mReturn = getpacket(200);		//200ms
							break;	
					}

					if(Oneshoutflag) {
						printf("DATA_STATE___RX data timer out.\r\n");
						mReturn = PACKET_LOST;
						replypacket(mReturn);
						break;
					}
					//RxPacket();
					mlen -= 62;		//two bytes header						
				}
#endif

				if(Oneshoutflag || (mReturn == TAG_MSG_PACKET_LOST)) {	//Don't need update EPD if data error.
					LossCnt++;
					replypacket(mReturn);	
					UpdateFailCount++;
					gState = ENTERPM1_STATE;
				}else {
					replypacket(TAG_MSG_RX_COMPLETED);
					/* RSSI_measurement */
					//RSSI_measurement();
					StrobeCmd(CMD_SLEEP);				
					gState = EPDDATA_STATE;
				}

				break;

			case EPDDATA_STATE:
#if 0			
				printf("Emulate EPD update 5 sec.\r\n");
				Delay1ms(1000);
				Delay1ms(1000);
				Delay1ms(1000);
				Delay1ms(1000);
				Delay1ms(800);
#endif				
				gState = EPDREFRESH_STATE;
				break;
				
			case 	EPDREFRESH_STATE:
//				printf("EPDREFRESH_STATE..\r\n");
/*
				for(Z=0; Z<4736; Z++)
				{
					printf("%02x ",EpdRFData[Z]);
				}	
*/				
				TagPowerUpFunction();
				WriteRAMqr_FULL(DataLen, EpdRFData);
				UpdateDisplay();	
				UpdateCount++;
				TAG_UPDATED = 1;
				TagPowerDownFunction();
				if(gTagDonotSleep == 1)
				{
					gState = PWRBEACON_STATE;
				}
				else
				{
					gState = ENTERPM1_STATE;
				}
				
				break;
				
			case ENTERPM1_STATE:
//				if(LossCnt > 5) ReadAndWriteRebootCount();
				if(gTagDonotSleep == 1)
				{
					gState = PWRBEACON_STATE;
				}
				else
				{
					printf("--PM1--\r\n");
					printTime();
					StrobeCmd(CMD_SLEEP);
					ClearIOSetting();

					EnterPM1();  //Sleep command	
					//EnterPM3();  //Sleep command					
					//printf("after EnterPM3...\r\n");
					RestoreIOSetting();
					StrobeCmd(CMD_STBY);                          // MCU enter PM1
					/* Spenser */
					if(LossCnt != 0) {
						LossCnt = 0;
						gState = PWRBEACON_STATE;

					}else gState = BEACON_STATE;//PWRBEACON_STATE;//BEACON_STATE;

				}

				break;
			
				
			case TAG_LIST_STATE:
					SetFIFOLvl(64);
//				printf("TAG_LIST_STATE........................................\r\n");
//					setSynTimer(1, 0);
					StrobeCmd(CMD_RX);
					//Spenser
					mReturn = getpacket(10);      //FOR SPENSER.
					//mReturn = getpacket(30);		//FOR JERRY OR LONG DIATANCE.//2ms OK,  1ms fail
					
					if(Oneshoutflag) {		//Rx Beacon time out.			

					printf("Lost Beacon...TAG_LIST_STATE\r\n");
					LossCnt++;
					gState = ENTERPM1_STATE;
							
					}else {
						switch(mReturn) {
							case TAG_MSG_HIT_ID:
								/* Spenser - multitags update */
								StrobeCmd(CMD_STBY);
								//printf("gTagSeq = %d\n", gTagSeq);
								if(gDatarate == RF_DATARATE_500K) {
									
									if(gTagSeq > 0){

										Delay1ms(gTagSeq*128);		//128(120.3: 127ms) (132 : 138)
										Delay1us(gTagSeq*850);		//850(500) 850
									}
								}
								/*-----------------------------*/
								gState = HEADER_STATE;
								break;
							case TAG_MSG_NO_HIT_ID:
								gState = ENTERPM1_STATE;
								break;
							case TAG_MSG_HOPPING:
								//gState = 	HOPPM1_STATE;
								printf("ghopcount = %d\r\n", ghopcount);
								if(ghopcount == 1){		//Hopping
									printf("Hop to channel = %d\r\n", CHTable[gCHindex]);
									StrobeCmd(CMD_STBY);  
				  				SetCH(CHTable[gCHindex]); //freq 2450MHz CH100
				  				//Delay1ms(1);
								}
								gState = ENTERPM1_STATE;
								break;
							case TAG_MSG_RX_TIMEOUT:
								RxTimeOutCnt++;
								if(RxTimeOutCnt >= EPD_RETRY_LIMIT)
								{
									ReadAndWriteRebootCount();
//									NVIC_SystemReset();
								}
								gState = 	ENTERPM1_STATE;
								break;
		
							case TAG_MSG_REPORT_STATUS_OK:
								gState = ENTERPM1_STATE;
								break;	
							default:
								gState = ENTERPM1_STATE;
								break;
						}
					}

				break;	
				
#if 0				
			case HOPPM1_STATE:
				printf("ghopcount = %d\r\n", ghopcount);
				if(ghopcount == 1){		//Hopping
					printf("Hop to channel = %d\r\n", CHTable[gCHindex]);
					StrobeCmd(CMD_STBY);  
  				SetCH(CHTable[gCHindex]); //freq 2450MHz CH100
  				//Delay1ms(1);
				}
			
				printf("Entering power down...\r\n");		
				EnterPM1();  //Sleep command
				
				if(ghopcount > 0)	ghopcount--;
					
				StrobeCmd(CMD_STBY);                          // MCU enter PM1
				//gState = HOPBEACON_STATE;
				gState = PWRBEACON_STATE;
				break;
#endif
				
		}

		
	}		//while
			
}

