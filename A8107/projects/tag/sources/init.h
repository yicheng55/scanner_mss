/**********************************************
 * init.h
 *
 **********************************************/
#ifndef _INIT_H_
#define _INIT_H_

void SetOneshotTimer0(TIMER_Type *timer, uint32_t msec);
void SetOneshotTimer1(TIMER_Type *timer, uint32_t msec);
void startTimer(TIMER_Type *timer, uint32_t msec);
void RTC_SETTING(void);
#endif
