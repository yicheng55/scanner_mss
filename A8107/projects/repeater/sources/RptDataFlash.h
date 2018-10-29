#ifndef _RPT_DATA_FLASH_H_
#define _RPT_DATA_FLASH_H_

#include <stdint.h>

/* Private define ------------------------------------------------------------*/
#define NVRAM_START_ADDRESS     ((uint32_t)0x10000 - FLASH_PAGE_SIZE) //(1024 * 1))
#define NVRAM_END_ADDRESS       ((uint32_t)0x10000)
#define NVRAM_TEST_PATTERN      ((uint32_t)0x55AA55AA)

#define FLASH_PAGE_SIZE         ((uint16_t)0x80)        //a page 128byte
#define NEW_APP_START_ADDR      ((uint32_t)0x00020000)   //write new App start address
#define NEW_APP_END_ADDR        ((uint32_t)0x00020000 + (1024 * 64)) //end address


uint8_t Rpt_Read_DataFlash(void);
void Rpt_Write_DataFlash(void);
void WriteNewAppBin(uint32_t BackAppStartAddr, uint8_t *NewAppBinBuf, uint32_t NewAppBinSize);
uint8_t Rpt_Flash_ReadU8(uint32_t ReadAddr);

#endif /* _RPT_DATA_FLASH_H_ */
