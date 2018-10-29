 /**************************************************************************//**
 * @file        SW_ECC.c
 * @version     V1.00.00
 * $Revision:   0 $
 * $Date:       2017-02-08 $
 * @brief       Software ECC(38, 32) function
 *
 * @note
 *
 * Copyright (C) 2017 AMICCOM Electronics Corp. All rights reserved.
 *****************************************************************************/

#include "AMICCOM_CM0.h"

const uint32_t H[6] = {
    0xF00FFC00,
    0x0F0F03F0,
    0x88C8E38E,
    0x44349A6D,
    0x22A2555B,
    0x11512CB7,
};

const uint8_t S[32] = {
    0x28, 0x24, 0x22, 0x21,
    0x18, 0x14, 0x12, 0x11,
    0x0A, 0x09, 0x06, 0x05,
    0x38, 0x34, 0x32, 0x31,
    0x2C, 0x2A, 0x29, 0x26,
    0x25, 0x23, 0x1C, 0x1A,
    0x19, 0x16, 0x15, 0x13,
    0x0E, 0x0D, 0x0B, 0x07,
};

const uint8_t XOR_TABLE[16] =
{
    0, 1, 1, 0,
    1, 0, 0, 1,
    1, 0, 0, 1,
    0, 1, 1, 0,
};

uint8_t xorByBits(uint32_t value)
{
    uint8_t i;
    uint8_t b = 0;
    for(i = 0; i < 8; i ++)
    {
        b ^= XOR_TABLE[value & 0xF];
        value >>= 4;
    }
    return b;
}

uint8_t encode(uint32_t message)
{
    uint8_t i, p;

    p = 0;
    for(i = 0; i < 6; i ++)
    {
        p <<= 1;
        p |= xorByBits(message & H[i]);
    }
    return p;
}

uint32_t decode(uint32_t rxd, uint8_t parity)
{
    uint8_t  i, check, syndrome;
	uint32_t ecc_bits, out;

    ecc_bits = 0;
    parity &= 0x3F;
    check = encode(rxd);
    syndrome = parity ^ check;
	if(0 != syndrome)
	{
		for(i = 0; i < 32; i ++)
		{
			if(S[i] == syndrome)
			{
				ecc_bits = (1 << (31-i));
				break;
			}
		}
	}
    out = rxd ^ ecc_bits;
    return out;
}

/**
 *
 * @brief   Software ECC Encode function.
 *
 * @param   in_data             32bits input data to encode.
 * @return  6bits parity (bit[7:6] = 00)
 */
uint8_t ECC_Encode(uint32_t in_data)
{
    uint32_t parity;
    parity = encode(in_data);
    return parity;
}

/**
 *
 * @brief   Software ECC Decode function.
 *
 * @param   message             32bits input data to decode.
 * @param   parity              6bits parity data to decode.
 * @return  32bits output data
 */
uint32_t ECC_Decode(uint32_t message, uint8_t parity)
{
    uint32_t out;
    out = decode(message, parity);
    return out;
}

