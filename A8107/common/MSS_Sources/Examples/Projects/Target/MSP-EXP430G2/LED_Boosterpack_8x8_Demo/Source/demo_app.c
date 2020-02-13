/******************************************************************************
* Copyright (c) 2012-2013, Leo Hendrawan
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*    * Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimer in the
*      documentation and/or other materials provided with the distribution.
*    * Neither the name of the MSS PROJECT nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE MSS PROJECT OR ITS
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

/**************************************************************************//**
* 
* @file     demo_app.c
* 
* @brief    MSP430 Launchpad LED Boosterpack MSS demo source file
*
* @version  0.2.1
* 
* @author   Leo Hendrawan
*
* @remark   This is a demo of MSS running on MSP430G2452 target device on the
*           MSP-EXP430G2 Launchpad with MSP-LED8x8-BOOSTERPACK from Olimex
*           (https://www.olimex.com/dev/msp-led8x8.html).
*           This demo shows how MSS can work with all features enabled.
*           To play with the demo, press S2 button to switch between running
*           text demo and animation demo, and give a strong input on the
*           microphone to start/stop the buzzer playing a song.
* 
******************************************************************************/

//*****************************************************************************
// Include section
//*****************************************************************************

#include "mss.h"
#include "demo_cfg.h"
#include "demo_app.h"
#include "buzzer.h"
#include "led_matrix.h"

//*****************************************************************************
// Global variables 
//*****************************************************************************


//*****************************************************************************
// Macros (defines), data types, static variables
//*****************************************************************************

// led matrix scan period in ms
#define LED_MATRIX_TIMER_SCAN_PERIOD  (2)

// event for animation display task
#define ANIM_DISP_TSK_TOGGLE_DISP_EVENT  (0x0001)

// event for push button task
#define LED_PB_TSK_PUSH_BUTTON_EVENT     (0x0001)

// event for buzzer task
#define BUZZER_TSK_MIC_INPUT_EVENT       (0x0001)

// task ID - see MSS_TASK_LIST
#define LED_MATRIX_SCAN_TASK_ID       (0)
#define MOV_TEXT_DISP_TASK_ID         (1)
#define ANIMATION_DISP_TASK_ID        (2)
#define BUZZER_TASK_ID                (3)
#define PB_TASK_ID                    (4)

#define APP_ASSERT(x)                 if(!(x)) do{__disable_interrupt(); \
	                                              while(1);}while(0)

// application flags
static uint16_t app_flags = 0;

#define APP_FLAG_ANIM_ACTIVE    (0x0001)
#define APP_FLAG_PB_PUSHED      (0x0002)
#define APP_FLAG_BUZZER_ON      (0x0004)

// display index
static uint16_t last_disp_idx = 0;

// buzzer index
static uint16_t last_buzzer_idx = 0;

// data structure for animation display
typedef struct {
  uint8_t disp_data[8];
  uint16_t delay;
} anim_disp_data_t;

// data structure for buzzer play
typedef struct {
  uint16_t buzzer_freq_cnt;
  uint16_t delay;
} buzzer_tone_data_t;

// average value of ADC mic input
static uint16_t adc_avg;

// message queue and memory for passing led display context
static mss_mque_t led_matrix_mque;
static mss_mem_t led_matrix_mem;

// semaphore for access to the led display
static mss_sema_t led_disp_sema;

// timers
static mss_timer_t pb_timer;
static mss_timer_t led_matrix_scan_tmr;
static mss_timer_t mov_text_disp_tmr;
static mss_timer_t animation_disp_tmr;
static mss_timer_t buzzer_tmr;

//*****************************************************************************
// Internal function declarations
//*****************************************************************************

static void init_demo(void);
static void measure_adc_mic_avg(void);
static uint16_t do_moving_text_display(void);
static uint16_t do_animation_display(void);
static uint16_t play_buzzer(void);

//*****************************************************************************
// External functions
//*****************************************************************************

/**************************************************************************//**
*
* main
*
* @brief      main function
*
* @param      -
*
* @return     -
*
******************************************************************************/
void main(void)
{
  // initialize mss
  mss_init();

  // initialize demo application tasks
  init_demo();

  // run mss - shall never return
  mss_run();
}

