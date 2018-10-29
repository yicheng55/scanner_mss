/************************************************************
 * init.c
 *
 ************************************************************/
#include "main.h"
#include "tagdefine.h"
#include "spi.h"
#include "RTC.h"
#include "EslDebug.h"

extern void DateAndHour(int Date, int Hour);
#if 0
/*******************************
 * TurnOn_External_RTC_Crystal()
 *	Initial external RTC Crystal for sleep timer.
 **********************************/
void TurnOn_External_RTC_Crystal(void)
{
  POWER->RCCTRL = POWER_RCCTRL_RCTS_Msk;
  POWER->RCCTRL2 = 0x03040012;
  POWER->RCTARGET = 977;
  POWER->RCCTRL = POWER_RCCTRL_MAN_Msk | POWER_RCCTRL_TMRE_Msk | POWER_RCCTRL_RCTS_Msk;
  Delay1ms(100);
}
#endif


void HW_init(void)
{
GPIO0->OUTENABLESET = 0xFFFFFF;
GPIO0->DATA = 0xFFFFFF;
while((GPIO0->DATA & (1<<20))==0);
}
 
 /************************************************************************
**  Init Uart0 for debug mesage
************************************************************************/
void initUart0(void)
{
	UartStdOutInit(UART0, 115200);      // set UART0 to stdio interface, baudrate=115200bps
  GPIO0->ALTFUNCSET = (1<<17);        // set UART0 TX IO
}

/************************************************************************
**  init Timer0 for 1ms.
************************************************************************/
void InitTimer0(void)
{
	NVIC_DisableIRQ(TIMER0_IRQn);
	TIMER0->RELOAD = SystemCoreClock/1000;
	TIMER0->VALUE = SystemCoreClock/1000;//1ms for Xtal=16MHz 
	TIMER0->INTCLEAR = 0x01; //clear timer interrupt
	
	TIMER0->CTRL = (1<<3) | (1<<0);//timer enable & interrupt
	NVIC_EnableIRQ(TIMER0_IRQn);  /* Enable Timer Interrupt */
}

void InitTimer1(void)
{
	NVIC_DisableIRQ(TIMER1_IRQn);
	TIMER1->RELOAD = SystemCoreClock/1000;
	TIMER1->VALUE = SystemCoreClock/1000;//1ms for Xtal=16MHz 
	TIMER1->INTCLEAR = 0x01; //clear timer interrupt

	TIMER1->CTRL = (1<<3) | (1<<0);//timer enable & interrupt
	NVIC_EnableIRQ(TIMER1_IRQn);  /* Enable Timer Interrupt */
}

/*******************************************************
 *
 * uint32_t msec: one shot period(ms).
 *******************************************************/
void SetOneshotTimer0(TIMER_Type *timer, uint32_t msec)
{
	NVIC_DisableIRQ(TIMER0_IRQn);
	timer->INTCLEAR = 0x01; //clear timer interrupt
	timer->RELOAD = (SystemCoreClock/1000) * msec;
	timer->VALUE = (SystemCoreClock/1000) * msec;//1ms for Xtal=16MHz 
	
	
	timer->CTRL = (1<<3) | (1<<0);//timer enable & interrupt
	NVIC_EnableIRQ(TIMER0_IRQn);  /* Enable Timer Interrupt */
	
}

void SetOneshotTimer1(TIMER_Type *timer, uint32_t msec)
{
	NVIC_DisableIRQ(TIMER1_IRQn);
	timer->INTCLEAR = 0x01; //clear timer interrupt
	timer->RELOAD = (SystemCoreClock/1000) * msec;
	timer->VALUE = (SystemCoreClock/1000) * msec;//1ms for Xtal=16MHz 
	
	
	timer->CTRL = (1<<3) | (1<<0);//timer enable & interrupt
	NVIC_EnableIRQ(TIMER1_IRQn);  /* Enable Timer Interrupt */
	
}

void startTimer(TIMER_Type *timer, uint32_t msec)
{
	timer->RELOAD = (SystemCoreClock/1000) * msec;
	timer->VALUE = (SystemCoreClock/1000) * msec;
	Timer_StartTimer(timer);
}

void SPI_init()
{
//	SPI_Initial(SPI, 1, 7, 0);
	SPI_Initial(SPI, 1, 1, 0);
	SPI_Set_DataWidth(9);
	GPIO0->ALTFUNCSET = (1<<0) | (1<<2) | (1<<3);//(0xF<<0);       // set SPI IO (P0_00~P0_03)
}


void GPIO_PIN_SETTING()
{
	GPIO_PIN_OUTPUT(GPIO0,14);
	GPIO_PIN_INPUT(GPIO0,13);	
}

void RTC_SETTING()
{
	//printf("Inital RTC: using 32.768KHz, set WEEK=0, HOUR=0, MIN=0, SEC=0\r\n");
	RTC_Initial(1, 0, 0, 0, 0);             // cksel=1=32.768KHz, week=0, hour=0, min=0, sec=0
	DateAndHour(0,0);
	RTC_Start();
}

void board_init(void)
{
	//HW_init();
	initUart0();
//	SetMessageFunc (FLAG_MF_APPLICATION | FLAG_MF_KERNEL | FLAG_MF_HARDWARE | FLAG_MF_SYSTEM | 
//					FLAG_MF_DRIVER | FLAG_MF_FIRMWARE | FLAG_MF_COMMUNICATION | FLAG_MF_CUSTOM1 | FLAG_MF_TAG);
	SetMessageFunc (FLAG_MF_APPLICATION | FLAG_MF_KERNEL | FLAG_MF_HARDWARE | FLAG_MF_SYSTEM | 
					FLAG_MF_DRIVER | FLAG_MF_FIRMWARE | FLAG_MF_COMMUNICATION | FLAG_MF_CUSTOM1 | FLAG_MF_TAG);
	SetMessageLevel (FLAG_MT_ALL);
	
	GPIO_PIN_SETTING();
	SPI_init();
	//TurnOn_External_RTC_Crystal();	//For sleep timer1
	//InitTimer0();
	SetOneshotTimer1(TIMER1, 200);
	
	
}

