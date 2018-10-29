/**************************************************************************//**
 * @file        GPIO.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       GPIO driver source file
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "AMICCOM_CM0.h"

/** @addtogroup Std_Driver Standard Driver
    @{
*/
/** @addtogroup GPIO_Driver GPIO Driver
    @{
*/

/*GPIO driver functions*/


/**
 *
 * @brief   Sets pins on a port as enable "0" / disable "1" pull-up resistor register.
 *
 * @param   *gpio               GPIO Pointer
 * @param   pun                 Bit pattern to be used to enable "0" / disable "1" Pullup Resistor
 * @return  none
 */
void GPIO_SetPUN(GPIO_Type *gpio, uint32_t pun)
{
    gpio->PUN = pun;
}

/**
 *
 * @brief   Sets pins on a port as enable "0" / disable "1" wakeup pin.
 *
 * @param   *gpio               GPIO Pointer
 * @param   wun                 Bit pattern to enable "0" / disable "1" Wakeup
 * @return  none
 */
void GPIO_SetWUN(GPIO_Type *gpio, uint32_t wun)
{
    gpio->WUN = wun;
}

/**
 *
 * @brief   Sets pins on a port as an output. Set the bit corresponding to the pin number to 1 for output
 *          i.e. Set bit 1 of outenable to 1 to set pin 1 as an output. This function is thread safe.
 *
 * @param   *gpio               GPIO Pointer
 * @param   outenable           Bit pattern to be used to set output enable register
 * @return  none
 */
void GPIO_SetOutEnable(GPIO_Type *gpio, uint32_t outenableset)
{
    gpio->OUTENABLESET = outenableset;
}

/**
 *
 * @brief   Sets pins on a port as an input. Set the bit corresponding to the pin number to 1 for input
 *          i.e. Set bit 1 of outenable to 1 to set pin 1 as an input. This function is thread safe.
 *
 * @param   *gpio               GPIO Pointer
 * @param   outenable           Bit pattern to be used to set output enable register
 * @return  none
 */
void GPIO_ClrOutEnable(GPIO_Type *gpio, uint32_t outenableclr)
{
    gpio->OUTENABLECLR = outenableclr;
}

/**
 *
 * @brief   returns a uint32_t which defines the whether pins on a port are set as inputs or outputs
 *          i.e. if bit 1 of the returned uint32_t is set to 1 then this means that pin 1 is an output.
 *
 * @param   *gpio               GPIO Pointer
 * @return  outputstatus
 */
uint32_t GPIO_GetOutEnable(GPIO_Type *gpio)
{
    return gpio->OUTENABLESET;
}

/**
 *
 * @brief   enables the alternative function for pins. Set the bit corresponding to the pin number to 1 for alternate function
 *          i.e. Set bit 1 of ALtFunc to 1 to set pin 1 to its alternative function. This function is thread safe.
 *
 * @param   *gpio               GPIO Pointer
 * @param   AltFuncset          uint32_t to specify whether the alternate function for the pins on the port is enabled
 * @return  none
 */
void GPIO_SetAltFunc(GPIO_Type *gpio, uint32_t AltFuncset)
{
    gpio->ALTFUNCSET = AltFuncset;
}

/**
 *
 * @brief   disables the alternative function for pins. Set the bit corresponding to the pin number to 1 to disable alternate function
 *          i.e. Set bit 1 of ALtFunc to 1 to set pin 1 to the orignal output function. This function is thread safe.
 *
 * @param   *gpio               GPIO Pointer
 * @param   AltFuncclr          uint32_t to specify whether the alternate function for the pins on the port is enabled
 * @return  none
 */
void GPIO_ClrAltFunc(GPIO_Type *gpio, uint32_t AltFuncclr)
{
    gpio->ALTFUNCCLR = AltFuncclr;
}

/**
 *
 * @brief   returns a uint32_t which defines the whether pins on a port are set to their alternative or their original output functionality
 *          i.e. if bit 1 of the returned uint32_t is set to 1 then this means that pin 1 is set to its alternative function.
 *
 * @param   *gpio               GPIO Pointer
 * @return  AltFuncStatus
 */
uint32_t GPIO_GetAltFunc(GPIO_Type *gpio)
{
    return gpio->ALTFUNCSET;
}

