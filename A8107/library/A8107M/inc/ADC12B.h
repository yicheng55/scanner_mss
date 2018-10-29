/**************************************************************************//**
 * @file        ADC12B.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       12Bits SAR ADC Driver Header File
 *
 * @note
 *
 * Copyright (C) 2016 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/
 
#ifndef __ADC12B_H__
#define __ADC12B_H__


/** @addtogroup A8107M_Driver Standard Driver
    @{
*/
/** @addtogroup ADC12B_Driver UART Driver
    @{
*/

/**
 * @brief   Initial 12Bits SAR ADC and Start running with adcch
 */
extern uint32_t ADC12B_Measure(ADC12B_Type *adc12b, uint32_t ch);

/*@}*/ /* end of group ADC12B_Driver */
/*@}*/ /* end of group A8107M_Driver */

#endif
