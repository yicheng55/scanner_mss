// Board.c: implementation of the board hardware functions.
//
//////////////////////////////////////////////////////////////////////
#include "main.h"
#include "EslDebug.h"
#include "Board.h"
//#include "tagdefine.h"
#include "spi.h"
#include "RTC.h"
#include "ProjectConfig.h"
#include "SPIConfig.h"

void DateAndHour(int Date, int Hour);

#if 0
/*******************************
 * TurnOn_External_RTC_Crystal()
 *  Initial external RTC Crystal for sleep timer.
 **********************************/
void TurnOn_External_RTC_Crystal(void)
{
    POWER->RCCTRL = POWER_RCCTRL_RCTS_Msk;
    POWER->RCCTRL2 = 0x03040012;
    POWER->RCTARGET = 977;
    POWER->RCCTRL = POWER_RCCTRL_MAN_Msk | POWER_RCCTRL_TMRE_Msk | POWER_RCCTRL_RCTS_Msk;
    Delay1ms(100);
}
#endif //0

// Remarked by Richard
void HW_init(void)
{
    GPIO0->OUTENABLESET = 0xFFFFFF;
    GPIO0->DATA = 0xFFFFFF;

    while ((GPIO0->DATA & (1 << 20)) == 0);
}

//==============================================================================
// Function     :
// Purpose      : Initial Peripherals
// Description  :
// Editor       : Richard Chung
// Update Date  : 2018-03-01
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
    /*SetMessageFunc (FLAG_MF_APPLICATION | FLAG_MF_KERNEL | FLAG_MF_HARDWARE | FLAG_MF_SYSTEM |
                    FLAG_MF_DRIVER | FLAG_MF_FIRMWARE | FLAG_MF_COMMUNICATION | FLAG_MF_SCANNER |
    FLAG_MF_REPEATER | FLAG_MF_TAG);
    */
    //SetMessageLevel (FLAG_MT_ALL);

    //HW_init();
    BoardInitUART0(UART0_BAUDRATE); // set UART0 for console
    //BoardInitUART2(UART2_BAUDRATE);   // set UART2 for Scanner

    BoardInitGPIO();
    //BoardInitSPI();

    //SetOneshotTimer0(100);
    SetOneshotTimer(TIMER0, 100);

    printf("Board initialization completed\r\n");
    //ScannerDebug(_T("Board initialization completed\r\n"));
}

//==============================================================================
// Function     : InitUart
// Purpose      : Init Uart for debug mesage
// Description  :
// Editor       : Richard Chung
// Update Date  : 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   :
// Return       :
// Remarks      :
//==============================================================================
void BoardInitUART0(uint32_t uiBaudrate)
{
    UartStdOutInit(UART0, uiBaudrate);   // set UART to stdio interface, baudrate=115200bps
    GPIO0->ALTFUNCSET = (1 << 17);        // set UART TX IO

    // Show information
    //INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("\r\nSet UART0 IO (P0_17)\r\n"));
    //INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Initial UART0: Baud rate = %d, Enable TX\r\n"), uiBaudrate);
}

#if 0 //non use
//==============================================================================
// Function     : InitUart
// Purpose      : Init Uart for debug mesage
// Description  :
// Editor       : Richard Chung
// Update Date  : 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   :
// Return       :
// Remarks      :
//==============================================================================
void BoardInitUART2(uint32_t uiBaudrate)
{
    //INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("\r\nSet UART2 IO (P0_20, P0_21)\r\n"));
    GPIO0->ALTFUNCSET = (1 << 20) | (1 << 21);              // set UART2 IO(RX, TX) enable

    //INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Initial UART2: Baud rate = %d, Enable TX, RX, TXINT, RXINT\r\n"), uiBaudrate);
    UART_init(UART2, XTAL / uiBaudrate, 1, 1, 1, 1, 0, 0); // Enable UART2 Tx, Rx, TxInt, RxInt

    //printf("Initial UART2: Baud rate = %d, Enable RX, RXINT\r\n", uiBaudrate);
    //UART_init(UART2, SystemCoreClock/uiBaudrate, 0, 1, 0, 1, 0, 0); // Enable UART2 Tx, Rx, TxInt, RxInt

    //INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Enable UART2_IRQn\r\n"));
    NVIC_EnableIRQ(UART2_IRQn);                         // Enable UART2_IRQn
}
#endif //0

//==============================================================================
// Function     : SetOneshotTimer
// Purpose      :
// Description  :
// Editor       : Richard Chung
// Update Date  : 2018-03-01
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

    timer->INTCLEAR = 0x01;                         //clear timer interrupt
    timer->RELOAD = ((SystemCoreClock / 1000) - 3) * msec;
    timer->VALUE = ((SystemCoreClock / 1000) - 3) * msec;   //1ms for Xtal=16MHz

    timer->CTRL = (1 << 3) | (1 << 0);                  //timer enable & interrupt

    // Enable Timer Interrupt
    if (timer == TIMER0)
        NVIC_EnableIRQ(TIMER0_IRQn);
    else if (timer == TIMER1)
        NVIC_EnableIRQ(TIMER1_IRQn);
}

