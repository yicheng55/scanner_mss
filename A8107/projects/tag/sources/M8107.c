/**********************************************
 * M8107.c
 *
 **********************************************/
#include "main.h"

extern void UserRegister(void);

void EnterPM1(void)
{
	
    RADIO->STROBE = CMD_SLEEP;
    POWER->POWERCTRL2 = 0xD0;
    POWER->FALSHCTRL = 0x0000A495;
    ADC12B->Control = 0x000001FE;           // VADS=0
    POWER->PowerManagementItem.STOP = 1;    // STOP=1
    // MCU Enter PM mode now.
    // Process will run continue when MCU wakeup.
    // Process may run interrupt first if occure.
    __nop();
    __nop();
    __nop();
    ADC12B->Control = 0x000061FE;           // VADS=6
    POWER->FALSHCTRL = 0x00006495;
}


/******************************************************************************
**  Enter PM3. Include some necessary action.
******************************************************************************/
void EnterPM3(void)
{
    uint32_t altfunset, outenset, pun;
    printf("EnterPM3...\r\n");
    __disable_irq();                        // disable all IRQ
    
    //-------------------------------------------------------------------------
    // Backup IO setting and any need peripherals setting
    // There include 
    //      GPIO0 / I2C / SPI / UART0,1,2 / TIMER0, 1 / Dualtimer / Watchdog / 
    //      MPU_LCD / RTC (Real Time Counter) / RF 
    // User need backup setting for restore if need.
    altfunset = GPIO0->ALTFUNCSET;
    outenset  = GPIO0->OUTENABLESET;
    pun       = GPIO0->PUN;
    //-------------------------------------------------------------------------
    
    //-------------------------------------------------------------------------
    // IO setting for PM3 power saving. 
    // User need modify this for fit HW.
		GPIO0->ALTFUNCCLR   = 0xFFFFFFFF;       // clean all IO function
		GPIO0->OUTENABLECLR = 0xFFFFFFFF;       // set all IO to input
		GPIO0->PUN          = 0; 								// set all IO to pull-high
		
		GPIO0->OUTENABLESET = (1<<0) | (1<<14);  // set IO to output
		GPIO0->PUN_PIN.P00 = 1;
		GPIO0->PUN_PIN.P14 = 1;

		GPIO0->OUTENABLECLR = (1<<12);
		GPIO0->PUN_PIN.P12 = 1; //without pull-up		
		
    //-------------------------------------------------------------------------

    //=========================================================================
    // PM3 process. Don't modify.
    RADIO->STROBE = CMD_SLEEP;              // set RF SLEEP
    POWER->FALSHCTRL =  0x0000A495;
    ADC12B->Control =   0x000001FE;
    POWER->POWERCTRL2 = 0x000000B0;
    POWER->POWERCTRL2 = 0x000000F8;
    POWER->PowerManagementItem.STOP = 1;
    // MCU Enter PM mode now.
    // Process will run continue when MCU wakeup.
    POWER->POWERCTRL2 = 0x000000B0;
    ADC12B->Control =   0x000061FE;
    POWER->FALSHCTRL =  0x00006495;
    //=========================================================================

    //-------------------------------------------------------------------------
    // restore IO setting and any need peripherals re-setting
    GPIO0->ALTFUNCSET   = altfunset;        // restore ALTFUNCSET
    GPIO0->OUTENABLESET = outenset;         // restore OUTENABLESET
    GPIO0->PUN          = pun;              // restore PUN
    // After PM3, some peripherals need setting again or restore
    UartStdOutInit(UART0, 115200);
    // Need run initRF() again or restore RF if user want use RF
    initRF();
			UserRegister();		//M8107 registers setting
	
  StrobeCmd(CMD_STBY);
  
  SetCH(200); //freq 2450MHz CH100
    //-------------------------------------------------------------------------

    __enable_irq();                         // enable IRQ
    // Interrupt will run if occure.
}
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

/*********************************************************************
** RSSI_measurement
*********************************************************************/
int16_t RSSI_measurement(void)
{
    int tmp;
		float  tmp1;
#if 0        
    StrobeCmd(CMD_RX);//entry RX mode
    Delay10us(1);
    while((RADIO->STATUS & (1<<8))==0) //Stay in RX mode until receiving ID code(sync ok)
    {
        tmp= RADIO->THRESHOLD & 0x1FF;        //read RSSI value(environment RSSI)
    }
    printf("environment RSSI = 0x%X\r\n", tmp);
#endif
    //printf("ADC9 Control register = 0x%X\r\n", RADIO->ADC9);
    tmp= RADIO->THRESHOLD & 0x1FF;            //read RSSI value(wanted signal RSSI)
		tmp1 = (tmp*0.4)-120;
//    printf("RSSI Code is = %f --------\r\n", tmp1);
    return abs((int)tmp1);
}

void SetFIFOLvl(uint32_t fifolen)
{
	RADIO->FIFOCTRL 	= fifolen -1;
}


