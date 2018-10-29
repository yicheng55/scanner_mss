/**********************************************
 * M8107.c
 *
 **********************************************/
#include "main.h"

extern void UserRegister(void);

//const
uint8_t AES_KEY[] =
{
    0x4C, 0x68, 0x38, 0x41,
    0x39, 0xF5, 0x74, 0xD8,
    0x36, 0xBC, 0xF3, 0x4E,
    0x9D, 0xFB, 0x01, 0xBF
};

uint8_t EncryptData[16];
uint8_t DecryptData[16];

#if 0
void EnterPM1(void)
{
    RADIO->STROBE     = CMD_SLEEP;
    POWER->POWERCTRL2 = 0xD0;
    POWER->FALSHCTRL  = 0x0000A495;
    ADC12B->Control   = 0x000001FE;           // VADS=0
    POWER->PowerManagementItem.STOP = 1;    // STOP=1
    // MCU Enter PM mode now.
    // Process will run continue when MCU wakeup.
    // Process may run interrupt first if occure.
    __nop();
    __nop();
    __nop();
    ADC12B->Control   = 0x000061FE;           // VADS=6
    POWER->FALSHCTRL  = 0x00006495;
}

/******************************************************************************
**  Enter PM3. Include some necessary action.
******************************************************************************/
void EnterPM3(void)
{
    uint32_t altfunset, outenset, pun;
    printf("EnterPM3...\r\n");
    __disable_irq(); // disable all IRQ

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
    GPIO0->ALTFUNCCLR   = 0xFFFFFFFF; // clean all IO function
    GPIO0->OUTENABLECLR = 0xFFFFFFFF; // set all IO to input
    GPIO0->PUN          = 0; // set all IO to pull-high

    GPIO0->OUTENABLESET = (1 << 0) | (1 << 14); // set IO to output
    GPIO0->PUN_PIN.P00  = 1;
    GPIO0->PUN_PIN.P14  = 1;

    GPIO0->OUTENABLECLR = (1 << 12);
    GPIO0->PUN_PIN.P12  = 1; //without pull-up

    //-------------------------------------------------------------------------

    //=========================================================================
    // PM3 process. Don't modify.
    RADIO->STROBE = CMD_SLEEP;              // set RF SLEEP
    POWER->FALSHCTRL  = 0x0000A495;
    ADC12B->Control   = 0x000001FE;
    POWER->POWERCTRL2 = 0x000000B0;
    POWER->POWERCTRL2 = 0x000000F8;
    POWER->PowerManagementItem.STOP = 1;
    // MCU Enter PM mode now.
    // Process will run continue when MCU wakeup.
    POWER->POWERCTRL2 = 0x000000B0;
    ADC12B->Control   = 0x000061FE;
    POWER->FALSHCTRL  = 0x00006495;
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

    UserRegister();     //M8107 registers setting

    StrobeCmd(CMD_STBY);

    SetCH(200); //freq 2450MHz CH100
    //-------------------------------------------------------------------------

    __enable_irq();                         // enable IRQ
    // Interrupt will run if occure.
}
#endif //0

/*******************************
 * TurnOn_External_RTC_Crystal()
 *  Initial external RTC Crystal for sleep timer.
 **********************************/
void TurnOn_External_RTC_Crystal(void)
{
    POWER->RCCTRL   = POWER_RCCTRL_RCTS_Msk;
    POWER->RCCTRL2  = 0x03040012;
    POWER->RCTARGET = 977;
    POWER->RCCTRL   = POWER_RCCTRL_MAN_Msk | POWER_RCCTRL_TMRE_Msk | POWER_RCCTRL_RCTS_Msk;
    Delay1ms(100);
}

/*********************************************************************
* RSSI_measurement
* Received Signal Strength Indication
*********************************************************************/
uint32_t RSSI_measurement(void)
{
    uint32_t tmp = 0;

#if 0
    StrobeCmd(CMD_RX);//entry RX mode
    Delay10us(1);
    while ((RADIO->STATUS & (1 << 8)) == 0) //Stay in RX mode until receiving ID code(sync ok)
    {
        tmp = RADIO->THRESHOLD & 0x1FF;        //read RSSI value(environment RSSI)
    }
    printf("environment RSSI = 0x%X\r\n", tmp);
#endif
    //printf("ADC9 Control register = 0x%X\r\n", RADIO->ADC9);
    tmp = RADIO->THRESHOLD & 0x1FF; //read RSSI value(wanted signal RSSI)
    //printf("wanted signal RSSI = 0x%X\r\n", tmp);

    return (tmp * 0.4) - 120;
}

void M8107RF_SetFIFOLvl(uint32_t fifolen)
{
    RADIO->FIFOCTRL = fifolen - 1;
}

/************************************************************************
** AES128_Encrypt
** Description:
** Parameters:      p_data=address start position for AES_DATA
**                  p_encrypt_key=address start position for AES_KEY
**                  p_dataresult=address start position for encrypted AES_data
** Return value:    none
** Note:
************************************************************************/
void AES128_Encrypt(uint8_t *p_data, uint8_t *p_encrypt_key, uint8_t *p_dataresult)
{
    uint8_t i;

    for (i = 0; i < 16; i++)
        HW8_REG(0x5000C00C + 15 - i) = *(p_encrypt_key + i); //AES_KEY

    for (i = 0; i < 16; i++)
        HW8_REG(0x5000C01C + 15 - i) = *(p_data + i); //AES_DATA

    HW8_REG(0x5000C001) = 0x00;     //AES_ENC
    HW8_REG(0x5000C000) = 0x01;     //AES_GO

    while (HW8_REG(0x5000C000) & 0x01);

    for (i = 0; i < 16; i++)
        *(p_dataresult + i) = HW8_REG(0x5000C01C + 15 - i);
}

/************************************************************************
** AES128_Decrypt
** Description:
** Parameters:      p_data=address start position for AES_DATA
**                  p_encrypt_key=address start position for AES_KEY
**                  p_dataresult=address start position for decrypted AES_data
** Return value:    none
** Note:
************************************************************************/
void AES128_Decrypt(uint8_t *p_data, uint8_t *p_decrypt_key, uint8_t *p_dataresult)
{
    uint8_t i;

    for (i = 0; i < 16; i++)
        HW8_REG(0x5000C00C + 15 - i) = *(p_decrypt_key + i); //AES_KEY

    for (i = 0; i < 16; i++)
        HW8_REG(0x5000C01C + 15 - i) = *(p_data + i); //AES_DATA

    HW8_REG(0x5000C001) = 0x01;//AES_DEC
    HW8_REG(0x5000C000) = 0x01;//AES_GO

    while (HW8_REG(0x5000C000) & 0x01);

    for (i = 0; i < 16; i++)
        *(p_dataresult + i) = HW8_REG(0x5000C01C + 15 - i);
}
