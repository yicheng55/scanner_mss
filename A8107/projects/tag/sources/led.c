#include "main.h"
#include "tagdefine.h"
#include "led.h"


void GPIO_PIN_LedSet( uint8_t data )
{
	switch(data) {
		case 3:
			GPIO_PIN_OUTPUT(GPIO0,24);
		break;		
		
		case 1:
			GPIO_PIN_OUTPUT(GPIO0,25);			
		break;		
		
		case 2:
			GPIO_PIN_OUTPUT(GPIO0,26);			
		break;		

		default:

		break;		
	}

}


void GPIO_PIN_LedClear( uint8_t data )
{
	switch(data) {
		case 3:
			GPIO_PIN_INPUT(GPIO0,24);
		break;		
		
		case 1:
			GPIO_PIN_INPUT(GPIO0,25);			
		break;		
		
		case 2:
			GPIO_PIN_INPUT(GPIO0,26);			
		break;		

		default:

		break;		
	}

}


void GPIO_PIN_LedSetting( void )
{
	GPIO_PIN_OUTPUT(GPIO0,24);

	GPIO_PIN_OUTPUT(GPIO0,25);

	GPIO_PIN_OUTPUT(GPIO0,26);

}


void GPIO_PIN_LedTest( void )
{
	GPIO_PIN_OUTPUT(GPIO0,24);
	Delay1ms(0x20000);
	GPIO_PIN_OUTPUT(GPIO0,25);
	Delay1ms(0x20000);
	GPIO_PIN_OUTPUT(GPIO0,26);
	Delay1ms(0x20000);
	
	GPIO_PIN_INPUT(GPIO0,24);	
	GPIO_PIN_INPUT(GPIO0,25);	
	GPIO_PIN_INPUT(GPIO0,26);	
	Delay1ms(0x20000);
	
}
