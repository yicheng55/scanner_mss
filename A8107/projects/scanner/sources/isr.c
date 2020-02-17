// isr.c: implementation of the ISR functions.
//
//////////////////////////////////////////////////////////////////////
#include "main.h"
#include "EslDebug.h"
#include "Scanner.h"
#include "RfPacket.h"

//==============================================================================
//--- Declarative Variables ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative Global Variables ---
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//--- Declarative Member Variables ---
//------------------------------------------------------------------------------
UINT_PTR m_nLastIDEvent = 0;

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
	//Oneshoutflag = 1;
	RfSetRxTimeout(true);
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
	//UINT_PTR nIDEvent = 0;
	DEBUG_MESSAGE(FLAG_MF_TAG, _T("*** Timer0 shot ***\r\n"));
	Timer_StopTimer(TIMER0);
	RfSetRxTimeout(true);
	TIMER0->INTCLEAR = 1;
	
	if (RfIsBeaconSyncTimeout())
		RfResetBindingID();
	
	OnTimer(m_nLastIDEvent);
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
extern  void MSS_TaskUseTimer(void);
void TIMER1_Handler(void)
{
	//TagDebug(("TIMER1 TimeOut\r\n"));
	//DEBUG_MESSAGE(FLAG_MF_TAG, _T("*** Timer1 shot ***\r\n"));
	//Timer_StopTimer(TIMER1);
	TIMER1->INTCLEAR = 1;
	//OnTimer(EVENT_TIMER_TIMING_TRIGGER);	
	MSS_TaskUseTimer();
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

//==============================================================================
// Function     : UART2_Handler
// Purpose      : UART2 callback function
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void UART2_Handler(void)
{
    if(UART_GetRxIRQStatus(UART2))
    {
        UART_ClearRxIRQ(UART2);     		// clean Rx interrupt flag
		// UART2 RX receive 8bits data
		ScannerInputData(UART_ReceiveChar(UART2));
    }

    if(UART_GetTxIRQStatus(UART2))
    {
        UART_ClearTxIRQ(UART2);     // clean Tx interrupt flag
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("UART2 Tx:\r\n"));
    }
}

