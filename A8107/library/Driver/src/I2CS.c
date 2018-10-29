/**************************************************************************//**
 * @file        I2CS.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       I2C Slave Driver source file
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "AMICCOM_CM0.h"

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup I2CS_Driver Watchdog Driver
    @{
*/

/**
 *
 * @brief   Initial I2C Slave controller with 7bits slave address
 *
 * @param   *i2cs                   I2CS Pointer
 * @param   addr                    I2CS Slave address (7bits)
 *
 * @return  none
 */
void I2CS_Initial_7Bits(I2CS_Type *i2cs, uint32_t addr)
{
    i2cs->CTRL = I2CS_CTRL_RSTB_Msk;    // reset I2CS
    i2cs->ADDR = addr & I2CS_ADDR_Msk;
    i2cs->CTRL = I2CS_CTRL_DA_Msk;      // Device Active
}

/**
 *
 * @brief   Initial I2C Slave controller with 10bits slave address
 *
 * @param   *i2cs                   I2CS Pointer
 * @param   addr                    I2CS Slave address (10bits)
 *
 * @return  none
 */
void I2CS_Initial_10Bits(I2CS_Type *i2cs, uint32_t addr)
{
    i2cs->CTRL = I2CS_CTRL_RSTB_Msk;    // reset I2CS
    i2cs->ADDR = addr & I2CS_ADDR_Msk;
    i2cs->ADDR10BITS = I2CS_ADDR10BITS_EN_Msk | (((addr >> 7) & I2CS_ADDR10BITS_A9A8A7_Msk) << I2CS_ADDR10BITS_A9A8A7_Pos);
    i2cs->CTRL = I2CS_CTRL_DA_Msk;      // Device Active
}

/**
 *
 * @brief   Enable the I2C Slave interrupt.
 *
 * @param   *i2cs              I2CS Pointer
 * @return  none
 */
void I2CS_EnableIRQ(I2CS_Type *i2cs)
{
    i2cs->INT = I2CS_INT_FLAG_Msk | I2CS_INT_ENABLE_Msk;
}

/**
 *
 * @brief   Disable the I2C Slave interrutp.
 *
 * @param   *i2cs              I2CS Pointer
 * @return  none
 */
void I2CS_DisableIRQ(I2CS_Type *i2cs)
{
    i2cs->INT &= ~I2CS_INT_ENABLE_Msk;
}


/*@}*/ /* end of group I2CS_Driver */
/*@}*/ /* end of group Std_Driver */
