// main.c: implementation of the main functions.
//
//////////////////////////////////////////////////////////////////////
#include "main.h"
#include "NonVolatileMemory.h"
#include "Board.h"
//#include "EPaperSSD1608.h"
//#include "tagdefine.h"
//#include "Flash.h"
#include "Scanner.h"
#include "RfPacket.h"
#include "System.h"
#include <stdlib.h>
#include "Led.h"


//==============================================================================
//--- Declarative Variables ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative Global Variables ---
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//--- Declarative Member Variables ---
//------------------------------------------------------------------------------
uint16_t m_uiRxTimeoutCount = 0;
uint32_t m_uiLostBeaconCount = 0;			// Lost Beacon count 
uint32_t m_uiBeaconCount = 0;				// Sync Beacon count 
uint32_t m_uiBindingBeaconCount = 0;		// Bind Beacon count (Binding Host)
uint32_t m_uiTimingCount = 0;

SYSTEMTIME g_stSystemTime;

//------------------------------------------------------------------------------
//--- Declarative Other Variables ---
//------------------------------------------------------------------------------



//==============================================================================
//--- Functions ---
//==============================================================================
void OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
		case EVENT_TIMER_BEACON_SYNC:
			if (RfIsScanningChannel())
				RfScanChannelFailed();
			break;
		case EVENT_TIMER_DATA_PACKET:
			break;
		case EVENT_TIMER_TIMING_TRIGGER:
			m_uiTimingCount++;
			if (m_uiTimingCount >= TIME_TIMING_90_DAY_COUNT)
				m_uiTimingCount = 0;
			
#if 0
			if ((m_uiTimingCount%100) == 0)
			{
				//DEBUG_MESSAGE(FLAG_MF_SYSTEM, _T("*** Timing  ***\r\n"));
				DEBUG_MESSAGE(FLAG_MF_SYSTEM, _T(" T "));
			}
#endif
			OnLedTriggerTimer(TIME_TIMING_TRIGGER);
			break;
		default:
			break;
	}
}

int main(void)
{
	uint8_t uiRfDataRate;
	uint8_t * puiMachineState;
	uint32_t uiResult = ERROR_SUCCESS;
	uint8_t byPacket[MAX_DATA_FIFO_LENGTH];
	
	memset(&g_stSystemTime, 0 ,sizeof(g_stSystemTime));

	// Initial Variables
	SetMachineState(OSMS_PWRBEACON_STATE);	// Initial state
	puiMachineState = GetMachineStatePointer();

	// Initial class construction

	// Initial function classes
	BoardInit();		//Initial hardware system
	RfPacketInit();	
	Scanner();			// Call Scanner constructor	
	//(+) 2018-08-16 Richard Chung
	// Remove EPaper Driver in scanner project
	//EPaperInit(EPAPER_MODE_DEFAULT, false);
	
	//(-) 2018-08-16 Richard Chung

	// Initial RF
	uiRfDataRate = RfGetDataRate();
	initRF(); 			//init RF
	//UserRegister(uiRfDataRate);		//M8107 registers setting
	RfSetDataRate(uiRfDataRate);		// Set data rate and M8107 registers setting
	StrobeCmd(CMD_STBY);
	SetCH(RfGetChannel());

	// Initial RTC
	TurnOnExternalRTC();
	BoardInitRTC();

	// Initial Interrupt
	//BackupIOSetting();

	//
	// Machine State
	//
	while (1)
	{
		uiResult = ERROR_SUCCESS;
		//ShowCurrentTime();
		
		switch (*puiMachineState)
		{			
			case OSMS_PWRBEACON_STATE:
			case OSMS_BEACON_STATE:

				// Receive RF Beacon Packet		
				uiResult = RfGetPacket(byPacket, MAX_BEACON_FIFO_LENGTH, MAX_BEACON_FIFO_LENGTH, TIME_PACKET_RECV_BEACON);

				if (RfIsRxTimeout())
				{	
					//Rx Beacon time out.
					//m_uiLostBeaconCount++;	
					BeaconCount(true, RfGetBeaconMode());
					DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Lost Beacon: State = 0x%02X, Count = %d\r\n"), (*puiMachineState), m_uiLostBeaconCount);
					SetMachineState(OSMS_BEACON_STATE);
					LedSetStatus(OSLS_IDENTIFIED_STATE);
				}
				else
				{	
					if (*puiMachineState == OSMS_WAITTING_RESULT_STATE)
					{
			#ifdef FLAG_SHOW_WAIT_SERVER_RESPONSE
						// Receive RF Reply Packet				
						uint32_t uiReplyResult = ERROR_SUCCESS;
						uiReplyResult = RfGetPacket(byPacket, MAX_DATA_FIFO_LENGTH, MAX_DATA_FIFO_LENGTH, TIME_PACKET_RECV_ACK);
						ShowErrorMessage(_T("Wait for ACK"), uiReplyResult);
			#else
						if (RfGetPacket(byPacket, MAX_DATA_FIFO_LENGTH, MAX_DATA_FIFO_LENGTH, TIME_PACKET_RECV_ACK) == ERROR_SUCCESS)
							LedSetStatus(OSLS_SUCCEEDED_STATE);
			#endif
					}	
					
			#ifdef FLAG_ENABLE_OSMS_ERROR_MESSAGE
					ShowErrorMessage(_T("Beacon"), uiResult);
			#endif
					if ((uiResult == ERROR_WAIT_TIMEOUT) && (m_uiRxTimeoutCount >= MAX_RETRY_LIMIT))
					{
						LedSetStatus(OSLS_FAILED_STATE);
					}				
					SetMachineState(OSMS_BEACON_STATE);		
									
				}
				break;
			default:
				break;
		}
		
	}//while	
}

void BeaconCount(bool fLost, uint8_t uiBeaconMode)
{
	if (fLost)
		m_uiLostBeaconCount++;	// Lost 
	else
		m_uiBeaconCount++;		// Sync
	
	if (uiBeaconMode == OSBM_BEACON_BINDING_MODE)
		m_uiBindingBeaconCount++;
	if ((m_uiBeaconCount > DAY_IN_SEC) || (m_uiBindingBeaconCount > DAY_IN_SEC))
	{
		m_uiBeaconCount = 0;	
		m_uiBindingBeaconCount = 0;
	}
#if 0
	if ((m_uiBeaconCount % MINUTE_IN_SEC == 01) || (m_uiBindingBeaconCount % MINUTE_IN_SEC == 01) || (m_uiLostBeaconCount % MINUTE_IN_SEC == 01))	
	{
		DEBUG_MESSAGE(FLAG_MF_SYSTEM, _T("BatteryDetect: Battery Value = %d, Beacon Count = %d\r\n"), BatteryValue(), m_uiBeaconCount);		
	}
#endif
}

