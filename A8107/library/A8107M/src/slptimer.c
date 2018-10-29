/**************************************************************************//**
 * @file        slptimer.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       SLEEP TIMER Driver source file
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "A8107M.h"

/** @addtogroup A8107M_Driver Standard Driver
    @{
*/
/** @addtogroup SLPTIMER_Driver DualTimer Driver
    @{
*/

/**
 *
 * @brief   Initial SLPTIMER and Start counting
 *
 * @param   *slptimer               SLPTIMER Pointer
 * @param   interval                SLPTIMER interval. interval must large then 4.
 * @param   cks                     For Timer1: 0=/1, 1=/5,  2=/10, 3=/20
                                    For Timer0: 0=/5, 1=/10, 2=/20, 3=/40
 * @param   ie                      0=interrupt disable, 1=interrupt enable
 * @return  none
 */
void SLPTIMER_Initial(SLPTIMER_Type *slptimer, uint32_t interval, uint32_t cks, uint32_t ie)
{
    slptimer->CONTROL = SLPTIMER_CONTROL_RST_Msk;
    slptimer->CONTROL = SLPTIMER_CONTROL_ON_Msk |
                        ((cks & 0x3ul) << SLPTIMER_CONTROL_CKS_Pos) |
                        ((ie  & 0x1ul) << SLPTIMER_CONTROL_IE_Pos);
    slptimer->INTERVAL = interval;
    slptimer->CONTROL |= SLPTIMER_CONTROL_CE_Msk;
}

/**
 *
 * @brief   Enable the slptimer interrupt.
 *
 * @param   *slptimer               SLPTIMER Pointer
 * @return  none
 */
void SLPTIMER_EnableIRQ(SLPTIMER_Type *slptimer)
{
    slptimer->CONTROL |= SLPTIMER_CONTROL_IE_Msk;
}

/**
 *
 * @brief   Disable the slptimer interrupt.
 *
 * @param   *slptimer               SLPTIMER Pointer
 * @return  none
 */
void SLPTIMER_DisableIRQ(SLPTIMER_Type *slptimer)
{
    slptimer->CONTROL &= ~SLPTIMER_CONTROL_IE_Msk;
}

/**
 *
 * @brief   Clears the slptimer IRQ if set.
 *
 * @param   *slptimer               SLPTIMER Pointer
 * @return  none
 */
void SLPTIMER_ClearIRQ(SLPTIMER_Type *slptimer)
{
    slptimer->CONTROL |= SLPTIMER_CONTROL_IF_Msk;
}

/**
 *
 * @brief   Start the SLPTIMER.
 *
 * @param   *slptimer               SLPTIMER Pointer
 * @return  none
 */
void SLPTIMER_StartTimer(SLPTIMER_Type *slptimer)
{
    slptimer->CONTROL |= SLPTIMER_CONTROL_CE_Msk;
}

/**
 *
 * @brief   Stop the SLPTIMER.
 *
 * @param   *slptimer               SLPTIMER Pointer
 * @return  none
 */
void SLPTIMER_StopTimer(SLPTIMER_Type *slptimer)
{
    slptimer->CONTROL &= ~SLPTIMER_CONTROL_CE_Msk;
}


/*@}*/ /* end of group SLPTIMER_Driver */
/*@}*/ /* end of group A8107M_Driver */
