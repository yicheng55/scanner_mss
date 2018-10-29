/* TString.h: Davicom string header for ESL system.                     */
/*                                                                              */
/*==============================================================================*/
/* Header Name	: TString.h                                                       */
/* Purpose      : TString header                                          */
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
#ifndef _DAVICOM_CSTRING_H_
#define _DAVICOM_CSTRING_H_

#include "ProjectConfig.h"


#pragma pack(1)
#pragma pack()


#ifdef _UNICODE
#define TString		wstring
#else
#define TString		string
#endif

//==============================================================================
//--- Declarative Public Functions ---
//==============================================================================

//------------------------------------------------------------------------------
//---  ---
//------------------------------------------------------------------------------

//==============================================================================
//--- Declarative Protected Functions ---
//==============================================================================
//------------------------------------------------------------------------------

class MyString
{
public:
	MyString(void);
	~MyString(void);

	void Test();
};	






#endif		//_DAVICOM_TSTRING_H_
