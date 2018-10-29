#include "EPaperSSD1608.h"

#include "Spiconfig.h"
#include "ESLDebug.h"
#include "string.h"
#include <stdio.h>
#include "A8107M.h"
#include "spi.h"
//#include "tagdefine.h"
#include <time.h>
#include <rt_misc.h>
#include "Utility.h"
#include "BarCode_Pic.h"
#include "TagFont.h"


// SSD1608
// 240 Source x 320 Gate AMEPD Display Driver with Controller

const uint8_t lut_data[] =	
{
		0x50,0xAA,0x55,0xAA,0x11,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0xFF,0xFF,0x1F,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00
//0x11,0x11,0x10,0x02,0x02,0x22,0x22,0x22,0x22,0x22,0x51,0x51,0x55,0x88,0x08,0x08,0x88,0x88,0x00,0x00, 0x34,0x23,0x12,0x21,0x24,0x28,0x22,0x21,0xA1,0x01
};

const uint8_t lut_data_partial[] = 
{
0x10,0x18,0x18,0x08,0x18
,0x18,0x08,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00
,0x13,0x14,0x44,0x12,0x00
,0x00,0x00,0x00,0x00,0x00	
};
const unsigned char lut_data_02[] =		//30Bytes
{
	0xaa, 0x09, 0x09, 0x19, 0x19, 
	0x11, 0x11, 0x11, 0x11, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 
	0x75, 0x77, 0x77, 0x77, 0x07, 
	0x00, 0x00, 0x00, 0x00, 0x00
};	

	 
void HWReset(void)
{
		GPIO0->DATAOUT_PIN.P14 = 1;  
		Delay1us(2000);
		GPIO0->DATAOUT_PIN.P14 = 0;  
		Delay1us(2000);
		GPIO0->DATAOUT_PIN.P14 = 1;  
		Delay1us(2000);
}	 
	 
void spi3w_wcommand(uint32_t command)
{	
	SPI_SS_LOW(SPI);
	SPI_WRITE_TX(SPI,command);
	while(0 == (SPI->FIFOTXSTUS & SPI_FIFORXSTUS_EMPTY_Msk)) ;
	SPI_SS_HIGH(SPI);
}

void spi3w_wdata(uint32_t data)
{	
	SPI_SS_LOW(SPI);
	SPI_WRITE_TX(SPI,(data | 0x100));
	while(0 == (SPI->FIFOTXSTUS & SPI_FIFORXSTUS_EMPTY_Msk)) ;
	SPI_SS_HIGH(SPI);
}

void READBUSY(void)
{
  while(1)
  {	 
     if(GPIO0->DATA_PIN.P13 ==0)
		 {
			 break;
		 }			 
  }      
  Delay1us(1000); //200000
}

void WriteLUT()
{

  uint8_t length = 30;
  uint8_t i = 0;

	SPI_SS_LOW(SPI);
	SPI_WRITE_TX(SPI,0x32);
  while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;	

  while(length)
  {
        SPI_WRITE_TX(SPI,*(lut_data + i) | 0x100); i++;

        while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;
        length--;
  }
	
	SPI_SS_HIGH(SPI);

}
void UpdateDisplay(void)
{
	WriteLUT();
//	spi3w_wcommand(0x21);
//  spi3w_wdata(0x93);//C7 C4
  spi3w_wcommand(0x22);
  spi3w_wdata(0xC7);//C7 C4
	spi3w_wcommand(0x20);
	Delay1us(200);		//1ms         //200 FAST
	READBUSY();
	Delay1us(250);	//300ms 300000  //250 FAST
	
//	Delay1us(250);	//300ms 300000
//	Delay1us(250);	//300ms 300000
//	Delay1us(250);	//300ms 300000
//	Delay1us(250);	//300ms 300000
}

void WriteLUT_Part()
{

  uint8_t length = 30;
  uint8_t i = 0;

	SPI_SS_LOW(SPI);
	SPI_WRITE_TX(SPI,0x32);
  while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;	

  while(length)
  {
        SPI_WRITE_TX(SPI,*(lut_data_02 + i) | 0x100); i++;

        while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;
        length--;
  }
	
	SPI_SS_HIGH(SPI);

}

void PartialUpdateDisplay(void)
{
	WriteLUT_Part();
	spi3w_wcommand(0x22);
  spi3w_wdata(0xC0);//C7 C4
//	spi3w_wcommand(0x21);
//  spi3w_wdata(0x93);//C7 C4
	spi3w_wcommand(0x22);
  spi3w_wdata(0xC4);//C7 C4

	spi3w_wcommand(0x20);
	Delay1us(200);		//1ms         //200 FAST
	READBUSY();
	Delay1us(250);	//300ms 300000  //250 FAST
	
//	Delay1us(250);	//300ms 300000
//	Delay1us(250);	//300ms 300000
//	Delay1us(250);	//300ms 300000
//	Delay1us(250);	//300ms 300000
}

