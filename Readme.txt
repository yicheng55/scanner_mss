Keil microVision 


假設我們將程式碼擺放在 C:\Davicom\Keil目錄，後面以[KEIL]表示

[KEIL]
├─common   存放跨所有專案與MCU的共用程式與宣告目錄
│  ├─asm                  存放跨專案組合語言
│  ├─include             存放跨專案宣告（特殊跨平台共同宣告，比如davicom_id_lists.h）
│  └─source              存放跨專案程式碼與宣告（配套的程式碼與宣告，比如base64.c and bace64.h）
│      ├─A8107       如果有針對不同MCU會有的特殊宣告與程式碼 
│      ├─…
│      └─ST2410     如果有針對不同MCU會有的特殊宣告與程式碼
│
├─A8107    這邊建議使用MCU當作命名方式
│  ├─common           存放此平台共用程式與宣告目錄
│  │  ├─asm          存放此平台共用組合語言
│  │  ├─include     存放此平台共用宣告（此平台共用宣告，比如davicom_a8107_setting.h）
│  │  └─source      存放此平台共程式碼與宣告（配套的程式碼與宣告，比如FuncXXX.c and FuncXXX.h）
│  ├─library              存放A8107及會使用的第三方套件與函示庫
│  │  ├─A8107M
│  │  │  ├─inc
│  │  │  ├─lib
│  │  │  └─src
│  │  ├─…
│  │  └─Driver
│  │      ├─inc
│  │      └─src
│  └─projects            存放所有A8107的專案
│      ├─repeater  Repeater專案
│      │  ├─Listings (編譯清單，編譯器自動產生，刪除後編譯時自動會再產生)
│      │  ├─Objects (編譯輸出，編譯器自動產生，刪除後編譯時自動會再產生)
│      │  └─sources     Repeater專案私有的程式碼，比如Repeater.c and Repeater.h
│      ├─scanner
│      │  ├─Listings
│      │  ├─Objects
│      │  └─sources     Scanner專案私有的程式碼，比如Scanner.c and Scanner.h
│      └─tag
│          ├─Listings
│          ├─Objects
│          └─sources     Tag專案私有的程式碼，比如RFpacket.c and RFpacket.h
├─MCU_XXX_1
├─...
├─MCU_XXX_N
│  ├─common           
│  │  ├─asm           
│  │  ├─include      
│  │  └─source      
│  ├─library              
│  │  ├─[Platform]
