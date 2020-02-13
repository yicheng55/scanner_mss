// Board.c: implementation of the board hardware functions.
//
//////////////////////////////////////////////////////////////////////
#include "main.h"
#include "Board.h"
#include "spi.h"
#include "RTC.h"
#include "Scanner.h"
#include "Led.h"
#include "NonVolatileMemory.h"
#include "System.h"

//==============================================================================
//--- Declarative Variables ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative Global Variables ---
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//--- Declarative Member Variables ---
//------------------------------------------------------------------------------


extern void DateAndHour(int nDate, int nHour);
extern UINT_PTR m_nLastIDEvent;	// For Timer0/Timer1 ISR
//==============================================================================
// Function     : 
// Purpose      : Construction
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void Board(void)
{
	BoardInit();
}

//==============================================================================
// Function     : 
// Purpose      : Initial Peripherals
// Description	: Initialize the scanner settings
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void BoardInit(void)
{
	//
	// Initial Message
	//
	SetMessageFunc (FLAG_MF_APPLICATION | FLAG_MF_KERNEL | FLAG_MF_HARDWARE | FLAG_MF_SYSTEM | 
					FLAG_MF_DRIVER | FLAG_MF_FIRMWARE | FLAG_MF_COMMUNICATION | FLAG_MF_SCANNER | FLAG_MF_TAG);
	//SetMessageLevel (FLAG_MT_ALL);	
	
	BoardInitUART0(UART0_BAUDRATE);	// set UART0 for console
	BoardInitUART2(UART2_BAUDRATE);	// set UART2 for Scanner 
	BoardInitGPIO();
	BoardInitSPI();
	//TurnOn_External_RTC_Crystal();	//For sleep timer1
	//InitTimer0();
	Led();
	
	// Load Configuration from flash
	NonVolatileMemory();
	
	//SetOneshotTimer(TIMER0, 100);
	BoardInitTimer(TIMER_ONESHOT, 100);		// 100ms for sync
	BoardInitTimer(TIMER_TIMING_TRIGGER, TIME_TIMING_TRIGGER);		// 100ms for sync
	ScannerDebug(_T("Board initialization completed\r\n"));
	
	

}



//==============================================================================
// Function     : InitUart
// Purpose      : Init Uart for debug mesage
// Description	: Initialize scanner's UART settings 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void BoardInitUART0(uint32_t uiBaudrate)
{
	UartStdOutInit(UART0, uiBaudrate);   // set UART to stdio interface, baudrate=115200bps
	GPIO0->ALTFUNCSET = (1<<17);        // set UART TX IO
	
	// Show information
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("\r\nSet UART0 IO (P0_17)\r\n"));
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Initial UART0: Baud rate = %d, Enable TX\r\n"), uiBaudrate);
}

//==============================================================================
// Function     : InitUart
// Purpose      : Init Uart for debug mesage
// Description	: Initialize scanner's UART settings 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void BoardInitUART2(uint32_t uiBaudrate)
{
	INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("\r\nSet UART2 IO (P0_20, P0_21)\r\n"));
    GPIO0->ALTFUNCSET = (1<<20) | (1<<21);              // set UART2 IO(RX, TX) enable

    INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Initial UART2: Baud rate = %d, Enable TX, RX, TXINT, RXINT\r\n"), uiBaudrate);
	UART_init(UART2, XTAL/uiBaudrate, 1, 1, 1, 1, 0, 0); // Enable UART2 Tx, Rx, TxInt, RxInt
    //UART_init(UART2, SystemCoreClock/uiBaudrate, 0, 1, 0, 1, 0, 0); // Enable UART2 Tx, Rx, TxInt, RxInt
	
    INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Enable UART2_IRQn\r\n"));
    NVIC_EnableIRQ(UART2_IRQn);                         // Enable UART2_IRQn
}

