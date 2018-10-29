#ifndef __M8107_RTC_H__
#define __M8107_RTC_H__

#include <stdint.h>

#define GET_HOUR(rtc)                               (int)((rtc->COUNT & RTC_COUNT_HOUR_Msk)>>RTC_COUNT_HOUR_Pos)
#define GET_MINUTE(rtc)                             (int)((rtc->COUNT & RTC_COUNT_MIN_Msk)>>RTC_COUNT_MIN_Pos)
#define GET_SECOND(rtc)                             (int)((rtc->COUNT & RTC_COUNT_SEC_Msk)>>RTC_COUNT_SEC_Pos)
#define HTONS(n)                                    (uint16_t)((((uint16_t) (n)) << 8) | (((uint16_t) (n)) >> 8))
#define HTONL(n)                                    (uint32_t)((n[0] << 24) | (n[1] << 16) | (n[2] << 8) | (n[3]));

#define YEAR_IN_SEC                                 31556926
#define DAY_IN_SEC                                  86400
#define HOUR_IN_SEC                                 3600
#define MINUTE_IN_SEC                               60

void DateAndHour(int Date, int Hour);
void getCurrentDay(void);
void printTime(void);
uint32_t GetStandbyTimeInSecond(void);

#endif /* M8107_RTC */
