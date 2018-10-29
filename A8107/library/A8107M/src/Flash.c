/**************************************************************************//**
 * @file        Flash.c
 * @version     V1.00.02
 * $Revision:   0 $
 * $Date:       2017-07-05 $
 * @brief       FLASH Driver source file
 *
 * @note
 *              V1.00.02 - add function:
 *                  Ble_Flash_Write_U32, Ble_Flash_Write_U8
 *                  Ble_Flash_PageErase, Ble_Flash_SectorErase
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "A8107M.h"

/** @addtogroup A8107M_Driver A8107M Driver
    @{
*/
/** @addtogroup FLASH_Driver FLASH Driver
    @{
*/

/**
 *
 * @brief   battery detect REGI >= 2.1V. It's maybe had 0.1V offset.
 *
 * @param   none
 * @return  1: REGI>=2.1V 0:REGI<2.1V
 */

uint32_t battery_detect(void)
{
    uint8_t n = 10;
    POWER->BatteryItem.BVT = 3;         // 2.1V
    POWER->BatteryItem.BDS = 1;
    while(POWER->BatteryItem.BDS)   ;
    while(n --);                        // Need a short time delay.
    return POWER->BatteryItem.BDF;
}

/**
 *
 * @brief   run flash action
 *
 * @param   address     The begin address for flash action. Bit[1:0] need keep 0.
 * @param   action      The spec. action (i.e. FLASH_CTRL_WRITE_Msk, FLASH_CTRL_PROG_Msk,
 *                                             FLASH_CTRL_PERASE_Msk, FLASH_CTRL_SERASE_Msk)
 * @param   length      The length for write or program by WORD 32Bits
* @return   0: PASS, others: FAIL.
 */
uint32_t flash_action(uint32_t address, uint32_t action, uint32_t length)
{
    uint32_t unlock;
    uint8_t  n;

    n = 5;
    while(n)
    {
        if(1 != battery_detect()) return 0xFFFFFFFF;
        n --;
    }

    n = address/(128*1024);
    unlock = (1 << ((address & 0x1FFFF) / (4 * 1024))) ^ 0xFFFFFFFF;
    FLASH->LOCK[n] = unlock;
    FLASH->ADDR = address;
    FLASH->PWD0 = FLASH->KEY0;
    FLASH->PWD1 = FLASH->KEY1;
    FLASH->LENGTH  = FLASH_LENGTH_PWE_Msk|length;
    FLASH->CTRL = (address >= 128*1024) ? (FLASH_CTRL_CE_Msk | FLASH_CTRL_F2M_Msk | action) : (FLASH_CTRL_CE_Msk | FLASH_CTRL_F1M_Msk | action);
    (*((volatile uint32_t *)(address)));    // It's must need to ready flash action, DON'T REMOVE this.
    FLASH->CTRL = FLASH_CTRL_CE_Msk;        // Flash action begin.
    FLASH->LENGTH  = 0;                     // Flash action end. PWE=0.

    return 0;
}

/**
 *
 * @brief   run flash action
 *
 * @param   address     The begin address for flash action. Bit[1:0] need keep 0.
 * @param   action      The spec. action (i.e. FLASH_CTRL_WRITE_Msk, FLASH_CTRL_PROG_Msk,
 *                                             FLASH_CTRL_PERASE_Msk, FLASH_CTRL_SERASE_Msk)
 * @param   length      The length for write or program by WORD 32Bits
* @return   0: PASS, others: FAIL.
 */
uint32_t flash_action_ble(uint32_t address, uint32_t action, uint32_t length)
{
    uint32_t unlock;
    uint8_t  n;

    n = 5;
    while(n)
    {
        if(1 != battery_detect()) return 0xFFFFFFFF;
        n --;
    }

    n = address/(128*1024);
    unlock = (1 << ((address & 0x1FFFF) / (4 * 1024))) ^ 0xFFFFFFFF;
    FLASH->LOCK[n] = unlock;
    FLASH->LOCK[0] |= 0xC0000000;       // always lock 0x0001E000~0x0001FFFF
    FLASH->LOCK[1] |= 0xFFFFFFFF;       // always lock 0x0002000~0x0003FFFF
    FLASH->ADDR = address;
    FLASH->PWD0 = FLASH->KEY0;
    FLASH->PWD1 = FLASH->KEY1;
    FLASH->LENGTH  = FLASH_LENGTH_PWE_Msk|length;
    FLASH->CTRL = (address >= 128*1024) ? (FLASH_CTRL_CE_Msk | FLASH_CTRL_F2M_Msk | action) : (FLASH_CTRL_CE_Msk | FLASH_CTRL_F1M_Msk | action);
    (*((volatile uint32_t *)(address)));    // It's must need to ready flash action, DON'T REMOVE this.
    FLASH->CTRL = FLASH_CTRL_CE_Msk;        // Flash action begin.
    FLASH->LENGTH  = 0;                     // Flash action end. PWE=0.

    return 0;
}

/**
 *
 * @brief  Write data from u32buf[] to flash
 *
 * @param   address     The begin address for flash action. Bit[1:0] need keep 0.
 * @param   u32buf[]    The data buffer that width 32bits for program to flash
 * @param   length      The length for write or program by WORD 32Bits. MAX(length)=32. MIN(length)=1.
 * @return  0: PASS, others: FAIL
 */