//==============================================================================
// Function     : BoardInitRTC
// Purpose      : 
// Description	: Initialize scanner's RTC settings 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void BoardInitRTC()
{
	DEBUG_MESSAGE(FLAG_MF_SYSTEM, _T("Inital RTC: using 32.768KHz, set WEEK=0, HOUR=0, MIN=0, SEC=0\r\n"));
	RTC_Initial(1, 0, 0, 0, 0);	// cksel=1=32.768KHz, week=0, hour=0, min=0, sec=0
	//DateAndHour(0,0);
	RTC_Start();
}

#if 0
//==============================================================================
// Function     : TurnOnExternalRTC
// Purpose      : Initial external RTC
// Description	: Initial external RTC Crystal for sleep timer
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void TurnOnExternalRTC(void)
{
  POWER->RCCTRL = POWER_RCCTRL_RCTS_Msk;
  POWER->RCCTRL2 = 0x03040012;
  POWER->RCTARGET = 977;
  POWER->RCCTRL = POWER_RCCTRL_MAN_Msk | POWER_RCCTRL_TMRE_Msk | POWER_RCCTRL_RCTS_Msk;
  Delay1ms(100);
}
#endif

//==============================================================================
// Function     : InitTimer
// Purpose      : Init Timer for 1ms.
// Description	: Initialize scanner's Timer settings
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void BoardInitTimer(TIMER_Type *timer, uint32_t msec)
{
	// Disable Timer Interrupt 
	if (timer == TIMER0)
		NVIC_DisableIRQ(TIMER0_IRQn);
	else if (timer == TIMER1)
		NVIC_DisableIRQ(TIMER1_IRQn);

	//1ms for Xtal=16MHz 
	timer->RELOAD = (SystemCoreClock/1000) * msec;
	timer->VALUE = (SystemCoreClock/1000) * msec;	

	// Timer Interrupt Setting
	timer->INTCLEAR = 0x01; 		//clear timer interrupt
	timer->CTRL = (1<<3) | (1<<0);	//timer enable & interrupt

	// Enable Timer Interrupt 
	if (timer == TIMER0)
		NVIC_EnableIRQ(TIMER0_IRQn);
	else if (timer == TIMER1)
		NVIC_EnableIRQ(TIMER1_IRQn);		
}


//==============================================================================
// Function     : InitTimer0
// Purpose      : init Timer0 for 1ms.
// Description	: Initialize scanner's Timer0 settings
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void BoardInitTimer0(void)
{
	NVIC_DisableIRQ(TIMER0_IRQn);
	TIMER0->RELOAD = SystemCoreClock/1000;
	TIMER0->VALUE = SystemCoreClock/1000;//1ms for Xtal=16MHz 
	TIMER0->INTCLEAR = 0x01; //clear timer interrupt
	
	TIMER0->CTRL = (1<<3) | (1<<0);//timer enable & interrupt
	NVIC_EnableIRQ(TIMER0_IRQn);  /* Enable Timer Interrupt */
}

//==============================================================================
// Function     : BoardInitSPI
// Purpose      : 
// Description	: Initialize scanner's SPI settings
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void BoardInitSPI()
{
//	SPI_Initial(SPI, 1, 7, 0);
	SPI_Initial(SPI, 1, 1, 0);
	//SPI_Set_DataWidth(9);
	SetSpiDataWidth(9);
	GPIO0->ALTFUNCSET = (1<<0) | (1<<2) | (1<<3);//(0xF<<0);       // set SPI IO (P0_00~P0_03)
}



//==============================================================================
// Function     : BoardInitGPIO
// Purpose      : Initialize GPIO
// Description	: Initialize scanner's GPIO settings
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void BoardInitGPIO()
{
	// EPD GPIO
	GPIO_PIN_OUTPUT(GPIO_EPD, PIN_EPD_RESET);	// RESET
	GPIO_PIN_INPUT(GPIO_EPD, PIN_EPD_BUSY);		// BUSY
	
	// LED GPIO
	GPIO_PIN_OUTPUT(GPIO_LED, PIN_LED1);
	GPIO_PIN_OUTPUT(GPIO_LED, PIN_LED2);		
}

