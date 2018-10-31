// isr.c: implementation of the RF functions.
//
//////////////////////////////////////////////////////////////////////
#include "main.h"
#include "Scanner.h"
#include "EslDebug.h"
#include "RfPacket.h"
#include "Encryption.h"
#include "System.h"
#include <stdlib.h>
#include "Led.h"
#include "NonVolatileMemory.h"

//==============================================================================
//--- Declarative Variables ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative Global Variables ---
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//--- RF ID ---
//------------------------------------------------------------------------------
const uint8_t ID_Tab[8] = { 0x44,0x61,0x76,0x69,0x63,0x6F,0x6D,0x5F }; //ID code (Provides reference to a8107m_rflib.o)


//------------------------------------------------------------------------------
//--- Declarative Member Variables ---
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//--- RF Chanel ---
//------------------------------------------------------------------------------
const uint8_t m_uiChannelTable[] = 
	{44,45,46,47,48,49,50,51,52,53,
	 54,55,56,92,93,94,95,96,97,98,
	 99,100,101,102,103,104,
#ifdef _DEBUG	
		142, 
#endif		
	 144,145,146,147,
	 148,149,150,151,152,153,154,155,156,157,
	 158,159,160,161,162,163,164,165,166,167,
#ifdef _DEBUG		
     184,200
#endif
}; // 184 and 200 are illegal channels	
const uint8_t m_uiChannelTableSize = (sizeof(m_uiChannelTable)/sizeof(uint8_t));


uint8_t m_uiTxSeqNo = 0;				//TX result sequence number

//
// Channels
//
uint8_t m_uiRfChannelIndex = 0;	
uint8_t m_uiRfChannelStart;	// for atuo scan
// Scan channels
bool m_fScanChannel = false;		// Auto scan
uint8_t m_uiScanChannelRetryCount = 0;
uint8_t m_uiScanChannelBeaconCount = 0;



// Device/Repeater/Gateway ID
//ESL_DEVICE_ID m_stDeviceID;
ESL_DEVICE_ID m_stRepeaterID;
ESL_DEVICE_ID m_stGatewayID;
ESL_DEVICE_ID m_stBindingID;

//bool m_fBindingMode = false;
uint8_t m_uiBeaconMode = OSBM_BEACON_SYNC_MODE;
uint32_t m_uiBeaconTickCount = 0;

bool m_fRxTimeout = false;				// Oneshoutflag;		//RX timeout flag
uint8_t m_uiMachineState;

#ifdef FLAG_SHOW_RX_TICK_COUNT
uint32_t m_uiLeastTime;
#endif


//------------------------------------------------------------------------------
//--- Encryption and decryption ---
//------------------------------------------------------------------------------
//bool m_fEncryptionMode = false;
const uint8_t m_aryAesKey[] =
{
	0x4C, 0x68, 0x38, 0x41,
	0x39, 0xF5, 0x74, 0xD8,
	0x36, 0xBC, 0xF3, 0x4E,
	0x9D, 0xFB, 0x01, 0xBF
};

// Encryption and decryption
uint8_t m_aryEncryptData[16];
uint8_t m_aryDecryptData[16];

//------------------------------------------------------------------------------
//--- Declarative Other Variables ---
//------------------------------------------------------------------------------



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
void RfPacket(void)
{
	RfPacketInit();
}

//==============================================================================
// Function     : RfInit
// Purpose      : Initial RF
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : uiDataRate
//              :    RF data rate
// Return       : 
// Remarks      : 
//==============================================================================
void RfPacketInit(void)
{
	//uint8_t byDeviceID[MAX_DEVICE_IDENTIFIER] = EDID_DEFAULT_ID;
	uint8_t stRepeaterID[MAX_DEVICE_IDENTIFIER] = { EISI_DEFAULT_ID, EUSI_REPEATER_ID };
	uint8_t stGatewayID[MAX_DEVICE_IDENTIFIER] = { EISI_DEFAULT_ID, EUSI_GATEWAY_ID };
	uint8_t uiIndex = 0;
	
	// Initial Device ID
	//memcpy(m_stDeviceID.byEFID, byDeviceID, MAX_DEVICE_IDENTIFIER);
	//memcpy(g_stNvmMappingData.tDeviceID.byEFID, g_stNvmData.stDeviceID.byEFID, MAX_DEVICE_IDENTIFIER);
	memcpy(m_stRepeaterID.byEFID, stRepeaterID, MAX_DEVICE_IDENTIFIER);
	memcpy(m_stGatewayID.byEFID, stGatewayID, MAX_DEVICE_IDENTIFIER);

	// Initial RF data rate (Move to g_stNvmData stuuartion)
	
	m_uiRfChannelIndex = 0;
	SetCH(m_uiChannelTable[0]);
	for (uiIndex = 0; uiIndex < m_uiChannelTableSize; uiIndex++)
	{
		if (m_uiChannelTable[uiIndex] == g_stNvmData.uiChannel)
		{
			m_uiRfChannelIndex = uiIndex;
			SetCH(g_stNvmData.uiChannel);
			break;
		}
	}
		
	// Encrption
	//m_fEncryptionMode = RF_DEFAULT_ENCRYPTION_MODE;
	//m_fEncryptionMode = g_stNvmData.fEncryptionMode;
	m_fRxTimeout = false;
	
#ifdef FLAG_SHOW_RX_TICK_COUNT	
	m_uiLeastTime = GetTickCount();
#endif
}

//==============================================================================
// Function     : RfIsBindingMode
// Purpose      : Whether the system is in binding mode
// Description	: Whether the system is in binding mode
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : uiDataRate
//              :    RF data rate
// Return       : 
// Remarks      : 
//==============================================================================
#if 0
bool RfIsBindingMode()
{
	return m_fBindingMode;
}

//==============================================================================
// Function     : RfSetBindingMode
// Purpose      : Set binding mode
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : uiDataRate
//              :    RF data rate
// Return       : 
// Remarks      : 
//==============================================================================
void RfSetBindingMode(bool fBindingMode)
{
	m_fBindingMode = fBindingMode;
	LedSetStatus(OSLS_IDENTIFIED_STATE);
	
#ifdef FLAG_SHOW_SYSTEM_MODE_SWITCHING_MESSAGE
	if (m_fBindingMode)
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("System switches to binding mode.\r\n"));
	else
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("System switches to default mode.\r\n"));
#endif
}
#endif

//==============================================================================
// Function     : RfGetBeaconMode
// Purpose      : Whether the system is in binding mode
// Description	: Whether the system is in binding mode
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : uiDataRate
//              :    RF data rate
// Return       : 
// Remarks      : 
//==============================================================================
uint8_t RfGetBeaconMode()
{
	return m_uiBeaconMode;
}

//==============================================================================
// Function     : RfSetBeaconMode
// Purpose      : Set Beacon mode
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : uiBeaconMode
//              :    Beacon mode
// Return       : 
// Remarks      : 
//==============================================================================
void RfSetBeaconMode(uint8_t uiBeaconMode)
{
	m_uiBeaconMode = uiBeaconMode;
	LedSetStatus(OSLS_IDENTIFIED_STATE);
	
#ifdef FLAG_SHOW_SYSTEM_MODE_SWITCHING_MESSAGE
	switch (m_uiBeaconMode)
	{
		case OSBM_BEACON_FREE_MODE:
		{
			DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("System switches to free mode.\r\n"));
		}
		break;
		case OSBM_BEACON_BINDING_MODE:
		{
			DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("System switches to binding mode.\r\n"));
		}
		break;
		default:
		case OSBM_BEACON_SYNC_MODE:
		{
			DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("System switches to synchronization mode.\r\n"));
		}
		break;
	}
