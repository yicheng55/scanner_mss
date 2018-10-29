/************************************************************
 * isr.c
 *
 ************************************************************/
#include "main.h"
#include "EslDebug.h"

uint8_t Oneshoutflag = 0;

//==============================================================================
// Function     : SLPTIMER1_Handler
// Purpose      : SLPTIMER1 callback function
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void SLPTIMER1_Handler(void)
{
    SLPTIMER_ClearIRQ(SLPTIMER1);
    //printf( "SLEEP TIMER 0 interrupt occured.\r\n");
    //GPIO0->DATAOUT_PIN.P00 ^= 1;
    //printf("Sleep Timer timeout... \r\n");
}

#if 0
//==============================================================================
// Function     : DUALTIMER_Handler
// Purpose      : DUALTIMER callback function
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void DUALTIMER_Handler(void)
{
	DualTimer_ClearIRQ(DUALTIMER1);
	printf("Dual Timer timeout\r\n");
	Oneshoutflag = 1;
}
#endif

//==============================================================================
// Function     : TIMER0_Handler
// Purpose      : TIMER0 callback function
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void TIMER0_Handler(void)
{
	DEBUG_MESSAGE(FLAG_MF_TAG, _T("\r\nTIMER0 shot\r\n"));
	Timer_StopTimer(TIMER0);
	Oneshoutflag=1;
	TIMER0->INTCLEAR = 1;
	//TIMER0->RELOAD = (SystemCoreClock/1000) * 200;

	return;
}

//==============================================================================
// Function     : TIMER1_Handler
// Purpose      : TIMER1 callback function
// Description	: 
// Editor       : James Tsai
// Update Date	: 2018-03-16
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void TIMER1_Handler(void)
{
//	printf("\r\nTIMER1 shot\r\n");
//	TagDebug(("TIMER1 TimeOut\r\n"));
	Timer_StopTimer(TIMER1);
	Oneshoutflag=1;
	TIMER1->INTCLEAR = 1;
	//TIMER0->RELOAD = (SystemCoreClock/1000) * 200;

	return;
}

//==============================================================================
// Function     : HardFault_Handler
// Purpose      : HardFault callback function
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void HardFault_Handler(void)
{
	ERROR_MESSAGE(FLAG_MF_SYSTEM, _T(">>>> HardFault <<<<\r\n"));
    while(1)
    {
    }
}