//==============================================================================
// Function     : SetTimer
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
//              : timer [in]
//              :     Type: HWND
//              :     A handle to the TIMER_Type to be associated with the timer. 
//              :     This timer must be owned by the calling thread. 
//              :     If a NULL value for pTimer is passed in along with an 
//              :     nIDEvent of an existing timer, that timer will be replaced 
//              :     in the same way that an existing non-NULL pTimer timer will be.
//              : nIDEvent [in]
//              :     Type: UINT_PTR
//              :     A nonzero timer identifier. If the pTimer parameter is NULL, and the nIDEvent does not match an existing timer then it is ignored and a new timer ID is generated. If the hWnd parameter is not NULL and the window specified by hWnd already has a timer with the value nIDEvent, then the existing timer is replaced by the new timer. When SetTimer replaces a timer, the timer is reset. Therefore, a message will be sent after the current time-out value elapses, but the previously set time-out value is ignored. If the call is not intended to replace an existing timer, nIDEvent should be 0 if the hWnd is NULL.
//              : uElapse [in]
//              :     Type: UINT
//              :     The time-out value, in milliseconds.
//              :     If uElapse is less than USER_TIMER_MINIMUM (0x0000000A), the timeout is set to USER_TIMER_MINIMUM. If uElapse is greater than USER_TIMER_MAXIMUM (0x7FFFFFFF), the timeout is set to USER_TIMER_MAXIMUM.
//              : lpTimerFunc [in, optional]
//              :     Type: TIMERPROC
//              :     A pointer to the function to be notified when the time-out value elapses. For more information about the function, see TimerProc. If lpTimerFunc is NULL, the system posts a WM_TIMER message to the application queue. The hwnd member of the message's MSG structure contains the value of the hWnd parameter.
// Return       : UINT_PTR
//              : xxxxxIf the function succeeds and the hWnd parameter is NULL, the return value is an integer identifying the new timer. An application can pass this value to the KillTimer function to destroy the timer.
//              : xxxxxIf the function succeeds and the hWnd parameter is not NULL, then the return value is a nonzero integer. An application can pass the value of the nIDEvent parameter to the KillTimer function to destroy the timer.
//              : If the function fails to create a timer, the return value is zero.
// Remarks      : 
//==============================================================================
UINT_PTR SetTimer(TIMER_Type *timer, UINT_PTR nIDEvent, UINT uiElapse, LPVOID lpTimerFunc)
{
	timer->RELOAD = (SystemCoreClock/1000) * uiElapse;
	timer->VALUE = (SystemCoreClock/1000) * uiElapse;
	Timer_StartTimer(timer);	

	if (nIDEvent != EVENT_TIMER_TIMING_TRIGGER)
	{
		m_nLastIDEvent = nIDEvent;
		return m_nLastIDEvent;
	}

	return EVENT_TIMER_TIMING_TRIGGER;
}


//==============================================================================
// Function     : 
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
//              : uint32_t msec: 
//              : one shot period(ms).
// Return       : 
// Remarks      : 
//==============================================================================
void SetOneshotTimer(TIMER_Type *timer, uint32_t msec)
{
	// Disable Timer Interrupt 
	if (timer == TIMER0)
		NVIC_DisableIRQ(TIMER0_IRQn);
	else if (timer == TIMER1)
		NVIC_DisableIRQ(TIMER1_IRQn);
	
	timer->INTCLEAR = 0x01; 						//clear timer interrupt
	timer->RELOAD = (SystemCoreClock/1000) * msec;
	timer->VALUE = (SystemCoreClock/1000) * msec;	//1ms for Xtal=16MHz 
	
	timer->CTRL = (1<<3) | (1<<0);					//timer enable & interrupt
	// Enable Timer Interrupt 
	if (timer == TIMER0)
		NVIC_EnableIRQ(TIMER0_IRQn);
	else if (timer == TIMER1)
		NVIC_EnableIRQ(TIMER1_IRQn);	
}
