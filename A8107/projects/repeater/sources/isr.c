/************************************************************
 * isr.c
 *
 ************************************************************/
#include "main.h"
#include "slptimer.h"
#include "RptComm.h"
#include "RptIAP.h"

volatile uint8_t Oneshoutflag = 0;

#if 0
void DUALTIMER_Handler(void)
{
    DualTimer_ClearIRQ(DUALTIMER1);
    printf("Dual Timer timeout\r\n");
    Oneshoutflag = 1;
}

void UART2_Handler(void)
{
    char szRecv;

    if (UART_GetRxIRQStatus(UART2))
    {
        UART_ClearRxIRQ(UART2);           // clean Rx interrupt flag
        // UART2 RX receive 8bits data
        szRecv = UART_ReceiveChar(UART2);
        //DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("UART2 Rx: %c\r\n"), szRecv);
    }

    if (UART_GetTxIRQStatus(UART2))
    {
        UART_ClearTxIRQ(UART2);     // clean Tx interrupt flag
        //DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("UART2 Tx:\r\n"));
    }
}
#endif //0

void SLPTIMER1_Handler(void)
{
    SLPTIMER_ClearIRQ(SLPTIMER1);
    //printf( "SLEEP TIMER 0 interrupt occured.\r\n");
    //GPIO0->DATAOUT_PIN.P00 ^= 1;
    //printf("Sleep Timer timeout... \r\n");
}

/************************************************************************
** TIMER1_Handler
************************************************************************/
void TIMER1_Handler(void)
{
    if (TIMER1->INTSTATUS != 0)
    {
        TIMER1->INTCLEAR = 1; //Clear Timer0 interrutp flag

        SetTimer1IntISR(SET);
    }
}

/************************************************************************
** TIMER0_Handler
************************************************************************/
void TIMER0_Handler(void)
{
    if (TIMER0->INTSTATUS != 0)
    {
        Timer_StopTimer(TIMER0);
        TIMER0->INTCLEAR = 1;

        Oneshoutflag = 1;
    }
}

//==============================================================================
// Function     : HardFault_Handler
// Purpose      : HardFault callback function
// Description  :
// Editor       : Richard Chung
// Update Date  : 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   :
// Return       :
// Remarks      :
//==============================================================================
/*void HardFault_Handler(void)
{
      //RROR_MESSAGE(FLAG_MF_SYSTEM, _T(">>>> HardFault <<<<\r\n"));
        printf(">>>> HardFault <<<<\r\n");

    //while(1) {
    //}
        HardFault_ReStartApp();
}
*/
