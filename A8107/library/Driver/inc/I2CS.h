/**************************************************************************//**
 * @file        i2cs.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       I2CS Driver Header File
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/

#ifndef __I2CS_H__
#define __I2CS_H__

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup I2CS_Driver Timer Driver
    @{
*/

/**
 * @brief   Initial I2C Slave controller with 7bits slave address
 */
extern void I2CS_Initial_7Bits(I2CS_Type *i2cs, uint32_t addr);

/**
 * @brief   Initial I2C Slave controller with 10bits slave address
 */
extern void I2CS_Initial_10Bits(I2CS_Type *i2cs, uint32_t addr);

/**
 * @brief   Enable the I2C Slave interrupt.
 */
extern void I2CS_EnableIRQ(I2CS_Type *i2cs);

/**
 * @brief   Disable the I2C Slave interrutp.
 */
extern void I2CS_DisableIRQ(I2CS_Type *i2cs);


/*@}*/ /* end of group I2CS_Driver */
/*@}*/ /* end of group Std_Driver */

#endif
