#include <stdio.h>
#include <string.h>
#include "A8107M.h"
#include "M8107RTC.h"

int PastHour = 0;
int CurrentHour = 0;
int CurrentYear = 0;
int CurrentDay = 0;

void DateAndHour(int Date, int Hour)
{
    PastHour = Hour;
    CurrentDay = Date;
}

void getCurrentDay(void)
{
    CurrentHour = ((RTC->COUNT & RTC_COUNT_HOUR_Msk) >> RTC_COUNT_HOUR_Pos);

    if (CurrentHour < PastHour)
    {
        CurrentDay += 1;
        PastHour = CurrentHour;
    }
    else
    {
        PastHour = CurrentHour;
    }
    //printf("CurrentHour = %d\r\n", CurrentHour);
    //printf("PastHour = %d\r\n", PastHour);

    if (CurrentDay > 365)
    {
        CurrentYear += 1;
        CurrentDay -= 366;
    }
}

void printTime(void)
{
    getCurrentDay();
    printf("[%dd:%2dh:%2dm:%2ds.%02d0]\r\n",
           CurrentDay,
           (int)((RTC->COUNT & RTC_COUNT_HOUR_Msk) >> RTC_COUNT_HOUR_Pos),
           (int)((RTC->COUNT & RTC_COUNT_MIN_Msk) >> RTC_COUNT_MIN_Pos),
           (int)((RTC->COUNT & RTC_COUNT_SEC_Msk) >> RTC_COUNT_SEC_Pos),
           (int)((RTC->COUNT & RTC_COUNT_TMS_Msk) >> RTC_COUNT_TMS_Pos));
}

uint32_t GetStandbyTimeInSecond(void)
{
    uint32_t Second = 0;

    getCurrentDay();

    Second = (CurrentYear * YEAR_IN_SEC) + (CurrentDay * DAY_IN_SEC) +
             (GET_HOUR(RTC) * HOUR_IN_SEC) + (GET_MINUTE(RTC) * MINUTE_IN_SEC) +
             GET_SECOND(RTC);

    //return htonl(Second);
    return Second;
}
