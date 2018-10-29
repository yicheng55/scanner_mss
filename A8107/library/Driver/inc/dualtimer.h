/**************************************************************************//**
 * @file        dualtimer.h
 * @version     V1.00.02
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       DUALTIMER Driver Header File
 *
 * @note        V1.00.02: Add Note.
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/

#ifndef __DUALTIMER_H__
#define __DUALTIMER_H__

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup DUALTIMER_Driver Timer Driver
    @{
*/

/**
 * @brief   Initial Dualtimer set to free-run mode. Value will set to 0xFFFFFFFF when value count down to 0.
 */
extern void DualTimer_Freerun_Initial(DUALTIMER_Type *dualtimer, uint32_t prescal, uint32_t size);

/**
 * @brief   Initial Dualtimer set to one-shot mode. DualTimer will stop when value count down to 0.
 */
extern void DualTimer_OneShot_Initial(DUALTIMER_Type *dualtimer, uint32_t prescal, uint32_t size, uint32_t load);

/**
 * @brief   Initial Dualtimer set to period mode. When value count down to 0, the value will set to load and continue count.
 */
extern void DualTimer_Period_Initial(DUALTIMER_Type *dualtimer, uint32_t prescal, uint32_t size, uint32_t load);

/**
 * @brief   Enable the microcontroller dualtimer interrupts.
 */
extern void DualTimer_EnableIRQ(DUALTIMER_Type *dualtimer);

/**
 * @brief   Disable the microcontroller dualtimer interrutps.
 */
extern void DualTimer_DisableIRQ(DUALTIMER_Type *dualtimer);

/**
 * @brief   Clears the dualtimer IRQ if set.
 */
extern void DualTimer_ClearIRQ(DUALTIMER_Type *dualtimer);

/**
 * @brief   Start the DualTimer.
 */
extern void DualTimer_StartTimer(DUALTIMER_Type *dualtimer);

/**
 * @brief   Stop the DualTimer.
 */
extern void DualTimer_StopTimer(DUALTIMER_Type *dualtimer);

/**
 * @brief   Returns the current value of the dualtimer.
 */
extern uint32_t DualTimer_GetValue(DUALTIMER_Type *dualtimer);

/**
 * @brief   Sets the load value of the dualtimer to the specified value. This value also set to current value immediately.
 */
extern void DualTimer_SetLoad(DUALTIMER_Type *dualtimer, uint32_t value);

/**
 * @brief   Returns the load value of the dualtimer.
 */
extern uint32_t DualTimer_GetLoad(DUALTIMER_Type *dualtimer);

/**
 * @brief   Sets the BGLoad value of the timer to the specified value. This value also set to Load immediately but not set to current value.
 */
extern void DualTimer_SetBGLoad(DUALTIMER_Type *dualtimer, uint32_t value);

/**
 * @brief   Returns the BGLoad value of the dualtimer.
 */
extern uint32_t DualTimer_GetBGLoad(DUALTIMER_Type *dualtimer);


/*@}*/ /* end of group DUALTIMER_Driver */
/*@}*/ /* end of group Std_Driver */

#endif
