/* Utility.h: Davicom common utility header for ESL system.                     */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: Encryption.h                                                     */
/* Purpose      : Encryption and decryption                                 */
/* Autor        : Richard Chuang                                                */
/* Email        : richard_chung@davicom.com.tw                                  */
/* Description  :                                                               */
/* Reference    :                                                               */
/* Copyright    : (c) Davicom Semiconductor Inc. All Rights Reserved.           */
/* -----------------------------------------------------------------------------*/
/* Version      : 1.0.0.0                                                       */
/* Update       : 2018-03-01                                                    */
/* Modified     : Richard Chung                                                 */
/* Description	:                                                               */
/* -----------------------------------------------------------------------------*/
/* How to use:                                                                  */
/*                                                                              */

#ifndef _DAVICOM_COMMON_ENCRYPTION_H_
#define _DAVICOM_COMMON_ENCRYPTION_H_

#include "Eslconfig.h"
#include "A8107M.h"
#include "system_A8107M.h"
#include <stdio.h>
#include "string.h"

//==============================================================================
//--- Declarative Functions ---
//==============================================================================
//------------------------------------------------------------------------------
//--- Declarative constructor ---
//------------------------------------------------------------------------------
void Encryption(void);

//------------------------------------------------------------------------------
//--- Declarative Public Functions ---
//------------------------------------------------------------------------------
void AES128_Encrypt(uint8_t *p_data, const uint8_t *p_encrypt_key, uint8_t *p_dataresult);
void AES128_Decrypt(uint8_t *p_data, const uint8_t *p_decrypt_key, uint8_t *p_dataresult);

#endif // _DAVICOM_COMMON_ENCRYPTION_H_

