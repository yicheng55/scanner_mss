#include <math.h>
#include "main.h"
#include "RptProc.h"
#include "RptDataFlash.h"
#include "RptComm.h"

/* Jerry add */
RPT_GLOBAL_VARIABLE RptGlblVar;

#if 0
RPT_DEFAULT_SETTING RptDefSet;

void Rpt_RxTask(void* parameter);
void Rpt_TxTask(void* parameter);

Task_Struct tasks[] =   
{  
	{0, 500, 500, Rpt_RxTask},  //????????????  
	{0, 500, 500, Rpt_TxTask},  
	//{0, 500, 500, task3},
	//{0,10,10,task4}  
};

uint32_t task_count = sizeof(tasks) / sizeof(Task_Struct); 

float Rpt_CalcDistByRSSI(int8_t rssi)    
{    
    uint8_t A = 47;  
    float n = 4.5;  
     
    uint8_t iRssi = abs(rssi);  
    float power = (iRssi - A) / (10 * n);     
		
    return pow(10, power);    
}    

uint8_t rssi_turn_dbm(uint8_t Rssi)
{
	uint8_t dbm ;
	
	//printf("RSSI = %d ", Rssi);
	
	if(Rssi >= 150 && Rssi <= 174){
		Rssi = (Rssi + 12) % 6;
		dbm = 50 + Rssi;
	}else if(Rssi >= 137 && Rssi <= 149){
		Rssi = (Rssi + 12) % 6;
		dbm = 60 + Rssi;
	}else if(Rssi >= 125 && Rssi <= 136){
		Rssi = (Rssi + 12) % 6;
		dbm = 65 + Rssi;
	}
	else if(Rssi >= 112 && Rssi <= 124){
		Rssi = (Rssi + 12) % 6;
		dbm = 70 + Rssi;
	}else if(Rssi >= 100 && Rssi <= 111){
		Rssi = (Rssi + 12) % 6;
		dbm = 75 + Rssi;
	}
	else if(Rssi >= 87 && Rssi <= 99){
		Rssi = (Rssi + 12) % 6;
		dbm = 80 + Rssi;
	}else if(Rssi >= 75 && Rssi <= 86){
		Rssi = (Rssi + 12) % 6;
		dbm = 85 + Rssi;
	}
	else if(Rssi >= 62 && Rssi <= 74){
		Rssi = (Rssi + 12) % 6;
		dbm = 90 + Rssi;
	}else if(Rssi >= 50 && Rssi <= 61){
		Rssi = (Rssi + 12) % 6;
		dbm = 95 + Rssi;
	}
	else if(Rssi >= 37 && Rssi <= 49){
		Rssi = (Rssi + 12) % 6;
		dbm = (100 + Rssi);
	}else if(Rssi >= 25 && Rssi <= 26){
		Rssi = (Rssi + 12) % 6;
		dbm = (105 + Rssi);
	}
	else if(Rssi > 12 && Rssi <= 25){
		Rssi = (Rssi + 12) % 6;
		dbm = 110 + Rssi;
	}else if(Rssi > 0 && Rssi <= 11){
		Rssi = (Rssi + 12) % 6;
		dbm = 115 + Rssi;
	}

	return dbm;
}

/**
  * @brief      negative number turn  positive number
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used negative number turn  positive number.
  */
int n_turn_p(int num)  
{  
    if (num < 0){  
      num = ~(num - 1);  
    }else if (num >0)  {  
      num = ~num + 1;  
    } 
		
    return num;  
} 

/**
  * @brief      Compare packet data 
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used array data is the same of Agent default Information.
  */
bool compare_array_date(uint8_t *src_array, uint8_t *dest_array, unsigned int array_size)
{
		uint8_t i;
		
		if(dest_array != NULL){
			for(i = 0; i < array_size; i++){
				if(src_array[i] != dest_array[i]){
					return false;
				}
			}
		}
		
		return true;
}

/**
  * @brief      Check the Array data is zero
  * @param[out] ProtocolHeader
  * @return     None
  * @details    This API is used array data is zero, if zero return true.
  */
