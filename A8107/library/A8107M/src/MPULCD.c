/**************************************************************************//**
 * @file        MPULCD.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       MPU LCD Driver source file
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "A8107M.h"

/** @addtogroup A8107M_Driver A8107M Driver
    @{
*/
/** @addtogroup MPULCD_Driver Real Time Clock Driver
    @{
*/

/**
 *
 * @brief   Write data through MPULCD interface.
 *
 * @param   *mpulcd         MPULCD_Type Pointer
 * @param   a0              Set A0 signal. 0: A0=0, 1: A0=1
 * @param   data            The data will send through MPULCD interface
 * @return  none
 */
void MPULCD_Write(MPULCD_Type *mpulcd, uint16_t data, uint8_t a0)
{
    mpulcd->CONFIG |= MPULCD_CONFIG_CMD_Msk;
    if(0 == a0)
        mpulcd->CONFIG &= ~MPULCD_CONFIG_A0_Msk;
    else
        mpulcd->CONFIG |= MPULCD_CONFIG_A0_Msk;
    mpulcd->CONFIG &= ~MPULCD_CONFIG_RNW_Msk;
    mpulcd->CMDDAT = data;
    mpulcd->CONFIG |= MPULCD_CONFIG_ENABLE_Msk;
    while( 0 != (mpulcd->CONFIG & MPULCD_CONFIG_BUSY_Msk) )  ;
    mpulcd->CONFIG &= ~MPULCD_CONFIG_ENABLE_Msk;
}

/**
 *
 * @brief   Read data through MPULCD interface.
 *
 * @param   *mpulcd         MPULCD_Type Pointer
 * @param   a0              Set A0 signal. 0: A0=0, 1: A0=1
 * @return  The data read through MPULCD interface.
 */
uint16_t MPULCD_Read(MPULCD_Type *mpulcd, uint8_t a0)
{
    uint16_t read;

    mpulcd->CONFIG |= MPULCD_CONFIG_CMD_Msk;
    if(0 == a0)
        mpulcd->CONFIG &= ~MPULCD_CONFIG_A0_Msk;
    else
        mpulcd->CONFIG |= MPULCD_CONFIG_A0_Msk;
    mpulcd->CONFIG |= MPULCD_CONFIG_RNW_Msk;
    mpulcd->CONFIG |= MPULCD_CONFIG_ENABLE_Msk;
    while( 0 != (mpulcd->CONFIG & MPULCD_CONFIG_BUSY_Msk) )  ;
    mpulcd->CONFIG &= ~MPULCD_CONFIG_ENABLE_Msk;

    read = MPULCD->CMDDAT;
    mpulcd->CONFIG &= ~MPULCD_CONFIG_RNW_Msk;
    return read;
}

/*@}*/ /* end of group MPULCD_Driver */
/*@}*/ /* end of group A8107M_Driver */
