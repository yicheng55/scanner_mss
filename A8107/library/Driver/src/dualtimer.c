/**************************************************************************//**
 * @file        dualtimer.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10$
 * @brief       DUALTIMER Driver source file
 *
 * @note        V1.00.01: Modify DualTIMER_Type to DUALTIMER_Type
 *              V1.00.02: Add note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "AMICCOM_CM0.h"

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup DUALTIMER_Driver DualTimer Driver
    @{
*/

/**
 *
 * @brief   Initial Dualtimer set to free-run mode. Value will set to 0xFFFFFFFF when value count down to 0.
 *
 * @param   *dualtimer              DualTimer Pointer
 * @param   prescal                 0=/1, 1=/16, 2=/256
 * @param   size                    0=16bits, 1=32bits
 * @return  none
 */
void DualTimer_Freerun_Initial(DUALTIMER_Type *dualtimer, uint32_t prescal, uint32_t size)
{
    uint32_t control = ((prescal&0x3ul)<<DUALTIMER_CTRL_PRESCALE_Pos) |
                       ((size&0x1ul)<<DUALTIMER_CTRL_SIZE_Pos);
    dualtimer->CTRL = control;
}

/**
 *
 * @brief   Initial Dualtimer set to one-shot mode. DualTimer will stop when value count down to 0.
 *
 * @param   *dualtimer              DualTimer Pointer
 * @param   prescal                 0=/1, 1=/16, 2=/256
 * @param   size                    0=16bits, 1=32bits
 * @param   load                    the value set to 0=16bits, 1=32bits
 * @return  none
 */
void DualTimer_OneShot_Initial(DUALTIMER_Type *dualtimer, uint32_t prescal, uint32_t size, uint32_t load)
{
    uint32_t control =  DUALTIMER_CTRL_ONESHOOT_Msk |
                        ((prescal&0x3ul)<<DUALTIMER_CTRL_PRESCALE_Pos) |
                        ((size&0x1ul)<<DUALTIMER_CTRL_SIZE_Pos);
    dualtimer->CTRL = control;
    dualtimer->LOAD = load;
}

/**
 *
 * @brief   Initial Dualtimer set to period mode. When value count down to 0, the value will set to load and continue count.
 *
 * @param   *dualtimer              DualTimer Pointer
 * @param   prescal                 0=/1, 1=/16, 2=/256
 * @param   size                    0=16bits, 1=32bits
 * @return  none
 */
void DualTimer_Period_Initial(DUALTIMER_Type *dualtimer, uint32_t prescal, uint32_t size, uint32_t load)
{
    uint32_t control =  DUALTIMER_CTRL_MODE_Msk |
                        ((prescal&0x3ul)<<DUALTIMER_CTRL_PRESCALE_Pos) |
                        ((size&0x1ul)<<DUALTIMER_CTRL_SIZE_Pos);
    dualtimer->CTRL = control;
    dualtimer->LOAD = load;
}

/**
 *
 * @brief   Enable the microcontroller dualtimer interrupts.
 *
 * @param   *dualtimer              DualTimer Pointer
 * @return  none
 */
void DualTimer_EnableIRQ(DUALTIMER_Type *dualtimer)
{
    dualtimer->CTRL |= DUALTIMER_CTRL_INTEN_Msk;
}

/**
 *
 * @brief   Disable the microcontroller dualtimer interrutps.
 *
 * @param   *dualtimer              DualTimer Pointer
 * @return  none
 */
void DualTimer_DisableIRQ(DUALTIMER_Type *dualtimer)
{
    dualtimer->CTRL &= ~DUALTIMER_CTRL_INTEN_Msk;
}

/**
 *
 * @brief   Clears the dualtimer IRQ if set.
 *
 * @param   *dualtimer              DualTimer Pointer
 * @return  none
 */
void DualTimer_ClearIRQ(DUALTIMER_Type *dualtimer)
{
    dualtimer->INTCLR = 1;
}


/**
 *
 * @brief   Start the DualTimer.
 *
 * @param   *dualtimer              DualTimer Pointer
 * @return  none
 */
void DualTimer_StartTimer(DUALTIMER_Type *dualtimer)
{
    dualtimer->CTRL |= DUALTIMER_CTRL_EN_Msk;
}

/**
 *
 * @brief   Stop the DualTimer.
 *
 * @param   *dualtimer              DualTimer Pointer
 * @return  none
 */
void DualTimer_StopTimer(DUALTIMER_Type *dualtimer)
{
    dualtimer->CTRL &= ~DUALTIMER_CTRL_EN_Msk;
}

/**
 *
 * @brief   Returns the current value of the dualtimer.
 *
 * @param   *dualtimer              DualTimer Pointer
 * @return  dualtimer current value
 */
uint32_t DualTimer_GetValue(DUALTIMER_Type *dualtimer)
{
    return dualtimer->VALUE;
}

/**
 *
 * @brief   Sets the load value of the dualtimer to the specified value. This value also set to current value immediately.
 *
 * @param   *dualtimer              DualTimer Pointer
 * @param   value                   Value to be loaded
 * @return  none
 */
void DualTimer_SetLoad(DUALTIMER_Type *dualtimer, uint32_t value)
{
    dualtimer->LOAD = value;
}

/**
 *
 * @brief   Returns the load value of the dualtimer.
 *
 * @param   *dualtimer              DualTimer Pointer
 * @return  load value
 */
uint32_t DualTimer_GetLoad(DUALTIMER_Type *dualtimer)
{
    return dualtimer->LOAD;
}

/**
 *
 * @brief   Sets the BGLoad value of the timer to the specified value. This value also set to Load immediately but not set to current value.
 *
 * @param   *dualtimer              DualTimer Pointer
 * @param   value                   Value to be loaded
 * @return  none
 */
void DualTimer_SetBGLoad(DUALTIMER_Type *dualtimer, uint32_t value)
{
    dualtimer->BGLOAD = value;
}

/**
 *
 * @brief   Returns the BGLoad value of the dualtimer.
 *
 * @param   *dualtimer              DualTimer Pointer
 * @return  load value
 */
uint32_t DualTimer_GetBGLoad(DUALTIMER_Type *dualtimer)
{
    return dualtimer->BGLOAD;
}

/*@}*/ /* end of group DUALTIMER_Driver */
/*@}*/ /* end of group Std_Driver */

