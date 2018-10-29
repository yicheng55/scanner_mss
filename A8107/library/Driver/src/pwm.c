/**************************************************************************//**
 * @file        PWM.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       PWM Driver source file
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "AMICCOM_CM0.h"

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup PWM_Driver Watchdog Driver
    @{
*/

/**
 *
 * @brief   Initialises the PWM.
 *
 * @param   *pwm                    PWM Pointer
 * @param   prescal                 PWM prescal. Value range = 0~5.
 * @param   cks                     PWM clock source select. 0=system clock, 1=RTC or IRC
 * @param   enable                  PWM enable. 0=disable PWM, 1=enable PWM.
 * @param   low                     PWM LOW  width setting. The low value must low+high<=256
 * @param   high                    PWM HIGH width setting. The high value range is 1~255
 * @return  0: PASS, others: FAIL
 */
uint32_t PWM_Initial(PWM_Type *pwm, uint32_t prescal, uint32_t cks, uint8_t enable, uint8_t low, uint8_t high)
{
    uint32_t cnt_low, cnt_high;

    if(0 == high)       return 1;
    if(low+high > 256)  return 1;
    if(prescal > 5)     return 1;

    cnt_high = 256-high;
    cnt_low  = 256-low-high;

    pwm->HIGH_LOW = (cnt_low<<PWM_HIGH_LOW_LOW_Pos) | (cnt_high<<PWM_HIGH_LOW_HIGH_Pos);
    pwm->CTRL = ((prescal&PWM_CTRL_PRESCAL_Msk)<<PWM_CTRL_PRESCAL_Pos);
    if(cks)     pwm->CTRL |= PWM_CTRL_CKS_Msk;
    if(enable)  pwm->CTRL |= PWM_CTRL_EN_Msk;
    return 0;
}

/**
 *
 * @brief   Enable PWM
 *
 * @param   *pwm                    PWM Pointer
 * @return  none
 */
void PWM_Enable(PWM_Type *pwm)
{
    pwm->CTRL |= PWM_CTRL_EN_Msk;
}

/**
 *
 * @brief   Disable PWM
 *
 * @param   *pwm                    PWM Pointer
 * @return  none
 */
void PWM_Disable(PWM_Type *pwm)
{
    pwm->CTRL &= ~PWM_CTRL_EN_Msk;
}



/*@}*/ /* end of group PWM_Driver */
/*@}*/ /* end of group Std_Driver */