/**************************************************************************//**
* 
* pb_task
* 
* @brief      task handling push buttons on MSP430 Launchpad board
*
* @param[in]  param     task parameter input (unused)
* 
* @return     -
* 
******************************************************************************/
void pb_task(void *param)
{
  static mss_task_ctx_t pb_task_ctx = MSS_TASK_CTX_STATE_INIT_VAL;
  mss_event_t pb_event;

  MSS_BEGIN(pb_task_ctx);

  // set P1.3 to generate interrupt on high-low transition
  P1IES |= BIT3;
  P1IE |= BIT3;

  while(1)
  {
    // block and wait for event from PORT1 ISR
    MSS_EVENT_WAIT(pb_event, pb_task_ctx);

    if(!(app_flags & APP_FLAG_PB_PUSHED))
    {
      if(pb_event & LED_PB_TSK_PUSH_BUTTON_EVENT)
      {
        // set debouncing timer - 20 ms
        mss_timer_start(pb_timer, MSS_TIMER_MS_TO_TICKS(20));
        MSS_TIMER_WAIT_EXPIRES(pb_timer, pb_task_ctx);
  
        // recheck IO input state
        if(!(P1IN & BIT3))
        {
          // pb is really pushed
          app_flags |= APP_FLAG_PB_PUSHED;
  
          // send event to toggle display
          mss_event_set(ANIMATION_DISP_TASK_ID, ANIM_DISP_TSK_TOGGLE_DISP_EVENT);
          
          // toggle PIES bit to detect PB release
          P1IES &= ~BIT3;
        }
      }
    }
    else
    {
      if(pb_event & LED_PB_TSK_PUSH_BUTTON_EVENT)
      {
        // event indicating push button is released
        app_flags &= ~APP_FLAG_PB_PUSHED;

        // set PIES bit to detect next PB push
        P1IES |= BIT3;
      }
    }

    // toggle PIES bit and reenable interrupt to detect PB release
    P1IFG &= ~BIT3;
    P1IE |= BIT3;
    
    MSS_RETURN(pb_task_ctx);
  }
  
  MSS_FINISH();
}

/**************************************************************************//**
*
* led_matrix_scan_task
*
* @brief      task handling LED Matrix scan on LED matrix boosterpack
*
* @param[in]  param     task parameter input (unused)
*
* @return     -
*
******************************************************************************/
void led_matrix_scan_task(void *param)
{
  static mss_task_ctx_t led_matrix_scan_task_ctx = MSS_TASK_CTX_STATE_INIT_VAL;
  static uint8_t row = 0, scan_data[8];
  mss_mque_msg_t *msg;

  MSS_BEGIN(led_matrix_scan_task_ctx);

  // trigger timers
  APP_ASSERT(mss_timer_periodic_start(led_matrix_scan_tmr,
		     MSS_TIMER_MS_TO_TICKS(LED_MATRIX_TIMER_SCAN_PERIOD),
		     MSS_TIMER_MS_TO_TICKS(LED_MATRIX_TIMER_SCAN_PERIOD)) == true);

  // setup initial display
  memset(scan_data, 0x00, 8);

  while(1)
  {
    if(row == 0)
    {
      if((msg = mss_mque_read(led_matrix_mque)) != NULL)
      {
        // get new data to be displayed
        memcpy(scan_data, msg->data, 8);

        // return memory
        mss_mem_free(led_matrix_mem, msg);
      }
    }

    if(mss_timer_check_expired(led_matrix_scan_tmr) == true)
    {
      // scan rows
      led_matrix_send_row(row, scan_data[row]);

      // update row
      ++row;
      row &= 0x07;
    }

    MSS_RETURN(led_matrix_scan_task_ctx);
  }

  MSS_FINISH();
}

/**************************************************************************//**
*
* mov_text_disp_task
*
* @brief      task handling LED Matrix moving text display on boosterpack
*
* @param[in]  param     task parameter input (unused)
*
* @return     -
*
******************************************************************************/
void mov_text_disp_task(void *param)
{
  static mss_task_ctx_t mov_text_disp_task_ctx = MSS_TASK_CTX_STATE_INIT_VAL;
  uint16_t delay;

  MSS_BEGIN(mov_text_disp_task_ctx);

  // trigger timer
  APP_ASSERT(mss_timer_start(mov_text_disp_tmr,
		     MSS_TIMER_MS_TO_TICKS(100)) == true);

  while(1)
  {
    if(mss_timer_check_expired(mov_text_disp_tmr) == true)
    {
      // wait for semaphore to be available
      MSS_SEMA_WAIT(led_disp_sema, mov_text_disp_task_ctx);

      // display moving text
      delay = do_moving_text_display();

      // release semaphore
      mss_sema_post(led_disp_sema);

      // re-trigger timer
      APP_ASSERT(mss_timer_start(mov_text_disp_tmr,
                 MSS_TIMER_MS_TO_TICKS(delay)) == true);
    }

    MSS_RETURN(mov_text_disp_task_ctx);
  }

  MSS_FINISH();
}

