// Led.c: implementation of the string functions.
//
//////////////////////////////////////////////////////////////////////
#include "Led.h"
#include "Utility.h"
#include "NonVolatileMemory.h"
#include "ProjectConfig.h"

//==============================================================================
//--- Declarative Variables ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative Global Variables ---
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//--- Declarative Member Variables ---
//------------------------------------------------------------------------------
uint8_t m_uiCurrentState = OSLS_POWER_ON_STATE;
uint8_t m_uiNextStatus = OSLS_POWER_ON_STATE;
uint32_t m_uiCycleTime = 0;							// State cycle time
uint8_t m_uiIndex = 0;								// LED value index 

//// Operating system led array index
//#define OSLI_INTERVAL_TIME				0
//#define OSLI_CYCLE_TIME						1
//#define OSLI_DATA_SIZE						2
//#define OSLI_DATA_BEGIN						3~
const uint32_t m_uiStatus[][13] = {
	{ 250, 1000, 2, 0x03, 0x00}, 				// Power On
	{ 1000, 1000, 1, 0x01},						// Online
	{ 100, 800, 8, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},	// Scan barcode/Scan action		
	{ 250, 1000, 2, 0x02, 0x00},				// Identified
	{ 200, 1000, 2, 0x01, 0x00},				// Transmission
	{ 1000, 1000, 1, 0x01},						// Succeeded (ACK)
	{ 1000, 1000, 1, 0x02},						// Failed
	{ 500, 1000, 2, 0x03, 0x00},				// No Device ID
	{ 200, 4000, 10, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00},	//  Predicted failure		
//	{ 500, 1500, 3, 0x03, 0x00, 0x00},			// Predicted failure
	{ 3000, 12000, 4, 0x01, 0x00, 0x02, 0x00},	// Spun down
	{ 250, 2000, 8, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},// Standby	
};	
		
							  

//==============================================================================
// Function     : 
// Purpose      : Construction
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void Led()
{
	//LedPowerOn();
	m_uiCurrentState = OSLS_POWER_ON_STATE;
	m_uiNextStatus = OSLS_POWER_ON_STATE;	
	m_uiCycleTime = 0;
}


//==============================================================================
// Function     : LedReSetStatus
// Purpose      : ReSet the state of the LED
// Description	: ReSet the next state of the LED
// Editor       : yi-cheng
// Update Date	: 2019-08-15
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void LedReSetStatus(uint8_t uiStatus)
{

		m_uiCurrentState = uiStatus;
		m_uiCycleTime = 0;
		m_uiIndex = 0;	
//	DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("m_uiCurrentState: %d.\r\n"), m_uiCurrentState);
}

//==============================================================================
// Function     : LedSetStatus
// Purpose      : Set the state of the LED
// Description	: Set the next state of the LED
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void LedSetStatus(uint8_t uiStatus)
{
	if ((g_stNvmMappingData.stDeviceID.stComboID.byEUSI[0] == 0x00) && 
		(g_stNvmMappingData.stDeviceID.stComboID.byEUSI[1] == 0x00) &&
		(g_stNvmMappingData.stDeviceID.stComboID.byEUSI[2] == 0x00) && (uiStatus != OSLS_POWER_ON_STATE))
	{
		m_uiNextStatus = OSLS_NO_DEVICE_ID;
	}
	else if (m_uiNextStatus == OSLS_STANDBY_STATE)
	{
		m_uiNextStatus = uiStatus;
	}
	else if (uiStatus != OSLS_ONLINE_STATE)
	{
		m_uiNextStatus = uiStatus;
	}			
//	DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("m_uiNextStatus: %d.\r\n"), m_uiNextStatus);
}

//==============================================================================
// Function     : LedSetValue
// Purpose      : Turn LED on or off
// Description	: Blink LED
// Editor       : Richard Chung
// Update Date	: 2018-04-17
// -----------------------------------------------------------------------------
// Parameters   : bool fOn
//              :     Lights LED if true, otherwise goes off.
// Return       : 
// Remarks      : 
//==============================================================================
void LedSetValue(uint32_t uiStatus)
{
	//INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("LedSetValue = %02X\r\n"), uiStatus);
	if (uiStatus & LED_INDICATOR_LED1_BIT)
		GPIO_LED->DATAOUT_PIN.PIN_LED1_P = LED_INDICATOR_ON;  
	else
		GPIO_LED->DATAOUT_PIN.PIN_LED1_P = LED_INDICATOR_OFF;
	
	if (uiStatus & LED_INDICATOR_LED2_BIT)
		GPIO_LED->DATAOUT_PIN.PIN_LED2_P = LED_INDICATOR_ON;  
	else
		GPIO_LED->DATAOUT_PIN.PIN_LED2_P = LED_INDICATOR_OFF;	
}

//==============================================================================
// Function     : LedOnTriggerTimer
// Purpose      : Turn LED on or off
// Description	: Blink LED
// Editor       : Richard Chung
// Update Date	: 2018-04-17
// -----------------------------------------------------------------------------
// Parameters   : uint8_t uiTime
//              :     Trigger time in milliseconds.
// Return       : 
// Remarks      : 
//==============================================================================
void OnLedTriggerTimer(uint8_t uiTime)
{
	m_uiCycleTime += uiTime;
	LedShowStatus();
}

