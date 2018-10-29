    AREA  |.ARM.__AT_0x28000|, DATA, READONLY
;    AREA  |.data __AT_0x20004000|, DATA, READONLY
;EXPORT  MyBinaryImage1

; Includes the binary file MyBinFile1.bin from the current source folder
;MyBinaryImage1
;        incbin  EPD_DATA_296_INCH.pbm1.jbg				;EPD_DATA_296_INCH.pbm1.jbg
;				incbin  00001.bin85.pbm.jbg				;EPD_DATA_296_INCH.pbm1.jbg   00002.bin85.pbm.jbg
				incbin  00002.bin85.pbm.jbg			
;				incbin  EPD_DATA_213_INCH85.pbm.jbg		
;MyBinaryImage1_End


;MyBinaryImage1_length
;        DCD     MyBinaryImage1_End - MyBinaryImage1

;        EXPORT  MyBinaryImage1_length
    END
            