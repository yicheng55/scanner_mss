#include "Encryption.h"
#include <stdio.h>
#include <stdlib.h>

//==============================================================================
//--- Declarative Member Variables ---
//==============================================================================
#if 0
const uint8_t AES_KEY[] =
{
	0x4C, 0x68, 0x38, 0x41,
	0x39, 0xF5, 0x74, 0xD8,
	0x36, 0xBC, 0xF3, 0x4E,
	0x9D, 0xFB, 0x01, 0xBF
};
#endif 

//==============================================================================
// Function     : 
// Purpose      : Construction
// Description	: 
// Editor       : Richard Chung
// Update Date	: 2018-03-01
// -----------------------------------------------------------------------------
// Parameters   : 
// Return       : 
// Remarks      : 
//==============================================================================
void Encryption(void)
{
	
}


/************************************************************************
** AES128_Encrypt
** Description:
** Parameters:		p_data=address start position for AES_DATA
**					p_encrypt_key=address start position for AES_KEY
**					p_dataresult=address start position for encrypted AES_data
** Return value: 	none
** Note:
************************************************************************/
void AES128_Encrypt(uint8_t *p_data, const uint8_t *p_encrypt_key, uint8_t *p_dataresult)
{
    uint8_t i;

    for(i = 0; i < 16; i++)
        HW8_REG(0x5000C00C + 15 - i) = *(p_encrypt_key + i); //AES_KEY

    for(i = 0; i < 16; i++)
        HW8_REG(0x5000C01C + 15 - i) = *(p_data + i); //AES_DATA

    HW8_REG(0x5000C001) = 0x00;     //AES_ENC
    HW8_REG(0x5000C000) = 0x01;     //AES_GO

    while(HW8_REG(0x5000C000) & 0x01);

    for(i = 0; i < 16; i++)
        *(p_dataresult + i) = HW8_REG(0x5000C01C + 15 - i);
}
/************************************************************************
** AES128_Decrypt
** Description:
** Parameters:		p_data=address start position for AES_DATA
**					p_encrypt_key=address start position for AES_KEY
**					p_dataresult=address start position for decrypted AES_data
** Return value: 	none
** Note:
************************************************************************/
void AES128_Decrypt(uint8_t *p_data, const uint8_t *p_decrypt_key, uint8_t *p_dataresult)
{
    uint8_t i;

    for(i = 0; i < 16; i++)
        HW8_REG(0x5000C00C + 15 - i) = *(p_decrypt_key + i); //AES_KEY

    for(i = 0; i < 16; i++)
        HW8_REG(0x5000C01C + 15 - i) = *(p_data + i); //AES_DATA

    HW8_REG(0x5000C001) = 0x01;//AES_DEC
    HW8_REG(0x5000C000) = 0x01;//AES_GO

    while(HW8_REG(0x5000C000) & 0x01);

    for(i = 0; i < 16; i++)
        *(p_dataresult + i) = HW8_REG(0x5000C01C + 15 - i);
}