#endif
}

//==============================================================================
// Function     : RfIsBeaconSyncTimeout
// Purpose      : Check timeout
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
//              : 
// Return       : 
// Remarks      : 
//==============================================================================
bool RfIsBeaconSyncTimeout(void)
{
	if ((m_uiBeaconMode == OSBM_BEACON_SYNC_MODE) && (m_uiBeaconTickCount != 0))
	{
		// Get Tickcount
		uint32_t uiTickCount = GetTickCount();
		if ((uiTickCount - m_uiBeaconTickCount) >= (TIME_BEACON_TIMEOUT * TIME_ONE_SECONE_TICKCOUNT))
		{
			ShowTickCount((uiTickCount - m_uiBeaconTickCount));
			DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Beacon Sync Timeout.(TickCount Value: %d)\r\n"), (uiTickCount - m_uiBeaconTickCount));		
			m_uiBeaconTickCount = 0;
			return true;
		}	
	}
	return false;
}

void RfResetBindingID(void)
{
	memset(&m_stBindingID, 0, sizeof(m_stBindingID));
}

//==============================================================================
// Function     : SetDataRate
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : uiDataRate
//              :    RF data rate
// Return       : 
// Remarks      : 
//==============================================================================
void RfSetDataRate(uint8_t uiDataRate)
{
	g_stNvmMappingData.uiDataRate = uiDataRate;	
	RADIO->DATARATE = uiDataRate;		// DateRate = 0/1/2 => 250Kbps/500Kbps/1Mbps
	RADIO->FIFOCTRL = 0x0000003F;		// FIFO control register
	RADIO->CODE = 0x01280011;			// Code1 register (Enable CRC)
	//int i;
	//for (i=0; i<4; i++)
	//	RADIO->SYNCB_ID[3-i] = ID_Tab[i];	//ID Bytes		
	RADIO->SYNC_ID = (ID_Tab[0] << 24) + (ID_Tab[1] << 16) + (ID_Tab[2] << 8) + ID_Tab[3];
	//DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Set data rate to %dK. (%s)\r\n"), nDataRateValue/1024, m_stBarcodePair.byCommand);
}

//==============================================================================
// Function     : GetDataRate
// Purpose      : Return current data rate
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
uint8_t RfGetDataRate()
{
	return g_stNvmMappingData.uiDataRate;
}

//==============================================================================
// Function     : SetRfChannel
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : uiDataRate
//              :    RF data rate
// Return       : 
// Remarks      : 
//==============================================================================
void RfSetChannel(uint8_t uiChannel)
{
	//m_fScanChannel = false;			// Don't auto scan
	DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Set channels to channel %d.\r\n"), uiChannel);
	g_stNvmMappingData.uiChannel = uiChannel;
	SetCH(g_stNvmMappingData.uiChannel);
}

//==============================================================================
// Function     : GetRfChannel
// Purpose      : Return current channel
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
uint8_t RfGetChannel(void)
{
	return g_stNvmMappingData.uiChannel;
}

#if 0
bool RfSaveChannel(uint8_t uiChannel)
{
	if (uiChannel < RF_CHANNEL_MINIMUM)
		return false;
	if (uiChannel > RF_CHANNEL_MAXIMUM)
		return false;
	g_stNvmMappingData.uiChannel = uiChannel;
	return true;
}

uint8_t RfLoadChannel(void)
{
	uint8_t uiChannel = g_stNvmMappingData.uiChannel;
	
	if (uiChannel < RF_CHANNEL_MINIMUM)
		uiChannel = RF_DEFFAULT_CHANNEL;
	if (uiChannel > RF_CHANNEL_MAXIMUM)
		uiChannel = RF_DEFFAULT_CHANNEL;	
	return uiChannel;
}
#endif

void RfSetScanChannel(bool fScan)
{
	m_fScanChannel = fScan;
	
#ifdef FLAG_SHOW_SCAN_CHANNEL_MESSAGE
	if (m_fScanChannel)
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("System starts scanning channels.\r\n"));
	}
	else
	{
		DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("System  stops scanning channels.\r\n"));
	}
#endif	
	if (m_fScanChannel)
	{
		m_uiRfChannelStart = RfGetChannel();
		m_uiScanChannelRetryCount = 0;
		m_uiScanChannelBeaconCount = 0;		
	}		
}

bool RfIsScanningChannel()
{
	return m_fScanChannel;
}

void RfScanChannelFailed()
{
	m_uiScanChannelRetryCount++;
	if (m_uiScanChannelRetryCount > MAX_CHANNEL_RETRY)
	{
		uint8_t uiIndex;
		uint8_t uiRfChannel = g_stNvmMappingData.uiChannel;
		m_uiScanChannelRetryCount = 0;
		m_uiRfChannelIndex = 0;
		for (uiIndex = 0; uiIndex < m_uiChannelTableSize; uiIndex++)
		{
			if (m_uiChannelTable[uiIndex] == g_stNvmMappingData.uiChannel)
			{
				m_uiRfChannelIndex = uiIndex;
				break;
			}
		}
		m_uiRfChannelIndex++;
		if (m_uiRfChannelIndex >= m_uiChannelTableSize)
			m_uiRfChannelIndex = 0;
	
		uiRfChannel = m_uiChannelTable[m_uiRfChannelIndex];
		
#if 0
		uiRfChannel += RF_CHANNEL_INTERVAL;
		if (uiRfChannel > RF_CHANNEL_MAXIMUM)
			uiRfChannel = RF_CHANNEL_MINIMUM;
#endif
		RfSetChannel(uiRfChannel);
		if (g_stNvmMappingData.uiChannel == m_uiRfChannelStart)
		{
			DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Failed to detect channel.\r\n"));
			RfSetScanChannel(false);	
			LedSetStatus(OSLS_FAILED_STATE);
			//m_stBarcodePair.fLock = false			
		}
	
	}	
}

void RfScanChannelSucceeded()
{
	m_uiScanChannelBeaconCount++;
	if (m_uiScanChannelBeaconCount > MIN_CHANNEL_BEACON)
	{
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Beacon signal detected.\r\n"));
		RfSetScanChannel(false);
		//RfSaveChannel(g_stNvmMappingData.uiChannel);
		//LedScanChannelSucceeded();
		LedSetStatus(OSLS_SUCCEEDED_STATE);
	}
}

// RF RSSI
//==============================================================================
// Function     : RfGetRssiCode 
// Purpose      : RSSI measurement
// Description	: Return current RSSI code 
// Editor       : Richard Chung
// Update Date	: 2018-04-11
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
uint16_t RfGetRssiCode(void)
{
    uint32_t uiRssicode;
	//DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("ADC9 Control register = 0x%X\r\n"), RADIO->ADC9);
    uiRssicode = RADIO->THRESHOLD & 0x1FF;            //read RSSI value(wanted signal RSSI)
	//DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("wanted signal RSSI = 0x%X\r\n"), uiRssicode);
    return uiRssicode;	
}
//==============================================================================
// Function     : RfGetRssiInputPower
// Purpose      : Convert current RSSI code to input power
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-04-11
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
int16_t RfConvertRssiInputPower(uint16_t uiRssiCode)
{
#if 0
	if (RfIsRssiEnableAGC())
	{
		// AGC On
		return RfGetRssiCode();	
	}
	else
	{
		// AGC Off
		return RfGetRssiCode();	
	}
#endif
	return RfGetRssiCode();
}

bool RfIsRssiEnableAGC()
{
	return true;
}