//void WriteLUT(void)
//{
//	int i;
//	SPI_SS_LOW(SPI);
//	SPI_WRITE_TX(SPI,0x32);
//	while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk)) ;
//	
//	for(i=0;i<30;i++) {
//		while(1 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_FULL_Msk)) ;
//		SPI_WRITE_TX(SPI,((uint32_t)lut_data[i] | 0x100));  
//	}
//	
//	while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk)) ;
//	SPI_SS_HIGH(SPI);
//}

//void WriteLUT_02()
//{
//	int i;
//	SPI_SS_LOW(SPI);
//	SPI_WRITE_TX(SPI,0x32);
//	while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk)) ;
//	
//	for(i=0;i<30;i++) {
//		while(1 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_FULL_Msk)) ;
//		SPI_WRITE_TX(SPI,(lut_data_02[i] | 0x100));  
//	}
//	
//	while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk)) ;
//	SPI_SS_HIGH(SPI);
//}

void SetWinAddr(uint8_t xs, uint8_t xe, uint16_t ys, uint16_t ye, uint8_t xc, uint16_t yc)
{

	/* Y RAM address start/end */
	spi3w_wcommand(0x45);
	spi3w_wdata(ys & 0xFF);			//YStart = 0x0
	spi3w_wdata((ys >> 8) & 0xFF);
	spi3w_wdata(ye & 0xFF);				//YEnd = 296(0x128) - 1 = 0x127
	spi3w_wdata((ye >> 8) & 0xFF);
	
		/* X RAM address start/end */
	spi3w_wcommand(0x44);
	spi3w_wdata(xs);			//XStart = 0x0
	spi3w_wdata(xe);			//XEnd = 128/8 = 16(0x10) - 1 = 0x0F

	/* RAM Y address counter */
	spi3w_wcommand(0x4F);
	spi3w_wdata(yc & 0xFF);
	spi3w_wdata((yc >> 8) & 0xFF);
	
		/* RAM X address counter */
	spi3w_wcommand(0x4E);
	spi3w_wdata(xc);
}

void Write_BarCode_FULL()//examplePix
{
	int j = 0;
	uint32_t bitmap = 0;
	int TotalLength = 4736;

	SetWinAddr(0x0, 0xF, 0x127, 0x0, 0, 0x127);
	printf("Write_BarCode_FULL..\r\n");
	SPI_SS_LOW(SPI);

	SPI_WRITE_TX(SPI,0x24);
  while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;	

  while(TotalLength)
  {
		    bitmap = (uint32_t)~(*(Lost_Beacon + j)) | 0x100;
		    SPI_WRITE_TX(SPI, bitmap); j++;

        while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;
        TotalLength--;
  }

	SPI_SS_HIGH(SPI);
}

void Write_QRCode_FULL()//examplePix
{
	int j = 0;
	uint32_t bitmap = 0;
	int TotalLength = 480;

//	SetWinAddr(0x0, 0xF, 0x127, 0x0, 0, 0x127);
//	SetWinAddr(0x03, 0xD, 0x109, 0x4C, 0x03, 0x109); //2189
	SetWinAddr(0x00, 0x8, 0x3B, 0x0, 0x00, 0x3B); //480
	printf("Write_QRCode_FULL..\r\n");
	SPI_SS_LOW(SPI);

	SPI_WRITE_TX(SPI,0x24);
  while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;	

  while(TotalLength)
  {
		    bitmap = (uint32_t)~(*(Lost_Beacon + j)) | 0x100;
		    SPI_WRITE_TX(SPI, bitmap); j++;

        while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;
        TotalLength--;
  }

	SPI_SS_HIGH(SPI);
}

void WhiteDM(void)
{
	uint32_t length = 4736;
	uint32_t bitmap = 0xFF | 0x100;
	
	spi3w_wcommand(0x11);
	spi3w_wdata(0x03);	
	SetWinAddr(0x00, 0x0F, 0x00, 0x127, 0, 0x00);

	SPI_SS_LOW(SPI);

	SPI_WRITE_TX(SPI,0x24);
  while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;	

  while(length)
  {
        SPI_WRITE_TX(SPI, bitmap);

        while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;
        length--;
  }
	SPI_SS_HIGH(SPI);
	
	spi3w_wcommand(0x11);
	spi3w_wdata(0x5);
}

