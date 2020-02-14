/**********************************************
 * M8107.h
 *
 **********************************************/
#ifndef _M8107_H_
#define _M8107_H_

void EnterPM1(void);
void EnterPM3(void);
void TurnOn_External_RTC_Crystal(void);
int16_t RSSI_measurement(void);
void SetFIFOLvl(uint32_t fifolen);
uint8_t debounce ( uint32_t PIN );
uint8_t DetectButton ( uint32_t PIN );
uint32_t Temperature_measurement(void);
int8_t  ConvertTemperature_measurement(void);
uint16_t GetAdcTemperature_measurement(void);
void WatchDog_Rst_Enable(void);
void WatchDog_Load_Clear(void);
	
#endif