uint32_t RfRandomDelay(uint32_t uiMaximum)
{
	// Random number between 0 and uiMaximum
#if 1
	uint8_t uiRandom = rand() % uiMaximum;		
	DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Random number: %d\r\n"), uiRandom);	
	return uiRandom;
#else
	return (rand() % uiMaximum);
#endif
}

//==============================================================================
// Function     : SetRfChannel
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : uiDataRate
//              :    RF data rate
// Return       : 
// Remarks      : 
//==============================================================================
void SetMachineState(uint8_t uiMachineState)
{
	m_uiMachineState = uiMachineState;
}

//==============================================================================
// Function     : GetRfChannel
// Purpose      : Return current channel
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
uint8_t GetMachineState(void)
{
	return m_uiMachineState;
}

uint8_t * GetMachineStatePointer(void)
{
	return &m_uiMachineState;
}

//==============================================================================
// Function     : RfSetEncryptionMode
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : fEnable
//              :    Enable Encryption Mode
// Return       : 
// Remarks      : 
//==============================================================================
void RfSetEncryptionMode(bool fEnable)
{
	g_stNvmMappingData.fEncryptionMode = fEnable;
}

//==============================================================================
// Function     : GetRfChannel
// Purpose      : Return current channel
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
uint8_t RfGetEncryptionMode(void)
{
	return g_stNvmMappingData.fEncryptionMode;
}


void RfSetRxTimeout(bool fTimeout)
{
	m_fRxTimeout = fTimeout;
}

bool RfIsRxTimeout()
{
	return m_fRxTimeout;
}

//==============================================================================
// Function     : 
// Purpose      : UserRegister
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
#if 0
void UserRegister(uint8_t uiDataRate)
{
	int i;
	RADIO->DATARATE = uiDataRate;		// DateRate=500Kbps
	RADIO->FIFOCTRL = 0x0000003F;		// FIFO control register
	RADIO->CODE = 0x01280011;		// Code1 register (Enable CRC)
//	const uint8_t ID_Tab[8]={0x44,0x61,0x76,0x69,0x63,0x6F,0x6D,0x5F}; //ID code
#if 1
	for (i=0; i<4; i++)
	{
		RADIO->SYNCB_ID[3-i] = ID_Tab[i];	//ID Bytes		
	}		
#else
	RADIO->SYNCB_ID[3] = ID_Tab[0];
	RADIO->SYNCB_ID[2] = ID_Tab[1];
	RADIO->SYNCB_ID[1] = ID_Tab[2];
	RADIO->SYNCB_ID[0] = ID_Tab[3];
#endif
}
#endif

#if 0
ESL_DEVICE_ID GetDeviceID()
{
	return m_stDeviceID;
}
ESL_DEVICE_ID  GetRepeaterID()
{
	return m_stRepeaterID;
}

ESL_DEVICE_ID  GetGatewayID()
{
	return m_stGatewayID;
}
#endif

bool GetDeviceID(ESL_DEVICE_ID * pstID)
{
	if (pstID == NULL)
		return false;	
	*pstID = g_stNvmMappingData.stDeviceID;
	return true;
}

bool SetDeviceID(ESL_DEVICE_ID stID)
{
	g_stNvmMappingData.stDeviceID = stID;
	return true;
}

bool GetRepeaterID(ESL_DEVICE_ID * pstID)
{
	if (pstID == NULL)
		return false;	
	*pstID = m_stRepeaterID;
	return true;
}

bool SetRepeaterID(ESL_DEVICE_ID stID)
{
	m_stRepeaterID = stID;
	return true;
}

bool GetGatewayID(ESL_DEVICE_ID * pstID)
{
	if (pstID == NULL)
		return false;	
	*pstID = m_stGatewayID;
	return true;
}

bool SetGatewayID(ESL_DEVICE_ID stID)
{
	m_stGatewayID = stID;
	return true;
}

bool GetBindingID(ESL_DEVICE_ID * pstID)
{
	if (pstID == NULL)
		return false;	
	*pstID = m_stBindingID;
	return true;
}

bool SetBindingID(ESL_DEVICE_ID stID)
{
	DEBUG_MESSAGE(FLAG_MF_SCANNER, _T("Binding ID: %02X-%02X-%02X-%02X-%02X\r\n"), stID.byEFID[0], stID.byEFID[1], stID.byEFID[2], stID.byEFID[3], stID.byEFID[4]);
	
	
	m_stBindingID = stID;
	return true;
}


/***************************************************************************
 * setSleepTimer(uint32_t sec)
 *		Just supports sec = 1, 2, 5, 10, 20, 40
 *	sec = 2, SLPTIMER1 is 16 bits counter
 *		cks = 0, BEACONTIMEOUT = 2, magicno = 68, sleep time = 1, 2 sec
 *		cks = 1, BEACONTIMEOUT = 3, magicno = 15, sleep time = 10 sec
 *		cks = 2, BEACONTIMEOUT = 2, magicno = 5, sleep time = 20 sec
 *		cks = 3, BEACONTIMEOUT = 2, magicno = 1, sleep time = 40 sec
 ***************************************************************************/

void ShowReceivePacket(LPBYTE lpbyBuffer, uint8_t uiLength)
{
#if 1
	if (m_uiMachineState == OSMS_PWRBEACON_STATE || m_uiMachineState == OSMS_BEACON_STATE)
	{
		PSYNC_BEACON pstSyncBeacon = (PSYNC_BEACON)lpbyBuffer;	
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("\r\n(+)=== SyncBeacon Packet ===\r\n"));
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("BeaconSeq: %02X\r\n"), pstSyncBeacon->byBeaconSeq);
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("BeaconType: %02X\r\n"), pstSyncBeacon->byBeaconType);
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Action: %02X\r\n"), pstSyncBeacon->byAction);
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("ChnCtrl: %02X\r\n"), pstSyncBeacon->byChnCtrl);
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("WakeUpTime: %02X\r\n"), pstSyncBeacon->byWakeUpTime);
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("GatewayID: %02X:%02X:%02X\r\n"), pstSyncBeacon->byGatewayID[0], pstSyncBeacon->byGatewayID[1], pstSyncBeacon->byGatewayID[2]);
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("(-)=== SyncBeacon Packet ===\r\n\r\n"));
		//if ((pstSyncBeacon->BeaconType == RF_PT_SYNC_BEACON) && (pstSyncBeacon->Action == RF_AL_SYNC))
		//	DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("\r\n*** Free Beacon ***\r\n"));
	}		
	else if (uiLength == MAX_DATA_FIFO_LENGTH)
#else
	if (uiLength == MAX_DATA_FIFO_LENGTH)	
#endif
	{
		PESL_COMMON_HEADER pstCommon = (PESL_COMMON_HEADER)lpbyBuffer;
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("\r\n(+)=== Common Packet ===\r\n"));
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("SequenceNumber: %02X\r\n"), pstCommon->bySequenceNumber);
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("PacketType: %02X\r\n"), pstCommon->byPacketType);
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Command: %02X\r\n"), pstCommon->byCommand);
		DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("(-)=== Common Packet ===\r\n\r\n"));
	}			
	//DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Received packet: %02X:%02X:%02X%02X:%02X:%02X\r\n"), aryPacket[0], aryPacket[1], aryPacket[2], aryPacket[3], aryPacket[4], aryPacket[5]);	
	
}