/**************************************************************************//**
*
* animation_disp_task
*
* @brief      task handling LED Matrix animation display on boosterpack
*
* @param[in]  param     task parameter input (unused)
*
* @return     -
*
******************************************************************************/
void animation_disp_task(void *param)
{
  static mss_task_ctx_t animation_disp_task_ctx = MSS_TASK_CTX_STATE_INIT_VAL;
  mss_event_t disp_event;
  uint16_t delay;

  MSS_BEGIN(animation_disp_task_ctx);

  while(1)
  {
    // check for event
    disp_event = mss_event_get();
    if(disp_event & ANIM_DISP_TSK_TOGGLE_DISP_EVENT)
    {
      // reset last display index
      last_disp_idx = 0;
  
      // toggle active flag
      if(!(app_flags & APP_FLAG_ANIM_ACTIVE))
      {
        app_flags |= APP_FLAG_ANIM_ACTIVE;

        // wait for semaphore to be available
        MSS_SEMA_WAIT(led_disp_sema, animation_disp_task_ctx);

        // trigger timer
        APP_ASSERT(mss_timer_start(animation_disp_tmr,
               MSS_TIMER_MS_TO_TICKS(100)) == true);
      }
      else
      {
        app_flags &= ~APP_FLAG_ANIM_ACTIVE;

        // release semaphore
        mss_sema_post(led_disp_sema);
      }
	}

    if(app_flags & APP_FLAG_ANIM_ACTIVE)
    {
      if(mss_timer_check_expired(animation_disp_tmr) == true)
      {
        // display animation
        delay = do_animation_display();
  
        // re-trigger timer
        APP_ASSERT(mss_timer_start(animation_disp_tmr,
               MSS_TIMER_MS_TO_TICKS(delay)) == true);
      }
    }
  
    MSS_RETURN(animation_disp_task_ctx);
  }

  MSS_FINISH();
}

/**************************************************************************//**
*
* buzzer_task
*
* @brief      handling buzzer on LED Matrix boosterpack
*
* @param[in]  param     task parameter input (unused)
*
* @return     -
*
******************************************************************************/
void buzzer_task(void *param)
{
  static mss_task_ctx_t buzzer_task_ctx = MSS_TASK_CTX_STATE_INIT_VAL;
  mss_event_t buzzer_event;
  uint16_t buzzer_delay;

  MSS_BEGIN(buzzer_task_ctx);

  // start continuous conversion
  ADC10CTL0 = REFON + MSC + SREF_1 + ADC10SHT_2 + ADC10ON + ADC10IE;
  ADC10CTL1 = INCH_0 + CONSEQ_2 + ADC10DIV_7 + ADC10SSEL_3;
  ADC10AE0 |= BIT0;  // P1.0 as mic input

  // start continuous conversions
  ADC10CTL0 |= ENC + ADC10SC;

  while(1)
  {
    // check event
    buzzer_event = mss_event_get();
    if(buzzer_event & BUZZER_TSK_MIC_INPUT_EVENT)
    {
      // toggle flag
      if(!(app_flags & APP_FLAG_BUZZER_ON))
      {
        // set flag to start playing melodies
      	app_flags |= APP_FLAG_BUZZER_ON;

        // reset play index
        last_buzzer_idx = 0;

        // trigger timer
        APP_ASSERT(mss_timer_start(buzzer_tmr, 
                   MSS_TIMER_MS_TO_TICKS(10)) == true);
      }
      else
      {
        // reset flag to stop playing melodies
      	app_flags &= ~APP_FLAG_BUZZER_ON;
      	buzzer_set_freq(0);
      }
    }

    if(app_flags & APP_FLAG_BUZZER_ON)
    {
      if(mss_timer_check_expired(buzzer_tmr) == true)
  	  {
	      // display animation
        buzzer_delay = play_buzzer();

        // re-trigger timer
        APP_ASSERT(mss_timer_start(buzzer_tmr,
               MSS_TIMER_MS_TO_TICKS(buzzer_delay)) == true);
      }
    }

    MSS_RETURN(buzzer_task_ctx);
  }

  MSS_FINISH();
}

