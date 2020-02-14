/**************************************************************************//**
 * @file        RTC.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       RTC Driver source file
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#ifndef _RTC_H_
#define _RTC_H_


/** @addtogroup A8107M_Driver Standard Driver
    @{
*/
/** @addtogroup RTC_Driver Real Time Clock Driver
    @{
*/

/**
 * @brief   Initial RTC and Start counting
 */
extern void RTC_Initial(uint32_t clksel, uint32_t week, uint32_t hour, uint32_t min, uint32_t sec);

/**
 * @brief   RTC Start counting
 */
extern void RTC_Start(void);

/**
 * @brief   RTC Stop count
 */
extern void RTC_Stop(void);

/**
 * @brief   Set Alarm. The Alarm interrupt will occure when RTC->CTRL[hour:min] == RTC->ALARM[hour:min]
 */
extern void RTC_SetAlarm(uint32_t hour, uint32_t min);

/**
 * @brief   Set Manual Count by 10ms
 */
extern void RTC_ManualCount_Set(uint32_t en, uint32_t count);

/**
 * @brief   Enable RTC interrupt
 */
extern void RTC_Interrupt_Enable(uint32_t alarm, uint32_t sec, uint32_t half_sec, uint32_t ten_ms, uint32_t manual_count);

void RTC_PrintTime(void);

/*@}*/ /* end of group RTC_Driver */
/*@}*/ /* end of group A8107M_Driver */

#endif
