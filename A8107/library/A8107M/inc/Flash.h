/**************************************************************************//**
 * @file        Flash.h
 * @version     V1.00.02
 * $Revision:   0 $
 * $Date:       2017-07-05  $
 * @brief       FLASH Driver Header File
 *
 * @note
 *              V1.00.02 - add function:
 *                  Ble_Flash_Write_U32, Ble_Flash_Write_U8
 *                  Ble_Flash_PageErase, Ble_Flash_SectorErase
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/

#ifndef __FLASH_H__
#define __FLASH_H__


/** @addtogroup A8107M_Driver Standard Driver
    @{
*/
/** @addtogroup FLASH_Driver UART Driver
    @{
*/

/**
 * @brief  Write data from u32buf[] to flash
 */
extern uint32_t Flash_Write_U32(uint32_t address, uint32_t u32buf[], uint8_t length);

/**
 * @brief  Write data from u32buf[] to flash
 */
extern uint32_t Ble_Flash_Write_U32(uint32_t address, uint32_t u32buf[], uint8_t length);

/**
 * @brief  Write data from u8buf[] to flash
 */
extern uint32_t Flash_Write_U8(uint32_t address, uint8_t u8buf[], uint8_t length);

/**
 * @brief  Write data from u8buf[] to flash
 */
extern uint32_t Ble_Flash_Write_U8(uint32_t address, uint8_t u8buf[], uint8_t length);

/**
 * @brief  Erase 1 paage (32 WORDs = 128 Bytes) flash
 */
extern uint32_t Flash_PageErase(uint32_t address);

/**
 * @brief  Erase 1 paage (32 WORDs = 128 Bytes) flash
 */
extern uint32_t Ble_Flash_PageErase(uint32_t address);

/**
 * @brief  Erase 1 sector (32x32 WORD = 4K Bytes) flash
 */
extern uint32_t Flash_SectorErase(uint32_t address);

/**
 * @brief  Erase 1 sector (32x32 WORD = 4K Bytes) flash
 */
extern uint32_t Ble_Flash_SectorErase(uint32_t address);

/*@}*/ /* end of group FLASH_Driver */
/*@}*/ /* end of group A8107M_Driver */

#endif
