/**
 ******************************************************************************
 * @file    		Led_Disp_Task.h 
 * @author  		Kenneth Chang
 * @version   	V1.0.0
 * @date      	28-Augst-2017
 * @brief   		Header for Led_Disp_Task.c module
 ******************************************************************************
*/
#ifndef _LED_DISP_TASK_H_
#define _LED_DISP_TASK_H_

//*****************************************************************************
// Include section
//*****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

//*****************************************************************************
// Global variable declarations 
//*****************************************************************************


//*****************************************************************************
// Function declarations
//*****************************************************************************
void LED_Disp_Task(void *param);
void LED_Disp_Task_Init(void);


//*****************************************************************************
// External variable declarations
//*****************************************************************************
// timer
extern uint16_t mss_timer_tick_cnt;

//*****************************************************************************
// External function declarations
//*****************************************************************************

//*****************************************************************************
// Macros (defines) and data types 
//*****************************************************************************

#endif /* _LED_DISP_TASK_H_*/