//*****************************************************************************
// Internal functions
//*****************************************************************************

/**************************************************************************//**
*
* init_demo
*
* @brief      initialize demo tasks
*
* @param      -
*
* @return     -
*
******************************************************************************/
static void init_demo(void)
{
  // initialize Launchpad push button and LEDs
  P1DIR |= (BIT0 + BIT6);
  P1OUT &= ~(BIT0 + BIT6);

  // init buzzer driver
  buzzer_init();

  // initialize led matrix driver
  led_matrix_init();

  // measure ADC MIC input average
  measure_adc_mic_avg();

  // initialize semaphore for the LED matrix display access
  led_disp_sema = mss_sema_create(1);
  APP_ASSERT(led_disp_sema != MSS_SEMA_INVALID_HDL);

  // create timers
  pb_timer = mss_timer_create(PB_TASK_ID);
  APP_ASSERT(pb_timer != MSS_TIMER_INVALID_HDL);
  led_matrix_scan_tmr = mss_timer_create(LED_MATRIX_SCAN_TASK_ID);
  APP_ASSERT(led_matrix_scan_tmr != MSS_TIMER_INVALID_HDL);
  mov_text_disp_tmr = mss_timer_create(MOV_TEXT_DISP_TASK_ID);
  APP_ASSERT(mov_text_disp_tmr != MSS_TIMER_INVALID_HDL);
  animation_disp_tmr = mss_timer_create(ANIMATION_DISP_TASK_ID);
  APP_ASSERT(animation_disp_tmr != MSS_TIMER_INVALID_HDL);
  buzzer_tmr = mss_timer_create(BUZZER_TASK_ID);
  APP_ASSERT(buzzer_tmr != MSS_TIMER_INVALID_HDL);

  // create mque and mem for passing led matrix display data
  led_matrix_mque = mss_mque_create(LED_MATRIX_SCAN_TASK_ID);
  APP_ASSERT(led_matrix_mque != MSS_MQUE_INVALID_HDL);
  led_matrix_mem = mss_mem_create(8 + sizeof(void*), 2);
  APP_ASSERT(led_matrix_mem != MSS_MEM_INVALID_HDL);
}

/**************************************************************************//**
*
* measure_adc_mic_avg
*
* @brief      measure ADC Mic input average
*
* @param      -
*
* @return     -
*
******************************************************************************/
static void measure_adc_mic_avg(void)
{
  uint8_t i;

#define NUM_OF_AVG_SAMPLES          (16)

  // configure ADC
  ADC10CTL0 = REFON + MSC + SREF_1 + ADC10SHT_3 + ADC10ON;
  ADC10CTL1 = INCH_0 + CONSEQ_0 + ADC10DIV_7 + ADC10SSEL_3;
  ADC10AE0 |= BIT0;  // P1.0 as mic input

  // star sampling for getting average value
  adc_avg = 0;
  for(i=0 ; i<NUM_OF_AVG_SAMPLES ; i++)
  {
    // start single conversion
    ADC10CTL0 |= ENC + ADC10SC;

    // wait until conversion finished
    while(!(ADC10CTL0 & ADC10IFG));
    ADC10CTL0 &= ~ADC10IFG;

    // calculate average value
    adc_avg += ADC10MEM/NUM_OF_AVG_SAMPLES;
  }

  // turn off ADC
  ADC10CTL0 = 0;
}

