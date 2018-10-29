/******************************************
 * main.h
 ******************************************/
#ifndef _MAIN_H_
#define _MAIN_H_

#include "includes.h"
#include "A8107M.h"
#include "uart.h"
#include "uart_stdout.h"
#include "A8107M_RFLIB.h"
//#include "slptimer.h"
#include "Utility.h"
#include "M8107.h"
#include "isr.h"
#include "Board.h"
//#include "dualtimer.h"
#include "timer.h"
#include "RfCommunication.h"
//#include "EslConfig.h"
//#include "tagdefine.h"

/* gState Machine State */
#define PWRBEACON_STATE                                 1
#define BEACON_STATE                                    2
#define HEADER_STATE                                    3
#define DATA_STATE                                      4
#define REPLYRESULT_STATE                               5
#define EPDDATA_STATE                                   6
#define EPDREFRESH_STATE                                7
#define ENTERPM1_STATE                                  8
#define HOPPM1_STATE                                    9
#define HOPBEACON_STATE                                 10
#define FAIL_STATE                                      11
#define TAG_LIST_STATE                                  12

/* Tag Message */
#define TAG_MSG_PACKET_LOST                             0x01
#define TAG_MSG_RX_TIMEOUT                              0x02
#define TAG_MSG_RX_COMPLETED                            0x03
#define TAG_MSG_HIT_ID                                  0x04
#define TAG_MSG_NO_HIT_ID                               0x05
#define TAG_MSG_HOPPING                                 0x06
#define TAG_MSG_REPORT_STATUS_OK                        0x07
#define TAG_MSG_STATE_NOT_RIGHT                         0x08
#define TAG_MSG_TO_RCV_TAG_LIST                         0x09
#define TAG_MSG_ONLY_SYNC                               0x0A
#define TAG_MSG_SINGAL_PACKET_OK                        0x0B
#define TAG_MSG_RCV_UNBOUND_MSG_OK                      0x0C
#define TAG_MSG_CHANGE_BEACON_SRC_OK                    0x0D
#define TAG_MSG_GATEWAY_ID_NOT_MATCH                    0x0E
#define TAG_MSG_REPEATER_ID_NOT_MATCH                   0x0F
#define TAG_MSG_OTA_PACKET_REPEATED                     0x12

#define Attribution_OTA_Repeater                        0x01
#define Attribution_OTA_Scanner                         0x02
#define Attribution_OTA_Tag                             0x03

#if 0
/* Data Rate */
#define DATARATE1M                                      0
#define DATARATE500K                                    1
#define DATARATE250K                                    2

/* recv Packet Type */
#define SYNCBEACON                                      0x01
#define HEADERPACKET                                    0x02
#define DATAPACKET                                      0x03
#define RXDATARESULT                                    0x04
#define EPDDATA                                         0x05
#define EPDREFRESH                                      0x06
#define ENTERPM1                                        0x07
#define HOPPINGBEACON                                   0x08

#define TAGSTATUS                                       0x09
#define ALWAYSWAKEBEACON                                0x0A
#define TAGSTATUSREPORT                                 0x0B
#define UPDATESTATUSREPORT                              0x0C
#define TAGUPDATELIST                                   0x20

/* Action List For Raspberry Gateway*/
#define SYNC                                            0x00
#define UPDATE_TAG                                      0x01
#define GET_TAG_STATUS                                  0x02
#define TAG_ALWAYS_WAKE_UP                              0x03
#define GET_REPEATER_STATUS                             0x04
#define HOP_TO_TAG                                      0x05
#define HOP_TO_REPEATER                                 0x06
#define ALL_WUTIME                                      0x07
#define UPDATEREPEATERTAG                               0x08 // Add for Repeater

extern uint8_t gState;
extern uint8_t gTagDonotSleep;
//extern uint32_t gRSSIvalue;;

extern void board_init(void);
extern void printTime(void);
extern void WriteLUT(void);
extern void ePaper_Init_03(void);

extern uint16_t UpdateCount;
extern uint16_t RebootCount;
//extern uint16_t UpdateFailCount;
extern uint8_t TAG_UPDATED;
/* Spenser added */
//extern uint8_t mindex;
#endif //0

/* Jerry Add */
extern uint8_t gDatarate;
extern const uint8_t CHTable[];

#endif //_MAIN_H_