/*********************************************************************
** RxPacket
*********************************************************************/
uint32_t RxPacket(LPBYTE lpbyBuffer, uint8_t uiBufferSize)
{
	//uint8_t aryPacket[MAX_DATA_FIFO_LENGTH];
	uint8_t uiPayloadLength = MAX_DATA_FIFO_LENGTH;	
	int i;
#ifdef FLAG_SHOW_RX_TICK_COUNT	
	uint32_t uiTickCount;
#endif
	uint8_t uiResult = ESL_MSG_UNRELATED_PACKET;

	PSYNC_BEACON pstSyncBeacon = (PSYNC_BEACON)lpbyBuffer;
	//PESL_REPLY_PACKET pstReply = (PESL_REPLY_PACKET) aryPacket;
	
	if (lpbyBuffer == NULL)
		return ERROR_INVALID_HANDLE;

	if (m_uiMachineState == OSMS_PWRBEACON_STATE || m_uiMachineState == OSMS_BEACON_STATE)
		uiPayloadLength = MAX_BEACON_FIFO_LENGTH;
	else
		uiPayloadLength = MAX_DATA_FIFO_LENGTH;
	
	if (uiBufferSize < uiPayloadLength)
		return ERROR_NOT_ENOUGH_MEMORY;


#if 1
	for (i = 0; i < uiPayloadLength; i++)
		lpbyBuffer[i] = RADIO->RX_DATA[i];
#else
	memcpy(lpbyBuffer, RADIO->RX_DATA, uiPayloadLength);
#endif
	
	DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Receive Packet Size: %d\r\n"), uiPayloadLength);
	
	// Packet Decryption
	if ((g_stNvmMappingData.fEncryptionMode == true) && (uiPayloadLength == MAX_BEACON_FIFO_LENGTH)) //Decrypt
	{
		AES128_Decrypt(lpbyBuffer, m_aryAesKey, m_aryDecryptData);
		if (m_aryDecryptData[1] == 0x01)
		{
			memcpy(lpbyBuffer, m_aryDecryptData, MAX_BEACON_FIFO_LENGTH);
		}
		else
		{
			//do nothing, because it is RAW data.
		}
	}
	
	//SHOW_RECEIVE_PACKET(aryPacket, uiPayloadLength);
#ifdef FLAG_SHOW_RX_TICK_COUNT
	uiTickCount = GetTickCount();
	ShowTickCount((uiTickCount - m_uiLeastTime));
	m_uiLeastTime = uiTickCount;
#endif
	
#ifdef FLAG_SHOW_RX_CURRENT_TIME
	ShowCurrentTime();	
#endif
	
	uiResult = RfCommunicationProcedure(pstSyncBeacon->byBeaconType, (LPVOID) lpbyBuffer, NULL);

	return uiResult;
}

//==============================================================================
// Function     : RfProcedure
// Purpose      : Communication Procedure
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : uiMessage
//              :     message identifier
//              : lpParam1
//              :     first message parameter
//              : lpParam2
//              :     second message parameter
// Return       : The return value is the result of the message processing and 
//              : depends on the message.
// Remarks      : 
//==============================================================================
uint32_t RfCommunicationProcedure(uint32_t uiMessage, LPVOID lpParam1, LPVOID lpParam2)
{
	uint32_t uiResult = ERROR_SUCCESS;
	//PSYNC_BEACON pstSyncBeacon = (PSYNC_BEACON) lpParam1;
	PESL_COMMON_HEADER pstCommon = (PESL_COMMON_HEADER) lpParam1;	
#ifdef FLAG_SHOW_RX_TICK_COUNT	
	uint32_t uiTickCount;
#endif	
	
#ifdef FLAG_SHOW_RX_TICK_COUNT
	uiTickCount = GetTickCount();
	ShowTickCount((uiTickCount - m_uiLeastTime));
	m_uiLeastTime = uiTickCount;
#endif
				
#ifdef FLAG_SHOW_RX_CURRENT_TIME
	ShowCurrentTime();	
#endif	

	switch(uiMessage)
	{
		case RF_PT_SYNC_BEACON:
			//uiResult = RfOnBeaconSync(pstSyncBeacon->Action, pstSyncBeacon);
			uiResult = RfOnBeaconSync(pstCommon->byCommand, (PSYNC_BEACON)pstCommon);
			if (uiResult != ERROR_SUCCESS)
			{
				//uiResult = ERROR_INVALID_FUNCTION;
				uiResult = ERROR_FUNCTION_FAILED;
			}
		break;

		case RF_PT_PACKET_HEADER:
		case RF_PT_DATA_PACKET:
		case RF_PT_RX_DATA_RESULT:
		case RF_PT_EPD_DATA:
		case RF_PT_EPD_REFRESH:
		case RF_PT_ENTER_PM1:
		case RF_PT_HOPPING_BEACON:
		case RF_PT_TAG_STATUS:
		case RF_PT_ALWAYS_WAKE_BEACON:
		case RF_PT_TAG_UPDATE_LIST:
		case RF_PT_TAG_STATUS_REPORT:
		case RF_PT_UPDATE_STATUS_REPORT:
			break;			
		case RF_PT_SCANNER_PACKET:
			DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Scanner Packet Reply"));
			uiResult = RfOnScannerPacket(pstCommon->byCommand, (PESL_COMMON_PACKET) pstCommon);
			break;
        // 
        // Process other messages. 
        // 			
		default:
            uiResult = RfDefCommProc(uiMessage, lpParam1, lpParam2); 			
			break;
	}
	return uiResult;	
}

//==============================================================================
// Function     : RfDefCommProc
// Purpose      : Default Communication Procedure
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : uiMessage
//              :     message identifier
//              : lpParam1
//              :     first message parameter
//              : lpParam2
//              :     second message parameter
// Return       : The return value is the result of the message processing and 
//              : depends on the message.
// Remarks      : 
//==============================================================================
uint32_t RfDefCommProc(uint32_t uiMessage, LPVOID lpParam1, LPVOID lpParam2)
{
	uint32_t uiResult = ERROR_SUCCESS;
	//DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Default Communication Procedure: %02X"), uiMessage);	
	
	return uiResult;	
}


uint32_t RfOnBeaconSync(uint32_t uiCommand, PSYNC_BEACON pstSyncBeacon)
{
	uint32_t uiResult = ERROR_SUCCESS;
	if (pstSyncBeacon == NULL)
		return ERROR_INVALID_HANDLE;
	
	if (uiCommand  == RF_AL_SYNC)
	{
		//BeaconSync(m_fBindingMode);
		BeaconCount(false, m_uiBeaconMode);
	}
	
#if 0
	// Show binding host and packet's GID/RID
	DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("BeaconMode = %d, GID: %02X%02X%02X, RID: %02X%02X%02X, BID: %02X%02X%02X\r\n"),
	m_uiBeaconMode,
	pstSyncBeacon->GatewayID[0], pstSyncBeacon->GatewayID[1], pstSyncBeacon->GatewayID[2], 
	pstSyncBeacon->RepeaterID[0], pstSyncBeacon->RepeaterID[1], pstSyncBeacon->RepeaterID[2],
	m_stBindingID.stComboID.byEUSI[0], m_stBindingID.stComboID.byEUSI[1], m_stBindingID.stComboID.byEUSI[2]);
