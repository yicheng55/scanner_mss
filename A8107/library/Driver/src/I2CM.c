/**************************************************************************//**
 * @file        I2CM.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       I2C Master Driver source file
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "AMICCOM_CM0.h"

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup I2CM_Driver Watchdog Driver
    @{
*/

/**
 *
 * @brief   Initial I2C Master controller and set SCL clock frequency.
 *
 * @param   *i2cm                   I2CM Pointer
 * @param   tp                      I2CM TP = 1~127
 *                                  SCL clock fequency = f(system)/2/10/(1+TP)
 * @return  none
 */
void I2CM_Initial(I2CM_Type *i2cm, uint32_t tp)
{
    if(0 == tp) tp = 1;
    i2cm->CTRL = I2CM_CTRL_RSTB_Msk;    // reset I2C master controller
    i2cm->TP = tp & I2CM_TP_Msk;
    i2cm->SCL_LP = 6;
    i2cm->SCL_HP = 4;
    i2cm->DATA_SETUP_TIME = 2;
}

/**
 *
 * @brief   Enable the I2C Master interrupt.
 *
 * @param   *i2cm              I2CM Pointer
 * @return  none
 */
void I2CM_EnableIRQ(I2CM_Type *i2cm)
{
    i2cm->INT = I2CM_INT_FLAG_Msk | I2CM_INT_ENABLE_Msk;
}

/**
 *
 * @brief   Disable the I2C Master interrutp.
 *
 * @param   *i2cm              I2CM Pointer
 * @return  none
 */
void I2CM_DisableIRQ(I2CM_Type *i2cm)
{
    i2cm->INT &= ~I2CM_INT_ENABLE_Msk;
}

/**
 *
 * @brief   Setup 7bits slave address and RnW bits
 *
 * @param   *i2cm              I2CM Pointer
 * @param   slave_addr         7bits Slave address
 * @param   rtx                0=RX, 1=TX
 * @return  none
 */
void I2CM_Setup(I2CM_Type *i2cm, uint8_t slave_addr, uint8_t rtx)
{
    uint8_t id = (slave_addr & 0x7Ful) << I2CM_SLAVE_ADDR_ADDR_Pos;
    i2cm->SLAVE_ADDR = id | ((0 == rtx) ? (I2CM_SLAVE_ADDR_RS_Msk) : (0));
}

/**
 *
 * @brief   I2C Master Transefer.
 *
 * @param   *i2cm              I2CM Pointer
 * @param   data               The data will be transfer if possible.
 * @param   mcr                control option: START, RUN, STOP
 * @return  0=no error, others=error occure
 */
uint32_t I2CM_Send(I2CM_Type *i2cm, uint8_t data, uint8_t mcr)
{
    uint32_t status;

    i2cm->BUF = data;
    i2cm->CTRL = mcr;

    while(0 == (i2cm->STATUS & I2CM_STATUS_BUSY_Msk)) ;
    while(0 != (i2cm->STATUS & I2CM_STATUS_BUSY_Msk)) ;

    status = i2cm->STATUS & (I2CM_STATUS_ARB_LOST_Msk | I2CM_STATUS_ERROR_Msk);
    if(status)
    {
        if(0 == (status & I2CM_STATUS_ARB_LOST_Msk))
            i2cm->CTRL = I2CM_CTRL_STOP_Msk;
    }

    return status;
}

/**
 *
 * @brief   I2C Master Receive.
 *
 * @param   *i2cm              I2CM Pointer
 * @param   *data              Data point. The data will be received if possible.
 * @param   mcr                control option: START, RUN, ACK, STOP
 * @return  0=no error, others=error occure
 */
uint32_t I2CM_Read(I2CM_Type *i2cm, uint8_t *data, uint8_t mcr)
{
    uint32_t status;

    i2cm->CTRL = mcr;

    while(0 == (i2cm->STATUS & I2CM_STATUS_BUSY_Msk)) ;
    while(0 != (i2cm->STATUS & I2CM_STATUS_BUSY_Msk)) ;

    status = i2cm->STATUS & (I2CM_STATUS_ARB_LOST_Msk | I2CM_STATUS_ERROR_Msk);

    if(status)
    {
        if(0 == (status & I2CM_STATUS_ARB_LOST_Msk))
            i2cm->CTRL = I2CM_CTRL_STOP_Msk;
    }
    else
    {
        *data = i2cm->BUF;
    }

    return status;
}


/*@}*/ /* end of group I2CM_Driver */
/*@}*/ /* end of group Std_Driver */
