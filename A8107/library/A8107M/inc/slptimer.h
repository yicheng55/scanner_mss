/**************************************************************************//**
 * @file        slptimer.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       SLPTIMER Driver Header File
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/

#ifndef __SLPTIMER_H__
#define __SLPTIMER_H__

/** @addtogroup A8107M_Driver Standard Driver
    @{
*/
/** @addtogroup SLPTIMER_Driver UART Driver
    @{
*/

/**
 * @brief   Initial SLPTIMER and Start counting
 */
extern void SLPTIMER_Initial(SLPTIMER_Type *slptimer, uint32_t interval, uint32_t cks, uint32_t ie);

/**
 * @brief   Enable the slptimer interrupt.
 */
extern void SLPTIMER_EnableIRQ(SLPTIMER_Type *slptimer);

/**
 * @brief   Disable the slptimer interrupt.
 */
extern void SLPTIMER_DisableIRQ(SLPTIMER_Type *slptimer);

/**
 * @brief   Clears the slptimer IRQ if set.
 */
extern void SLPTIMER_ClearIRQ(SLPTIMER_Type *slptimer);

/**
 * @brief   Start the SLPTIMER.
 */
extern void SLPTIMER_StartTimer(SLPTIMER_Type *slptimer);

/**
 * @brief   Stop the SLPTIMER.
 */
extern void SLPTIMER_StopTimer(SLPTIMER_Type *slptimer);

/*@}*/ /* end of group SLPTIMER_Driver */
/*@}*/ /* end of group A8107M_Driver */

#endif
