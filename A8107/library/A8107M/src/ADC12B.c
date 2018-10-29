/**************************************************************************//**
 * @file        ADC12B.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       12Bits SAR ADC Driver source file
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "A8107M.h"

/** @addtogroup A8107M_Driver Standard Driver
    @{
*/
/** @addtogroup ADC12B_Driver ADC12B Driver
    @{
*/

/**
 *
 * @brief   Initial 12Bits SAR ADC and Start running with adcch
 *
 * @param   *adc12b                 ADC12B Pointer
 * @param   ch                      Select ADC Channel. Value=0~7.
 * @return  ADC value by moving average 8 times
 */
uint32_t ADC12B_Measure(ADC12B_Type *adc12b, uint32_t ch)
{
    uint32_t control = 0;
    uint32_t mvadc;

    adc12b->Control = ADC12B_CONTROL_ADC12RN_Msk;   // ADC reset
    control = ADC12B_CONTROL_ADCE_Msk |         // start ADC measure
            (6 << ADC12B_CONTROL_VADS_Pos) |    // Ref Voltage = 1.8V
            (3 << ADC12B_CONTROL_MVS_Pos) |     // Average 8 times
            (0 << ADC12B_CONTROL_CKS_Pos) |     // clock 4MHz
            ADC12B_CONTROL_ENADC_Msk |          // ADC clock on
            ADC12B_CONTROL_MODE_Msk;            // continue mode

    adc12b->IOSEL = ((ch << ADC12B_IOSEL_CH_Pos) & ADC12B_IOSEL_CH_Msk) | ADC12B_IOSEL_EN_Msk;
    POWER->DCDCCTRL2 = 0;                       // switch to DCDC bypass mode
    adc12b->Control = control;                  // ADC measure start

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

    mvadc = adc12b->ValueItem.MVADC;            // read mvadc value
    adc12b->Control = 0;                        // close ADC 12Bits
    adc12b->IOSEL = 0;                          // close ADC channel
    POWER->DCDCCTRL2 = 241;                     // switch to DCDC PWM mode

    return mvadc;
}

/*@}*/ /* end of group ADC12B_Driver */
/*@}*/ /* end of group A8107M_Driver */

