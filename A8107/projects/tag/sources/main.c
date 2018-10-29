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
#include <stdlib.h>
#include "TAG_BW_2.9_inch\EPaperSSD1608.h"
#include "TAG_RBW_2.9_inch\EPaperSSD1675A.h"
#include "TAG_RBW_2.13_inch\EPaperSSD1675A.h"
#include "tagdefine.h"
#include "Flash.h"
#include "EslDebug.h"
#include "RTC.h"
#include "led.h"
#include "fifo.h"
#include "jbig\jbig85.h"

uint8_t TimeoutFlag;
uint16_t	    RxCnt;
uint32_t		Err_ByteCnt;
uint32_t  	Err_BitCnt;
uint8_t		LossCnt = 0;			//Lost Beacon count -- SPenser
uint8_t		tmpbuf[256];
//uint8_t RebootCnt[2];
uint32_t gBeaconwait = RF_TIMEOUT_BEACON;		//Receive Beacon timeout ms.
//uint8_t gDatarate = 1;
uint32_t altfunset, outenset, pun; //IO Setting Info.

const uint8_t BitCount_Tab[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
const uint8_t ID_Tab[8]={0x44,0x61,0x76,0x69,0x63,0x6F,0x6D,0x5F}; //ID code
const uint8_t PageTab[8]={0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70};//page select

//const uint8_t CHTable[16] = {184, 200, 95, 94, 201, 250, 150, 145,
//							139, 135, 136, 140, 142, 144, 160, 168};
const int CHTable[16] = {184, 200, 95, 94, 201, 250, 150, 145,
							139, 135, 136, 140, 142, 144, 160, 168};
//const uint8_t CHTable[50] = {44,45,46,47,48,49,50,51,52,53,54,55,56,92,93,94,95,96,97,98,99,100,101,102,103,104,
//144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167};

uint8_t PN9_Tab[]=
{   0x01,0x00,0x00,0x00,0x07,0xBC,0x4E,0xD1,	
    0xE7,0xCD,0x8A,0x91,0xC6,0xD5,0xC4,0xC4,
    0x40,0x21,0x18,0x4E,0x55,0x86,0xF4,0xDC,
    0x8A,0x15,0xA7,0xEC,0x92,0xDF,0x93,0x53,
    0x30,0x18,0xCA,0x34,0xBF,0xA2,0xC7,0x59,
    0x67,0x8F,0xBA,0x0D,0x6D,0xD8,0x2D,0x7D,
    0x54,0x0A,0x57,0x97,0x70,0x39,0xD2,0x7A,
    0xEA,0x24,0x33,0x85,0xED,0x9A,0x1D,0xE0			
};	
	
uint8_t EpdRFData[4736];
uint8_t StatusInFlash[64];		

unsigned long y_0;
fpos_t ypos;
int ypos_error = 1;
unsigned long ymax = 0;
char    strTagID[20];	

char    LedStatus[4];			

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
extern uint8_t TagID[];
extern void OTA_Init(int);
//extern uint8_t gDatarate[];

/* Spenser added */
uint8_t gTagSeq = 0;
extern uint8_t mindex;
int EnterLongSleep = 0;
/************************************************************************
** UserRegister
************************************************************************/
void UserRegister(void)
{
	int i;
	
	RADIO->DATARATE = GetRfDatarate();		//DateRate=500Kbps
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



/***********************************************
 **  JBIG Function
 ***********************************************/
/*
 * Call-back routine for merged image output
 */

char g_byFileBuffer[5000];
uint32_t g_uiPos = 0;
int line_out(const struct jbg85_dec_state *s,
             unsigned char *start, size_t len, unsigned long y, void *file)
{
	char * pszPos = &g_byFileBuffer[g_uiPos];
  int nLength;
  if (y == 0) {
    /* prefix first line with PBM header */
    //fprintf((FILE *) file, "P4\n");
    //fprintf((FILE *) file, "%10lu\n", jbg85_dec_getwidth(s));
        g_uiPos = 0;     // File start
        pszPos = &g_byFileBuffer[g_uiPos];
        nLength = sprintf(pszPos, "P4\n");
        if (nLength > 0)
        {
                g_uiPos += nLength;       
                pszPos = &g_byFileBuffer[g_uiPos];            
        }
        nLength = sprintf(pszPos, "%10lu\n", jbg85_dec_getwidth(s));
        if (nLength > 0)
        {
                g_uiPos += nLength;       
                pszPos = &g_byFileBuffer[g_uiPos]; 
        }
    /* store file position of height, so we can update it after NEWLEN */
    y_0 = jbg85_dec_getheight(s); 
    //ypos_error = fgetpos((FILE *) file, &ypos);
    if (g_uiPos < sizeof(g_byFileBuffer))
        {
                ypos_error = 0; // Succeed
                ypos.__pos = g_uiPos;
                ypos.__mbstate.__state1 = 0;
                ypos.__mbstate.__state2 = 0;
        }
        else
        {
                //print( "fgetpos error" ); 
                ypos_error = -1;  // Please see <stdio.h> for details.
        }
        //fprintf((FILE *) file, "%10lu\n", y_0); /* pad number to 10 bytes */
        nLength = sprintf(pszPos, "%10lu\n", y_0);
        if (nLength > 0)
        {
                g_uiPos += nLength;               
                pszPos = &g_byFileBuffer[g_uiPos];
        }
  }
  //fwrite(start, len, 1, (FILE *) file);
  memcpy(pszPos, start, len);
  if (len > 0)
        {
          g_uiPos += len;               
          pszPos = &g_byFileBuffer[g_uiPos];
        }
  return y == ymax - 1;
}


int JbigToPbm(unsigned char * pdata,size_t mlen )
{
	FILE *fout = stdout;
  int result;
  struct jbg85_dec_state s;

  unsigned char *inbuf, *outbuf;
  unsigned int inbuflen = 5000, outbuflen, len, cnt, cnt2;
  unsigned long xmax = 5000;
  size_t bytes_read = 0;

	inbuf = (unsigned char *) malloc(inbuflen);
  outbuflen = ((xmax >> 3) + !!(xmax & 7)) * 3;
	//outbuflen = inbuflen;
  outbuf = (unsigned char *) malloc(outbuflen);

  /* send input file to decoder */
  jbg85_dec_init(&s, outbuf, outbuflen, line_out, fout);
  result = JBG_EAGAIN;
	
//	  while ((len = fread(inbuf, 1, inbuflen, fin))) {
		//len = 581;  //data len  for EPD_DATA_296_INCH.pbm1.jbg
		
		len = mlen;			//1409;   //8180000001b-b.pbm.jbg  
		//len = 1221;  //data len  for 00002.bin85.pbm.jbg		//Source img.jbg len.  2018-06-01
		//len = 1388;  //data len  for 00001.bin85.pbm.jbg
		//len = 515;  //data len  for EPD_DATA_213_INCH85.pbm.jbg  
		//inbuf = (void *) 0x28000;
		memcpy(inbuf,(unsigned char *)pdata,len );		//Source img.jbg
	  while (1) {	
    result = jbg85_dec_in(&s, inbuf, len, &cnt);
    bytes_read += cnt;
    while (result == JBG_EOK_INTR) {
      /* demonstrate decoder interrupt at given line number */
      printf("Decoding interrupted after %lu lines and %lu BIE bytes "
	     "... continuing ...\n", s.y, (unsigned long) bytes_read);
      /* and now continue decoding */
      result = jbg85_dec_in(&s, inbuf + cnt, len - cnt, &cnt2);
      bytes_read += cnt2;
      cnt += cnt2;
    }
    if (result != JBG_EAGAIN)
      break;
  }
	
  if (result == JBG_EAGAIN || result == JBG_EOK_INTR) {
    /* signal end-of-BIE explicitely */
    result = jbg85_dec_end(&s);
    while (result == JBG_EOK_INTR) {
      /* demonstrate decoder interrupt at given line number */
      printf("Decoding interrupted after %lu lines and %lu BIE bytes "
	     "... continuing ...\n", s.y, (unsigned long) bytes_read);
      result = jbg85_dec_end(&s);
    }
  }
	
  if (result != JBG_EOK) {
    printf( "Problem with input file '%s': %s\n"
            "(error code 0x%02x, %lu = 0x%04lx BIE bytes "
	    "and %lu pixel rows processed)\n",
	    "inbuf", jbg85_strerror(result), result,
	    (unsigned long) bytes_read, (unsigned long) bytes_read, s.y);
  }

	free(inbuf);
	free(outbuf);
	
	return 0;
}

uint8_t UnzipAndWrite(uint8_t EPD_Color,uint8_t EPD_Size, uint8_t *EpdRFData, uint16_t DataLen)
{
	int J_Big_Data_Length = 0, J_Big_Red_Data_Length = 0;
//	J_Big_Data_Length = (EpdRFData[0]<<24) + (EpdRFData[1]<<16)+ (EpdRFData[2]<<8)+EpdRFData[3];
//	J_Big_Red_Data_Length = (EpdRFData[4]<<24) + (EpdRFData[5]<<16)+ (EpdRFData[6]<<8)+EpdRFData[7];
	switch(EPD_Color)
	{
		case B_W:
			printf("case B_W:\r\n");
			TagPowerUpFunction();
			CleanEPD();
			Write_EPD_FULL(EPAPER_COLOR_BLACK_WHITE,DataLen,EpdRFData);
			break;
		
		case R_B_W:
			if(EPD_Size == Size_290)
			{
				printf("case R_B_W_290:\r\n");
				TagPowerUpFunction();
				CleanEPD();
				J_Big_Data_Length = (EpdRFData[0]<<24) + (EpdRFData[1]<<16)+ (EpdRFData[2]<<8)+EpdRFData[3];
				J_Big_Red_Data_Length = (EpdRFData[4]<<24) + (EpdRFData[5]<<16)+ (EpdRFData[6]<<8)+EpdRFData[7];
				JbigToPbm(EpdRFData + 8,J_Big_Data_Length);
				Write_EPD_FULL(EPAPER_COLOR_BLACK_WHITE,4736,(uint8_t *) g_byFileBuffer+25);	
				if(J_Big_Red_Data_Length != 0x00)
				{
					JbigToPbm(EpdRFData + 8 + J_Big_Data_Length,J_Big_Red_Data_Length);
					Write_EPD_FULL(EPAPER_COLOR_RED,4736,(uint8_t *) g_byFileBuffer+25);  //g_byFileBuffer[] data for jbgtopbm85.
				}
			}
			else
			{
				printf("case R_B_W_213:\r\n");
				TagPowerUpFunction();
				CleanEPD();
			J_Big_Data_Length = (EpdRFData[0]<<24) + (EpdRFData[1]<<16)+ (EpdRFData[2]<<8)+EpdRFData[3];
			J_Big_Red_Data_Length = (EpdRFData[4]<<24) + (EpdRFData[5]<<16)+ (EpdRFData[6]<<8)+EpdRFData[7];
				JbigToPbm(EpdRFData + 8,J_Big_Data_Length);
				Write_EPD_FULL(EPAPER_COLOR_BLACK_WHITE,2756,(uint8_t *) g_byFileBuffer+25);		
				if(J_Big_Red_Data_Length != 0x00)
				{
					JbigToPbm(EpdRFData + 8 + J_Big_Data_Length,J_Big_Red_Data_Length);
//					JbigToPbm((pTagCompressData->Compressed_Data + (int)pTagCompressData->Length_Black),(int)pTagCompressData->Length_Red);
					Write_EPD_FULL(EPAPER_COLOR_RED,2756,(uint8_t *) g_byFileBuffer+25);  //g_byFileBuffer[] data for jbgtopbm85.
				}
			}

		break;
	}
	
	return 0;
}

int main(void)
{
//	uint8_t TEST[64];
	uint8_t mReturn;
	uint8_t FIFO_Return = 0;
	uint8_t RxTimeOutCnt = 0;
	uint32_t addressTest = 0;
	uint16_t Pervious_DataLen = 0;
	int PartOfOtaLength = 4736;
	extern uint8_t BeaconDataSnif[16];
	int mlen, mcnt, Z;
	int8_t Expect_Index = 0;
	int TotalOfDataLen = 0;
//	gDatarate = RF_DATARATE_500K;//DATARATE500K;//DATARATE1M;			//Assign the data rate.
	
	board_init();		//Initial hardware system
	
	TagDebug(("Compiler Date: %s  Time:%s \r\n", __DATE__, __TIME__));
	
	EPaperInit(EPAPER_MODE_DEFAULT, true);
//	EPDShowTAGDftStatus();
	initRF(); //init RF
	//TurnOn_External_RTC_Crystal();
	UserRegister();		//M8107 registers setting
  StrobeCmd(CMD_STBY);
  SetCH(GetRfChannel()); //freq 2450MHz CH100 JAMES TEST is 184 or JERRY is 200
	EPDShowTAGDftStatus(0);
	TurnOn_External_RTC_Crystal();
	RTC_SETTING();
	NVIC_EnableIRQ(SLPTIMER1_IRQn);

	BackupIOSetting();
	gState = PWRBEACON_STATE;
	GPIO0->WUN_PIN.P08 = 0;

	while(0)
	{
		printf("Temperature is %d\r\n",Temperature_measurement());
		Delay1ms(1000);
	}
//	for(Z = 0; Z < 10; Z++)
//	{
//		printf("Z = %d \r\n",Z);
//		if(Z==7)
//		{
//			Z = 7-1;
//		}
//	}
#if LED_Mode
	fifo_init(LedStatus, 4);
#else	
	//do nothing.
#endif	

	while(1) {
		switch(gState) {
		case PWRBEACON_STATE:
			if(EnterLongSleep < Starting_40s_Sleep)
			{
//				setSynTimer(GetTagWakeUpInterval(), 0);							//sleep time 40S, 1S 2018-05-15 YC
				setSynTimer(1, 3);							//sleep time 40S, 1S 2018-05-15 YC
			}
			else
			{
//				setSynTimer(2, 0);							//sleep time 40S, 1S 2018-05-15 YC
				setSynTimer(40, 3);

			}
			SetFIFOLvl(16);
				StrobeCmd(CMD_RX);
				mReturn = getpacket(900);      //FOR SPENSER.
				//mReturn = getpacket(30);		//FOR JERRY OR LONG DIATANCE.//2ms OK,  1ms fail
				
				if(Oneshoutflag) {		//Rx Beacon time out.			
						TagDebug(("PWRBEACON_STATE: Lost Beacon.\r\n"));
//						LossCnt++;
						EnterLongSleep++;
						printf("%d ",EnterLongSleep);
						if(EnterLongSleep < LongSleep_Threshold)
						{
							gState = ENTERPM1_STATE;
						}
						else
						{
							if(EnterLongSleep >= LongSleep_Threshold)
							{
								TagPowerUpFunction();
								EPDShowTAGDftStatus(3);
								TagPowerUpFunction();
								Write_QRCode_FULL();
								UpdateDisplay();
								TagPowerDownFunction();
								SLPTIMER_StopTimer(SLPTIMER1);
//								gState = ENTERPM1_STATE;
								gState = LONG_SLEEP_STATE;
							}
							
						}
//						gState = ENTERPM1_STATE;
						
				}else {
					switch(mReturn) {
					case TAG_MSG_NO_HIT_ID:
					case TAG_MSG_ONLY_SYNC:	
							gState = ENTERPM1_STATE;
							break;
					case TAG_MSG_HOPPING:
							gState = ENTERPM1_STATE;
							break;
					case TAG_MSG_GATEWAY_ID_NOT_MATCH:
					case TAG_MSG_REPEATER_ID_NOT_MATCH:		
							printf("TAG_MSG_GATEWAY_ID_NOT_MATCH\r\n");
							NonStop_Searching_New_Beacon_Src(10,200);
							gState = ENTERPM1_STATE;
							break;
					case TAG_MSG_AFTER_CHANGE_BEACON_SRC:
//								NonStop_Searching_New_Beacon_Src(10,1000);
								gState = PWRBEACON_STATE;
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

						case TAG_MSG_TO_RCV_TAG_LIST:
							gState = TAG_LIST_STATE;
							break;
						default:
//							TagDebug(("JAMES: DEFAULT.\r\n"));
//							gState = ENTERPM1_STATE;
							break;
					}
				}

				break;			

			case BEACON_STATE:
//					setSynTimer(1, 0);
					SetFIFOLvl(16);
					StrobeCmd(CMD_RX);
					//Spenser
					mReturn = getpacket(20);      //FOR SPENSER.
					//mReturn = getpacket(30);		//FOR JERRY OR LONG DIATANCE.//2ms OK,  1ms fail
					
					if(Oneshoutflag) {		//Rx Beacon time out.			

							printf("Lost Beacon...BEACON_STATE\r\n");
//							TagDebug(("BEACON_STATE: Lost Beacon.\r\n"));
							LossCnt++;
							gState = ENTERPM1_STATE;
							
					}else {
						switch(mReturn) {
							case TAG_MSG_NO_HIT_ID:
							case TAG_MSG_ONLY_SYNC:	
								gState = ENTERPM1_STATE;
								break;
							case TAG_MSG_HOPPING:
									gState = ENTERPM1_STATE;
									break;
							case TAG_MSG_GATEWAY_ID_NOT_MATCH:
							case TAG_MSG_REPEATER_ID_NOT_MATCH:	
									printf("TAG_MSG_GATEWAY_ID_NOT_MATCH\r\n");
									NonStop_Searching_New_Beacon_Src(10,200);
									gState = ENTERPM1_STATE;
									break;
							case TAG_MSG_AFTER_CHANGE_BEACON_SRC:
//								NonStop_Searching_New_Beacon_Src(10,1000);
								gState = PWRBEACON_STATE;
								break;							

							case TAG_MSG_RX_TIMEOUT:
								LossCnt++;
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

			case HEADER_STATE:
				SetFIFOLvl(64);
//				printf("HEADER_STATE...\r\n");
				StrobeCmd(CMD_RX);
				//Spenser
				mReturn = getpacket(20);	      //FOR SPENSER
				//mReturn = getpacket(50);		//FOR JERRY OR LONG DIATANCE.//3ms OK, 1ms fail
				switch(mReturn){
					case TAG_MSG_HIT_ID:
						gState = DATA_STATE;
						break;
					case TAG_MSG_OTA_HIT_ID:
						printf("TAG_MSG_OTA_HIT_ID  DATA_STATE.\r\n");
						gState = DATA_STATE;//OTA_STATE;
						break;
					case TAG_MSG_RX_TIMEOUT:
						printf("Receive Header timer out.\r\n");
//						replypacket(mReturn);
						LossCnt++;
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

				switch(GetRfDatarate()){
						case RF_DATARATE_1M:
							startTimer(TIMER1, 100);		//100ms
							break;

						case RF_DATARATE_500K:
							startTimer(TIMER1, 150);		//135ms, Cover all data transmit time
							break;
						case RF_DATARATE_250K:
							startTimer(TIMER1, 200);		//200ms
							
							break;	
				}
				while(mlen > 0) {
//					StrobeCmd(CMD_RX);
					mReturn = getdatapacket();
					
					if(mReturn != TAG_MSG_SINGAL_PACKET_OK)
					{
						printf("mReturn = %02x ",mReturn);
						break;
					}

//					if(Oneshoutflag) {
//						printf("DATA_STATE___RX data timer out.\r\n");
//						memset(EpdRFData,0x00,4736);
//						mReturn = TAG_MSG_PACKET_LOST;
//						gState = ENTERPM1_STATE;
//						break;
//					}
					//RxPacket();
					mlen -= 62;		//two bytes header						
				}
				Timer_StopTimer(TIMER1);
				/*-----------------------------------------------------*/

//				if(Oneshoutflag || (mReturn == TAG_MSG_PACKET_LOST) ||(mReturn == TAG_MSG_STATE_NOT_RIGHT)) {	//Don't need update EPD if data error.
//					LossCnt++;
//					StrobeCmd(CMD_SLEEP);	
//					gState = ENTERPM1_STATE;
//				}else {
//					replypacket(TAG_MSG_RX_COMPLETED);
//					/* RSSI_measurement */
//					//RSSI_measurement();
//					StrobeCmd(CMD_SLEEP);				
//					gState = EPDDATA_STATE;
//				}
				
				switch(mReturn){
					case TAG_MSG_PACKET_LOST:
					case TAG_MSG_STATE_NOT_RIGHT:
					case TAG_MSG_RX_TIMEOUT:
//						printf("TAG_MSG_RX_TIMEOUT.\r\n");
						LossCnt++;
						StrobeCmd(CMD_SLEEP);
						gState = ENTERPM1_STATE;
						break;
					case TAG_MSG_SINGAL_PACKET_OK:
//						printf("TAG_MSG_SINGAL_PACKET_OK.\r\n");

							ReplyUpdateResult(TAG_MSG_RX_COMPLETED);
//						for(Z = 0; Z < 3; Z++)
//						{
//							replypacket(TAG_MSG_RX_COMPLETED);
//						}
//						replypacket(TAG_MSG_RX_COMPLETED);
						StrobeCmd(CMD_SLEEP);				
					  gState = EPDDATA_STATE;
					break;
					default:
						gState = ENTERPM1_STATE;
						break;								
				}

				break;

			case EPDDATA_STATE:
				gState = EPDREFRESH_STATE;
				break;
				
			case 	EPDREFRESH_STATE:
				printf("EPDREFRESH_STATE..\r\n");
			
//				for(Z=0; Z<10; Z++)
//				{
//					while(1)
//					{
//						Z = 0x86;
//						printf("In While Loop..\r\n");
//						break;
//					}
//					if(Z == 0x86)
//					{
//						printf("0x86..\r\n");
//						break;
//					}
//					printf("END While Loop..\r\n");
//				}
//				printf("For Loop End..\r\n");
				StrobeCmd(CMD_STBY);	
//				for(Z=0; Z<4736; Z++)
//				{
//					printf("%02x ",EpdRFData[Z]);
//				}	
//				Flash_SectorErase(0x20000);
//				Flash_PageErase(0x20000);
//				memset(EpdRFData,0x88,4736);
//				printTime();
//				EraseSectorForOtaData(0x08,0x20000);
////				printf("1..\r\n");
//				addressTest = 0x20000;
//				printf("addressTest = %x ",addressTest);
////				printf("2..\r\n");
//				WriteOTADataToFlash(addressTest,4736,EpdRFData);
////				printf("3..\r\n");
//				addressTest = addressTest + 4736;
//				printf("addressTest = %x ",addressTest);
//				WriteOTADataToFlash(addressTest,4736,EpdRFData);
//				addressTest = addressTest + 4736;
//				printf("addressTest = %x ",addressTest);
//				WriteOTADataToFlash(addressTest,4736,EpdRFData);
//				addressTest = addressTest + 4736;
//				printf("addressTest = %x ",addressTest);
//				WriteOTADataToFlash(addressTest,4736,EpdRFData);
//				addressTest = addressTest + 4736;
//				printf("addressTest = %x ",addressTest);
//				WriteOTADataToFlash(addressTest,4736,EpdRFData);
//				addressTest = addressTest + 4736;
//				printf("addressTest = %x ",addressTest);
//				WriteOTADataToFlash(addressTest,4736,EpdRFData);
//				addressTest = addressTest + 4736;
//				printf("addressTest = %x ",addressTest);
//				WriteOTADataToFlash(addressTest,4352,EpdRFData);
////				addressTest = addressTest + 4736;
////				printf("addressTest = %x ",addressTest);
////				WriteOTADataToFlash(addressTest,3584,EpdRFData);
//				printTime();
////				printf("EraseSectorForOtaData OK..\r\n");
#if LED_Mode
			
				Z = GetColor();
				FIFO_Return = fifo_write( &Z, 1);
				if(FIFO_Return == 1)
				{
					GPIO_PIN_LedSet(GetColor());
					TagDebug(("FIFO_Return = 1\r\n"));
				}
				Show_Fifo_Data();
#else	
	//do nothing.
#endif
				
				EnterLongSleep = 0;
				LossCnt = 0;
				TagPowerUpFunction();
				UnzipAndWrite(GetEpdColor(),GetEpdSize(),EpdRFData,DataLen);
//				WhiteDM();
////				WriteRAMqr_FULL(DataLen, EpdRFData);
//				Write_EPD_FULL(EPAPER_COLOR_BLACK_WHITE,DataLen,EpdRFData);	
				UpdateDisplay();
//				EPDShowTAGDftStatus(1);
				
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
				printf("ghopcount = %d\r\n", ghopcount);
				StrobeCmd(CMD_STBY);  
				Z=ConvertTemperature_measurement();		//Test point 2018-08-24
				printf("Temperature = %d\r\n", Z);
			
				if(ghopcount == 1){		//Hopping
					printf("Hop to channel = %d\r\n", CHTable[gCHindex]);
					StrobeCmd(CMD_STBY);  
					SetCH(CHTable[gCHindex]); //freq 2450MHz CH100
					ghopcount = 0;
				}
				else if(ghopcount > 0)
				{
					ghopcount--;
				}			
			
				if(gTagDonotSleep == 1)
				{
					gState = PWRBEACON_STATE;
				}
				else
				{
					
//**** Start  Wakeup MCU to let P0_08 = 0.  *****  				
					mcnt =0;
					while( debounce(GPIO0->DATA_PIN.P08) == 0)
					{
						Delay1ms(100);    // This delay is for send last 1byte: 10bits*1/115200 = 87us				
						mcnt++;
						if(mcnt == 3)
						{
#if LED_Mode
							fifo_read( &Z, 1);
							GPIO_PIN_LedClear( Z);
							Show_Fifo_Data();
#else	
							//do nothing.
#endif	
						}					
						if(mcnt == 40)
						{
							EPDShowTAGDftStatus(2);
						}		
						if(mcnt >= 70)
						{
//								NVIC_SystemReset();
								TagPowerUpFunction();
							  sprintf(strTagID,"%#2X:%#2X:%#2X:%#2X:%#2X",TagID[0],TagID[1],TagID[2],TagID[3],TagID[4]);		//
								//sprintf(strTagID,"%03d:%03d:%03d:%03d:%03d",TagID[0],TagID[1],TagID[2],TagID[3],TagID[4]);							
								EPaperShowTagStatus( _T(strTagID), _T("Update code..."), _T("Auto Reset."), _T("Happy GO... GO... GO..."));		//TagID				
								TagPowerDownFunction();	
						}					
					}
					
					if(mcnt >= 40 && mcnt < 60)  //if(mcnt >= 2 && mcnt < 10)
					{
						TagDebug(("P0_08 = 0, mcnt=%d \r\n" , mcnt ));
						TagDebug(("Set Wakeup pin = P0_08\r\n"));
						SLPTIMER_StopTimer(SLPTIMER1);
						ClearIOSetting();
						EnterPM1();  //Sleep command						
						RestoreIOSetting();			
						EPDShowTAGDftStatus(1);
						SLPTIMER_Initial(SLPTIMER1, (6533 - 6), 1, 1);
//						Restart_Binding_Process(); //if need.
					}				
					else if(mcnt >= 70) //else if(mcnt >= 60)
					{
						NVIC_SystemReset();
//							while(1)
//							{
//								TagDebug(("P0_08 = 0, mcnt=%d \r\n" , mcnt ));
//								Delay1ms(0x2000);
//								TagDebug(( "Update User code ....\r\n"));

//								OTA_Init(256);
//								
//								Delay1ms(0x2000);
//								TagDebug(( "Reset().. \r\n"));
//								Delay1ms(0x2000);
//								NVIC_SystemReset();										
//								//while(1);								
//							}									
					}						
					TagDebug(("TagDebug: %s  ", "--PM1--  "));
					printTime();
					Delay1ms(1);
					StrobeCmd(CMD_SLEEP);

#if LED_Mode
					//do nothing
#else	
					ClearIOSetting();
#endif	
					EnterPM1();  //Sleep command			2018-03-26 OTA test disable.
					//EnterPM3();  //Sleep command					
					//printf("after EnterPM3...\r\n");
					RestoreIOSetting();		
					memset(EpdRFData,0x00,4736);					
					StrobeCmd(CMD_STBY);                          // MCU enter PM1

					if(LossCnt >= 1) { //LossCnt >= 6
//						LossCnt = 0;
						gState = PWRBEACON_STATE;
					}
					else 
					{
//						if(EnterLongSleep < 10)
//						{
//							gState = PWRBEACON_STATE;
//						}
//						else
//						{
//							LossCnt = 0;
							gState = BEACON_STATE;//PWRBEACON_STATE;//BEACON_STATE;
//						}
						
					}
//					printf("WAKE UP!!!!!\r\n");
//					if(LossCnt != 0) {
//						LossCnt = 0;
//						gState = PWRBEACON_STATE;
//					}
//					else 
//					{
//						gState = BEACON_STATE;//PWRBEACON_STATE;//BEACON_STATE;
//					}

				}

				break;
			
				
			case TAG_LIST_STATE:
					SetFIFOLvl(64);
//				printf("TAG_LIST_STATE........................................\r\n");
//					setSynTimer(1, 0);
					StrobeCmd(CMD_RX);
					//Spenser
					mReturn = getpacket(30);      //FOR SPENSER.
					//mReturn = getpacket(30);		//FOR JERRY OR LONG DIATANCE.//2ms OK,  1ms fail
					
					if(Oneshoutflag) {		//Rx Beacon time out.			

					printf("Lost Beacon...TAG_LIST_STATE\r\n");
					LossCnt++;
					gState = ENTERPM1_STATE;
							
					}else {
						switch(mReturn) {
							case TAG_MSG_HIT_ID:
//								printf("TAG_MSG_HIT_ID.......................................\r\n");
								/* Spenser - multitags update */
								StrobeCmd(CMD_STBY);
								//printf("gTagSeq = %d\n", gTagSeq);
								if(GetRfDatarate() == RF_DATARATE_500K) {
									if(gTagSeq > 0){
										Delay1ms(gTagSeq*130);		//128(120.3: 127ms) (132 : 138)
										Delay1us(gTagSeq*250);		//850(500) 850
//										Delay1ms(gTagSeq*128);		//128(120.3: 127ms) (132 : 138)
//										Delay1us(gTagSeq*850);		//850(500) 850
									}
								}
								/*-----------------------------*/
								gState = HEADER_STATE;
								break;
							case TAG_MSG_OTA_HIT_ID:
//								printf("TAG_MSG_OTA_HIT_ID.......................................\r\n");
								gState = OTA_STATE;//OTA_STATE;
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
								LossCnt++;
//								RxTimeOutCnt++;
//								if(RxTimeOutCnt >= EPD_RETRY_LIMIT)
//								{
//									ReadAndWriteRebootCount();
////									NVIC_SystemReset();
//								}
								gState = 	ENTERPM1_STATE;
								break;
		
							case TAG_MSG_REPORT_STATUS_OK:
							case TAG_MSG_RCV_UNBOUND_MSG_OK:	
								printf("TAG_MSG_RCV_UNBOUND_MSG_OK\r\n");
								gState = ENTERPM1_STATE;
								break;	
							case TAG_MSG_CHANGE_BEACON_SRC_OK:	
								NonStop_Searching_New_Beacon_Src(10,1000);
								printf("TAG_MSG_CHANGE_BEACON_SRC_OK\r\n");
								gState = ENTERPM1_STATE;
								break;	
							default:
								gState = ENTERPM1_STATE;
								break;
						}
					}

				break;	
			
			case LONG_SLEEP_STATE:
//				GetPacketCount();
				printf("LONG_SLEEP_STATE\r\n");
//				SetAlarm(0,5);
				SetAlarm(GetLongSleepAlarmHour(),GetLongSleepAlarmMin());
				gState = ENTERPM1_STATE;
				EnterLongSleep = 40;
				break;	

			case OTA_STATE:
				SetFIFOLvl(64);
//				printf("OTA_STATE\r\n");
				for(Expect_Index = 0; Expect_Index < GetTotalOfPacket(); Expect_Index++) //7
				{
					printf("Expect_Index = %d\r\n", Expect_Index);
					//RCV OTA HEADER
//					Start_OTA_Header_Process();
					
					
	//				printf("HEADER_STATE...\r\n");
					StrobeCmd(CMD_RX);
					//Spenser
					mReturn = getpacket(1450);
					if(mReturn == TAG_MSG_OTA_HIT_ID)
					{
						mlen = (int)DataLen; //OTA DATA TOTAL
						mindex = 0;
						Oneshoutflag = 0;		//TIme out flag
	//					Delay1ms(1);
						//GetOtaPacketIndex();
						//RCV OTA DATA if OTA hit ID
						switch(GetRfDatarate())
						{
								case RF_DATARATE_1M:
									startTimer(TIMER1, 100);		//100ms
									break;

								case RF_DATARATE_500K:
									startTimer(TIMER1, 500);		//135ms, Cover all data transmit time
									break;
								case RF_DATARATE_250K:
									startTimer(TIMER1, 200);		//200ms
									
									break;	
						}
						
	//					while(mlen > 0) 
						while(mlen > 0) 	//4736 B
						{
		//					StrobeCmd(CMD_RX);
							mReturn = getdatapacket();
							if( mReturn == TAG_MSG_RX_TIMEOUT)
							{
								break;
							}
							else if(mReturn == TAG_MSG_PACKET_LOST)
							{
	//							mlen = DataLen - ((GetCurrentIndex() + 1) * 62);
								break;
							}
							else
							{
								mlen -= 62;
							}
	//						
	//						if(mReturn != TAG_MSG_SINGAL_PACKET_OK)
	//						{
	//							printf("mReturn = %02x ",mReturn);
	//							break;
	//						}

	//						mlen -= 62;		//two bytes header						
						}
						Timer_StopTimer(TIMER1);
	//					PartOfOtaLength = 4736;
						/*-----------------------------------------------------*/						
					}


					switch(mReturn){
						case TAG_MSG_PACKET_LOST:
						case TAG_MSG_STATE_NOT_RIGHT:
							mReturn = TAG_MSG_SINGAL_PACKET_OK;
//							printf("TAG_MSG_PACKET_LOST....................\r\n");
//							Delay1ms((75 - GetCurrentIndex()) * 1.629);
							StrobeCmd(CMD_STBY);	
//							WriteOTADataToFlash( (Tag_OTA_File_StartPosition + Expect_Index*DataLen),DataLen,EpdRFData);				
							replyOTAStatus(TAG_MSG_OTA_PACKET_LOST, GetOtaPacketIndex(),(77 - GetCurrentIndex()));
							printf("GetOtaPacketIndex()= %d\r\n", GetOtaPacketIndex());
//							replyOTAStatus(TAG_MSG_OTA_PACKET_LOST, GetOtaPacketIndex(),10);
							Expect_Index = Expect_Index - 1;		
							SetCurrentIndexZero();
//							continue;
//							printf("CASE TAG_MSG_PACKET_LOST....................\r\n");
							break;
						
						case TAG_MSG_RX_TIMEOUT:
						case TAG_MSG_NO_HIT_ID:	
							SetOtaStatusDone();
							SLPTIMER_Initial(SLPTIMER1, (6533 - 6), 1, 1);
//							setSynTimer(1, 3);
							StrobeCmd(CMD_SLEEP);
							gState = ENTERPM1_STATE;
							break;
						
						case TAG_MSG_SINGAL_PACKET_OK:
							if(Expect_Index == GetOtaPacketIndex()) //EXCEPT = RCV
							{
								//write nvm and reply	
								replyOTAStatus(TAG_MSG_RX_COMPLETED, GetOtaPacketIndex(), 10);
								if(Expect_Index == 0)
								{
									EraseSectorForOtaData(0x08,Tag_OTA_File_StartPosition);
									Pervious_DataLen = DataLen;
								}
//								memset(EpdRFData,0x99,4736);
								StrobeCmd(CMD_STBY);
								
								if(Expect_Index == (GetTotalOfPacket() - 1))
								{
									WriteOTADataToFlash( (Tag_OTA_File_StartPosition + Expect_Index*Pervious_DataLen),DataLen,EpdRFData);
									printf("address0 = %x DataLen = %d\r\n",(Tag_OTA_File_StartPosition + Expect_Index*Pervious_DataLen),DataLen);									
								}
								else
								{
									WriteOTADataToFlash( (Tag_OTA_File_StartPosition + Expect_Index*DataLen),DataLen,EpdRFData);	
									printf("address = %x DataLen = %d\r\n",(Tag_OTA_File_StartPosition + Expect_Index*DataLen),DataLen);
								}									
//								printf("address = %x DataLen = %d\r\n",(0x20000 + Expect_Index*DataLen),DataLen);		
								TotalOfDataLen = TotalOfDataLen + DataLen;
								printf("GetOtaPacketIndex() = %d \r\n",GetOtaPacketIndex());
								if(Expect_Index == (GetTotalOfPacket() - 1)) //RCV all 4736
								{
									printf("Last Index = %d \r\n",GetOtaPacketIndex());
//									printf("TotalOfDataLen= %d\r\n", TotalOfDataLen);
//									printf("GetBinFileSize()= %d\r\n", GetBinFileSize());
									if(TotalOfDataLen == GetBinFileSize())
									{
										printf("TotalOfDataLen= %d\r\n", TotalOfDataLen);
										printf("GetBinFileSize()= %d\r\n", GetBinFileSize());
										mReturn = TAG_MSG_OTA_RCV_DONE;
									}
								}
								
							}
							else if((Expect_Index - GetOtaPacketIndex()) == 1) // EXCEPT > RCV means repeated
							{
								//reply repeated
								StrobeCmd(CMD_STBY);	
								replyOTAStatus(TAG_MSG_OTA_PACKET_REPEATED, GetOtaPacketIndex(), 10);	
								WriteOTADataToFlash( (Tag_OTA_File_StartPosition + Expect_Index*DataLen),DataLen,EpdRFData);				
								Expect_Index = Expect_Index - 1;	
							}
							else if(Expect_Index < GetOtaPacketIndex())
							{
								//reply false
								StrobeCmd(CMD_STBY);	
//								WriteOTADataToFlash( (Tag_OTA_File_StartPosition + Expect_Index*DataLen),DataLen,EpdRFData);				
								replyOTAStatus(TAG_MSG_OTA_PACKET_REPEATED, GetOtaPacketIndex(), 10);	
								Expect_Index = Expect_Index - 1;	
							}
							else
							{
								
							}
//	//						printf("TAG_MSG_SINGAL_PACKET_OK.\r\n");
////							EraseSectorForOtaData(0x02,0x20000);
//							StrobeCmd(CMD_STBY);	
//							WriteOTADataToFlash( (0x20000 + Expect_Index*DataLen),DataLen,EpdRFData);				
//							replyOTAStatus(TAG_MSG_RX_COMPLETED, GetOtaPacketIndex(), 10);
////							StrobeCmd(CMD_SLEEP);				
////							gState = EPDDATA_STATE;
						break;
						default:
							gState = ENTERPM1_STATE;
							break;								
					}
					if(mReturn != TAG_MSG_SINGAL_PACKET_OK || mReturn == TAG_MSG_OTA_RCV_DONE)
					{
						break;
					}
				}
				if(mReturn == TAG_MSG_SINGAL_PACKET_OK || mReturn == TAG_MSG_OTA_RCV_DONE)
				{
					SetOtaStatusDone();
					gState = OTA_INIT_STATE;
					printf("GetBinFileSize()= %d\r\n", GetBinFileSize());
				}
				else
				{
					printf("LAST ELSE.\r\n");
						SetOtaStatusDone();
						SLPTIMER_Initial(SLPTIMER1, (6533 - 6), 1, 1);
						StrobeCmd(CMD_SLEEP);
						gState = ENTERPM1_STATE;
				}
				break;			
				
				
			case OTA_INIT_STATE:
				Delay1ms(0x2000);
				TagDebug(( "OTA DATA is ready to init.\r\n"));
				
				if((GetBinFileSize() % Page_Size) != 0)
				{
					OTA_Init((GetBinFileSize() / Page_Size) + 1);
				}
				else
				{
					printf("(GetBinFileSize()) = %d\r\n", (GetBinFileSize()));
					printf("(Page_Size) = %d\r\n", (Page_Size));
					printf("(GetBinFileSize() / Page_Size) = %d\r\n", (GetBinFileSize() / Page_Size));
					OTA_Init(GetBinFileSize() / Page_Size);
				}

				
				Delay1ms(0x2000);
				TagDebug(( "Reset().. \r\n"));
				Delay1ms(0x2000);
				NVIC_SystemReset();			
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

