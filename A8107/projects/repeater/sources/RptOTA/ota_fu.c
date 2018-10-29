#include <stdio.h>
#include "A8107M.h"
#include "Flash.h"

#define APP_START_ADDRESS           0x00000000
#define APP_BAK_BIN_ADDRESS     0x00020000
#define OTA_TABLE_ADDRESS           0x0003EF80

/* Typedefs */
typedef struct
{
    uint32_t         start_addr;
    uint32_t       len;
    uint32_t       start_data;
    uint32_t       chk;
} Rec_Table;

typedef struct
{
    uint32_t         num;
    uint32_t       update;
    uint32_t       protect;
    uint32_t       chk;
    Rec_Table            rec1;
    Rec_Table            rec2;
} OTA_Table;
OTA_Table ota_table1 ;

void OTA_Init(void)
{
    uint32_t address = OTA_TABLE_ADDRESS;                           //Write OTA table (128)

    printf( "OTA_Init()...\r\n");

    ota_table1.num                          = 1;                                        //Max = 31
    ota_table1.update                       = 0x00010001;                       //update code value = 0x00010001 other none.
    ota_table1.protect                  = 0x00000000;                   //Set 0x0000_0001 flash read Protect.
    ota_table1.chk                          = (ota_table1.num + ota_table1.update + ota_table1.protect);

    ota_table1.rec1.start_addr  = APP_START_ADDRESS;        //Start App code address.
    ota_table1.rec1.len                 = 256;                                  //Update code length. 32768 bytes / 128(page) = 256 length.
    ota_table1.rec1.start_data  = APP_BAK_BIN_ADDRESS;  //Start Update code data. For UpdateCode.s
    ota_table1.rec1.chk                 = (ota_table1.rec1.start_addr + ota_table1.rec1.len + ota_table1.rec1.start_data);

    Flash_PageErase(address);                                                       //Erase  128 bytes.

    printf("size of ota_table1= %u...\r\n", sizeof(ota_table1));
    Flash_Write_U32(address, (uint32_t *) &ota_table1, sizeof(ota_table1) / sizeof(uint32_t));
}

