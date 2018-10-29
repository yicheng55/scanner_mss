/**************************************************************************//**
 * @file        SW_ECC.h
 * @version     V1.00.00
 * $Revision:   0 $
 * $Date:       2017-02-08 $
 * @brief       SW_ECC.c Header File
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 ******************************************************************************/

#ifndef __SW_ECC_H__
#define __SW_ECC_H__

extern uint8_t ECC_Encode(uint32_t in_data);
extern uint32_t ECC_Decode(uint32_t message, uint8_t parity);

#endif