void CleanEPD()
{
	WhiteDM();
}

void WriteRAMqr_FULL(int TotalLength, uint8_t* ReceiveData)//examplePix
{
	int j = 0;
	uint32_t bitmap = 0;

	SetWinAddr(0x0, 0xF, 0x127, 0x0, 0, 0x127);
//	printf("WriteRAMqr_FULL..\r\n");
	SPI_SS_LOW(SPI);

	SPI_WRITE_TX(SPI,0x24);
	while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;	

	while(TotalLength)
	{
		bitmap = (uint32_t)~(*(ReceiveData + j)) | 0x100;
		SPI_WRITE_TX(SPI, bitmap); j++;

        while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;
		TotalLength--;
	}
	SPI_SS_HIGH(SPI);
}

void Write_EPD_FULL(uint8_t color, int TotalLength, uint8_t* ReceiveData)//examplePix
{
	int j = 0;
	uint32_t bitmap = 0;

	SetWinAddr(0x0, 0xF, 0x127, 0x0, 0, 0x127);
//	printf("WriteRAMqr_FULL..\r\n");
	SPI_SS_LOW(SPI);

	SPI_WRITE_TX(SPI,0x24);
	while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;	

	while(TotalLength)
	{
		bitmap = (uint32_t)~(*(ReceiveData + j)) | 0x100;
		SPI_WRITE_TX(SPI, bitmap); j++;

        while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;
		TotalLength--;
	}
	SPI_SS_HIGH(SPI);
}

void TagPowerDownFunction(void)
{
	spi3w_wcommand(0x10);			
	spi3w_wdata(0x1);				// EPD Enter Deep Sleep Mode
}

void TagPowerUpFunction(void)
{
	HWReset();
	spi3w_wcommand(0x10);			
	spi3w_wdata(0x0);				// EPD exit Deep Sleep Mode
	
}

void powerOn(void)
{
	//printf("powerOn\r\n");
	//HWReset();
	spi3w_wcommand(0x22);			
	spi3w_wdata(0xc0);		
	spi3w_wcommand(0x20);	
	READBUSY();
}

void powerOff(void)
{
	spi3w_wcommand(0x22);			
	spi3w_wdata(0xC3);		
	spi3w_wcommand(0x20);	
	READBUSY();

}

void Print_Char(uint8_t Col, uint8_t Line, char ascii)
{
	uint8_t i, j;	
	uint32_t tmp;
	
	/* Data Entry mode */
	spi3w_wcommand(0x11);
	spi3w_wdata(0x4);	
	
	for(j=0; j<2; j++) {
		SetWinAddr((Line*2)-1,(Line-1)*2, 295, 295-((Col+1)*8), (Line*2)-2+j, 295-(Col*8));
		for(i=0; i<8; i++) {
			tmp=Font8x16[(ascii-0x20)*16+(i+j*8)];
			spi3w_wcommand(0x24);
			spi3w_wdata(~tmp);
		}
	}

	/* Data Entry mode */
	spi3w_wcommand(0x11);
	spi3w_wdata(0x5);	
}

void Print_Char_BIG_Font(uint8_t Col, uint8_t Line, int Width ,int Addr,char ascii) //FOR BIG NO COMPRESS
{
	uint8_t i, j;	
	uint32_t tmp;

	/* Data Entry mode */
	spi3w_wcommand(0x11);
	spi3w_wdata(0x5);	

	for(j=0; j<3; j++) {

		SetWinAddr(Line,Line+2, 295, 295-((2)*Width), (Line+j), 295 - Addr);
//		printf("%d ",Addr);
			for(i=0; i<(custom_font_length[ascii-0x2D])/3; i++) {
				tmp=custom_font8x16_5[ascii-0x2D][i+j*((custom_font_length[ascii-0x2D])/3)];

			spi3w_wcommand(0x24);
			spi3w_wdata(~tmp);
		}
	}


	/* Data Entry mode */
	spi3w_wcommand(0x11);
	spi3w_wdata(0x5);	
}

void Print_Line_Big_Font(uint8_t Line, char Text[])
{
	uint8_t Col;
	int Width = 0;
	int First_Text_Width = 0;
	int Starting_Address = 0;
	Line = (Line*3) - 2;
	for (Col=0; Col<strlen(Text); Col++) 
	{
		
		if(Col == 0)
		{
			Width = custom_font_Width[Text[Col]-0x2D];
//			printf("%d ",Width);
			First_Text_Width = custom_font_Width[Text[Col]-0x2D];
			Starting_Address = 0;
			Print_Char_BIG_Font(Col, Line, Width,Starting_Address,Text[Col]);
		}
		else
		{
			Width = Width + custom_font_Width[Text[Col-1]-0x2D];// - First_Text_Width;
//			printf("%d ",Width);
			Starting_Address = Width - First_Text_Width;
			Print_Char_BIG_Font(Col, Line, Width,Starting_Address,Text[Col]);
		}
//		Print_Char_BIG(Col, Line, Width,Text[Col]);
	}
			
}

