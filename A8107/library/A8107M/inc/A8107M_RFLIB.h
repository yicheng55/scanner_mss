/**************************************************************************//**
 * @file        A8107M_RFLIB.h
 * @version     V1.00.01
 * $Revision:   0 $
 * $Date:       2017-01-10 $
 * @brief       The Header file for A8107M_RFLIB.lib
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/
 
#ifndef _A8107M_RFLIB_H_
#define _A8107M_RFLIB_H_

/********************************************************************
** function declaration
********************************************************************/
extern void initRF(void);
extern void SetCH(uint8_t ch);
extern void StrobeCmd(uint8_t cmd);

#endif

