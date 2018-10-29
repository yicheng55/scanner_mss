;/**************************************************************************//**
; * @file       startup_A8107M.s
; * @brief
; * @version    V1.00.02
; * @date       2017-01-11
; *
; * @note       V1.00.01: Remove not use peripherals.
; *             V1.00.02: Modify OTA to absolute address. Add note.
; * 
; * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
; ******************************************************************************/
;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000200

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000100

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler
                DCD     0                         ;
                DCD     UART0_Handler             ; UART 0 Handler
                DCD     SLPTIMER0_Handler         ; SLEEP TIMER 0 Handler
                DCD     RADIO_Handler             ; RADIO Handler
                DCD     0                         ;
                DCD     UART2_Handler             ; UART 2 Handler
                DCD     GPIO0_Handler             ; GPIO0 Combined Handler
                DCD     0                         ; 
                DCD     TIMER0_Handler            ; TIMER 0 handler
                DCD     TIMER1_Handler            ; TIMER 1 handler
                DCD     DUALTIMER_Handler         ; Dual timer handler
                DCD     MPULCD_Handler            ;
                DCD     0                         ;
                DCD     UART1_Handler             ; UART 1 Handler
                DCD     0                         ;
                DCD     WUN_Handler               ;
                DCD     SPI_Handler               ; SPI Handler
                DCD     I2C_Handler               ; I2C Handler
                DCD     RTC_Handler               ; Read Time Clock Handler
                DCD     0                         ;
                DCD     AES_CCM_Handler           ;
                DCD     ADC_12BIT_Handler         ; 12-bit ADC Handler
                DCD     0                         ;
                DCD     0                         ;
                DCD     SLPTIMER1_Handler         ; SLEEP TIMER 1 Handler
                DCD     0                         ;
                DCD     0                         ;
                DCD     0                         ;
                DCD     0                         ;
                DCD     0                         ;
                DCD     0                         ;
                DCD     SOFT_Handler              ;
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler            [WEAK]
                B       .
                ENDP
Default_Handler PROC
                EXPORT UART0_Handler              [WEAK]
                EXPORT SLPTIMER0_Handler          [WEAK]
                EXPORT RADIO_Handler              [WEAK]
                EXPORT UART2_Handler              [WEAK]
                EXPORT GPIO0_Handler              [WEAK]
                EXPORT TIMER0_Handler             [WEAK]
                EXPORT TIMER1_Handler             [WEAK]
                EXPORT DUALTIMER_Handler          [WEAK]
                EXPORT MPULCD_Handler             [WEAK]
                EXPORT UART1_Handler              [WEAK]
                EXPORT WUN_Handler                [WEAK]
                EXPORT SPI_Handler                [WEAK]
                EXPORT I2C_Handler                [WEAK]
                EXPORT RTC_Handler                [WEAK]
                EXPORT AES_CCM_Handler            [WEAK]
                EXPORT ADC_12BIT_Handler          [WEAK]
                EXPORT SLPTIMER1_Handler          [WEAK]
                EXPORT SOFT_Handler               [WEAK]
UART0_Handler
SLPTIMER0_Handler
RADIO_Handler
UART2_Handler
GPIO0_Handler
TIMER0_Handler
TIMER1_Handler
DUALTIMER_Handler
MPULCD_Handler
UART1_Handler
WUN_Handler
SPI_Handler
I2C_Handler
RTC_Handler
AES_CCM_Handler
ADC_12BIT_Handler
SLPTIMER1_Handler
SOFT_Handler
                B       .
                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, = (Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END