void Print_Line(uint8_t Line, char Text[])
{
	uint8_t Col;
	  for (Col=0; Col<strlen(Text); Col++) 
			Print_Char(Col, Line, Text[Col]);
}

#if 0
// This code will be removed in the future
void ePaper_Init(void)
{

//	uint8_t TestBuffer[9];
//	uint8_t Test_Buffer[9];
//	clock_t Start, End;
	//printf("ePaper_Init_Start\r\n");
	HWReset();		//nRST & BUSY
	/*** initial code ***/
	spi3w_wcommand(0x10);			// Exit Deep Sleep Mode
	spi3w_wdata(0x0);				
	
	/* Panel configration. */
	spi3w_wcommand(0x01);			// 
	spi3w_wdata(0x27);				// Gate: 296 = 0x127+1
	spi3w_wdata(0x01);
	spi3w_wdata(0x00);				// GD, SM, TB
	
	/* Booster Soft start Control */ 
	spi3w_wcommand(0x0C);   // Soft Start
	spi3w_wdata(0xD7);  // 0xCF
	spi3w_wdata(0xD6);  // 0xCE
	spi3w_wdata(0x9D);  // 0x8D 

	/* Gate scan start position */
	spi3w_wcommand(0x0F);//  Gate Scan Start Position
	spi3w_wdata(0x00);  //
	spi3w_wdata(0x00);  //
          
	/* Gate driving voltage control */
	spi3w_wcommand(0x03);
	spi3w_wdata(0x00); //0x75
	//spi3w_wdata(0xAA);				// VGH = 20V, VGL = -20V
//	
	/* Source driving voltage control */
	spi3w_wcommand(0x04);
	spi3w_wdata(0x0A);				// VSH/VSL = 15V

//	

	/* Set dummy line pulse period */
	spi3w_wcommand(0x3A);
	spi3w_wdata(0x1A);		
	//spi3w_wdata(0x1A);
//	
	/* Set Gate line width */
	spi3w_wcommand(0x3B);
	spi3w_wdata(0x08);		
	//spi3w_wdata(0x3E);				// 62us
//	
	/* Set Border waveform */
	spi3w_wcommand(0x3C);
	spi3w_wdata(0x33);	 //0x33	
	//spi3w_wdata(0x00);
//	
	/** Data operations **/
	/* Data Entry mode */
	spi3w_wcommand(0x11);
	spi3w_wdata(0x5);				
//	
	/* X RAM address start/end */
	spi3w_wcommand(0x44);
	spi3w_wdata(0x00);			//XStart = 0x0
	//spi3w_wdata(0xF);			//XEnd = 128/8 = 16(0x10) - 1 = 0x0F
	spi3w_wdata(0x5);
//	
	/* Y RAM address start/end */
	spi3w_wcommand(0x45);
	spi3w_wdata(0x00);			//YStart = 0x0
	spi3w_wdata(0x00);
  //spi3w_wdata(0x27);				//YEnd = 296(0x128) - 1 = 0x127
	//spi3w_wdata(0x01);
	spi3w_wdata(0x17);
	spi3w_wdata(0x00);

	/* RAM X address counter */
	spi3w_wcommand(0x4E);
	spi3w_wdata(0x00);
//	
	/* RAM Y address counter */
	spi3w_wcommand(0x4F);
	spi3w_wdata(0x00);
	spi3w_wdata(0x00);

//	spi3w_wcommand(0x21);
//	spi3w_wdata(0x83);	
//	spi3w_wcommand(0xF0);
//	spi3w_wdata(0x1F);

	spi3w_wcommand(0x2C);   // Write VCOM Register
	spi3w_wdata(0xA8);//A8

	WriteLUT();
	WhiteDM();
	TagPowerDownFunction();
	//powerOff();
	//printf("Paper_Init_End\r\n");
}

