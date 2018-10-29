
//#include "board.h"
#include "includes.h"
#include "main.h"
#include "A7127RF_Drv.h"
#include "A7127RF_Reg.h"
#include "RptComm.h"

//uint8_t ID_Tab[8] = {0x34, 0x75, 0xC5, 0x2A, 0xC7, 0x33, 0x45, 0xEA}; //ID code
//uint8_t ID_Tab[8] = {0x44, 0x61, 0x76, 0x69, 0x63, 0x6F, 0x6D, 0x5F}; //ID code
//const uint8_t AES128Tab[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
//const uint8_t PageTab[8] = {0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70};//page select

extern uint8_t ID_Tab[8];
extern const uint8_t PageTab[8];
extern uint8_t AES_KEY[];

/******************************************************************
 *  MODE register(02h): 0x62
 *      FMS(bit1) = 1, FIFO mode
 *
 *  FIFO Register I(03h): 0x3F, 0x00
 *      FEP [11:0](bit[15: FIFO End Pointer for TX FIFO and Rx FIFO = 0x003F
 *
 *  GIO2 Pin Control Register II (Address: 0Ch): 0x39
 *      GIO2OE(bit0): GIO2 pin Output Enable. = 1
 *      GIO2S [3:0](bit[5:2]): GIO2 pin function select = 1110b, VPOAK (Valid Packet or Auto ACK OK Output)
 *
 *  CODE1 register(20h): 0x17
 *      IDL[1:0](bit[3:2]): ID Code Length Select. = 01b, 4 bytes ID.
 *      CRCS(bit4): CRC Select = 1, Enable
 *
 *  CODE2 register(21h): 0x85
 *      ETH [2:0](Bit[4:2]): Received ID Code Error Tolerance. = 001b, 1 bit tolerance.
 *      EDRL(bit6): Enable FIFO Dynamic Length
 *      MSCRC(bit7): Mask CRC (CRC Data Filtering Enable). = 1, Enable
 *
 *  FCR register(3Ah): 0x17
 *      FCL [1:0](bit[7:6]) : FCB Length = 00b No Frame Control.
 *      ARC [3:0](bit[5:2] : Auto Resend Cycle Setting. = 0101b, 5 resend cycle.
 *      EAK(bit1) : Enable Auto ACK = 1
 *      EAR(bit0) : Enable Auto Resend. = 1
 *
 *  AFEP Register (Address: 3Ch): 0x00
 *      EAF(bit7): Enable ACK FIFO = 0b, Disable ACJ FIFO
 *
 ******************************************************************/