#if 0 //non use
//==============================================================================
// Function     : InitTimer0
// Purpose      : init Timer0 for 1ms.
// Description  :
// Editor       : Richard Chung
// Update Date  : 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   :
// Return       :
// Remarks      :
//==============================================================================
void BoardInitTimer0(void)
{
    NVIC_DisableIRQ(TIMER0_IRQn);

    TIMER0->RELOAD = SystemCoreClock / 1000;
    TIMER0->VALUE = SystemCoreClock / 1000;//1ms for Xtal=16MHz
    TIMER0->INTCLEAR = 0x01; //clear timer interrupt

    TIMER0->CTRL = (1 << 3) | (1 << 0);//timer enable & interrupt

    NVIC_EnableIRQ(TIMER0_IRQn);  /* Enable Timer Interrupt */
}

//==============================================================================
// Function     :
// Purpose      :
// Description  :
// Editor       : Richard Chung
// Update Date  : 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   :
//              : uint32_t msec:
//              : one shot period(ms).
// Return       :
// Remarks      :
//==============================================================================
void SetOneshotTimer0(uint32_t msec)
{
    NVIC_DisableIRQ(TIMER0_IRQn);

    TIMER0->INTCLEAR = 0x01;                                                //clear timer interrupt

    TIMER0->RELOAD = ((SystemCoreClock / 1000) - 3) * msec;
    TIMER0->VALUE = ((SystemCoreClock / 1000) - 3) * msec;//1ms for Xtal=16MHz

    TIMER0->CTRL = (1 << 3) | (1 << 0);                                 //timer enable & interrupt

    NVIC_EnableIRQ(TIMER0_IRQn);                                    // Enable Timer Interrupt
}
#endif //0

//==============================================================================
// Function     :
// Purpose      :
// Description  :
// Editor       : Richard Chung
// Update Date  : 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   :
// Return       :
// Remarks      :
//==============================================================================
void SetTimer(TIMER_Type *timer, uint32_t msec)
{
    timer->RELOAD = ((SystemCoreClock / 1000) - 3) * msec;
    timer->VALUE = ((SystemCoreClock / 1000) - 3) * msec;

    Timer_StartTimer(timer);
}

/**
 *
 * @brief   SPI master mode transfer data and receive data throught SPI
 *
 * @param   *spi                    SPI Pointer
 * @param   *txBuf                  8bits buffer for send
 * @param   *rxBuf                  8bits buffer for receive
 * @param   length                  Length for send data
 * @return  none
 */
void SPI_Set_DataWidth(uint8_t u32DataWidth)
{
    SPI->CTRL = (SPI->CTRL & 0xF0FF) | ((u32DataWidth - 1) << SPI_CTL_TX_BIT_LEN_Pos);
    SPI->STATUS |= 0x01;
}

//==============================================================================
// Function     :
// Purpose      :
// Description  :
// Editor       : Richard Chung
// Update Date  : 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   :
// Return       :
// Remarks      :
//==============================================================================
void BoardInitSPI()
{
    //SPI_Initial(SPI, 1, 7, 0);
    SPI_Initial(SPI, 1, 1, 0);

    SPI_Set_DataWidth(9);

    GPIO0->ALTFUNCSET = (1 << 0) | (1 << 2) | (1 << 3);//(0xF<<0);       // set SPI IO (P0_00~P0_03)
}

//==============================================================================
// Function     :
// Purpose      :
// Description  :
// Editor       : Richard Chung
// Update Date  : 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   :
// Return       :
// Remarks      :
//==============================================================================
void BoardInitGPIO()
{
    GPIO0->OUTENABLESET = (1 << 14);
    GPIO0->OUTENABLECLR = (1 << 13);
    //GPIO0->OUTENABLECLR = (1 << 8);

    GPIO0->OUTENABLESET = (1 << 11);
    GPIO0->OUTENABLESET = (1 << 15);

    GPIO0->DATA_PIN.P08 = 0;
}


//==============================================================================
// Function     :
// Purpose      :
// Description  :
// Editor       : Richard Chung
// Update Date  : 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   :
// Return       :
// Remarks      :
//==============================================================================
void BoardInitRTC()
{
    //printf("Inital RTC: using 32.768KHz, set WEEK=0, HOUR=0, MIN=0, SEC=0\r\n");
    RTC_Initial(1, 0, 0, 0, 0);             // cksel=1=32.768KHz, week=0, hour=0, min=0, sec=0
    DateAndHour(0, 0);
    RTC_Start();
}