#endif
	
	
	if (m_uiBeaconMode == OSBM_BEACON_BINDING_MODE)
	{
		if (IsGatewayID(m_stBindingID.byEFID))
		{
			if ((m_stBindingID.stComboID.byEUSI[0] != pstSyncBeacon->byGatewayID[0]) ||
				(m_stBindingID.stComboID.byEUSI[1] != pstSyncBeacon->byGatewayID[1]) ||
				(m_stBindingID.stComboID.byEUSI[2] != pstSyncBeacon->byGatewayID[2]))
			{
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Unbind gateway(%02X-%02X-%02X) synchronization signal.\r\n"), 
				pstSyncBeacon->byGatewayID[0], pstSyncBeacon->byGatewayID[1], pstSyncBeacon->byGatewayID[2]);
				return ERROR_SUCCESS;
			}
		}
		else if (IsRepeaterID(m_stBindingID.byEFID))
		{
			if ((m_stBindingID.stComboID.byEUSI[0] != pstSyncBeacon->byRepeaterID[0]) ||
				(m_stBindingID.stComboID.byEUSI[1] != pstSyncBeacon->byRepeaterID[1]) ||
				(m_stBindingID.stComboID.byEUSI[2] != pstSyncBeacon->byRepeaterID[2]))
			{
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Unbind repeater(%02X-%02X-%02X) synchronization signal.\r\n"), 
				pstSyncBeacon->byRepeaterID[0], pstSyncBeacon->byRepeaterID[1], pstSyncBeacon->byRepeaterID[2]);
				return ERROR_SUCCESS;
			}			
		}
		else
		{
			DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Synchronization signal of unknown device. (GID: %02X-%02X-%02X, RID: %02X-%02X-%02X)\r\n"), 
			pstSyncBeacon->byGatewayID[0], pstSyncBeacon->byGatewayID[1], pstSyncBeacon->byGatewayID[2], 
			pstSyncBeacon->byRepeaterID[0], pstSyncBeacon->byRepeaterID[1], pstSyncBeacon->byRepeaterID[2]);
			return ERROR_SUCCESS;
		}
	}
	else if (m_uiBeaconMode == OSBM_BEACON_SYNC_MODE)
	{
		//uint32_t uiTickCount;
		if (RfIsBeaconSyncTimeout())
			RfResetBindingID();	
		
		// First sync
		if ((m_stBindingID.stComboID.byEUSI[0] == 0x00) && (m_stBindingID.stComboID.byEUSI[1] == 0x00)&& (m_stBindingID.stComboID.byEUSI[2] == 0x00))
		{
			ESL_DEVICE_ID stDeviceID;		
			if (((pstSyncBeacon->byGatewayID[0] != 0x00) || (pstSyncBeacon->byGatewayID[1] != 0x00)|| (pstSyncBeacon->byGatewayID[2] != 0x00)) &&
				(pstSyncBeacon->byRepeaterID[0] == 0x00) && (pstSyncBeacon->byRepeaterID[1] == 0x00)&& (pstSyncBeacon->byRepeaterID[2] == 0x00))
			{
				// Bind to the gateway			
				uint8_t byDeviceID[MAX_DEVICE_IDENTIFIER] = {EISI_DEFAULT_ID, EUSI_GATEWAY_ID};
				memcpy(stDeviceID.stComboID.byEISI, byDeviceID, sizeof(stDeviceID.stComboID.byEISI));				
				memcpy(stDeviceID.stComboID.byEUSI, pstSyncBeacon->byGatewayID, sizeof(stDeviceID.stComboID.byEUSI));		
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Bind to the gateway(%02X-%02X-%02X).\r\n"), 
				pstSyncBeacon->byGatewayID[0], pstSyncBeacon->byGatewayID[1], pstSyncBeacon->byGatewayID[2]);	
				SetBindingID(stDeviceID);
				
			}
			else if (((pstSyncBeacon->byGatewayID[0] != 0x00) || (pstSyncBeacon->byGatewayID[1] != 0x00)|| (pstSyncBeacon->byGatewayID[2] != 0x00)) &&
				(pstSyncBeacon->byRepeaterID[0] == 0x00) && (pstSyncBeacon->byRepeaterID[1] == 0x00)&& (pstSyncBeacon->byRepeaterID[2] == 0x00))
			{
				// Bind to the repeater
				uint8_t byDeviceID[MAX_DEVICE_IDENTIFIER] = {EISI_DEFAULT_ID, EUSI_REPEATER_ID};
				memcpy(stDeviceID.stComboID.byEISI, byDeviceID, sizeof(stDeviceID.stComboID.byEISI));
				memcpy(stDeviceID.stComboID.byEUSI, pstSyncBeacon->byRepeaterID, sizeof(stDeviceID.stComboID.byEUSI));
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Bind to the repeater(%02X-%02X).\r\n"), 
				pstSyncBeacon->byRepeaterID[0], pstSyncBeacon->byRepeaterID[1], pstSyncBeacon->byRepeaterID[2]);				
				SetBindingID(stDeviceID);
			}
		}
		
		if (IsGatewayID(m_stBindingID.byEFID))
		{
			if ((m_stBindingID.stComboID.byEUSI[0] != pstSyncBeacon->byGatewayID[0]) ||
				(m_stBindingID.stComboID.byEUSI[1] != pstSyncBeacon->byGatewayID[1]) ||
				(m_stBindingID.stComboID.byEUSI[1] != pstSyncBeacon->byGatewayID[2]))
			{
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Synchronization signal of unsynchronized gateway(%02X-%02X-%02X).\r\n"), 
				pstSyncBeacon->byGatewayID[0], pstSyncBeacon->byGatewayID[1], pstSyncBeacon->byGatewayID[2]);
				return ERROR_SUCCESS;
			}
		}
		else if (IsRepeaterID(m_stBindingID.byEFID))
		{
			if ((m_stBindingID.stComboID.byEUSI[0] != pstSyncBeacon->byRepeaterID[0]) ||
				(m_stBindingID.stComboID.byEUSI[1] != pstSyncBeacon->byRepeaterID[1]) ||
				(m_stBindingID.stComboID.byEUSI[2] != pstSyncBeacon->byRepeaterID[2]))
			{
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Synchronization signal of unsynchronized repeater(%02X-%02X-%02X).\r\n"), 
				pstSyncBeacon->byRepeaterID[0], pstSyncBeacon->byRepeaterID[1], pstSyncBeacon->byRepeaterID[2]);
				return ERROR_SUCCESS;
			}			
		}
		else if ((pstSyncBeacon->byGatewayID[0] == 0x00) && (pstSyncBeacon->byGatewayID[1] == 0x00) && (pstSyncBeacon->byGatewayID[2] == 0x00))
		{
			DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Synchronization signal of illegal unsynchronized device. (GID: %02X-%02X-%02X, RID: %02X-%02X-%02X)\r\n"), 
			pstSyncBeacon->byGatewayID[0], pstSyncBeacon->byGatewayID[1], pstSyncBeacon->byGatewayID[2], 
			pstSyncBeacon->byRepeaterID[0], pstSyncBeacon->byRepeaterID[1], pstSyncBeacon->byRepeaterID[2]);
			return ERROR_SUCCESS;			
		}
		else
		{
			DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Synchronization signal of unknown unsynchronized device. (GID: %02X-%02X-%02X, RID: %02X-%02X-%02X)\r\n"), 
			pstSyncBeacon->byGatewayID[0], pstSyncBeacon->byGatewayID[1], pstSyncBeacon->byGatewayID[2], 
			pstSyncBeacon->byRepeaterID[0], pstSyncBeacon->byRepeaterID[1], pstSyncBeacon->byRepeaterID[2]);
			return ERROR_SUCCESS;
		}
		m_uiBeaconTickCount = GetTickCount();
	}	
	
	switch (uiCommand)
	{
	case RF_AL_SYNC:
		//DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("\r\n*** Free Beacon ***\r\n"));	
		if (RfIsScanningChannel())
		{
			RfScanChannelSucceeded();	
		}
		// Send Pairing Mode
		else if (ScannerIsPairing())
		{
			LedSetStatus(OSLS_TRANSMISSION_STATE);
			memcpy(m_stGatewayID.stComboID.byEUSI, pstSyncBeacon->byGatewayID, sizeof(m_stGatewayID.stComboID.byEUSI));		
			memcpy(m_stRepeaterID.stComboID.byEUSI, pstSyncBeacon->byRepeaterID, sizeof(m_stRepeaterID.stComboID.byEUSI));
			uiResult = ScannerSendPairing();	
			switch(uiResult)
			{	
			case ERROR_SUCCESS:
				// Do not set LED status here, it will be handled in RfOnScannerPacket function
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Sending pairing successful.\r\n"));
				uiResult = ERROR_SUCCESS;	
				break;
			case ERROR_NOT_READY:
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("No pairing data.\r\n"));
				uiResult = ERROR_SUCCESS;	
				break;
			case ERROR_WAIT_TIMEOUT:
				// Retransmit over limit
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Retransmit more than %d limits.\r\n"), MAX_SEND_PAIRING_RETRY_TIME);			
				uiResult = ERROR_WAIT_TIMEOUT;	
				LedSetStatus(OSLS_FAILED_STATE);
				break;				
			default:
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("ScannerSendPairing: 0x%02X\r\n"), uiResult);
				uiResult = ERROR_SUCCESS;	
				break;
			}				
		}
		// Send Registration Mode
		else if (ScannerIsRegistration())
		{
			LedSetStatus(OSLS_TRANSMISSION_STATE);
			memcpy(m_stGatewayID.stComboID.byEUSI, pstSyncBeacon->byGatewayID, sizeof(m_stGatewayID.stComboID.byEUSI));
			memcpy(m_stRepeaterID.stComboID.byEUSI, pstSyncBeacon->byRepeaterID, sizeof(m_stRepeaterID.stComboID.byEUSI));
			uiResult = ScannerSendRegistration();
			switch(uiResult)
			{	
			case ERROR_SUCCESS:
				// Do not set LED status here, it will be handled in RfOnScannerPacket function
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Sending registration successful.\r\n"));
				uiResult = ERROR_SUCCESS;	
				break;
			case ERROR_NOT_READY:
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("No command data.\r\n"));
				uiResult = ERROR_SUCCESS;	
				break;
			case ERROR_WAIT_TIMEOUT:
				// Retransmit over limit
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Retransmit more than %d limits.\r\n"), MAX_SEND_PAIRING_RETRY_TIME);
				uiResult = ERROR_WAIT_TIMEOUT;
				LedSetStatus(OSLS_FAILED_STATE);			
				break;				
			default:
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("ScannerSendCommand: 0x%02X\r\n"), uiResult);
				uiResult = ERROR_SUCCESS;	
				break;
			}			
		}
		// Send Command Mode
		else if (ScannerIsCommand())
		{
			LedSetStatus(OSLS_TRANSMISSION_STATE);
			memcpy(m_stGatewayID.stComboID.byEUSI, pstSyncBeacon->byGatewayID, sizeof(m_stGatewayID.stComboID.byEUSI));
			memcpy(m_stRepeaterID.stComboID.byEUSI, pstSyncBeacon->byRepeaterID, sizeof(m_stRepeaterID.stComboID.byEUSI));
			uiResult = ScannerSendCommand();
			switch(uiResult)
			{	
			case ERROR_SUCCESS:
				// Do not set LED status here, it will be handled in RfOnScannerPacket function
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Sending command successful.\r\n"));
				uiResult = ERROR_SUCCESS;	
				break;
			case ERROR_NOT_READY:
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("No command data.\r\n"));
				uiResult = ERROR_SUCCESS;	
				break;
			case ERROR_WAIT_TIMEOUT:
				// Retransmit over limit
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Retransmit more than %d limits.\r\n"), MAX_SEND_PAIRING_RETRY_TIME);
				uiResult = ERROR_WAIT_TIMEOUT;
				LedSetStatus(OSLS_FAILED_STATE);			
				break;				
			default:
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("ScannerSendCommand: 0x%02X\r\n"), uiResult);
				uiResult = ERROR_SUCCESS;	
				break;
			}						
		}
		// Send Barcode Mode
		else if (ScannerIsBarcode())
		{
			LedSetStatus(OSLS_TRANSMISSION_STATE);
			memcpy(m_stGatewayID.stComboID.byEUSI, pstSyncBeacon->byGatewayID, sizeof(m_stGatewayID.stComboID.byEUSI));
			memcpy(m_stRepeaterID.stComboID.byEUSI, pstSyncBeacon->byRepeaterID, sizeof(m_stRepeaterID.stComboID.byEUSI));
			uiResult = ScannerSendBarcode();
			switch(uiResult)
			{	
			case ERROR_SUCCESS:
				// Do not set LED status here, it will be handled in RfOnScannerPacket function
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Sending barcode successful.\r\n"));
				uiResult = ERROR_SUCCESS;	
				break;
			case ERROR_NOT_READY:
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("No barcode data.\r\n"));
				uiResult = ERROR_SUCCESS;	
				break;
			case ERROR_WAIT_TIMEOUT:
				// Retransmit over limit
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Retransmit more than %d limits.\r\n"), MAX_SEND_PAIRING_RETRY_TIME);
				uiResult = ERROR_WAIT_TIMEOUT;
				LedSetStatus(OSLS_FAILED_STATE);			
				break;				
			default:
				DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("ScannerSendBarcode: 0x%02X\r\n"), uiResult);
				uiResult = ERROR_SUCCESS;	
				break;
			}					
		}
		else
		{
			DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Sync\r\n"));
			//uiResult = ESL_MSG_BEACON_SYNC;
			LedSetStatus(OSLS_ONLINE_STATE);
		}
		break;
	default:
		break;
	}	
	return uiResult;
}

