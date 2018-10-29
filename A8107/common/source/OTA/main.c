
#include <stdio.h>
#include "A8107M.h"
#include "uart.h"
#include "uart_stdout.h"

extern   void OTA_Init(void);

int res = 0, i=4,j=8;
				
				
void HardFault_Handler(void)
{
    printf( ">>>> HardFault <<<<\r\n");
    while(1)
    {
    }
}

void GPIO0_Handler(void)
{
    uint32_t i;
    for(i=0; i<32; i++)
    {
        if(GPIO0->INTSTATUS & (1<<i) )
        {
            printf( "INT: GPIO0.%02d, Clean Flag\r\n", i);
            GPIO0->INTCLEAR = (1<<i);
        }
    }
}




void Delay1ms(uint32_t n)
{
    while(n)
    {
				n++;
			  n--;

				n++;
			  n--;
			
        n--;
    }
}


int main(void)
{
	  uint32_t cnt=0;
    UartStdOutInit(UART0, 115200);      // set UART0 to stdio interface, baudrate=115200bps
    GPIO0->ALTFUNCSET = (1<<17);        // set UART0 IO(TX) enable

		printf( "\r\nUser code..\r\n");
    printf( "Enable GPIO0.P00 interrupt\r\n");
    GPIO0->INTENSET = (1<<0);

    printf( "Set GPIO0.P00 interrupt TYPE=1 & POL=0 (Falling Edge)\r\n");
    printf( "    TYPE=0, POL=0: Low Level\r\n");
    printf( "    TYPE=0, POL=1: High Level\r\n");
    printf( "    TYPE=1, POL=0: Falling Edge\r\n");
    printf( "    TYPE=1, POL=1: Rising Edge\r\n");
    GPIO0->INTTYPESET = (1<<0);     // TYPE=1
    GPIO0->INTPOLCLR  = (1<<0);     // POL =0

    printf( "Eanble GPIO0_IRQn\r\n");
    NVIC_EnableIRQ(GPIO0_IRQn);     // Enable GPIO0_IRQn

    while(1)
    {
			Delay1ms(0x2000000);
			printf( "Delay1ms()= %u .\r\n", cnt++);

			OTA_Init();
			
			Delay1ms(0x20000);
			printf( "Reset().. \r\n");
			Delay1ms(0x2000);
			NVIC_SystemReset();
					
			while(1)
			{
			}
			
    }
}
