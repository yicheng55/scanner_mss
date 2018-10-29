/**************************************************************************//**
 * @file        i2cm.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       I2CM Driver Header File
 *
 * @note
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/

#ifndef __I2CM_H__
#define __I2CM_H__

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup I2CM_Driver Timer Driver
    @{
*/

/**
 * @brief   Initial I2C Master controller and set SCL clock frequency.
 */
extern void I2CM_Initial(I2CM_Type *i2cm, uint32_t tp);

/**
 * @brief   Enable the I2C Master interrupt.
 */
extern void I2CM_EnableIRQ(I2CM_Type *i2cm);

/**
 * @brief   Disable the I2C Master interrutp.
 */
extern void I2CM_DisableIRQ(I2CM_Type *i2cm);

/**
 * @brief   Setup 7bits slave address and RnW bits
 */
extern void I2CM_Setup(I2CM_Type *i2cm, uint8_t slave_addr, uint8_t rtx);

/**
 * @brief   I2C Master Transefer.
 */
extern uint32_t I2CM_Send(I2CM_Type *i2cm, uint8_t data, uint8_t mcr);

/**
 * @brief   I2C Master Receive.
 */
extern uint32_t I2CM_Read(I2CM_Type *i2cm, uint8_t *data, uint8_t mcr);


/*@}*/ /* end of group I2CM_Driver */
/*@}*/ /* end of group Std_Driver */

#endif
