/**************************************************************************//**
 * @file        system_A8107M.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       The header file for system_A8107M.h
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/
 
#ifndef _SYSTEM_A8107M_H_
#define _SYSTEM_A8107M_H_

#include <stdint.h>
extern uint32_t SystemFrequency;        /*!< System Clock Frequency (Core Clock)  */
extern uint32_t SystemCoreClock;        /*!< Processor Clock Frequency            */

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);

/**
 * Update SystemCoreClock variable
 *
 * @param  none
 * @return none
 *
 * @brief  Updates the SystemCoreClock with current core Clock
 *         retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);

#endif /* _SYSTEM_A8107M_H_ */