uint32_t RfOnScannerPacket(uint32_t uiCommand, PESL_COMMON_PACKET pstPacket)
{
	uint32_t uiResult = ERROR_SUCCESS;	
	PESL_REPLY_PACKET pstReply = (PESL_REPLY_PACKET) pstPacket;
	DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Scanner Packet Reply"));
	switch (uiCommand)
	{			
		case RF_CMD_SCANNER_CONFIRMED_ACK:	
			LedSetStatus(OSLS_SUCCEEDED_STATE);	
			if ((pstReply->stStatus.nFormat == RF_FMT_EXTENSION_STATUS) && (pstReply->stStatus.nLength == sizeof(ESL_EXTENSION_STATUS)))
			{
				//if (pstReply->stStatus.uiStatus == RF_STATUS_OK)
				//	uiResult = ESL_MSG_PAIRING_ACK;
				//else
				//	uiResult = ESL_MSG_PAIRING_NAK;
				ScannerResetToStandbyMode();
			}
			else
			{
				//uiResult = ESL_MSG_PAIRING_ACK;
				ScannerResetToStandbyMode();
			}	
			uiResult = ERROR_SUCCESS;
			break;
		default:
			DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Scanner Packet: %02X"), pstReply->byCommand);
			break;
	}	
	return uiResult;
}