const uint8_t A7127Config[] = //Amiccom Kevin supports 1Mbps data rate.
{
    0x00, //RESET register,         only reset, not use on config
    0x62, //MODE register,
    0x00, //CALIBRATION register,   only read, not use on config
    0x3F, //FIFO1 register,
    0x00, //FIFO2 register,
    0x00, //FIFO register,          for fifo read/write
    0x00, //IDDATA register,        for idcode
    0x00, //RCOSC1 register,
    0x00, //RCOSC2 register,
    0x00, //RCOSC3 register,
    0x00, //CKO register,
    0x01, //GPIO1 register
    0x01, //GPIO2 register,
    0x5F, //DATARATECLOCK register,
    0x64, //PLL1 register,
    0x0E, //PLL2 register,              RFbase 2400.001MHz
    0x96, //PLL3 register,
    0x00, //PLL4 register,
    0x04, //PLL5 register,
    0x3C, //ChannelGroup1 register,
    0x78, //ChannelGroup1 register,
    0xAD, //TX1 register,               gaussian 0.5T //0xAE Kevin
    0x40, //TX2 register,
    0x12, //DELAY1 register,
    0x60, //DELAY2 register,
    0x70, //RX register,
    0x6F, //RXGAIN1 register,
    0xC2, //RXGAIN2 register,
    0x0D, //RXGAIN3 register,//0x1D Kevin
    0xC7, //RXGAIN4 register,
    0x00, //RSSI register,
    0xF1, //ADC register,
    0x07, //CODE1 register, 0x20
    0x05, //CODE2 register,
    0x2A, //CODE3 register,
    0x20, //IFCAL1 register,
    0xFF, //IFCAL2 register,    0x7F
    0x80, //VCOCCAL register,
    0xD0, //VCOCAL1 register,
    0x00, //VCOCAL2 register,
    0x30, //VCO deviation 1 register,//0x70 Kevin
    0x00, //VCO deviation 2 register,   //28
    0x00, //DSA register,
    0x89, //VCO Modulation delay register,  //0x9C Kevin
    0xF0, //BATTERY register,
    0x37, //TXTEST register,
    0x47, //RXDEM1 register,5F
    0xF7, //RXDEM2 register,
    0xF0, //CPC1 register,
    0x37, //CPC2 register,
    0x55, //CRYSTAL register,0x51
    0x15, //PLLTEST register,
    0x15, //VCOTEST register,
    0x00, //RF Analog register,
    0x00, //Key data register,
    0x17, //Channel select register, //0x37 Kevin
    0x00, //ROM register,
    0x00, //DataRate register,
    0x00, //FCR register,
    0x00, //ARD register,
    0x00, //AFEP register,
    0x00, //FCB register,
    0x00, //KEYC register,
    0x00  //USID register,
};

const uint8_t A7127_Addr2A_Config[] =
{
    0x34, //0x0D,   //Amicomm sugestion for Victor new board. //Default 0x34, //page0, 2E
    0x49, //page1,
    0x00, //Page2,
    0x80, //page3,
    0x80, //page4,
    0x00, //page5,
    0x0A, //page6, 0x00
    0x40, //page7,
};

const uint8_t A7127_Addr38_Config[] =
{
    0x00, //page0,
    0x10, //page1,
    0x20, //page2,
    0x24, //page3,
    0x20, //page4,
};

/*
=================================================================================
SPI_ExchangeByte( );
Function : Exchange a byte via the SPI bus
INTPUT   : input, The input byte
OUTPUT   : The output byte from SPI bus
Modify: Spenser
=================================================================================
*/
uint8_t SPI_ExchangeByte(uint8_t input)
{
    while ((SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_FULL_Msk) != 0);

    SPI_WRITE_TX(SPI, (input & 0xFF));      //Read command + Register offset address

    while ((SPI->FIFORXSTUS & SPI_FIFORXSTUS_EMPTY_Msk) != 0);

    return (SPI_READ_RX(SPI) & 0xFF);
}

/************************************************************************
**  A7127_WriteReg
************************************************************************/
void A7127_WriteReg(uint8_t addr, uint8_t dataByte)
{
    __disable_irq();

    L01_CSN_LOW( );

    SPI_ExchangeByte(addr);
    SPI_ExchangeByte(dataByte);

    L01_CSN_HIGH( );

    __enable_irq();
}

void A7127_WriteMultiReg(uint8_t StartAddr, uint8_t *pBuff, uint8_t Length)
{
    uint8_t i;

    __disable_irq();

    L01_CSN_LOW( );

    SPI_ExchangeByte(StartAddr);

    for (i = 0; i < Length; i++)
    {
        SPI_ExchangeByte(*( pBuff + i ));
    }

    L01_CSN_HIGH( );

    __enable_irq();
}

/*********************************************************************
** A7127_ReadReg
*********************************************************************/
uint8_t A7127_ReadReg(uint8_t Addr)
{
    uint8_t btmp;

    __disable_irq();

    L01_CSN_LOW( );

    SPI_ExchangeByte(R_REGISTER | Addr);

    btmp = SPI_ExchangeByte(0xFF);

    L01_CSN_HIGH( );

    __enable_irq();

    return btmp;
}

