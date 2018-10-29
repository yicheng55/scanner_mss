#ifndef _DAVICOM_EPAPER_SSD1608_H_
#define _DAVICOM_EPAPER_SSD1608_H_

#include "basic_windef.h"
#include <Stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
//==============================================================================
//--- Declarative Constants ---
//==============================================================================
#define EPAPER_MODE_DEFAULT			0	// The best way
#define EPAPER_MODE_METHOD1			1	// Test method 1
#define EPAPER_MODE_METHOD2			2	// Test	method 1

//==============================================================================
//--- Declarative Public Functions ---
//==============================================================================
void EPaperInit(int nEpaperMode, bool fPowerDown); // nEpaperMode = EPAPER_MODE_DEFAULT/EPAPER_MODE_METHOD1/...

//==============================================================================
//--- EPD Colors ---
//==============================================================================
#define EPAPER_COLOR_BLACK_WHITE			0
#define EPAPER_COLOR_RED			1

//void spi3w_wcommand(uint32_t command);
//void spi3w_wdata(uint32_t data);
//void HWReset(void);
//void ePaper_Init(void);
void WriteRAMqr_FULL(int TotalLength, uint8_t* ReceiveData);
void UpdateDisplay(void);
void TagPowerDownFunction(void);
void TagPowerUpFunction(void);
extern void Write_QRCode_FULL(void);
extern void WhiteDM(void);
//void ePaper_Init_03(bool fPowerDown);
extern void Write_EPD_FULL(uint8_t, int, uint8_t*);
extern void CleanEPD(void);
void EPaperShowScannerStatus(const TCHAR *szScanCode, const TCHAR *szTagID, const TCHAR *szProductID, const TCHAR *szStatus, bool fCommand);
void EPaperShowTagStatus(const TCHAR *szTagID, const TCHAR *szTagStatus, const TCHAR *szTagMsg1, const TCHAR *szTagMsg2);
void EPaperShowTagDefaultStatus(const TCHAR *szTagID, const TCHAR *szTagStatus, const TCHAR *szTagMsg1, const TCHAR *szTagMsg2, const TCHAR *szTagMsg4);
#endif	// _DAVICOM_EPAPER_SSD1608_H_;
