/**************************************************************************//**
 * @file        timer.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       TIMER Driver source file
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "AMICCOM_CM0.h"

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup TIMER_Driver Timer Driver
    @{
*/

/**
 *
 * @brief   Enable the microcontroller timer interrupts.
 *
 * @param   *timer              Timer Pointer
 * @return  none
 */
void Timer_EnableIRQ(TIMER_Type *timer)
{
    timer->CTRL |= TIMER_CTRL_IRQEN_Msk;
}

/**
 *
 * @brief   Disable the microcontroller timer interrutps.
 *
 * @param   *timer              Timer Pointer
 * @return  none
 */
void Timer_DisableIRQ(TIMER_Type *timer)
{
    timer->CTRL &= ~TIMER_CTRL_IRQEN_Msk;
}

/**
 *
 * @brief   Start the Timer.
 *
 * @param   *timer              Timer Pointer
 * @return  none
 */

void Timer_StartTimer(TIMER_Type *timer)
{
    timer->CTRL |= TIMER_CTRL_EN_Msk;
}

/**
 *
 * @brief   Stop the Timer.
 *
 * @param   *timer              Timer Pointer
 * @return  none
 */
void Timer_StopTimer(TIMER_Type *timer)
{
    timer->CTRL &= ~TIMER_CTRL_EN_Msk;
}

/**
 *
 * @brief   Returns the current value of the timer.
 *
 * @param   *timer              Timer Pointer
 * @return  TimerValue
 */
uint32_t Timer_GetValue(TIMER_Type *timer)
{
    return timer->VALUE;
}

/**
 *
 * @brief  Sets the timer to the specified value.
 *
 * @param   *timer              Timer Pointer
 * @param   value               The value to which the timer is to be set
 * @return  TimerValue
 */
void Timer_SetValue(TIMER_Type *timer, uint32_t value)
{
    timer->VALUE = value;
}

/**
 *
 * @brief   Returns the reload value of the timer. The reload value is the value which the timer is set to after an underflow occurs.
 *
 * @param   *timer              Timer Pointer
 * @return  TimerReload
 */
uint32_t Timer_GetReload(TIMER_Type *timer)
{
    return timer->RELOAD;
}

/**
 *
 * @brief   Sets the reload value of the timer to the specified value. The reload value is the value which the timer is set to after an underflow occurs.
 *
 * @param   *timer              Timer Pointer
 * @param   value               Value to be loaded
 * @return  none
 */
void Timer_SetReload(TIMER_Type *timer, uint32_t value)
{
    timer->RELOAD = value;
}

/**
 *
 * @brief   Clears the timer IRQ if set.
 *
 * @param   *timer              Timer Pointer
 * @return  none
 */
void Timer_ClearIRQ(TIMER_Type *timer)
{
    timer->INTCLEAR = TIMER_INTCLEAR_Msk;
}

/**
 *
 * @brief   Returns the IRQ status of the timer in question.
 *
 * @param   *timer              Timer Pointer
 * @return  none
 */
uint32_t  Timer_StatusIRQ(TIMER_Type *timer)
{
    return timer->INTSTATUS;
}

/**
 *
 * @brief   Initialises the timer to use the internal clock and specifies the timer reload value and whether IRQ is enabled or not.
 *
 * @param   *timer                  Timer Pointer
 * @param   reload                  The value to which the timer is to be set after an underflow has occurred
 * @param   irq_en                  Defines whether the timer IRQ is to be enabled
 * @return  none
 */
void Timer_Init_IntClock(TIMER_Type *timer, uint32_t reload, uint32_t irq_en)
{
    timer->CTRL = 0;
    timer->VALUE = reload;
    timer->RELOAD = reload;
    if (irq_en!=0) {                                                                    /* non zero - enable IRQ */
        timer->CTRL = (TIMER_CTRL_IRQEN_Msk | TIMER_CTRL_EN_Msk);
    }
    else {                                                                              /* zero - do not enable IRQ */
        timer->CTRL = ( TIMER_CTRL_EN_Msk);                                             /* enable timer */
    }
}

/**
 *
 * @brief   Initialises the timer to use the external clock and specifies the timer reload value and whether IRQ is enabled or not.
 *
 * @param   *timer              Timer Pointer
 * @param   reload              The value to which the timer is to be set after an underflow has occurred
 * @param   irq_en              Defines whether the timer IRQ is to be enabled
 * @return  none
 */
void Timer_Init_ExtClock(TIMER_Type *timer, uint32_t reload, uint32_t irq_en)
{
    timer->CTRL = 0;
    timer->VALUE = reload;
    timer->RELOAD = reload;
    if (irq_en!=0) {                                                                        /* non zero - enable IRQ */
        timer->CTRL = (TIMER_CTRL_IRQEN_Msk | TIMER_CTRL_SELEXTCLK_Msk | TIMER_CTRL_EN_Msk);
    }
    else  {                                                                                 /* zero - do not enable IRQ */
        timer->CTRL = (                       TIMER_CTRL_SELEXTCLK_Msk | TIMER_CTRL_EN_Msk);/* enable timer */
    }
}

/**
 *
 * @brief  Initialises the timer to use the internal clock but with an external enable. It also specifies the timer reload value and whether IRQ is enabled or not.
 *
 * @param   *timer              Timer Pointer
 * @param   reload              The value to which the timer is to be set after an underflow has occurred
 * @param   irq_en              Defines whether the timer IRQ is to be enabled
 * @return  none
 */
void Timer_Init_ExtEnable(TIMER_Type *timer, uint32_t reload, uint32_t irq_en)
{
    timer->CTRL = 0;
    timer->VALUE = reload;
    timer->RELOAD = reload;
    if (irq_en!=0) {                                                                        /* non zero - enable IRQ */
        timer->CTRL = (TIMER_CTRL_IRQEN_Msk | TIMER_CTRL_SELEXTEN_Msk | TIMER_CTRL_EN_Msk);
    }
    else {                                                                                  /* zero - do not enable IRQ */
        timer->CTRL = (                       TIMER_CTRL_SELEXTEN_Msk | TIMER_CTRL_EN_Msk); /* enable timer */
    }
}

/*@}*/ /* end of group TIMER_Driver */
/*@}*/ /* end of group Std_Driver */