void A7127_ReadMultiReg(uint8_t StartAddr, uint8_t nBytes, uint8_t *pBuff)
{
    uint8_t btmp;

    __disable_irq();

    L01_CSN_LOW( );

    SPI_ExchangeByte(R_REGISTER | StartAddr);

    for (btmp = 0; btmp < nBytes; btmp++)
    {
        *(pBuff + btmp) = SPI_ExchangeByte(0x00);
    }

    L01_CSN_HIGH( );

    __enable_irq();
}

/*********************************************************************
** Strobe Command
*********************************************************************/
void A7127RF_StrobeCmd(uint8_t cmd)
{
    __disable_irq();

    L01_CSN_LOW( );

    SPI_ExchangeByte(cmd);

    L01_CSN_HIGH( );

    __enable_irq();
}

/*********************************************************************
** A7127_WriteReg_Page
*********************************************************************/
void A7127_WriteReg_Page(uint8_t addr, uint8_t wbyte, uint8_t page)
{
    A7127_WriteReg(RFANALOG_REG, A7127Config[RFANALOG_REG] | PageTab[page]);//page select
    A7127_WriteReg(addr, wbyte);
}

/*********************************************************************
** SetCH
*********************************************************************/
void A7127RF_SetCHAN(uint8_t ch)
{
    A7127_WriteReg(PLL1_REG, ch); //RF freq = RFbase + (CH_Step * ch)
}

void A7127RF_SetFIFOLen(uint32_t fifolen)
{
    //uint8_t mlen[2];

    A7127_WriteReg(FIFO1_REG, fifolen - 1);
    /*mlen[0] = (uint8_t)(fifolen);
    mlen[1] = (uint8_t)(fifolen >> 8);

    A7127_WriteMultiReg(FIFO1_REG, mlen, 2);
    */
}

/************************************************************************
**  Reset_RF
************************************************************************/
void A7127_Reset(void)
{
    A7127_WriteReg(MODE_REG, 0x00); //reset RF chip

    //A7127_WriteReg(GPIO1_REG, 0x19);  //4-wire SPI
    //printf("MODE_REG = 0x%X\n", A7127_ReadReg(0xA));
}

/************************************************************************
**  WriteID
************************************************************************/
void A7127_WriteID(void)
{
    A7127_WriteMultiReg(IDCODE_REG, ID_Tab, 8);
}

void A7127_ReadID(void)
{
    uint8_t mbuf[8];

    A7127_ReadMultiReg(IDCODE_REG, 8, mbuf );

    printf("RF ID Code: %02X:%02X:%02X:%02X\r\n", mbuf[0], mbuf[1], mbuf[2], mbuf[3]);
}

/*********************************************************************
** A7127_Config
*********************************************************************/
void A7127_Config(void)
{
    uint8_t i;

    //0x00 mode register, for reset
    //0x05 fifo data register
    //0x06 id code register
    //0x3F USID register, read only
    //0x36 key data, 16 bytes
    //0x3D FCB register,4 bytes

    for (i = 0x01; i <= 0x04; i++)
        A7127_WriteReg(i, A7127Config[i]);

    for (i = 0x07; i <= 0x29; i++)
        A7127_WriteReg(i, A7127Config[i]);

    for (i = 0; i <= 7; i++)//0x2A DAS
        A7127_WriteReg_Page(0x2A, A7127_Addr2A_Config[i], i);

    for (i = 0x2B; i <= 0x35; i++)
        A7127_WriteReg(i, A7127Config[i]);

    /* AES128 Table */
    /*for(i = 0; i < 16; i++) {
        //A7127_WriteReg(0x36, AES128Tab[i]);
        A7127_WriteReg(0x36, AES_KEY[i]);
    }*/
    A7127_WriteMultiReg(0x36, AES_KEY, 16);

    A7127_WriteReg(0x37, A7127Config[0x37]);

    for (i = 0; i <= 4; i++)//0x38 ROM
        A7127_WriteReg_Page(0x38, A7127_Addr38_Config[i], i);

    for (i = 0x39; i <= 0x3C; i++)
        A7127_WriteReg(i, A7127Config[i]);

    A7127_WriteReg(0x3E, A7127Config[0x3E]);
}