uint32_t Flash_Write_U32(uint32_t address, uint32_t u32buf[], uint8_t length)
{
    uint8_t i;
    if(address & 0x3) return 1;
    if(length == 0)   return 1;
    if(length > 32)   return 1;
    __disable_irq();
    for(i = 0; i < length; i ++)
        FLASH->DATA32[i] = u32buf[i];
    i = flash_action(address, FLASH_CTRL_WRITE_Msk, length);
    __enable_irq();
    return i;
}

/**
 *
 * @brief  Write data from u32buf[] to flash
 *
 * @param   address     The begin address for flash action. Bit[1:0] need keep 0.
 * @param   u32buf[]    The data buffer that width 32bits for program to flash
 * @param   length      The length for write or program by WORD 32Bits. MAX(length)=32. MIN(length)=1.
 * @return  0: PASS, others: FAIL
 */
uint32_t Ble_Flash_Write_U32(uint32_t address, uint32_t u32buf[], uint8_t length)
{
    uint8_t i;
    if(address & 0x3) return 1;
    if(length == 0)   return 1;
    if(length > 32)   return 1;
    __disable_irq();
    for(i = 0; i < length; i ++)
        FLASH->DATA32[i] = u32buf[i];
    i = flash_action_ble(address, FLASH_CTRL_WRITE_Msk, length);
    __enable_irq();
    return i;
}


/**
 *
 * @brief  Write data from u8buf[] to flash
 *
 * @param   address     The begin address for flash action. Bit[1:0] need keep 0.
 * @param   u8buf[]     The data buffer that width 32bits for program to flash
 * @param   length      The length for write or program by WORD 32Bits. MAX(length)=128. MIN(length)=1.
 * @return  0: PASS, others: FAIL
 */
uint32_t Flash_Write_U8(uint32_t address, uint8_t u8buf[], uint8_t length)
{
    uint8_t i, n;
    if(address & 0x3)    return 1;
    if(length == 0)      return 1;
    if(length > 128)     return 1;
    __disable_irq();
    n = (4 - (length & 0x3)) % 4;
    for(i=0; i<length; i++)
        FLASH->DATA8[i] = u8buf[i];
    for(i = length; i < length+n; i ++)
        FLASH->DATA8[i] = 0xFF;
    i = flash_action(address, FLASH_CTRL_WRITE_Msk, ((length + 3) / 4));
    __enable_irq();
    return i;
}

/**
 *
 * @brief  Write data from u8buf[] to flash
 *
 * @param   address     The begin address for flash action. Bit[1:0] need keep 0.
 * @param   u8buf[]     The data buffer that width 32bits for program to flash
 * @param   length      The length for write or program by WORD 32Bits. MAX(length)=128. MIN(length)=1.
 * @return  0: PASS, others: FAIL
 */
uint32_t Ble_Flash_Write_U8(uint32_t address, uint8_t u8buf[], uint8_t length)
{
    uint8_t i, n;
    if (address & 0x3)    return 1;
    if (length == 0)      return 1;
    if (length > 128)     return 1;
    __disable_irq();
    n = (4 - (length & 0x3)) % 4;
    for(i=0; i<length; i++)
        FLASH->DATA8[i] = u8buf[i];
    for(i = length; i < length+n; i ++)
        FLASH->DATA8[i] = 0xFF;
    i = flash_action_ble(address, FLASH_CTRL_WRITE_Msk, ((length + 3) / 4));
    __enable_irq();
    return i;
}


/**
 *
 * @param   address     The begin address for flash action. Bit[1:0] need keep 0.
 * @return  0: PASS, others: FAIL
 *
 * @brief  Erase 1 paage (32 WORD = 128 Bytes) flash
 */
uint32_t Flash_PageErase(uint32_t address)
{
    uint32_t i;
    if(address & 0x3) return 1;
    __disable_irq();
    i = flash_action(address, FLASH_CTRL_PERASE_Msk, 1);
    __enable_irq();
    return i;
}

/**
 *
 * @param   address     The begin address for flash action. Bit[1:0] need keep 0.
 * @return  0: PASS, others: FAIL
 *
 * @brief  Erase 1 paage (32 WORD = 128 Bytes) flash
 */
uint32_t Ble_Flash_PageErase(uint32_t address)
{
    uint32_t i;
    if(address & 0x3) return 1;
    __disable_irq();
    i = flash_action_ble(address, FLASH_CTRL_PERASE_Msk, 1);
    __enable_irq();
    return i;
}

/**
 *
 * @param   address     The begin address for flash action. Bit[1:0] need keep 0.
 * @return  0: PASS, others: FAIL
 *
 * @brief  Erase 1 sector (32x32 WORD = 4K Bytes) flash
 */
uint32_t Flash_SectorErase( uint32_t address )
{
    uint32_t i;
    if(address & 0x3) return 1;
    __disable_irq();
    i = flash_action(address, FLASH_CTRL_SERASE_Msk, 1);
    __enable_irq();
    return i;
}

/**
 *
 * @param   address     The begin address for flash action. Bit[1:0] need keep 0.
 * @return  0: PASS, others: FAIL
 *
 * @brief  Erase 1 sector (32x32 WORD = 4K Bytes) flash
 */
uint32_t Ble_Flash_SectorErase( uint32_t address )
{
    uint32_t i;
    if(address & 0x3) return 1;
    __disable_irq();
    i = flash_action_ble(address, FLASH_CTRL_SERASE_Msk, 1);
    __enable_irq();
    return i;
}


/*@}*/ /* end o group FLASH_Driver */
/*@}*/ /* end of group A8107M_Driver */
