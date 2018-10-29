/**************************************************************************//**
 * @file        watchdog.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       WATCHDOG Driver Header File
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/

#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup WATCHDOG_Driver Timer Driver
    @{
*/

/**
 * @brief   Initialises the watchdog to specifies the load value.
 */
extern void Watchdog_Initial(WATCHDOG_Type *watchdog, uint32_t load, uint32_t irq_en, uint32_t resen);

/**
 * @brief   Returns the current value of the watchdog.
 */
extern uint32_t Watchdog_GetValue(WATCHDOG_Type *watchdog);

/**
 * @brief   Enable watchdog and IRQ.
 */
extern void Watchdog_EnableIRQ(WATCHDOG_Type *watchdog);

/**
 * @brief   Disable watchdog and IRQ.
 */
extern void Watchdog_DisableIRQ(WATCHDOG_Type *watchdog);

/**
 * @brief   Clears the watchdog IRQ.
 */
extern void Watchdog_ClearIRQ(WATCHDOG_Type *watchdog);

/**
 * @brief   Enable watchdog reset.
 */
extern void Watchdog_EnableReset(WATCHDOG_Type *watchdog);

/**
 * @brief   Disable watchdog reset.
 */
extern void Watchdog_DisableReset(WATCHDOG_Type *watchdog);


/*@}*/ /* end of group DUALTIMER_Driver */
/*@}*/ /* end of group Std_Driver */

#endif