/*********************************************************************
** A7127_Err_State
*********************************************************************/
void A7127_Err_State(void)
{
    //ERR display
    //Error Proc...
    //...
    while (1);
}

/*********************************************************************
** CHGroupCal
*********************************************************************/
void A7127_CHGroupCal(uint8_t ch)
{
    uint8_t tmp;
    uint8_t vb, vbcf;
    uint8_t deva, adev;

    A7127_WriteReg(PLL1_REG, ch);

    A7127_WriteReg(CALIBRATION_REG, 0x0C);
    do
    {
        tmp = A7127_ReadReg(CALIBRATION_REG);
        tmp &= 0x0C;
    }
    while (tmp);

    //for check
    tmp = A7127_ReadReg(VCOCAL1_REG);
    vb = tmp & 0x07;
    vbcf = (tmp >> 3) & 0x01;

    tmp = A7127_ReadReg(VCODEVCAL1_REG);
    deva = tmp;

    tmp = A7127_ReadReg(VCODEVCAL2_REG);

    adev = tmp;
    if ((adev < 45) || (adev > 68))
    {
        printf("CHGroupCal fail... adev = %d\r\n", adev);
        A7127_Err_State();//error
    }

    if (vbcf)
    {
        printf("CHGroupCal fail... vbcf = %d\r\n", vbcf);
        A7127_Err_State();//error
    }
}

/*********************************************************************
** calibration
*********************************************************************/
void A7127_Cal(void)
{
    uint8_t tmp = 0, loopCnt = 0;
    uint8_t fb = 0, fbcf = 0, fcd = 0, vcb = 0, vccf = 0, rhc = 0, rlc = 0;

    A7127RF_StrobeCmd(CMD_PLL); //calibration @PLL state

    //calibration IF procedure
    while (1)
    {
        A7127_WriteReg(CALIBRATION_REG, 0x02);
        do
        {
            tmp = A7127_ReadReg(CALIBRATION_REG);

            tmp &= 0x02;
        }
        while (tmp);

        tmp = A7127_ReadReg(IFCAL1_REG);
        printf("Cal IFCAL1_REG = 0x%X\r\n", tmp);
        fb = tmp & 0x0F;
        if (fb > 3 && fb < 11)
            break;

        loopCnt++;
        if (loopCnt >= 10)
            break;
    }

    if (loopCnt >= 10)
    {
        printf("A7127_Cal fail, loopCnt = %d\r\n", loopCnt);
        A7127_Err_State();
    }

    //calibration RSSI, VCC procedure
    A7127_WriteReg(CALIBRATION_REG, 0x11);
    do
    {
        tmp = A7127_ReadReg(CALIBRATION_REG);
        tmp &= 0x11;
    }
    while (tmp);

    //calibration VBC,VDC procedure
    A7127_CHGroupCal(30); //calibrate channel group Bank I
    A7127_CHGroupCal(90); //calibrate channel group Bank II
    A7127_CHGroupCal(150); //calibrate channel group Bank III
    A7127RF_StrobeCmd(CMD_STBY); //return to STBY state

    //for check
    tmp = A7127_ReadReg(IFCAL1_REG);
    fb = tmp & 0x0F;
    fbcf = (tmp >> 4) & 0x01;

    tmp = A7127_ReadReg(IFCAL2_REG);
    fcd = tmp & 0x1F;

    tmp = A7127_ReadReg(VCOCCAL_REG) & 0x1F;
    vcb = tmp & 0x0F;
    vccf = (tmp >> 4) & 0x01;

    rhc = A7127_ReadReg(RXGAIN2_REG);
    rlc = A7127_ReadReg(RXGAIN3_REG);

    if (fbcf || vccf)
    {
        printf("Calibration fail... 0x%X, 0x%X\n", fbcf, vccf);
        A7127_Err_State();
    }
}

