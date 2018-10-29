#include "main.h"
#include "flash.h"
#include "RptComm.h"
#include "RptDataFlash.h"

//FLASH_Status FLASHStatus = FLASH_COMPLETE;
/*
#if   (defined ( __CC_ARM ))
  __IO uint32_t NvRamTable[128] __attribute__((at(NVRAM_START_ADDRESS)));
#elif (defined (__ICCARM__))
#pragma location = NVRAM_START_ADDRESS
  __no_init __IO uint32_t VectorTable[128];
#elif defined   (  __GNUC__  )
  __IO uint32_t NvRamTable[128] __attribute__((section(".NvRamTable")));
#elif defined ( __TASKING__ )
  __IO uint32_t NvRamTable[128] __at(NVRAM_START_ADDRESS);
#endif
*/
uint8_t Rpt_Flash_ReadU8(uint32_t ReadAddr)
{
    return  *(volatile uint32_t *)ReadAddr;
}

/**
* @brief  This function handles Write AT DATA to Flash request.
* @param  None
* @retval None
*/
void Rpt_Write_DataFlash(void)
{
    uint8_t FLASHStatus = 0;
    uint32_t u32Data;
    uint32_t u32Addr;
    uint32_t test_addr = NVRAM_TEST_PATTERN;

    u32Addr = NVRAM_START_ADDRESS;

    FLASH->LOCK[0] |= 0xC0000000;

    FLASHStatus = Flash_PageErase(u32Addr);

    if (FLASHStatus == 0)
    {
        Flash_Write_U32(u32Addr, &test_addr, 1);

        u32Addr += 4;
        u32Data = (RptDefSet.RptBatLv << 24) | (RptDefSet.RptDataRate << 16) |
                  (RptDefSet.Tchn << 8) | (RptDefSet.Gchn);
        Flash_Write_U32(u32Addr, &u32Data, 1);

        u32Addr += 4;
        u32Data = (RptDefSet.RptVerBLD << 24) | (RptDefSet.RptVerRLSE << 16) |
                  (RptDefSet.RptVerMINR << 8) | (RptDefSet.RptVerMJR);
        Flash_Write_U32(u32Addr, &u32Data, 1);

        /*u32Addr += 4;
        u32Data = (0 << 16 ) | (RptDefSet.RptReSync);
        Flash_Write_U32(u32Addr, &u32Data, 1);

        u32Addr += 4;
        u32Data = RptDefSet.StartSec;
        Flash_Write_U32(u32Addr, &u32Data, 1);
        */

        u32Addr += 4;
        u32Data = (RptDefSet.RptID[0] << 24);
        Flash_Write_U32(u32Addr, &u32Data, 1);

        u32Addr += 4;
        u32Data = (RptDefSet.RptID[4] << 24) | (RptDefSet.RptID[3] << 16) |
                  (RptDefSet.RptID[2] << 8) | (RptDefSet.RptID[1]);
        Flash_Write_U32(u32Addr, &u32Data, 1);

#ifdef TEST_RF_CHANNEL
        u32Addr += 4;
        u32Data = (RptDefSet.ChanCnt << 24);
        Flash_Write_U32(u32Addr, &u32Data, 1);
#endif //
    }

    FLASH->LOCK[0] &= ~0xC0000000;
}

/**
* @brief  This function handles Read AT DATA for Flash request.
* @param  None
* @retval None
*/
uint8_t Rpt_Read_DataFlash(void)
{
    uint8_t mflag = 0;
    uint32_t u32Addr;
    uint32_t u32Data;

    u32Addr = NVRAM_START_ADDRESS;

    //Check data flash was programmed?
    u32Data =  *(volatile uint32_t *)u32Addr;
    if (u32Data != NVRAM_TEST_PATTERN)
    {
        DBGMSG("Data Flash is empty...\r\n");
        mflag = 2;
        goto lexit;
    }

    u32Addr += 4;
    u32Data =  *(volatile uint32_t *)u32Addr;
    RptDefSet.Gchn = (uint8_t)(u32Data);
    RptDefSet.Tchn = (uint8_t)(u32Data >> 8);
    RptDefSet.RptDataRate = (uint8_t)(u32Data >> 16);

    u32Addr += 4;
    u32Data =  *(volatile uint32_t *)u32Addr;
    RptDefSet.RptVerMJR = (uint8_t)(u32Data);
    RptDefSet.RptVerMINR = (uint8_t)(u32Data >> 8);
    RptDefSet.RptVerRLSE = (uint8_t)(u32Data >> 16);
    RptDefSet.RptVerBLD = (uint8_t)(u32Data >> 24);

    /*u32Addr += 4;
    u32Data =  *(volatile uint32_t *)u32Addr;
    RptDefSet.RptReSync = (uint16_t)(u32Data >> 16);

    u32Addr += 4;
    u32Data =  *(volatile uint32_t *)u32Addr;
    RptDefSet.StartSec = (uint32_t)(u32Data);
    */

    u32Addr += 4;
    u32Data =  *(volatile uint32_t *)u32Addr;
    RptDefSet.RptID[0] = (uint8_t)(u32Data >> 24);
    u32Addr += 4;
    u32Data =  *(volatile uint32_t *)u32Addr;
    RptDefSet.RptID[1] = (uint8_t)(u32Data);
    RptDefSet.RptID[2] = (uint8_t)(u32Data >> 8);
    RptDefSet.RptID[3] = (uint8_t)(u32Data >> 16);
    RptDefSet.RptID[4] = (uint8_t)(u32Data >> 24);

#ifdef TEST_RF_CHANNEL
    u32Addr += 4;
    u32Data =  *(volatile uint32_t *)u32Addr;
    RptDefSet.ChanCnt = (uint8_t)(u32Data >> 24);
#endif //

lexit:
    return mflag;
}