//==============================================================================
// Function     : getpacket
// Purpose      : 
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : uint8_t uiTimeout
//              :    Receive packet timeout in milliseconds
// Return       : 
// Remarks      : 
//==============================================================================
uint32_t RfGetPacket(LPBYTE lpbyBuffer, uint8_t uiBufferSize, uint8_t uiPayloadLength, uint32_t uiTimeout)
{
	PESL_COMMON_HEADER pstCommon = (PESL_COMMON_HEADER)lpbyBuffer;	
	uint8_t uiResult = ERROR_SUCCESS;
	uint8_t uiWaitTxRxFlag = 0;
	//uint8_t uiEvent =
#if 0
	uint32_t uiLeastTime = GetTickCount();
#endif

	if (lpbyBuffer == NULL)
		return ERROR_INVALID_HANDLE;	
	
	if (m_uiMachineState == OSMS_PWRBEACON_STATE || m_uiMachineState == OSMS_BEACON_STATE)
		uiPayloadLength = MAX_BEACON_FIFO_LENGTH;
	else
		uiPayloadLength = MAX_DATA_FIFO_LENGTH;	
	
	if (uiBufferSize < uiPayloadLength)
		return ERROR_NOT_ENOUGH_MEMORY;	

	//DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Wait Packet Size: %d\r\n"), uiPayloadLength);

	// Receive Packet
	m_fRxTimeout = false;		//Time out flag	
	SetFifoLength(uiPayloadLength);
	StrobeCmd(CMD_RX);
	if (m_uiMachineState == OSMS_PWRBEACON_STATE || m_uiMachineState == OSMS_BEACON_STATE)
		SetTimer(TIMER_ONESHOT, EVENT_TIMER_BEACON_SYNC, uiTimeout, NULL);		//Start receive timeout timer
	else
		SetTimer(TIMER_ONESHOT, EVENT_TIMER_DATA_PACKET, uiTimeout, NULL);		//Start receive timeout timer
	
	while (1) 
	{
#if 0		
		// Get Tickcount
		uint32_t uiTickCount = GetTickCount();
		if ((uiTickCount - uiLeastTime) >= 256)
		{
			ShowTickCount((uiTickCount - uiLeastTime));
			DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("TickCount Value: %d, m_fRxTimeout = %d, WTRFlag = %d\r\n"), (uiTickCount - uiLeastTime), m_fRxTimeout, WTRFlag);		
			uiLeastTime = uiTickCount;
		}
#endif
		
		if (uiWaitTxRxFlag == 0) 
		{		//Wait CMD_RX action, WTR = 1.
			if ((RADIO->STATUS & (1 << 7))) uiWaitTxRxFlag = 1;
		}
		else if (!(RADIO->STATUS & (1 << 7)))
		//else
		{
			//if (!(RADIO->STATUS & (1 << 7)) || m_fRxTimeout) 
			//{		//WTR = 0.
				Timer_StopTimer(TIMER0);
				StrobeCmd(CMD_STBY);				//Back to Standby
				
				if (!m_fRxTimeout)
				{	
					//Received packet
#if 1
					int i;
					for (i = 0; i < uiPayloadLength; i++)
						lpbyBuffer[i] = RADIO->RX_DATA[i];
					#if 0
					DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Packet = %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X  %02X-%02X-%02X-%02X (L=%d).\r\n"), 
					lpbyBuffer[0], lpbyBuffer[1], lpbyBuffer[2], 
					lpbyBuffer[3], lpbyBuffer[4], lpbyBuffer[5], 
					lpbyBuffer[6], lpbyBuffer[7], lpbyBuffer[8],
					lpbyBuffer[9], lpbyBuffer[10], lpbyBuffer[11], lpbyBuffer[12], uiPayloadLength);					
					#endif
#else
					// *************************************
					// *** This way will lose the packet ***
					// *************************************
					memcpy(lpbyBuffer, (const void *)RADIO->RX_DATA, uiPayloadLength);
#endif
					
					//DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("Receive Packet Size: %d\r\n"), uiPayloadLength);
					// Packet Decryption
					if ((g_stNvmMappingData.fEncryptionMode == true) && (uiPayloadLength == MAX_BEACON_FIFO_LENGTH)) //Decrypt
					{
						AES128_Decrypt(lpbyBuffer, m_aryAesKey, m_aryDecryptData);
						if (m_aryDecryptData[1] == 0x01)
						{
							memcpy(lpbyBuffer, m_aryDecryptData, MAX_BEACON_FIFO_LENGTH);
						}
						else
						{
							//do nothing, because it is RAW data.
						}
					}

					// Show Packet Information
					//SHOW_RECEIVE_PACKET(lpbyBuffer, uiPayloadLength);
					uiResult = RfCommunicationProcedure(pstCommon->byPacketType, (LPVOID) lpbyBuffer, NULL);			
				}
				//else
				//{	
				//	// Time out
				//	return ERROR_WAIT_TIMEOUT;
				//}

			//}
			//return uiResult;
			break;
		}
		else if (m_fRxTimeout)
		{
			// Time out
			return ERROR_WAIT_TIMEOUT;		
		}
	}
	return uiResult;
}


bool RfSendBarcodePairing(BARCODE_PAIR *pstBarcode)
{
	uint8_t aryBuf[MAX_DATA_FIFO_LENGTH];
	int i;
	//PESL_PACKET pstScannerPacket = (PESL_PACKET)aryBuf;
	PESL_PAIRING_PACKET pstScannerPacket = (PESL_PAIRING_PACKET)aryBuf;
	//bool fPairing = true;

	if (pstBarcode == NULL)
		return false;
	
	memset(aryBuf, 0, sizeof(aryBuf));
	pstScannerPacket->bySequenceNumber = m_uiTxSeqNo++;
	pstScannerPacket->byPacketType = RF_PT_SCANNER_PACKET;
	pstScannerPacket->byCommand = pstBarcode->uiCommand;  // RF_CMD_SCANNER_PAIRING, RF_CMD_SCANNER_UNPAIRING
	memcpy(pstScannerPacket->byGatewayID, m_stGatewayID.stComboID.byEUSI, sizeof(pstScannerPacket->byGatewayID));
	memcpy(pstScannerPacket->byRepeaterID, m_stRepeaterID.stComboID.byEUSI, sizeof(pstScannerPacket->byRepeaterID));
	memcpy(pstScannerPacket->byDeviceID, g_stNvmMappingData.stDeviceID.stComboID.byEUSI, sizeof(pstScannerPacket->byDeviceID));
	pstScannerPacket->stPairing.nLength = sizeof(ESL_EXTENSION_PAIRING);  
	if (pstScannerPacket->byCommand == RF_CMD_SCANNER_PAIRING)
	{
		memcpy(pstScannerPacket->stPairing.byProduct, pstBarcode->byProduct, sizeof(pstScannerPacket->stPairing.byProduct));
	}
	memcpy(pstScannerPacket->stPairing.byDevice, pstBarcode->byDevice, sizeof(pstScannerPacket->stPairing.byDevice));
	
	// Binding Host ID
	pstScannerPacket->stBinding.nLength = sizeof(ESL_EXTENSION_BINDING);
	pstScannerPacket->stBinding.nFormat = RF_FMT_EXTENSION_BINDING;
	pstScannerPacket->stBinding.stDeviceID = m_stBindingID;
	
	// RSSI Information
	pstScannerPacket->stRssi.nLength = sizeof(ESL_EXTENSION_RSSI);
	pstScannerPacket->stRssi.nFormat = RF_FMT_EXTENSION_RSSI;
	pstScannerPacket->stRssi.fEnableAGC = RfIsRssiEnableAGC();
	pstScannerPacket->stRssi.uiRssiCode = RfGetRssiCode();
	//pstScannerPacket->stRssi.nInputPower = RfConvertRssiInputPower(pstScannerPacket->stRssi.uiRssiCode);
	

	SetFifoLength(MAX_DATA_FIFO_LENGTH);	
	// 64 Bytes
	for (i = 0; i < MAX_DATA_FIFO_LENGTH; i++) 
	{
		RADIO->TX_DATA[i] = aryBuf[i]; //
	}

	// Random delay time
	if (pstBarcode->uiCount > 0)
	{
		Delay1us(RfRandomDelay(MAX_RANDOM_TIME));
	}
	else
	{
		pstBarcode->uiCount++; 
	}

	
	StrobeCmd(CMD_TX); //entry tx & transmit
	Delay10us(1);
	while ((RADIO->STATUS & (1 << 7))); 	//WTR
	
	
#if 0
	// Swith to receive ACK mode
	SetFifoLength(MAX_DATA_FIFO_LENGTH);
	StrobeCmd(CMD_RX);

	DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("***RfSendBarcodePairing***\r\n"));
