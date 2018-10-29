/**************************************************************************//**
 * @file        gpio.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       GPIO Driver Header File
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/

#ifndef __GPIO_H__
#define __GPIO_H__

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup GPIO_Driver GPIO Driver
    @{
*/

/**
* @brief Set CMSDK GPIO Output Enable.
*/

/*GPIO Driver Declarations*/

/**
* @brief Set GPIO PUN register.
*/
extern void GPIO_SetPUN(GPIO_Type *gpio, uint32_t pun);

/**
* @brief Set GPIO WUN register.
*/
extern void GPIO_SetWUN(GPIO_Type *gpio, uint32_t wun);

/**
* @brief Set GPIO Output Enable.
*/
extern void GPIO_SetOutEnable(GPIO_Type *gpio, uint32_t outenableset);

/**
* @brief Clear GPIO Output Enable.
*/
extern void GPIO_ClrOutEnable(GPIO_Type *gpio, uint32_t outenableclr);

/**
* @brief Returns GPIO Output Enable.
*/
extern uint32_t GPIO_GetOutEnable(GPIO_Type *gpio);

/**
* @brief Set GPIO Alternate function Enable.
*/
extern void GPIO_SetAltFunc(GPIO_Type *gpio, uint32_t AltFuncset);

/**
* @brief Clear GPIO Alternate function Enable.
*/
extern void GPIO_ClrAltFunc(GPIO_Type *gpio, uint32_t AltFuncclr);

/**
* @brief Returns GPIO Alternate function Enable.
*/
extern uint32_t GPIO_GetAltFunc(GPIO_Type *gpio);

/**
* @brief Clear GPIO Interrupt request.
*/
extern uint32_t GPIO_IntClear(GPIO_Type *gpio, uint32_t Num);

/**
* @brief Enable GPIO Interrupt request.
*/
extern uint32_t GPIO_SetIntEnable(GPIO_Type *gpio, uint32_t Num);

/**
* @brief Disable GPIO Interrupt request.
*/
extern uint32_t GPIO_ClrIntEnable(GPIO_Type *gpio, uint32_t Num);

/**
* @brief Setup GPIO Interrupt as high level.
*/
extern void GPIO_SetIntHighLevel(GPIO_Type *gpio, uint32_t Num);

/**
* @brief Setup GPIO Interrupt as rising edge.
*/
extern void GPIO_SetIntRisingEdge(GPIO_Type *gpio, uint32_t Num);

 /**
* @brief Setup GPIO Interrupt as low level.
*/
extern void GPIO_SetIntLowLevel(GPIO_Type *gpio, uint32_t Num);

/**
* @brief Setup GPIO Interrupt as falling edge.
*/
extern void GPIO_SetIntFallingEdge(GPIO_Type *gpio, uint32_t Num);

/**
* @brief Setup GPIO output value using Masked access.
*/
extern void GPIO_MaskedWrite(GPIO_Type *gpio, uint32_t value, uint32_t mask);

/*@}*/ /* end of group GPIO_Driver */
/*@}*/ /* end of group Std_Driver */

#endif