void ePaper_Init_02(void)
{

//	uint8_t TestBuffer[9];
//	uint8_t Test_Buffer[9];
//	clock_t Start, End;
	//printf("ePaper_Init_Start\r\n");
	HWReset();		//nRST & BUSY
	/*** initial code ***/
	spi3w_wcommand(0x10);			// Exit Deep Sleep Mode
	spi3w_wdata(0x0);				
	
	/* Panel configration. */
	spi3w_wcommand(0x01);			// 
	spi3w_wdata(0x27);				// Gate: 296 = 0x127+1
	spi3w_wdata(0x01);
	spi3w_wdata(0x00);				// GD, SM, TB
	
	/* Booster Soft start Control */ 
	spi3w_wcommand(0x0C);   // Soft Start
	spi3w_wdata(0xD7);  // 0xCF
	spi3w_wdata(0xD6);  // 0xCE
	spi3w_wdata(0x9D);  // 0x8D 

	/* Gate scan start position */
	spi3w_wcommand(0x0F);//  Gate Scan Start Position
	spi3w_wdata(0x00);  //
	spi3w_wdata(0x00);  //
          
	/* Gate driving voltage control */
	spi3w_wcommand(0x03);
	spi3w_wdata(0x00); //0x75
	//spi3w_wdata(0xAA);				// VGH = 20V, VGL = -20V
//	
	/* Source driving voltage control */
	spi3w_wcommand(0x04);
	spi3w_wdata(0x0A);				// VSH/VSL = 15V

//	
//	spi3w_wcommand(0x0B);
//	spi3w_wdata(0x5); //0x75
	/* Set dummy line pulse period */
	spi3w_wcommand(0x3A);	
	spi3w_wdata(0x16); //016 //1A
//	
	/* Set Gate line width */
	spi3w_wcommand(0x3B);
//	spi3w_wdata(0xFF);		//08
	spi3w_wdata(0x08);				// 62us
//	
	/* Set Border waveform */
	spi3w_wcommand(0x3C);
//	spi3w_wdata(0x33);	 //0x33	
	spi3w_wdata(0x33);
//	
	/** Data operations **/
	/* Data Entry mode */
	spi3w_wcommand(0x11);
	spi3w_wdata(0x5);				
//	
	/* X RAM address start/end */
	spi3w_wcommand(0x44);
	spi3w_wdata(0x00);			//XStart = 0x0
	//spi3w_wdata(0xF);			//XEnd = 128/8 = 16(0x10) - 1 = 0x0F
	spi3w_wdata(0x5);
//	
	/* Y RAM address start/end */
	spi3w_wcommand(0x45);
	spi3w_wdata(0x00);			//YStart = 0x0
	spi3w_wdata(0x00);
  //spi3w_wdata(0x27);				//YEnd = 296(0x128) - 1 = 0x127
	//spi3w_wdata(0x01);
	spi3w_wdata(0x17);
	spi3w_wdata(0x00);

	/* RAM X address counter */
	spi3w_wcommand(0x4E);
	spi3w_wdata(0x00);
//	
	/* RAM Y address counter */
	spi3w_wcommand(0x4F);
	spi3w_wdata(0x00);
	spi3w_wdata(0x00);

//	spi3w_wcommand(0x21);
//	spi3w_wdata(0x03);	
//	spi3w_wcommand(0xF0);
//	spi3w_wdata(0x1F);

	spi3w_wcommand(0x2C);   // Write VCOM Register
	spi3w_wdata(0xA8);//A8

	WriteLUT();
	WhiteDM();
	TagPowerDownFunction();
	//powerOff();
	//printf("Paper_Init_End\r\n");
}

