/*************************************************************
 * main.c
 *  Amiccom 2.4G RF Master role for test.
 *  V0.2 - 2017/9/6
 *      1. Add Beacon synchronization and powerdown/wakeup.
 *      2. update 4736 bytes time = 87ms.
 *  V0.3 - 2017/9/12
 *      1. wait RX timeout Timer. Uses TIMER0
 *      2. reply result
 *      3. SLPTIMER_Initial(SLPTIMER1, (32768 - 68), 0, 1);
 *      4. #define BEACONTIMEOUT        2
 *
 *  V0.4 - 2017/9/15
 *      1. Add Beacon field: Heart beat period.
 *      2. Add sleep timer option = 1, 2, 5, 10, 20,40 sec
 *    3. Emulate EPD update time 5s.
 *
 *  V0.5 - 2017/9/22
 *      1. Update Beacon format for Multi-Tag update and Hopping.
 *
 *  V0.6 - 2017/9/27
 *      1. Add DataRate setting. 1Mbps, 500Kbps & 250Kbps.
 *
 *  V0.7 - 2017/10/2
 *      1. Add AES128 security
 *      2. Add RSSI measurement @SYNCBEACON.
 *
 *************************************************************/
#include "main.h"
#include "RptProc.h"
#include "RptComm.h"

uint8_t gDatarate = 1;
uint32_t gBeaconwait = RF_TIMEOUT_BEACON;       //Receive Beacon timeout ms.
uint32_t altfunset, outenset, pun; //IO Setting Info.

const uint8_t BitCount_Tab[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
const uint8_t ID_Tab[8] = {0x44, 0x61, 0x76, 0x69, 0x63, 0x6F, 0x6D, 0x5F}; //ID code
const uint8_t PageTab[8] = {0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70};//page select
uint8_t OriginalData[16] = {0xAA, 0x1, 0, 0, 2, 3, 0x99, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const uint8_t CHTable[16] =
{
    184, 200, 95, 94, 201, 250, 150, 145,
    139, 135, 136, 140, 142, 144, 160, 168
};

/*const uint8_t CHTable_NonWifiRang[50] = {
        44, 45, 46, 47, 48, 49,
        50, 51, 52, 53, 54, 55, 56,
        92, 93, 94, 95, 96, 97, 98,99,
        100, 101, 102, 103, 104,
        144, 145, 146, 147, 148, 149,
        150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
        160, 161,162, 163, 164, 165, 166, 167
};
*/

#if 0
void UserRegister(void)
{
    uint8_t i;

    for (i = 0; i < 4; i++)
        RADIO->SYNCB_ID[3 - i] = ID_Tab[i];//ID Bytes

    POWER->BATTERY      = 0x00000000;//Battery detect register
    POWER->POWERCTRL    = 0x0000C001;//Power control1 register
    POWER->POWERCTRL2   = 0x000000D0;//Power control2 register
    //RADIO->RESET=;//Reset register
    RADIO->FIFOCTRL     = 0x0000003F;//fifo control register
    RADIO->THRESHOLD    = 0x00000032;//RSSI threshold register
    //RADIO->CODE       = 0x01280000;//Code1 register
    RADIO->CODE           = 0x01280011;//Code1 register //Enable CRC
    RADIO->CODE2          = 0x0000002A;//Code2 threshold register
    ADC12B->Control     = 0x000061FE;//ADC control1 register
    ADC12B->IOSEL       = 0x00000000;//ADC channel register
}

void ClearIOSetting()
{
    GPIO0->OUTENABLESET = 0;
    GPIO0->ALTFUNCCLR   = 0xFFFFFFFF;       // clean all IO function
    GPIO0->OUTENABLECLR = 0xFFFFFFFF;       // set all IO to input
    GPIO0->PUN          = 0;                                // set all IO to pull-high
}
#endif //0

/************************************************************************
** UserRegister
************************************************************************/
void UserRegister(void)
{
    uint8_t i = 0;

    RADIO->DATARATE = gDatarate;        //DateRate=500Kbps
    RADIO->FIFOCTRL = 0x0000003F;       //fifo control register
    RADIO->CODE         = 0x01280011;       //Code1 register    //Enable CRC

    for (i = 0; i < 4; i++)
    {
        RADIO->SYNCB_ID[3 - i] = ID_Tab[i];//ID Bytes
    }
}

void ResetReadPointer(void)
{
    RADIO->RESET = 0x20;
}

void BackupIOSetting(void)
{
    altfunset = GPIO0->ALTFUNCSET;
    outenset  = GPIO0->OUTENABLESET;
    pun       = GPIO0->PUN;
}

void RestoreIOSetting(void)
{
    GPIO0->ALTFUNCSET   = altfunset;        // restore ALTFUNCSET
    GPIO0->OUTENABLESET = outenset;         // restore OUTENABLESET
    GPIO0->PUN          = pun;              // restore PUN
}

void ClearIOSetting()
{
    //GPIO0->OUTENABLESET = 0;
    GPIO0->ALTFUNCCLR   = 0xFFFFFFFF;       // clean all IO function
    GPIO0->OUTENABLECLR = 0xFFFFFFFF;       // set all IO to input
    GPIO0->PUN          = 0;                                // set all IO to pull-high

    GPIO0->OUTENABLESET = (1 << 0) | (1 << 14);  // set IO to output
    GPIO0->PUN_PIN.P00 = 1;
    GPIO0->PUN_PIN.P14 = 1;


    GPIO0->OUTENABLECLR = (1 << 12);
    GPIO0->PUN_PIN.P12 = 1; //without pull-up
}

int main(void)
{
    BoardInit();            //Initial hardware system

    initRF(); //init RF

    UserRegister();     //M8107 registers setting

    TurnOn_External_RTC_Crystal();
    BoardInitRTC();

    //NVIC_EnableIRQ(SLPTIMER1_IRQn);

    //BackupIOSetting();

    Rpt_GlobalVar_Init();

    while (1)
    {
        Rpt_Process();
    }

    return 0;
}