//==============================================================================
// Function     : LedShowStatus
// Purpose      : LED lights up according to status
// Description	: Change LED status according to time
// Editor       : Richard Chung
// Update Date	: 2018-04-17
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void LedShowStatus()
{
	uint16_t uiIntervalTime = m_uiStatus[m_uiCurrentState][OSLI_INTERVAL_TIME];
	uint16_t uiCycleTime = m_uiStatus[m_uiCurrentState][OSLI_CYCLE_TIME];
	
	/*
	if ((g_stNvmMappingData.stDeviceID.Split.byESID[0] == 0x00) && (g_stNvmMappingData.stDeviceID.Split.byESID[1] == 0x00) && (m_uiCurrentState != OSLS_POWER_ON_STATE))
	{
		m_uiCurrentState = OSLS_NO_DEVICE_ID;
		uiIntervalTime = m_uiStatus[OSLS_NO_DEVICE_ID][OSLI_INTERVAL_TIME];
		uiCycleTime = m_uiStatus[OSLS_NO_DEVICE_ID][OSLI_CYCLE_TIME];	
		//INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Current = %0d, uiDataSize = %d, uiIntervalTime = %d, uiCycleTime = %d, uiStatus(%d) = 0x%02X\r\n"), m_uiCurrentState, uiDataSize, uiIntervalTime, uiCycleTime, m_uiIndex, uiStatus);
		INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Device ID: %02X:%02X:%02X:%02X:%02X\r\n"), g_stNvmMappingData.stDeviceID.byEFID[0],g_stNvmMappingData.stDeviceID.byEFID[1], g_stNvmMappingData.stDeviceID.byEFID[2], g_stNvmMappingData.stDeviceID.byEFID[3], g_stNvmMappingData.stDeviceID.byEFID[4]);
	}	*/

	if ((m_uiCycleTime % uiIntervalTime) == 0)
	{
		uint8_t uiDataSize = m_uiStatus[m_uiCurrentState][OSLI_DATA_SIZE];
		uint32_t uiStatus;		
		if (m_uiIndex > uiDataSize-1)
			m_uiIndex = 0;
		uiStatus = m_uiStatus[m_uiCurrentState][OSLI_DATA_BEGIN + m_uiIndex];
//		INFORMATION_MESSAGE(FLAG_MF_SCANNER, _T("Current = %0d, uiDataSize = %d, uiIntervalTime = %d, uiCycleTime = %d, uiStatus(%d) = 0x%02X\r\n"), m_uiCurrentState, uiDataSize, uiIntervalTime, uiCycleTime, m_uiIndex, uiStatus);
		LedSetValue(uiStatus);
		m_uiIndex++;	
	}
	
	if (m_uiCycleTime >= uiCycleTime)
	{
		m_uiCycleTime = 0;
		m_uiIndex = 0;
		if (m_uiNextStatus != OSLS_STANDBY_STATE)
		{
			m_uiCurrentState = m_uiNextStatus;
			if ((m_uiNextStatus != OSLS_TRANSMISSION_STATE) && 
				(m_uiNextStatus != OSLS_SUCCEEDED_STATE) && 		
				(m_uiNextStatus != OSLS_FAILED_STATE) &&
				(m_uiNextStatus != OSLS_SCAN_BARCODE_STATE)) 
				m_uiNextStatus = OSLS_STANDBY_STATE;
		}
	}
}



#if 1

void SetLed1(bool fOn)
{
	if (fOn)
		GPIO_LED->DATAOUT_PIN.PIN_LED1_P = LED_INDICATOR_ON;  
	else
		GPIO_LED->DATAOUT_PIN.PIN_LED1_P = LED_INDICATOR_OFF; 
}

//==============================================================================
// Function     : SetLed1
// Purpose      : Turn LED on or off
// Description	: Blink LED
// Editor       : Richard Chung
// Update Date	: 2018-04-17
// -----------------------------------------------------------------------------
// Parameters   : bool fOn
//              :     Lights LED if true, otherwise goes off.
// Return       : 
// Remarks      : 
//==============================================================================
void SetLed2(bool fOn)
{
	if (fOn)
		GPIO_LED->DATAOUT_PIN.PIN_LED2_P = LED_INDICATOR_ON;  
	else
		GPIO_LED->DATAOUT_PIN.PIN_LED2_P = LED_INDICATOR_OFF; 
}


void armm0_toggle_led(uint8_t led)
{
	static uint8_t stled1=0,stled2=0;
	
  if(led == 1)
  {
		stled1 = ~stled1;
		printf("stled1=%x  ,",stled1);
    SetLed1(stled1);
  }
  else if(led == 2)
  {
		stled2 = ~stled2;
		printf("stled2=%x  ,",stled2);
    SetLed2(stled2);
  }
}

#endif
