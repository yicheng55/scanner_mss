/******************************************
 * main.h
 ******************************************/
#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <string.h>
#include "A8107M.h"
#include "uart.h"
#include "uart_stdout.h"
#include "A8107M_RFLIB.h"
#include "slptimer.h"
#include "Utility.h"
#include "M8107.h"
#include "RFpacket.h"
#include "isr.h"
#include "Board.h"
//#include "dualtimer.h"
#include "timer.h"
#include "RfCommunication.h"
#include "EslConfig.h"



/* Machine State */
#define PWRBEACON_STATE				1	
#define BEACON_STATE					2
#define HEADER_STATE					3
#define DATA_STATE						4
#define REPLYRESULT_STATE			5
#define EPDDATA_STATE					6
#define EPDREFRESH_STATE			7
#define ENTERPM1_STATE				8
#define HOPPM1_STATE					9
#define HOPBEACON_STATE				10
#define FAIL_STATE				11
#define TAG_LIST_STATE				12


void SetFIFOLvl(uint32_t fifolen);
void setSynTimer(uint32_t sec, uint8_t hitflag);

	
#endif		//_MAIN_H_
