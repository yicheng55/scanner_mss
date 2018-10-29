#ifndef _RPT_IAP_H_
#define _RPT_IAP_H_

#include <stdint.h>
#include "core_cm0.h"

#define RestartAppAddress           0x00000000
#define BootloaderAddress           0x00000000
#define ApplicationAddress          0x00003000
#define HandlerOffset               0x20000000

#define STM32_FLASH_SIZE            512 //FLASH capacity size (in K) of the selected STM32
#define STM32_FLASH_WREN            1 //Enable FLASH write (0, not enable; 1, enable)
#define STM32_FLASH_BASE            0x00000000 //STM32 FLASH starting address

typedef __IO uint32_t   vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef void (*pFunction)(void);

void NVIC_SetVectorTable(uint32_t len);
void IAP_JumpTo_APP(void);
void APP_JumpTo_IAP(void);
void HardFault_ReStartApp(void);

uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);
void STMFLASH_WriteLenByte(uint32_t WriteAddr, uint32_t DataToWrite, uint16_t Len);
uint32_t STMFLASH_ReadLenByte(uint32_t ReadAddr, uint16_t Len);
void STMFLASH_Write(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite);
void STMFLASH_Read(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead);
void Test_Write(uint32_t WriteAddr, uint16_t WriteData);

#endif /* _RPT_IAP_H_ */