#endif
	
	return true;
}

bool RfSendBarcodeRegistration(BARCODE_PAIR *pstBarcode)
{
	uint8_t aryBuf[MAX_DATA_FIFO_LENGTH];
	int i;
	//int result = 0;
	PESL_REGISTRATION_PACKET pstScannerPacket = (PESL_REGISTRATION_PACKET)aryBuf;

	if (pstBarcode == NULL)
		return false;
	memset(aryBuf, 0, sizeof(aryBuf));
	pstScannerPacket->bySequenceNumber = m_uiTxSeqNo++;
	pstScannerPacket->byPacketType = RF_PT_SCANNER_PACKET; 
	pstScannerPacket->byCommand = pstBarcode->uiCommand;
	memcpy(pstScannerPacket->byGatewayID, m_stGatewayID.stComboID.byEUSI, sizeof(pstScannerPacket->byGatewayID));
	memcpy(pstScannerPacket->byRepeaterID, m_stRepeaterID.stComboID.byEUSI, sizeof(pstScannerPacket->byRepeaterID));
	memcpy(pstScannerPacket->byDeviceID, g_stNvmMappingData.stDeviceID.stComboID.byEUSI, sizeof(pstScannerPacket->byDeviceID));
	
	// Registration Information
	pstScannerPacket->stRegistration.nLength = sizeof(ESL_EXTENSION_REGISTRATION);
	pstScannerPacket->stRegistration.nFormat = RF_FMT_EXTENSION_REGISTRATION;	
	memcpy(pstScannerPacket->stRegistration.byDevice, pstBarcode->byDevice, sizeof(pstScannerPacket->stRegistration.byDevice));


	SetFifoLength(MAX_DATA_FIFO_LENGTH);
	// 64 Bytes
	for (i = 0; i < MAX_DATA_FIFO_LENGTH; i++) 
	{
		RADIO->TX_DATA[i] = aryBuf[i]; //
	}
	
	// Random delay time
	if (pstBarcode->uiCount > 0)
	{
		Delay1us(RfRandomDelay(MAX_RANDOM_TIME));
	}	
	else
	{
		pstBarcode->uiCount++; 
	}
	
	StrobeCmd(CMD_TX); //entry tx & transmit
	Delay10us(1);
	while ((RADIO->STATUS & (1 << 7))); 	//WTR
#if 1	
	DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("***RfSendBarcodeRegistration***\r\n"));
#endif
	
	return true;
}



bool RfSendBarcodeCommand(BARCODE_PAIR *pstBarcode)
{
	uint8_t aryBuf[MAX_DATA_FIFO_LENGTH];
	int i;
	//int result = 0;
	PESL_FACTORY_PACKET pstScannerPacket = (PESL_FACTORY_PACKET)aryBuf;

	if (pstBarcode == NULL)
		return false;
	memset(aryBuf, 0, sizeof(aryBuf));
	pstScannerPacket->bySequenceNumber = m_uiTxSeqNo++;
	pstScannerPacket->byPacketType = RF_PT_SCANNER_PACKET; 
	pstScannerPacket->byCommand = pstBarcode->uiCommand;
	memcpy(pstScannerPacket->byGatewayID, m_stGatewayID.stComboID.byEUSI, sizeof(pstScannerPacket->byGatewayID));
	memcpy(pstScannerPacket->byRepeaterID, m_stRepeaterID.stComboID.byEUSI, sizeof(pstScannerPacket->byRepeaterID));
	memcpy(pstScannerPacket->byDeviceID, g_stNvmMappingData.stDeviceID.stComboID.byEUSI, sizeof(pstScannerPacket->byDeviceID));
	
	pstScannerPacket->stFactory.nLength = sizeof(ESL_EXTENSION_FACTORY);  
	pstScannerPacket->stFactory.nFormat = RF_FMT_EXTENSION_COMMAND;
	memcpy(pstScannerPacket->stFactory.byCommand, pstBarcode->byCommand, sizeof(pstScannerPacket->stFactory.byCommand));

	SetFifoLength(MAX_DATA_FIFO_LENGTH);
	// 64 Bytes
	for (i = 0; i < MAX_DATA_FIFO_LENGTH; i++) 
	{
		RADIO->TX_DATA[i] = aryBuf[i]; //
	}
	
	// Random delay time
	if (pstBarcode->uiCount > 0)
	{
		Delay1us(RfRandomDelay(MAX_RANDOM_TIME));
	}	
	else
	{
		pstBarcode->uiCount++; 
	}
	
	StrobeCmd(CMD_TX); //entry tx & transmit
	Delay10us(1);
	while ((RADIO->STATUS & (1 << 7))); 	//WTR
#if 1	
	DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("***RfSendBarcodeCommand***\r\n"));
#endif
	
	return true;
}

bool RfSendBarcodeInformation(BARCODE_PAIR *pstBarcode)
{
	uint8_t aryBuf[MAX_DATA_FIFO_LENGTH];
	int i;
	//int result = 0;
	PESL_BARCODE_PACKET pstScannerPacket = (PESL_BARCODE_PACKET)aryBuf;

	if (pstBarcode == NULL)
		return false;
	memset(aryBuf, 0, sizeof(aryBuf));
	pstScannerPacket->bySequenceNumber = m_uiTxSeqNo++;
	pstScannerPacket->byPacketType = RF_PT_SCANNER_PACKET; 
	pstScannerPacket->byCommand = pstBarcode->uiCommand;
	memcpy(pstScannerPacket->byGatewayID, m_stGatewayID.stComboID.byEUSI, sizeof(pstScannerPacket->byGatewayID));
	memcpy(pstScannerPacket->byRepeaterID, m_stRepeaterID.stComboID.byEUSI, sizeof(pstScannerPacket->byRepeaterID));
	memcpy(pstScannerPacket->byDeviceID, g_stNvmMappingData.stDeviceID.stComboID.byEUSI, sizeof(pstScannerPacket->byDeviceID));
	
	
	pstScannerPacket->stBarcode.nLength = sizeof(ESL_EXTENSION_BARCODE);  
	pstScannerPacket->stBarcode.nFormat = RF_FMT_EXTENSION_BAROCDE;
	memcpy(pstScannerPacket->stBarcode.byProduct, pstBarcode->byProduct, sizeof(pstScannerPacket->stBarcode.byProduct));

	SetFifoLength(MAX_DATA_FIFO_LENGTH);
	// 64 Bytes
	for (i = 0; i < MAX_DATA_FIFO_LENGTH; i++) 
	{
		RADIO->TX_DATA[i] = aryBuf[i]; //
	}
	
	// Random delay time
	if (pstBarcode->uiCount > 0)
	{
		Delay1us(RfRandomDelay(MAX_RANDOM_TIME));
	}	
	else
	{
		pstBarcode->uiCount++; 
	}
	
	StrobeCmd(CMD_TX); //entry tx & transmit
	Delay10us(1);
	while ((RADIO->STATUS & (1 << 7))); 	//WTR
#if 1	
	DEBUG_MESSAGE(FLAG_MF_COMMUNICATION, _T("***RfSendBarcodeInformation***\r\n"));
#endif
	
	return true;
}