uint8_t debounce ( uint32_t PIN )
{
#define COUNT_TARGET 25u
#define LOW  0
#define HIGH 1
	
unsigned char count_low;
unsigned char count_high;
count_low = count_high = 0;
do
{
	// Delay 1ms
	Delay1ms(1);
		
	if (PIN == 0){
		count_low++;
		count_high = NULL;
		}	
		else{
		count_low = NULL;
		count_high++;
		}

	//Wait till pin has been at same level for 25 count.
	}while(count_low< COUNT_TARGET && count_high < COUNT_TARGET);

	if (count_low >= COUNT_TARGET){
		//printf("return 0.\r\n");
		return 	LOW; 		//LOW
		}	
		else{
		//printf("return 1.\r\n");		
		return HIGH;		//HIGH
		}
}

/************************************************************************
** Temperature_measurement
** Description:		get current Temperature Code
** Parameters:		none
** Return value: 	none
** Note:
************************************************************************/
uint32_t Temperature_measurement(void)
{
    uint32_t Temperature;
		uint32_t control = 0;
		uint8_t FBG = 0;
//		FBG = HW8_REG(0x3FE00);
//		printf("FBG = 0x%X\r\n",HW8_REG(0x3FE00));
		RADIO->RFTEST = 0x2420C04A | (FBG << 8); //FBG = 0 - 31
//		printf("RADIO->ADC9 = 0x%X\r\n",RADIO->ADC9);
		RADIO->ADC9 = (RADIO->ADC9 & 0x7B07);//RSS=0 @Offset: 0x240
//	printf("RADIO->ADC9 = 0x%X\r\n",RADIO->ADC9);
		POWER->POWERCTRL3 = 0x0094000C;
	
	  ADC12B->Control = ADC12B_CONTROL_ADC12RN_Msk;   // ADC reset
    control = ADC12B_CONTROL_ADCE_Msk |         // start ADC measure
            (6 << ADC12B_CONTROL_VADS_Pos) |    // Ref Voltage = 1.8V
            (3 << ADC12B_CONTROL_MVS_Pos) |     // Average 8 times
            (0 << ADC12B_CONTROL_CKS_Pos) |     // clock 4MHz
            (1 << ADC12B_CONTROL_ENADC_Pos)|          // ADC clock on
            ADC12B_CONTROL_MODE_Msk |            // continue mode
						ADC12B_CONTROL_DTMP_Msk |
						ADC12B_CONTROL_BUPS_Msk;

		POWER->DCDCCTRL2 = 0;
		ADC12B->Control = control;
//		printf("ADC12B->Control = 0x%X\r\n",ADC12B->Control);
	
    //-----------------------------------------------------
    // Delay for ADC measure
    // delay time = 2*(32*8)/4=128us
    SysTick->LOAD = ((SystemCoreClock * 128 / 1000000) - 1);//setting
    SysTick->VAL = 0;   //clear counter
    SysTick->CTRL |=  (1 << 2) | (1 << 0);
    while((SysTick->CTRL & 0x10000) == 0);
    SysTick->CTRL  = 0x10000;
    SysTick->CTRL = 0;
    //-----------------------------------------------------	
//		Delay1us(128);
		Temperature = ADC12B->ValueItem.MVADC;
//		printf("Temperature = %d\r\n", Temperature);
//		Temperature = ADC12B->ValueItem.ADC;
//		printf("Temperature(ADC) = %d\r\n", Temperature);
		ADC12B->Control = 0; 
		POWER->DCDCCTRL2 = 241;
		POWER->POWERCTRL3 = POWER->POWERCTRL3 & 0x0090000C;
		return Temperature;
}


/************************************************************************
** ConvertTemperature_measurement
** Description:		Convert current Temperature Code
** Parameters:		none
** Return value: 	none
** Note:
************************************************************************/
int16_t  ConvertTemperature_measurement(void)
{
    uint32_t Temperature;
	  int32_t ValueTemperature;
		float ConvertTemperature,ZeroOffsetTemperature;
		
		uint16_t TempBase = GetTempBase();
		uint16_t AdcBase = GetAdcBase();

		Temperature = Temperature_measurement();

		ZeroOffsetTemperature= (TempBase/0.22); 
		ValueTemperature = (Temperature - (AdcBase-ZeroOffsetTemperature));
		ConvertTemperature = (ValueTemperature*0.22);
		//printf("ValueTemperature = %d   ,TempBase=%d   ,ConvertTemperature=%.1f  \r\n", ValueTemperature ,TempBase, ConvertTemperature );
		return (int16_t)ConvertTemperature;
}

/************************************************************************
** GetAdcTemperature_measurement
** Description:		Get current Temperature Code
** Parameters:		none
** Return value: 	none
** Note:
************************************************************************/
uint16_t GetAdcTemperature_measurement(void)
{
    uint16_t Temperature=0;
	  int16_t AvgTemperature=0;
	
		Temperature_measurement();
		Delay1ms(0x1000);
		Temperature += Temperature_measurement();
		Delay1ms(0x1000);
		Temperature += Temperature_measurement();
		AvgTemperature = Temperature/2;

		printf("AvgTemperature = %d  \r\n", AvgTemperature);

		return AvgTemperature;
}



