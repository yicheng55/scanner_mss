/**************************************************************************//**
 * @file        watchdog.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       Watchdog Driver source file
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "AMICCOM_CM0.h"

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup WATCHDOG_Driver Watchdog Driver
    @{
*/

/**
 *
 * @brief   Initialises the watchdog to specifies the load value.
 *
 * @param   *watchdog           Watchdog Pointer
 * @param   load                The value to which the watchdog is to be set.
 * @param   irq_en              set 1 to enalbe Watchdog and interrupt. Clean 0 to disable Watchdog and interrupt.
 * @param   resen               set 1 to enalbe watchdog reset. Clean 0 to disable Watchdog resetarnd interrupt.
 *                              MCU will be reset when second enter interrupt and enalbe reset and not clear interrupt.
 * @return  none
 */
void Watchdog_Initial(WATCHDOG_Type *watchdog, uint32_t load, uint32_t irq_en, uint32_t resen)
{
    watchdog->LOCK = 0x1ACCE551;    // unlock
    watchdog->LOAD = load;
    watchdog->CTRL = ((irq_en << WATCHDOG_CTRL_INTEN_Pos) & WATCHDOG_CTRL_INTEN_Msk) | 
                     ((resen  << WATCHDOG_CTRL_RESEN_Pos) & WATCHDOG_CTRL_RESEN_Msk);
    watchdog->LOCK = 0;             // lock
}

/**
 *
 * @brief   Returns the current value of the watchdog.
 *
 * @param   *watchdog           Watchdog Pointer
 * @return  Value
 */
uint32_t Watchdog_GetValue(WATCHDOG_Type *watchdog)
{
    return watchdog->VALUE;
}


/**
 *
 * @brief   Enable watchdog and IRQ.
 *
 * @param   *watchdog           Watchdog Pointer
 * @return  none
 */
void Watchdog_EnableIRQ(WATCHDOG_Type *watchdog)
{
    watchdog->LOCK = 0x1ACCE551;    // unlock
    watchdog->CTRL |= WATCHDOG_CTRL_INTEN_Msk;
    watchdog->LOCK = 0;             // lock
}

/**
 *
 * @brief   Disable watchdog and IRQ.
 *
 * @param   *watchdog           Watchdog Pointer
 * @return  none
 */
void Watchdog_DisableIRQ(WATCHDOG_Type *watchdog)
{
    watchdog->LOCK = 0x1ACCE551;    // unlock
    watchdog->CTRL &= ~WATCHDOG_CTRL_INTEN_Msk;
    watchdog->LOCK = 0;             // lock
}

/**
 *
 * @brief   Clears the watchdog IRQ.
 *
 * @param   *watchdog            Watchdog Pointer
 * @return  none
 */
void Watchdog_ClearIRQ(WATCHDOG_Type *watchdog)
{
    watchdog->LOCK = 0x1ACCE551;    // unlock
    watchdog->INTCLR = 1;
    watchdog->LOCK = 0;             // lock
}

/**
 *
 * @brief   Enable watchdog reset.
 *
 * @param   *watchdog           Watchdog Pointer
 * @return  none
 */
void Watchdog_EnableReset(WATCHDOG_Type *watchdog)
{
    watchdog->LOCK = 0x1ACCE551;    // unlock
    watchdog->CTRL |= WATCHDOG_CTRL_RESEN_Msk;
    watchdog->LOCK = 0;             // lock
}

/**
 *
 * @brief   Disable watchdog reset.
 *
 * @param   *watchdog           Watchdog Pointer
 * @return  none
 */
void Watchdog_DisableReset(WATCHDOG_Type *watchdog)
{
    watchdog->LOCK = 0x1ACCE551;    // unlock
    watchdog->CTRL &= ~WATCHDOG_CTRL_RESEN_Msk;
    watchdog->LOCK = 0;             // lock
}


/*@}*/ /* end of group WATCHDOG_Driver */
/*@}*/ /* end of group Std_Driver */
