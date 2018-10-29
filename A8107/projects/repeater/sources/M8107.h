/**********************************************
 * M8107.h
 *
 **********************************************/
#ifndef _M8107_H_
#define _M8107_H_

#include <stdint.h>

extern uint8_t EncryptData[16];
extern uint8_t DecryptData[16];

void EnterPM1(void);
void EnterPM3(void);
void TurnOn_External_RTC_Crystal(void);
uint32_t RSSI_measurement(void);
void M8107RF_SetFIFOLvl(uint32_t fifolen);

void AES128_Encrypt(uint8_t *p_data, uint8_t *p_encrypt_key, uint8_t *p_dataresult);
void AES128_Decrypt(uint8_t *p_data, uint8_t *p_decrypt_key, uint8_t *p_dataresult);

#endif /* _M8107_H_ */