void ePaper_Init_03(bool fPowerDown)
{

//	uint8_t TestBuffer[9];
//	uint8_t Test_Buffer[9];
//	clock_t Start, End;
	//printf("ePaper_Init_Start\r\n");
	HWReset();		//nRST & BUSY
	/*** initial code ***/
	spi3w_wcommand(0x10);			// Exit Deep Sleep Mode
	spi3w_wdata(0x0);				
	
	/* Panel configration. */
	spi3w_wcommand(0x01);			// 
	spi3w_wdata(0x27);				// Gate: 296 = 0x127+1
	spi3w_wdata(0x01);
	spi3w_wdata(0x00);				// GD, SM, TB
	
	/* Booster Soft start Control */ 
	spi3w_wcommand(0x0C);   // Soft Start
	spi3w_wdata(0xD7);  // 0xCF
	spi3w_wdata(0xD6);  // 0xCE
	spi3w_wdata(0x9D);  // 0x8D 

	/* Gate scan start position */
	spi3w_wcommand(0x0F);//  Gate Scan Start Position
	spi3w_wdata(0x00);  //
	spi3w_wdata(0x00);  //
          
	/* Gate driving voltage control */
	spi3w_wcommand(0x03);
	spi3w_wdata(0x00); //0x75
	//spi3w_wdata(0xAA);				// VGH = 20V, VGL = -20V
//	
	/* Source driving voltage control */
	spi3w_wcommand(0x04);
	spi3w_wdata(0x0A);				// VSH/VSL = 15V

//	
//	spi3w_wcommand(0x0B);
//	spi3w_wdata(0x5); //0x75
	/* Set dummy line pulse period */
	spi3w_wcommand(0x3A);	
	spi3w_wdata(0x1A); //016 //1A
//	
	/* Set Gate line width */
	spi3w_wcommand(0x3B);
//	spi3w_wdata(0xFF);		//08
	spi3w_wdata(0x08);				// 62us
//	
	/* Set Border waveform */
	spi3w_wcommand(0x3C);
//	spi3w_wdata(0x33);	 //0x33	
	spi3w_wdata(0x00);
//	
	/** Data operations **/
	/* Data Entry mode */
	spi3w_wcommand(0x11);
	spi3w_wdata(0x5);				
//	
	/* X RAM address start/end */
	spi3w_wcommand(0x44);
	spi3w_wdata(0x00);			//XStart = 0x0
	//spi3w_wdata(0xF);			//XEnd = 128/8 = 16(0x10) - 1 = 0x0F
	spi3w_wdata(0x5);
//	
	/* Y RAM address start/end */
	spi3w_wcommand(0x45);
	spi3w_wdata(0x00);			//YStart = 0x0
	spi3w_wdata(0x00);
  //spi3w_wdata(0x27);				//YEnd = 296(0x128) - 1 = 0x127
	//spi3w_wdata(0x01);
	spi3w_wdata(0x17);
	spi3w_wdata(0x00);

	/* RAM X address counter */
	spi3w_wcommand(0x4E);
	spi3w_wdata(0x00);
//	
	/* RAM Y address counter */
	spi3w_wcommand(0x4F);
	spi3w_wdata(0x00);
	spi3w_wdata(0x00);

	spi3w_wcommand(0x2C);   // Write VCOM Register
	spi3w_wdata(0xA8);//A8
	
	spi3w_wcommand(0x3C);//board
	spi3w_wdata(0x63);
	WriteLUT();
	
	WhiteDM();
//	Print_Line(1,"JamesTsai.");
//	Print_Line(2,"JamesTsai.");
//	Print_Line(3,"JamesTsai.");
//	Print_Line(4,"JamesTsai.");
//	Print_Line(5,"JamesTsai.");
//	Print_Line(6,"DAVICOM Semiconductor Inc.");
//	Print_Line(7,"JamesTsai.");
//	Print_Line(8,"JamesTsai.");
	UpdateDisplay();
//	Write_BarCode_FULL();
//	UpdateDisplay();
	if (fPowerDown == true)
	TagPowerDownFunction();

}
#endif

void EPaperInit(int nMode, bool fPowerDown)
{
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)EPaperInit\r\n"));
	HWReset();		//nRST & BUSY
	/*** initial code ***/
	spi3w_wcommand(0x10);			// Exit Deep Sleep Mode
	spi3w_wdata(0x0);				
	
	/* Panel configration. */
	spi3w_wcommand(0x01);			// 
	spi3w_wdata(0x27);				// Gate: 296 = 0x127+1
	spi3w_wdata(0x01);
	spi3w_wdata(0x00);				// GD, SM, TB
	
	/* Booster Soft start Control */ 
	spi3w_wcommand(0x0C);   // Soft Start
	spi3w_wdata(0xD7);  // 0xCF
	spi3w_wdata(0xD6);  // 0xCE
	spi3w_wdata(0x9D);  // 0x8D 

	/* Gate scan start position */
	spi3w_wcommand(0x0F);//  Gate Scan Start Position
	spi3w_wdata(0x00);  //
	spi3w_wdata(0x00);  //
          
	/* Gate driving voltage control */
	spi3w_wcommand(0x03);
	switch(nMode)
	{
	case EPAPER_MODE_METHOD1:
		spi3w_wdata(0xAA); //0x75
		break;
	case EPAPER_MODE_METHOD2:
		spi3w_wdata(0x00); //0x75
		break;
	case EPAPER_MODE_DEFAULT: 
		spi3w_wdata(0xEA); //0x75
		break;
	}

	//spi3w_wdata(0xAA);				// VGH = 20V, VGL = -20V
	//	
	/* Source driving voltage control */
	spi3w_wcommand(0x04);
	spi3w_wdata(0x0A);				// VSH/VSL = 15V


	/* Set dummy line pulse period */
	spi3w_wcommand(0x3A);

	switch(nMode)
	{
	case EPAPER_MODE_METHOD1:
	case EPAPER_MODE_METHOD2:
		spi3w_wdata(0x16);		
		break;
	case EPAPER_MODE_DEFAULT: 
		spi3w_wdata(0x7F); //016 //1A
		break;
	}