bool check_array_zero(uint8_t *array, unsigned int array_size)
{
		uint8_t i;
		
		if(array != NULL){
			for(i = 0; i < array_size; i++){
				if(array[i] != 0){
					return false;
				}
			}
		}
		
		return true;
}

/**
  * @brief      change timer value.
	* @param[in]  timer is the base address of TIMER module.
  * @param[in]  msec is the change current use timer value.
  * @return     None
  * @details    This API is used reset timer value.
  */
void RstTimerValue(TIMER_Type *timer, uint32_t msec)
{
		timer->VALUE = ((SystemCoreClock / 1000) - 3) * msec;
}

/**
  * @brief      Settup TIMER1.
  * @param[in]  msec is the set use timer value.
  * @return     None
  * @details    This API is used settup timer1.
  */
void SetOneShotTimer1(uint32_t msec)
{
		NVIC_DisableIRQ(TIMER1_IRQn); /* Disable Timer Interrupt */
	
		RptGlblVar.Tmr1OneShoutFlag = 0;
	
		TIMER1->CTRL = (0 << 3) | (0 << 0); //timer enable & interrupt
	
		TIMER1->INTCLEAR = 1; //clear timer interrupt
		TIMER1->RELOAD = ((SystemCoreClock / 1000) - 3) * msec;
		TIMER1->VALUE = ((SystemCoreClock / 1000) - 3) * msec; //1ms for Xtal = 16MHz 
			
		TIMER1->CTRL = (1 << 3) | (1 << 0); //timer enable & interrupt
	
		NVIC_EnableIRQ(TIMER1_IRQn);  /* Enable Timer Interrupt */
}

void Rpt_DefaultSetting(void)
{
	RPT_DEFAULT_SETTING _RptDefSet;

	_RptDefSet.Gchn = GW_CHN;
	_RptDefSet.Tchn = TAG_CHN;
	_RptDefSet.RptDataRate = RF_DATARATE_500K;
	_RptDefSet.RptVerMJR = RPT_VERSION_MAJOR;
	_RptDefSet.RptVerMINR = RPT_VERSION_MINOR;
	_RptDefSet.RptVerRLSE = RPT_VERSION_RELEASE;
	_RptDefSet.RptVerBLD = RPT_VERSION_BUILD;
	_RptDefSet.RptReSync = 0;
	_RptDefSet.RptBatLv = 100;
	_RptDefSet.StartSec = GetStandbyTimeInSecond();
	
	_RptDefSet.RptID[0] = RPT_ID1;
	_RptDefSet.RptID[1] = RPT_ID2;
	_RptDefSet.RptID[2] = RPT_ID3;
	_RptDefSet.RptID[3] = RPT_ID4;
	_RptDefSet.RptID[4] = RPT_ID5;
	
	memcpy(&RptDefSet, &_RptDefSet, sizeof(RPT_DEFAULT_SETTING));
}

/**
  * @brief      change RF channel
  * @param[in]  Channel is wants to change the value of the RF channel
  * @return     None
  * @details    This API is used change RF channel.
  */
void Rpt_SetRF_Channel(uint8_t Channel)
{
		StrobeCmd(CMD_STBY);
		SetCH(Channel);
		Delay10us(1);
}

void Rpt_RxTask(void* parameter)  
{ 
	RptGlblVar.Tmr1OneShoutFlag = 0;

	do{	
		Rpt_RxTask_Proc(RptGlblVar.gState);
	}while(RptGlblVar.Tmr1OneShoutFlag != 1);
	
	RptGlblVar.gState = BEACON_STATE;
	Rpt_SetRF_Channel(RptDefSet.Tchn); //Change RF Channel of Tag	
}  

void Rpt_TxTask(void* parameter)  
{  
	RptGlblVar.Tmr1OneShoutFlag = 0;

	//do{
		Rpt_TxTask_Proc();
	//}while(RptGlblVar.Tmr1OneShoutFlag != 1);
	
	Rpt_SetRF_Channel(GW_CHN);
	
	RptGlblVar.AccuTagRsltLen = RptGlblVar.RxUpdDataDone = 0; //clear 
	
	while(RptGlblVar.Tmr1OneShoutFlag == 1);
}

