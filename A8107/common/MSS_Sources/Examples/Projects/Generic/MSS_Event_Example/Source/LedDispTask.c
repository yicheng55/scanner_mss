/**
 ******************************************************************************
 * @file    		Led_Disp_Task.C 
 * @author  		Kenneth Chang
 * @version   	V1.0.0
 * @date      	28-Augst-2017
 * @brief   		
 ******************************************************************************
 *
 *
 *	
 ******************************************************************************
*/

#include "mss.h"
#include "hal.h"
#include "event_app.h"
#include "A8107M.h"
#include "Utility.h"
#include "RTC.h"

//*****************************************************************************
// Global variables
//*****************************************************************************

//*****************************************************************************
// Private variables
//*****************************************************************************
static mss_timer_t LED_Disp_tmr;

//*****************************************************************************
// Macros (defines), data types, static variables
//*****************************************************************************
//#define TASK_1_ID                       (0)
//#define TASK_2_ID                       (1)
#define LED_Disp_Task_ID           		  (3)


#define APP_ASSERT(cond)                HAL_ASSERT(cond)

//#define APP_ASSERT(x)                 if(!(x)) do{ IRQ_globalDisable(); \  	/* ??????  «Ý³B²z...... */
	                                              //while(1);}while(0)

//*****************************************************************************
// LED Display Task Service
//*****************************************************************************
void LED_Disp_Task(void *param)
{
	  static mss_task_ctx_t LED_Disp_Task_ctx = MSS_TASK_CTX_STATE_INIT_VAL;
//		static mss_task_tf_t LED_Disp_Task_tf 	= MSS_TASK_CTX_STATE_INIT_VAL;
	 // mss_event_t LED_Disp_event;
	
	  MSS_BEGIN(LED_Disp_Task_ctx);
	
	  /*trigger timer*/ 	
    APP_ASSERT(mss_timer_start(LED_Disp_tmr,MSS_TIMER_MS_TO_TICKS(1000)) == true);
	  while(1)
	  {	
	    DEBUG_MESSAGE(FLAG_MF_SYSTEM, _T("LED_Disp_Task():\r\n"));
			
	    if (mss_timer_check_expired(LED_Disp_tmr) == true)
				{
//#if (Printf_Debug_Viewer == true)	
					printf("Execute LED_Disp_Task , Task no : %d\n", LED_Disp_Task_ID);
//#endif
//					if (LED_Disp_Task_tf == true) 
						{
							mss_timer_start(LED_Disp_tmr, MSS_TIMER_MS_TO_TICKS(1000));				  						
							HAL_LED3_TOGGLE();			
//							LED_Disp_Task_tf	=	false;
						}
//					else
//						{
//							mss_timer_start(LED_Disp_tmr, MSS_TIMER_MS_TO_TICKS(1000));				  						
//							BSP_LED_Toggle(LED4);						
//							LED_Disp_Task_tf	=	true;
//						
//						}			
					}
	    MSS_RETURN(LED_Disp_Task_ctx);
	  }
	  
	  MSS_FINISH();
}

//*****************************************************************************
//LED_Display_TASK initialization
//*****************************************************************************
void LED_Disp_Task_Init(void)
{
		// create timers
	  LED_Disp_tmr = mss_timer_create(LED_Disp_Task_ID);
	  APP_ASSERT(LED_Disp_tmr != MSS_TIMER_INVALID_HDL);
}









