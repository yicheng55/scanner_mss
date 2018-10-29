/**************************************************************************//**
 * @file        retarget.c
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       The interface to STDIO.h
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/
 
#include <stdio.h>
#include <time.h>
#include <rt_misc.h>
#pragma import(__use_no_semihosting_swi)

extern unsigned char UartGetc(void);
extern unsigned char UartPutc(unsigned char my_ch);
struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
  return (UartPutc(ch));
}

int fgetc(FILE *f) {
  return (UartPutc(UartGetc()));
}

int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}

void _ttywrch(int ch) {
  UartPutc (ch);
}

void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}