/**************************************************************************************
* FunctionName   : TaskRemarks()
* Description    : Taskflag handle
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TaskRemarks(void)
{
	uint8_t i;

	for (i = 0; i < task_count; ++i) //Task-by-task time processing
	{
		if (tasks[i].TimerSlice)  //Timer isn't zero
		{
			--tasks[i].TimerSlice; //Tiemr - 1
			if (0 == tasks[i].TimerSlice) //Timer is zero
			{
				tasks[i].isRun = 0x01; //can run the task
				tasks[i].TimerSlice = tasks[i].SliceNumber; //Recovery timer
				RptGlblVar.Tmr1OneShoutFlag = 1;
			}
		}
	}
}

/*
 * FunctionName   : TaskProcess()
 * Description    : Task processing
 * EntryParameter : None
 * ReturnValue    : None
 */
void Task_Process(void) 
{
	uint8_t i = 0;  
	
	for (i = 0; i < task_count; ++i) //Task-by-task time processing
	{  
		if (tasks[i].isRun) //Timer isn't zero
		{  
			tasks[i].TaskPointer(NULL); //run the task
			tasks[i].isRun = 0;//clear task flag
		}  
	} 
}

void Rpt_Process1(void)
{
	switch(RptGlblVar.gMode) {
		case RPT_PWRON_MODE:
			do{
				Rpt_RxTask_Proc(RptGlblVar.gState);
			}while(RptGlblVar.PwrOnSyncBeacon != 1);
			RptGlblVar.PwrOnSyncBeacon = 0;
			break;
			
		case RPT_TASK_MODE:
			while(1){
				Task_Process();
				
				if(RptGlblVar.RtnPwrModeCnt >= 10){
					break;
				}
			}
			break;
	}
}

void Rpt_Task_Process(void) 
{
	uint8_t DelayBeacon = RPT_ID5;
	uint16_t Tmr1Cnt = 0;
	
	//***************** Rpt RX Mode ****************
	while(RptGlblVar.Tmr1OneShoutFlag != 1);
	RptGlblVar.Tmr1OneShoutFlag = 0;
	
	do{
		Rpt_RxModeTask(RptGlblVar.gState); //receive Gateway RF packet
	}while(RptGlblVar.Tmr1OneShoutFlag != 1);
	
	//***********************************************
	
	//***************** Rpt TX Mode *****************
	RptGlblVar.Tmr1OneShoutFlag = 0; //clear Timer1 interrupt flag
	
	// Recount Timer1 of Tx mode
	/*Tmr1Cnt = ((RPT_TASK_TIMER_VAL - (RPT_END_EARLY_TIMER - 1)) - 
										(RPT_TASK_TIMER_VAL - (TIMER1->VALUE / ((SystemCoreClock / 1000) - 3))));
	*/
	Tmr1Cnt = ((RPT_TASK_TIMER_VAL - RPT_END_EARLY_TIMER - 1) - 
										(RPT_TASK_TIMER_VAL - TASK_CUR_RUNTIME(TIMER1)));
	RstTimerValue(TIMER1, Tmr1Cnt);
	
	GREEN_LED_HIGH();
	
	/* Confirm completion of receiving updates, trigger flag */
	if(RptGlblVar.UpdCntTmp1 != 0){
		RptGlblVar.RxUpdDataDone = 1;
	}
				
	//Change RF Channel of Tag
	Rpt_SetRF_Channel(RptDefSet.Tchn); 

	//Delay send beacon, 1ms + (RepeaterID * 10 us)
	Delay1ms(1); 
	Delay10us(DelayBeacon);
	
	//Repeater Tx Task process
	Rpt_TxTask_Proc(); 
	
	//Tx task end, change RF channel of gateway
	Rpt_SetRF_Channel(RptDefSet.Gchn); 
	
	//Clear Repeater rx update data total length
	RptGlblVar.MultiTagCnt = RptGlblVar.UpdCntTmp1 = 
		RptGlblVar.UpdCntTmp2 = RptGlblVar.RxUpdBufPtrAddr = 0; 
	
	GREEN_LED_LOW();
	//************************************************
}


