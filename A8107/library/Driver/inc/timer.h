/**************************************************************************//**
 * @file        timer.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       TIMER Driver Header File
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/

#ifndef __TIMER_H__
#define __TIMER_H__

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup TIMER_Driver Timer Driver
    @{
*/

/**
 * @brief   Initialises the timer to use the internal clock and specifies the timer reload value and whether IRQ is enabled or not.
 */
extern void Timer_Init_IntClock(TIMER_Type *timer, uint32_t reload, uint32_t irq_en);

/**
* @brief Set CMSDK Timer for multi-shoot mode with external enable
*/
extern void Timer_Init_ExtClock(TIMER_Type *timer, uint32_t reload, uint32_t irq_en);


/**
* @brief Set CMSDK Timer for multi-shoot mode with external clock
*/
extern void Timer_Init_ExtEnable(TIMER_Type *timer, uint32_t reload, uint32_t irq_en);

/**
* @brief CMSDK Timer interrupt clear
*/
extern void Timer_ClearIRQ(TIMER_Type *timer);

/**
* @brief Returns timer IRQ status
*/
uint32_t  Timer_StatusIRQ(TIMER_Type *timer);

/**
* @brief Returns Timer Reload value.
*/
extern uint32_t Timer_GetReload(TIMER_Type *timer);

/**
* @brief Sets Timer Reload value.
*/
extern void Timer_SetReload(TIMER_Type *timer, uint32_t value);

/**
* @brief Returns Timer current value.
*/
uint32_t Timer_GetValue(TIMER_Type *timer);

/**
* @brief Sets Timer current value.
*/
extern void Timer_SetValue(TIMER_Type *timer, uint32_t value);

/**
* @brief Stops CMSDK Timer.
*/
extern void Timer_StopTimer(TIMER_Type *timer);

/**
* @brief Starts CMSDK Timer.
*/
extern void Timer_StartTimer(TIMER_Type *timer);

/**
* @brief Enables CMSDK Timer Interrupt requests.
*/
extern void Timer_EnableIRQ(TIMER_Type *timer);

/**
* @brief Disables CMSDK Timer Interrupt requests.
*/
extern void Timer_DisableIRQ(TIMER_Type *timer);

/*@}*/ /* end of group TIMER_Driver */
/*@}*/ /* end of group Std_Driver */

#endif
