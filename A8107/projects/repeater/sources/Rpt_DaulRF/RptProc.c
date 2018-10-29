#include <math.h>
#include "main.h"
#include "RptProc.h"
#include "RptDataFlash.h"
#include "RptComm.h"
#include "A7127RF_Drv.h"
#include "M8107RTC.h"

/* Jerry add */
RPT_GLOBAL_VARIABLE RptGlblVar;

/**
  * @brief      Repeater main process 2.
  * @param[in]  None
  * @return     None
  * @details    This API is used perform repeater actions.
  */
void Rpt_Process(void)
{
		switch(RptGlblVar.gMode) {
			case RPT_PWRON_MODE:
				do{
					Rpt_RxModeTask(RptGlblVar.gState);
				}while(RptGlblVar.PwrOnSyncBeacon != 1);
				RptGlblVar.PwrOnSyncBeacon = 0;
				break;

			case RPT_TASK_MODE:
				while(RptGlblVar.Tmr1OneShoutFlag != 1);
				RptGlblVar.Tmr1OneShoutFlag = 0;

				do{
					Rpt_RxModeTask(RptGlblVar.gState);
				}while(TASK_CUR_RUNTIME(TIMER1) > 3);

				Rpt_ResetRxTaskState(RptGlblVar.RtnPwrOnMode);
				break;
		}
}

void Rpt_GlobalVar_Init(void)
{
		uint8_t mflag = 0, i = 0;
				
		mflag = Rpt_Read_DataFlash();	//Read DataFlash parameter
		if(mflag != 0) {
			Rpt_DefaultSetting();		//read default value
			Rpt_Write_DataFlash();	//write default parameter to dataflash
		}
		
		gDatarate = RptDefSet.RptDataRate; //set RF datarate
		M8107RF_SetCHAN(RptDefSet.Gchn); //Set M8107 RF to gateway channel
		A7127RF_Enable(); //Enable A7127 RF module, and set A7127 RF to Tag channel
		
		printf("Rpt ID = ");
		for(i = 0; i < 5; i++){
			printf("%x ", RptDefSet.RptID[i]);
		}
		printf(", Ver : %x.%x.%x.%x, Date : %d\r\n", 
						RptDefSet.RptVerMJR, RptDefSet.RptVerMINR, 
							RptDefSet.RptVerRLSE, RptDefSet.RptVerBLD, 
								RPT_BUILD_PROJECT_DATE);
		
		switch(RptDefSet.RptDataRate){
			case RF_DATARATE_1M:
				printf("RF DataRate 1M, ");
				break;
			case RF_DATARATE_500K:
				printf("RF DataRate 500K, ");
				break;
			case RF_DATARATE_250K:
				printf("RF DataRate 250K, ");
				break;
		}
		printf("Gw CHN = %d, Tag CHN = %d\r\n", RptDefSet.Gchn, RptDefSet.Tchn);
		
		memset(&RptGlblVar, 0, sizeof (RPT_GLOBAL_VARIABLE));	//init repeater global variable
		
		RptGlblVar.gMode = RPT_PWRON_MODE; //RPT_RX_MODE;//RptPowerOnMode;
		RptGlblVar.gState = PWRBEACON_STATE; //PWRBEACON_STATE;

		RptGlblVar.RecvLen = SYNC_LEN;
		RptGlblVar.RecvTimer = RPT_PWRON_BEACON_TIMER;
}