/**
  * @brief      Repeater main process 2.
  * @param[in]  None
  * @return     None
  * @details    This API is used perform repeater actions.
  */
void Rpt_Process2(void)
{
	uint16_t Tmr1Cnt = 0;
	uint8_t DelayBeacon = RPT_ID5;
	
	switch(RptGlblVar.gMode) {
		case RPT_PWRON_MODE:
			YELLOW_LED_LOW();
			GREEN_LED_LOW();
			do{
				Rpt_RxTask_Proc(RptGlblVar.gState);
			}while(RptGlblVar.PwrOnSyncBeacon != 1);
			RptGlblVar.PwrOnSyncBeacon = 0; //clear power on beacon sync flag
			break;

		case RPT_TASK_MODE:
			while(1){
				Rpt_Task_Process();
				if(RptGlblVar.gMode == RPT_PWRON_MODE) {
					break;
				}
			}
			break;
				
		case RPT_RX_MODE:	
			while(RptGlblVar.Tmr1OneShoutFlag != 1);
			RptGlblVar.Tmr1OneShoutFlag = 0;
			
			do{
				Rpt_RxTask_Proc(RptGlblVar.gState);
			}while(RptGlblVar.Tmr1OneShoutFlag != 1);
			
			RptGlblVar.gMode = RPT_TX_MODE;
			break;
			
		case RPT_TX_MODE:	
			while(RptGlblVar.Tmr1OneShoutFlag != 1);//{
				RptGlblVar.Tmr1OneShoutFlag = 0;
				
				/*Tmr1Cnt = ((RPT_TASK_TIMER_VAL - (RPT_END_EARLY_TIMER - 1)) - 
										(RPT_TASK_TIMER_VAL - (TIMER1->VALUE / ((SystemCoreClock / 1000) - 3))));
				*/
				RstTimerValue(TIMER1, RPT_TASK_TIMER_VAL - (RPT_END_EARLY_TIMER - 1)); //490
				
				Rpt_SetRF_Channel(RptDefSet.Tchn); //Change RF Channel of Tag
	
				Delay1ms(DelayBeacon);
								
				Rpt_TxTask_Proc(); //Repeater Tx Beacon & update data process
				
				Rpt_SetRF_Channel(RptDefSet.Gchn); //Change RF Channel of Gateway
				
				RptGlblVar.MultiTagCnt = RptGlblVar.UpdCntTmp1 = 
					RptGlblVar.UpdCntTmp2 = RptGlblVar.RxUpdBufPtrAddr = 0; //Clear Repeater rx update data total length
				
				//while(RptGlblVar.Tmr1OneShoutFlag != 1);
				//break;
			//}
		break;
	}
}
#endif //0

void Rpt_GlobalVar_Init(void)
{
	uint8_t mflag = 0, i = 0;
	
	//Read DataFlash parameter
	mflag = Rpt_Read_DataFlash();
	if(mflag != 0) {
		//DataFlash zero, read default setup
		Rpt_DefaultSetting();
		//write default parameter to dataflash
		Rpt_Write_DataFlash();
	}
	
	gDatarate = RptDefSet.RptDataRate; //RF data rate
	M8107RF_SetCHAN(RptDefSet.Gchn); //RF channel
	
	printf("Rpt ID = ");
	for(i = 0; i < 5; i++){
		printf("%x ", RptDefSet.RptID[i]);
	}
	printf("\r\n");
	
	// init runtime value
	memset(&RptGlblVar, 0, sizeof (RPT_GLOBAL_VARIABLE));  
	
	RptGlblVar.gMode = RPT_PWRON_MODE;//RPT_RX_MODE;//RptPowerOnMode;
	RptGlblVar.gState = PWRBEACON_STATE;//PWRBEACON_STATE;
	//SetOneShotTimer1(RPT_TASK_TIMER_VAL);
}
