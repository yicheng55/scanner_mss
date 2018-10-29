/**************************************************************************//**
 * @file        pwm.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       PWM Driver Header File
 *
 * @note
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/

#ifndef __PWM_H__
#define __PWM_H__

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup PWM_Driver Timer Driver
    @{
*/

/**
 * @brief   Initialises the PWM.
 */
extern uint32_t PWM_Initial(PWM_Type *pwm, uint32_t prescal, uint32_t cks, uint32_t enable, uint8_t low, uint8_t high);

/**
 * @brief   Enable PWM
 */
extern void PWM_Enable(PWM_Type *pwm);

/**
 * @brief   Disable PWM
 */
extern void PWM_Disable(PWM_Type *pwm);

/*@}*/ /* end of group PWM_Driver */
/*@}*/ /* end of group Std_Driver */

#endif
