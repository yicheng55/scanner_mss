/**********************************************
 * isr.h
 *
 **********************************************/
#ifndef _ISR_H_
#define _ISR_H_

#include <stdint.h>

extern volatile uint8_t Oneshoutflag;

void SLPTIMER1_Handler(void);
void DUALTIMER_Handler(void);

#endif /* _ISR_H_ */