void GIOSetDataMode(void)
{
    A7127_WriteReg(GPIO1_REG, 0x19);
}

void GIOSetTXMode(void)
{
    A7127_WriteReg(GPIO1_REG, 0x01);    //WTR
    //A7127_WriteReg(GPIO2_REG, 0x39);  //VPOAK
    A7127_WriteReg(GPIO2_REG, 0x11);    //CWTR
    A7127RF_StrobeCmd(CMD_TX);
}

void GIOSetRXMode(void)
{
    A7127_WriteReg(GPIO1_REG, 0x19);    //MISO
    A7127_WriteReg(GPIO2_REG, 0x39);    //VPOAK
    //A7127_WriteReg(GPIO2_REG, 0x01);
    A7127RF_StrobeCmd(CMD_RX);
}

void EnableAutoACKResend(void)
{
    A7127_WriteReg(FCR_REG, 0x17);  // 5 resend
    //A7127_WriteReg(FCR_REG, 0x0B);        // 2 resend
}

void DisableAutoACKResend(void)
{
    A7127_WriteReg(FCR_REG, 0x00);
}

void EnableAutoACK(void)
{
    //A7127_WriteReg(FCR_REG, 0x17);    // 5 resend
    A7127_WriteReg(FCR_REG, 0x0A);      // 2 resend
}

void DisableAutoACK(void)
{
    A7127_WriteReg(FCR_REG, 0x00);
}

void EnableCRCFilter(void)
{
    A7127_WriteReg(CODE1_REG, 0x17);
    A7127_WriteReg(CODE2_REG, 0x85);
}

void DisableCRCFilter(void)
{
    A7127_WriteReg(CODE1_REG, 0x07);
    A7127_WriteReg(CODE2_REG, 0x05);
}

/*********************************************************************
** init RF A7127
*********************************************************************/
void A7127RF_Iinit(void)
{
    A7127_Reset(); //reset A7105 RF chip

    A7127_WriteID(); //write ID code

    A7127_Config(); //config A7105 chip
    //A7127_WriteReg(DATARATECLOCK, 0x01);  //DataRate = 500Kbps

    EnableCRCFilter();
    //EnableAutoACK();
    //EnableAutoACKResend();
    GIOSetDataMode();   //SPI 4-wire

    A7127_Cal(); //calibration IF,VCO,VCOC

    A7127_ReadID(); //Read ID code
}

void A7127RF_SPI_Config(void)
{
    SPI_Initial(SPI, 1, 7, 0);

    SPI_DATA_WIDTH(SPI, 8);

    //set SPI IO (P0_00(CSS), P0_01(MISO), P0_02(MOSI), P0_03(CLK))
    GPIO0->ALTFUNCSET = (A7127_SPI_CSS_PIN | A7127_SPI_MISO_PIN | A7127_SPI_MOSI_PIN | A7127_SPI_SCLK_PIN);
    //GPIO0->ALTFUNCSET = (0xF << 0);
}

void A7127RF_Enable(void)
{
    A7127RF_SPI_Config();

    A7127RF_Iinit();    //DataRate setting

    A7127_WriteReg(DATARATECLOCK, RptDefSet.RptDataRate);   //DataRate = 500Kbps

    /* Enable AES128 */
    //A7127_WriteReg(KEYC_REG, 0x09);

    GIOSetDataMode();   //GIO1 = MISO

    A7127RF_StrobeCmd(CMD_STBY);

    //A7127RF_SetCHAN(RptDefSet.Gchn);

    printf("Enable A7127 RF Module ...!!!\r\n");
}