/**************************************************************************//**
*
* do_moving_text_display
*
* @brief      handle moving text display
*
* @param      -
*
* @return     delay to next display movement (in milliseconds)
*
* @remark     change the mov_test_disp_data[] array for changing the displayed
*             text
*
******************************************************************************/
static uint16_t do_moving_text_display(void)
{
  static const uint8_t mov_text_disp_data[] = TEXT_DISPLAY_DATA;
  uint16_t i, disp_idx, delay;
  mss_mque_msg_t *msg;

#define MOV_TEXT_DATA_MAX_SIZE     (sizeof(mov_text_disp_data)/sizeof(uint8_t))

  // allocate memory for message
  msg = (mss_mque_msg_t*) mss_mem_alloc(led_matrix_mem);
  APP_ASSERT(msg != NULL);

  // create message
  disp_idx = last_disp_idx;
  for(i=0; i<8 ; i++)
  {
    // copy character
    msg->data[i] = mov_text_disp_data[disp_idx];

    // adjust index
    if(++disp_idx >= MOV_TEXT_DATA_MAX_SIZE)
    {
      disp_idx = 0;
    }
  }

  // send message
  mss_mque_send(led_matrix_mque, msg);

  if(last_disp_idx == 8)
  {
  	delay = 4000;
  }
  else
  {
  	delay = 250;
  }

  // adjust index
  if(++last_disp_idx >= MOV_TEXT_DATA_MAX_SIZE)
  {
    last_disp_idx = 0;
  }

  return delay;
}

/**************************************************************************//**
*
* do_animation_display
*
* @brief      handle animation display
*
* @param      -
*
* @return     delay to next display movement (in milliseconds)
*
* @remark     change the anim_data[] to change the animation pattern
*
******************************************************************************/
static uint16_t do_animation_display(void)
{
  uint16_t ret_delay;
  mss_mque_msg_t *msg;

  static const anim_disp_data_t anim_data[] = ANIMATION_DATA;

#define ANIM_DATA_MAX_SIZE     (sizeof(anim_data)/sizeof(anim_disp_data_t))

  // allocate memory for message
  msg = (mss_mque_msg_t*) mss_mem_alloc(led_matrix_mem);
  APP_ASSERT(msg != NULL);

  // create message
  memcpy(msg->data, anim_data[last_disp_idx].disp_data, 8);

  // send message
  mss_mque_send(led_matrix_mque, msg);

  // return delay
  ret_delay = anim_data[last_disp_idx].delay;

  // adjust index
  if(++last_disp_idx >= ANIM_DATA_MAX_SIZE)
  {
    last_disp_idx = 0;
  }

  return ret_delay;
}

/**************************************************************************//**
*
* play_buzzer
*
* @brief      play buzzer
*
* @param      -
*
* @return     delay to next buzzer note
*
******************************************************************************/
static uint16_t play_buzzer(void)
{
  static const buzzer_tone_data_t buzzer_data[] = BUZZER_TONE_DATA;
  uint16_t delay;

#define BUZZER_DATA_MAX_SIZE     (sizeof(buzzer_data)/sizeof(buzzer_tone_data_t))

  // play buzzer
  buzzer_set_freq(buzzer_data[last_buzzer_idx].buzzer_freq_cnt);

  delay = buzzer_data[last_buzzer_idx].delay;

  // adjust index
  if(++last_buzzer_idx >= BUZZER_DATA_MAX_SIZE)
  {
  	last_buzzer_idx = 0;
  }

  return delay;
}

/**************************************************************************//**
*
* Port1_Isr
*
* @brief      PORT 1 ISR handle function
*
* @param      -
*
* @return     -
*
******************************************************************************/
#pragma vector=PORT1_VECTOR
__interrupt void Port1_Isr(void)
{
  if(P1IFG & BIT3)
  {
	  // disable interrupt and clear flag
  	P1IE &= ~BIT3;
    P1IFG &= ~BIT3;

    // send event to pb task
    mss_event_set(PB_TASK_ID, LED_PB_TSK_PUSH_BUTTON_EVENT);
  }
}

/**************************************************************************//**
*
* ADC10_Isr
*
* @brief      ADC10 ISR handle function
*
* @param      -
*
* @return     -
*
******************************************************************************/
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_Isr(void)
{
  static uint8_t counter = 0;

#define ADC_THRESHOLD          (70)

  // clear interrupt
  ADC10CTL0 &= ~ADC10IFG;

  // check if conversion exceeds threshold value
  if(ADC10MEM > (adc_avg + ADC_THRESHOLD))
  {
    counter++;
  }
  else
  {
    counter = 0;
  }

  if(counter > 2)
  {
    // set event to activate/deactivate buzzer task
    mss_event_set(BUZZER_TASK_ID, BUZZER_TSK_MIC_INPUT_EVENT);
  }
}

