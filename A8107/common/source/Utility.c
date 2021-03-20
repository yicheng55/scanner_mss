#include <stdio.h>
#include <stdlib.h>
#include "Utility.h"
#include "A8107M.h"
#include "slptimer.h"

// Redefine the XTAL of the utility and rename XTAL to DELAY_XTAL
//#define XTAL	(SystemCoreClock/1000)	//16000, 16MHz
#define DELAY_XTAL	(SystemCoreClock/1000)	//16000, 16MHz

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
//void Utility(void)
//{
//	
//}


/************************************************************************
**  Delay1ms
************************************************************************/
#if 1
void Delay1ms(uint32_t n)
{
    SysTick->LOAD = 16000-1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while(n)
    {
        while( 0 == (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) ) {};
        n--;
    }
    SysTick->CTRL = 0;
}
#endif

#if 0
void Delay1ms(uint32_t n)
{
	SysTick->LOAD = (((DELAY_XTAL)*n)-1);//setting 
	SysTick->VAL = 0;//clear counter
	SysTick->CTRL |=  (1<<2) | (1<<0);
	while((SysTick->CTRL & 0x10000)==0);
	SysTick->CTRL  = 0x10000;
	SysTick->CTRL = 0;
}
#endif
/************************************************************************
**  Delay100us
************************************************************************/
void Delay100us(uint32_t n)
{
	SysTick->LOAD = (((DELAY_XTAL/10)*n)-1);//setting 
	SysTick->VAL = 0;//clear counter
	SysTick->CTRL |=  (1<<2) | (1<<0);
	while((SysTick->CTRL & 0x10000)==0);
	SysTick->CTRL  = 0x10000;
	SysTick->CTRL = 0;
}

/************************************************************************
**  Delay_10us
************************************************************************/
void Delay10us(uint32_t n)
{
	SysTick->LOAD = (((DELAY_XTAL/100)*n)-50);//setting 
	SysTick->VAL = 0;//clear counter
	SysTick->CTRL |=  (1<<2) | (1<<0);
	while((SysTick->CTRL & 0x10000)==0);
	SysTick->CTRL  = 0x10000;
	SysTick->CTRL = 0;
}

/************************************************************************
**  Delay_1us
************************************************************************/
void Delay1us(uint32_t n)
{
	SysTick->LOAD = (((DELAY_XTAL/1000)*n)-1);//setting 
	SysTick->VAL = 0;//clear counter
	SysTick->CTRL |= (1<<2) | (1<<0);;//(1<<2) | (1<<1) | (1<<0);

	while((SysTick->CTRL & 0x10000)==0);
	SysTick->CTRL  = 0x10000;
	SysTick->CTRL = 0;
}


void Set_TAG_Mili_SleepTime(int SLPT)
{
	float Time = 0;
	uint32_t uTime = 0;
	
	Time = (SLPT / 1000.0);
//	printf("SLPT = %d..\r\n",SLPT);
//	printf("Time = %lf..\r\n",Time);	
	
	Time = (SLPT / 1000.0) * 1638.4;
//	printf("Time = %lf..\r\n",Time);
	uTime = Time;
	SLPTIMER_Initial(SLPTIMER1, uTime, 3, 1);
//	printf("uTime = %u..\r\n",uTime);
}