//	
	/* Set Gate line width */
	spi3w_wcommand(0x3B);
	switch(nMode)
	{
	case EPAPER_MODE_METHOD1:
	case EPAPER_MODE_METHOD2:
		spi3w_wdata(0x08);		
		break;
	case EPAPER_MODE_DEFAULT: 
		spi3w_wdata(0x38);
		break;
	}
	
//	
	/* Set Border waveform */
//	spi3w_wcommand(0x3C);
//	switch(nMode)
//	{
//	case EPAPER_MODE_METHOD1:
//	case EPAPER_MODE_METHOD2:
//		spi3w_wdata(0x33);	 //0x33	
//		break;
//	case EPAPER_MODE_DEFAULT: 
//		spi3w_wdata(0x00);
//		break;
//	}	
	/** Data operations **/
	/* Data Entry mode */
	spi3w_wcommand(0x11);
	spi3w_wdata(0x5);				
//	
	/* X RAM address start/end */
	spi3w_wcommand(0x44);
	spi3w_wdata(0x00);			//XStart = 0x0
	//spi3w_wdata(0xF);			//XEnd = 128/8 = 16(0x10) - 1 = 0x0F
	spi3w_wdata(0x5);
//	
	/* Y RAM address start/end */
	spi3w_wcommand(0x45);
	spi3w_wdata(0x00);			//YStart = 0x0
	spi3w_wdata(0x00);
  //spi3w_wdata(0x27);				//YEnd = 296(0x128) - 1 = 0x127
	//spi3w_wdata(0x01);
	spi3w_wdata(0x17);
	spi3w_wdata(0x00);

	/* RAM X address counter */
	spi3w_wcommand(0x4E);
	spi3w_wdata(0x00);
//	
	/* RAM Y address counter */
	spi3w_wcommand(0x4F);
	spi3w_wdata(0x00);
	spi3w_wdata(0x00);

	spi3w_wcommand(0x2C);   // Write VCOM Register
	spi3w_wdata(0x88);//A8

	if (nMode == EPAPER_MODE_DEFAULT)
	{
		spi3w_wcommand(0x3C);//board
		spi3w_wdata(0x33);
	}

//	WriteLUT();
//	WhiteDM();
//	UpdateDisplay();
//	WhiteDM();
//	UpdateDisplay();
//	Write_BarCode_FULL();
//	PartialUpdateDisplay();
//	Write_QRCode_FULL();
//	UpdateDisplay();
//	PartialUpdateDisplay();
//	PartialUpdateDisplay();
//	spi3w_wcommand(0x22);
//  spi3w_wdata(0xC4);//C7 C4
//	spi3w_wcommand(0x20);

//	Print_Line_Big_Font(1,"OFFLINE");
//	Print_Line_Big_Font(2,"142_2");
//	Print_Line_Big_Font(3,"DMT2900-0200000001");
//	Print_Line_Big_Font(5,"TOM-SUN");
//	if (nMode == EPAPER_MODE_DEFAULT)
//		Write_BarCode_FULL();	
//		UpdateDisplay();

	if (fPowerDown == true)
		TagPowerDownFunction();

	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(-)EPaperInit\r\n"));
}

int LinePrint(uint8_t Line, const TCHAR *szFormat, ...)
{
	va_list arg;
	uint8_t Col;	
	char szText[80];
	int nResult = 0;
	va_start (arg, szFormat);	
	//nResult = vfprintf (stdout, szFormat, arg);		// for _MBCS
	nResult = vsprintf (szText, szFormat, arg);		// for _MBCS
	//nResult = _vftprintf (stdout, szFormat, arg);	// for TCHAR
	va_end (arg);

	for ( Col=0; Col<strlen(szText); Col++) 
		Print_Char(Col, Line, szText[Col]);	
	
	return nResult;
}

int LinePrint_Big_Font(uint8_t Line, const TCHAR *szFormat, ...)
{
	va_list arg;
	uint8_t Col;	
	char szText[80];
	int nResult = 0;
	
	int Width = 0;
	int First_Text_Width = 0;
	int Starting_Address = 0;
	Line = (Line*3) - 2;
	
	va_start (arg, szFormat);	
	//nResult = vfprintf (stdout, szFormat, arg);		// for _MBCS
	nResult = vsprintf (szText, szFormat, arg);		// for _MBCS
	//nResult = _vftprintf (stdout, szFormat, arg);	// for TCHAR
	va_end (arg);

	for (Col=0; Col<strlen(szText); Col++) 
	{
		
		if(Col == 0)
		{
			Width = custom_font_Width[szText[Col]-0x2D];
//			printf("%d ",Width);
			First_Text_Width = custom_font_Width[szText[Col]-0x2D];
			Starting_Address = 0;
			Print_Char_BIG_Font(Col, Line, Width,Starting_Address,szText[Col]);
		}
		else
		{
			Width = Width + custom_font_Width[szText[Col-1]-0x2D];// - First_Text_Width;
//			printf("%d ",Width);
			Starting_Address = Width - First_Text_Width;
			Print_Char_BIG_Font(Col, Line, Width,Starting_Address,szText[Col]);
		}
//		Print_Char_BIG(Col, Line, Width,Text[Col]);
	}
	return nResult;
}