/**
 *
 * @brief   Clears the interrupt flag for the specified pin and then returns the new interrupt status of the pin. This function is thread safe.
 *
 * @param   *gpio               GPIO Pointer
 * @param   Num                 The pin number for which to clear the Interrupt
 * @return  NewIntStatus
 */
uint32_t GPIO_IntClear(GPIO_Type *gpio, uint32_t Num)
{
    gpio->INTCLEAR = (1 << Num);
    return gpio->INTSTATUS;
}

/**
 *
 * @brief   Enables interrupts for the specified pin and then returns the new interrupt enable status of the pin. This function is thread safe.
 *
 * @param   *gpio               GPIO Pointer
 * @param   Num                 The pin number for which to enable the Interrupt
 * @return  NewIntEnStatus
 */
uint32_t GPIO_SetIntEnable(GPIO_Type *gpio, uint32_t Num)
{
    gpio->INTENSET = (1 << Num);
    return gpio->INTENSET;
}

/**
 *
 * @brief   Disables interrupts for the specified pin and then returns the new interrupt enable status of the pin. This function is thread safe.
 *
 * @param   *gpio               GPIO Pointer
 * @param   Num                 The pin number for which to disable the Interrupt
 * @return  NewIntEnStatus
 */
uint32_t GPIO_ClrIntEnable(GPIO_Type *gpio, uint32_t Num)
{
    gpio->INTENCLR = (1 << Num);
    return gpio->INTENCLR;
}

/**
 *
 * @brief   Changes the interrupt type for the specified pin to a high level interrupt. This function is thread safe.
 *
 * @param   *gpio               GPIO Pointer
 * @param   Num                 The pin number for which to set the Interrupt type
 * @return  none
 */
void GPIO_SetIntHighLevel(GPIO_Type *gpio, uint32_t Num)
{
    gpio->INTTYPECLR = (1 << Num); /* Clear INT TYPE bit */
    gpio->INTPOLSET = (1 << Num);  /* Set INT POLarity bit */
}

/**
 *
 * @brief   Changes the interrupt type for the specified pin to a rising edge interrupt. This function is thread safe.
 *
 * @param   *gpio               GPIO Pointer
 * @param   Num                 The pin number for which to set the Interrupt type
 * @return  none
 */
void GPIO_SetIntRisingEdge(GPIO_Type *gpio, uint32_t Num)
{
    gpio->INTTYPESET = (1 << Num); /* Set INT TYPE bit */
    gpio->INTPOLSET = (1 << Num);  /* Set INT POLarity bit */
}

/**
 *
 * @brief   Changes the interrupt type for the specified pin to a low level interrupt. This function is thread safe.
 *
 * @param   *gpio               GPIO Pointer
 * @param   Num                 The pin number for which to set the Interrupt type
 * @return  none
 */
void GPIO_SetIntLowLevel(GPIO_Type *gpio, uint32_t Num)
{
    gpio->INTTYPECLR = (1 << Num);  /* Clear INT TYPE bit */
    gpio->INTPOLCLR = (1 << Num);   /* Clear INT POLarity bit */
}

/**
 *
 * @brief   Changes the interrupt type for the specified pin to a falling edge interrupt. This function is thread safe.
 *
 * @param   *gpio               GPIO Pointer
 * @param   Num                 The pin number for which to set the Interrupt type
 * @return  none
 */
void GPIO_SetIntFallingEdge(GPIO_Type *gpio, uint32_t Num)
{
    gpio->INTTYPESET = (1 << Num);  /* Set INT TYPE bit */
    gpio->INTPOLCLR = (1 << Num);   /* Clear INT POLarity bit */
}

/**
 *
 * @brief Outputs the specified value on the desired port using the user defined mask to perform Masked access.
 *
 * @param   *gpio               GPIO Pointer
 * @param   mask                The output port mask
 * @param   value               The value to output to the specified port
 * @return  none
 */
void GPIO_MaskedWrite(GPIO_Type *gpio, uint32_t value, uint32_t mask)
{
    gpio->LB_MASKED[0x00FF & mask] = value;
    gpio->UB_MASKED[((0xFF00 & mask) >> 8)] = value;
}


/*@}*/ /* end of group GPIO_Driver */
/*@}*/ /* end of group Std_Driver */
