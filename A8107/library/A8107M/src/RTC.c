/**************************************************************************//**
 * @file        RTC.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       RTC TIMER Driver source file
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "A8107M.h"

/** @addtogroup A8107M_Driver A8107M Driver
    @{
*/
/** @addtogroup RTC_Driver Real Time Clock Driver
    @{
*/

/**
 *
 * @brief   Initial RTC
 *
 * @param   clksel                  Clock source select. 0=32KHz, 1=32.768KHz
 * @param   week                    Set WEEK, range=0~6
 * @param   hour                    Set HOUR, range=0~23
 * @param   min                     Set MIN,  range=0~59
 * @param   sec                     Set SEC,  range=0~59
 * @return  none
 */
void RTC_Initial(uint32_t clksel, uint32_t week, uint32_t hour, uint32_t min, uint32_t sec)
{
    RTC->CTRL = 0;
    RTC->COUNT = ((week & 0x3ul) << RTC_COUNT_WEEK_Pos) |
                ((hour & 0x1Ful) << RTC_COUNT_HOUR_Pos) |
                ((min & 0x3Ful) << RTC_COUNT_MIN_Pos) |
                ((sec & 0x3Ful) << RTC_COUNT_SEC_Pos);
    RTC->CTRL = ((clksel & 0x1ul) << RTC_CTRL_CLKSEL_Pos) |
                RTC_CTRL_ON_Msk;
}

/**
 *
 * @brief   RTC Start counting
 *
 * @return  none
 */
void RTC_Start(void)
{
    RTC->CTRL |= RTC_CTRL_EN_Msk;
}


/**
 *
 * @brief   RTC Stop count
 *
 * @return  none
 */
void RTC_Stop(void)
{
    RTC->CTRL &= ~RTC_CTRL_EN_Msk;
}


/**
 *
* @brief   Set Alarm. The Alarm interrupt will occure when RTC->CTRL[hour:min] == RTC->ALARM[hour:min]
 *
 * @param   hour                    Set HOUR, range=0~23
 * @param   min                     Set MIN,  range=0~59
 * @return  none
 */
void RTC_SetAlarm(uint32_t hour, uint32_t min)
{
    RTC->ALARM = ((hour & 0x3Ful) << RTC_ALARM_HOUR_Pos) |
                 ((min  & 0x3Ful) << RTC_ALARM_MIN_Pos);
}

/**
 *
 * @brief   Set Manual Count
 *
 * @param   en                      Menaul Count Enable. 0=disable, 1=enable
 * @param   count                   Menaul Count interrupt
 * @return  none
 */
void RTC_ManualCount_Set(uint32_t en, uint32_t count)
{
    RTC->MANUALCOUNT = (count & 0xFFul) << RTC_MANUALCOUNT_COUNT_Pos;
    RTC->CTRL |= (en & 0x1ul) << RTC_CTRL_MANUALCOUNTEN_Pos;
}

/**
 *
 * @brief   Enable RTC interrupt
 *
 * @param   alarm                   Alarm interrupt. 0=disable, 1=enable.
 * @param   sec                     Sec interrupt. 0=disable, 1=enable.
 * @param   half_sec                Half sec interupt. 0=disable, 1=enable.
 * @param   ten_ms                  Ten ms interrupt. 0=disable, 1=enable.
 * @param   manual                  Menaul Count interrupt. 0=disable, 1=enable.
 * @return  none
 */
void RTC_Interrupt_Enable(uint32_t alarm, uint32_t sec, uint32_t half_sec, uint32_t ten_ms, uint32_t manual_count)
{
    RTC->CTRL |= ((manual_count & 0x1ul) << RTC_CTRL_MANUALCOUNTIE_Pos) |
                    ((ten_ms    & 0x1ul) << RTC_CTRL_TMSIE_Pos) |
                    ((half_sec  & 0x1ul) << RTC_CTRL_HSECIE_Pos) |
                    ((sec       & 0x1ul) << RTC_CTRL_SECIE_Pos) |
                    ((alarm     & 0x1ul) << RTC_CTRL_ALARMIE_Pos);
}


/*@}*/ /* end of group RTC_Driver */
/*@}*/ /* end of group A8107M_Driver */