void EPaperShowScannerStatus(const TCHAR *szScanCode, const TCHAR *szTagID, const TCHAR *szProductID, const TCHAR *szStatus, bool fCommand)
{
	WhiteDM();
	//OUTPUT_MESSAGE(FLAG_MF_APPLICATION, FLAG_MT_INFORMATION, _T("--- %s ---\r\n"), szScanCode);
	if (szScanCode != NULL)
		LinePrint(1,"Scan code: %s", szScanCode);
	else
		LinePrint(1,"Scan code: ");
	
	LinePrint(2,"");
	if (szTagID != NULL)
		LinePrint(3,"Tag ID: %s", szTagID);
	else
		LinePrint(3,"Tag ID:");
	if (fCommand == true)
	{
		if (szProductID != NULL)	
			LinePrint(4,"Command ID: %s", szProductID);
		else
			LinePrint(4,"Command ID: ");
	}
	else
	{
		if (szProductID != NULL)	
			LinePrint(4,"Product ID: %s", szProductID);
		else
			LinePrint(4,"Product ID: ");		
	}
	
	LinePrint(5,"");
	
	if (szStatus != NULL)
		LinePrint(6,"Status Message: %s", szStatus);
	else
		LinePrint(6,"Status Message: ");
	
	LinePrint(7,"");
	LinePrint(8,"");
	UpdateDisplay();	
}

void EPaperShowTagStatus(const TCHAR *szTagID, const TCHAR *szTagStatus, const TCHAR *szTagMsg1, const TCHAR *szTagMsg2)
{
	WhiteDM();
	//OUTPUT_MESSAGE(FLAG_MF_APPLICATION, FLAG_MT_INFORMATION, _T("--- %s ---\r\n"), szScanCode);
	LinePrint(1,"");	
	LinePrint(2,"");
	
	if (szTagID != NULL)
		LinePrint(3," Tag ID: %s", szTagID);
	else
		LinePrint(3," Tag ID: NULL");


	if (szTagStatus != NULL)	
		LinePrint(4," Tag Status: %s", szTagStatus);
	else
		LinePrint(4," Tag Status: NULL");

	if (szTagMsg1 != NULL)	
		LinePrint(5," Message: %s", szTagMsg1);
	else
		LinePrint(5," Message: ");		
		
	if (szTagMsg2 != NULL)
		LinePrint(6," %s", szTagMsg2);
	else
		LinePrint(6,"");
	
	LinePrint(7,"");
	LinePrint(8,"");
	UpdateDisplay();	
}

void EPaperShowTagDefaultStatus(const TCHAR *szTagID, const TCHAR *szTagStatus, const TCHAR *szTagMsg1, const TCHAR *szTagMsg2, const TCHAR *szTagMsg4)
{
	printf( "EPaperShowTagStatus.\r\n");
	TagPowerUpFunction();
	WhiteDM();
	LinePrint_Big_Font(1,"%s",szTagStatus);
//	PartialUpdateDisplay();
	LinePrint_Big_Font(2,"%s_%s_%d",szTagMsg1,szTagMsg2, GetTagWakeUpInterval());
//	PartialUpdateDisplay();
//	LinePrint_Big_Font(3,"NEW_FW0801");
	LinePrint_Big_Font(4,"%s",szTagMsg4);
	LinePrint_Big_Font(5,"DMT2900-%s",szTagID);
//	PartialUpdateDisplay();
	UpdateDisplay();	
	WhiteDM();
	LinePrint_Big_Font(1,"%s",szTagStatus);
	LinePrint_Big_Font(2,"%s_%s_%d",szTagMsg1,szTagMsg2, GetTagWakeUpInterval());
//  LinePrint_Big_Font(3,"NEW_FW0801");
	LinePrint_Big_Font(4,"%s",szTagMsg4);
	LinePrint_Big_Font(5,"DMT2900-%s",szTagID);
	TagPowerDownFunction();

	
}
