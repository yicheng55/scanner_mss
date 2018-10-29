#include "EPaperSSD1675A.h"
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


const uint8_t Look_Up_Table[] =	
{

//0xA5,	0x89,	0x10,	0x00,	0x00,	0x00,	0x00,	0xA5,	0x19,	0x80,	0x00,	0x00,	0x00,	0x00,	0xA5,	0xA9,
//0x9B,	0x00,	0x00,	0x00,	0x00,	0xA5,	0xA9,	0x9B,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
//0x00,	0x00,	0x00,	0x0F,	0x0F,	0x0F,	0x0F,	0x02,	0x10,	0x10,	0x0A,	0x0A,	0x03,	0x08,	0x08,	0x09,
//0x43,	0x07,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
//0x00,	0x00,	0x00,	0x00,	0x00,	0x00,										

//0xAA,	0x99,	0x10,	0x00,	0x00,	0x00,	0x00,	0x55,	0x99,	0x80,	0x00,	0x00,	0x00,	0x00,	0x8A,	0xA8,
//0x9B,	0x00,	0x00,	0x00,	0x00,	0x8A,	0xA8,	0x9B,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
//0x00,	0x00,	0x00,	0x0F,	0x0F,	0x0F,	0x0F,	0x02,	0x14,	0x14,	0x14,	0x14,	0x06,	0x14,	0x14,	0x0C,
//0x82,	0x08,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
//0x00,	0x00,	0x00,	0x00,	0x00,	0x00,				


//---2.9--Normal--//
0x22,	0x11,	0x10,	0x00,	0x10,	0x00,	0x00,	0x11,	0x88,	0x80,	0x80,	0x80,	0x00,	0x00,	0x6A,	0x9B,
0x9B,	0x9B,	0x9B,	0x00,	0x00,	0x6A,	0x9B,	0x9B,	0x9B,	0x9B,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x04,	0x18,	0x04,	0x16,	0x01,	0x0A,	0x0A,	0x0A,	0x0A,	0x02,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x04,	0x04,	0x08,	0x3C,	0x07,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,

//---2.9--Low_Temperature--//
//0x66,	0x50,	0x10,	0x00,	0x10,	0x00,	0x00,	0x99,	0x0A,	0x80,	0x00,	0x80,	0x00,	0x00,	0xA6,	0x5A,
//0x9B,	0x9B,	0x9B,	0xBB,	0x00,	0xA6,	0x5A,	0x9B,	0x9B,	0x9B,	0xBB,	0x00,	0x00,	0x00,	0x00,	0x00,
//0x00,	0x00,	0x00,	0x0E,	0x1B,	0x0A,	0x11,	0x09,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
//0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x14,	0x14,	0x05,	0x6A,	0x09,	0x00,	0x00,	0x00,	0x00,
//0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	

//0xA5,	0x89,	0x10,	0x00,	0x00,	0x00,	0x00,	0xA5,	0x19,	0x80,	0x00,	0x00,	0x00,	0x00,	0xA5,	0xA9,
//0x9B,	0x00,	0x00,	0x00,	0x00,	0xA5,	0xA9,	0x9B,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
//0x00,	0x00,	0x00,	0x0F,	0x0F,	0x0F,	0x0F,	0x02,	0x10,	0x10,	0x0A,	0x0A,	0x03,	0x08,	0x08,	0x09,
//0x43,	0x07,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
//0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
};	

void Hardware_Reset(void)
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
//	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)READBUSY\r\n"));
  while(1)
  {	 
     if(GPIO0->DATA_PIN.P13 == 0)
		 {
			 break;
		 }			 
  }      
//  Delay1us(1000); //200000
//	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(-)READBUSY\r\n"));
}

void Software_Reset(void)
{
	spi3w_wcommand(0x12);
}

void UpdateDisplay(void)
{
//	 spi3w_wcommand(0x21);
//  spi3w_wdata(0x00);
  spi3w_wcommand(0x22);
  spi3w_wdata(0xC7);//C7 C4
	spi3w_wcommand(0x20);
//	Delay1us(200);		//1ms         //200 FAST
	READBUSY();
//	Delay1us(250);	//300ms 300000  //250 FAST
	
//	Delay1us(250);	//300ms 300000
//	Delay1us(250);	//300ms 300000
//	Delay1us(250);	//300ms 300000
//	Delay1us(250);	//300ms 300000
}

void WriteLUT()
{

  uint8_t length = 70;
  uint8_t i = 0;

	SPI_SS_LOW(SPI);
	SPI_WRITE_TX(SPI,0x32);
  while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;	

  while(length)
  {
        SPI_WRITE_TX(SPI,*(Look_Up_Table + i) | 0x100); i++;

        while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;
        length--;
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
	Hardware_Reset();
	spi3w_wcommand(0x10);			
	spi3w_wdata(0x0);				// EPD exit Deep Sleep Mode
}



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

	SPI_WRITE_TX(SPI,0x26);
  while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;	

  while(TotalLength)
  {
		    bitmap = (uint32_t)(*(Lost_Beacon + j)) | 0x100;
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
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)WHITE\r\n"));
	spi3w_wcommand(0x11);
	spi3w_wdata(0x03);	
	SetWinAddr(0x00, 0X0F, 0x00, 0x127, 0, 0x00);

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
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(-)WHITE\r\n"));
}

void RedDM(void)
{
	uint32_t length = 4736;
	uint32_t bitmap = 0x00 | 0x100;
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)RED\r\n"));
	spi3w_wcommand(0x11);
	spi3w_wdata(0x03);	
//	SetWinAddr(0x00, 12, 0x00, 211, 0, 0x00);
	SetWinAddr(0x00, 0X0F, 0x00, 0x127, 0, 0x00);

	SPI_SS_LOW(SPI);

	SPI_WRITE_TX(SPI,0x26);
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
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(-)RED\r\n"));
}

void CleanEPD()
{
	WhiteDM();
	RedDM();
}

void WriteRAMqr_FULL(int TotalLength, uint8_t* ReceiveData)//examplePix
{
	int j = 0;
	uint32_t bitmap = 0;

	SetWinAddr(0x0, 0xF, 0x127, 0x0, 0, 0x127);
//	printf("WriteRAMqr_FULL..\r\n");
	SPI_SS_LOW(SPI);

	SPI_WRITE_TX(SPI,0x26);
	while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;	

	while(TotalLength)
	{
//		bitmap = (uint32_t)~(*(ReceiveData + j)) | 0x100;
		bitmap = (uint32_t)(*(ReceiveData + j)) | 0x100;
		SPI_WRITE_TX(SPI, bitmap); j++;

        while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;
		TotalLength--;
	}
	SPI_SS_HIGH(SPI);
}

void Write_BarCode_FULL(uint8_t color, uint8_t* ReceiveData)//examplePix
{
	int j = 0;
	uint32_t bitmap = 0;
	int TotalLength = 2756;

//	SetWinAddr(0x0, 0xF, 0x127, 0x0, 0, 0x127);
//	SetWinAddr(0x00, 12, 0x00, 211, 0, 0x00);
	SetWinAddr(0x00, 12, 211, 0, 0, 211);
	printf("Write_BarCode_FULL..\r\n");
	SPI_SS_LOW(SPI);

	
	if(color == EPAPER_COLOR_BLACK_WHITE)
	{
		SPI_WRITE_TX(SPI,0x24); //For B/W
	}
	else
	{
		SPI_WRITE_TX(SPI,0x26); //for RED
	}
  while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;	

  while(TotalLength)
  {
		if(color == EPAPER_COLOR_BLACK_WHITE)
		{
			bitmap = (uint32_t)~(*(ReceiveData + j)) | 0x100; //For B/W
		}
		else
		{
			bitmap = (uint32_t)(*(ReceiveData + j)) | 0x100; //for RED
		}
		
		    SPI_WRITE_TX(SPI, bitmap); j++;
        while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;
        TotalLength--;
  }

	SPI_SS_HIGH(SPI);
}


void Write_EPD_FULL(uint8_t color, int TotalLength, uint8_t* ReceiveData)  //examplePix
{
	int j = 0;
	uint32_t bitmap = 0;
//	int TotalLength = 4736;

	SetWinAddr(0x0, 0xF, 0x127, 0x0, 0, 0x127);
//	SetWinAddr(0x00, 12, 0x00, 211, 0, 0x00);
//	SetWinAddr(0x00, 12, 211, 0, 0, 211);
	printf("Write_BarCode_FULL..\r\n");
	SPI_SS_LOW(SPI);

	
	if(color == EPAPER_COLOR_BLACK_WHITE)
	{
		SPI_WRITE_TX(SPI,0x24); //For B/W
	}
	else
	{
		SPI_WRITE_TX(SPI,0x26); //for RED
	}
  while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;	

  while(TotalLength)
  {
		if(color == EPAPER_COLOR_BLACK_WHITE)
		{
			bitmap = (uint32_t)~(*(ReceiveData + j)) | 0x100; //For B/W
		}
		else
		{
			bitmap = (uint32_t)(*(ReceiveData + j)) | 0x100; //for RED
		}
		
		    SPI_WRITE_TX(SPI, bitmap); j++;
        while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;
        TotalLength--;
  }

	SPI_SS_HIGH(SPI);
}

//void Write_BarCode_FULL()//examplePix
//{
//	int j = 0;
//	uint32_t bitmap = 0;
//	int TotalLength = 2756;
//	
//	spi3w_wcommand(0x11);
//	spi3w_wdata(0x05);	
//	SetWinAddr(0x00, 12, 0x00, 211, 0, 0x00);
//	printf("Write_BarCode_FULL..\r\n");
//	SPI_SS_LOW(SPI);

//	SPI_WRITE_TX(SPI,0x24);
//  while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;	

//  while(TotalLength)
//  {
//		    bitmap = (uint32_t)(*(EPD_DATA_213_INCH + j)) | 0x100;
//				printf("%x ", bitmap);
//		    SPI_WRITE_TX(SPI, bitmap); j++;

//        while(0 == (SPI->FIFOTXSTUS & SPI_FIFOTXSTUS_EMPTY_Msk))    ;
//        TotalLength--;
//  }

//	SPI_SS_HIGH(SPI);
//	
//	spi3w_wcommand(0x11);
//	spi3w_wdata(0x5);
//}
	
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

//			spi3w_wcommand(0x24);
//			spi3w_wdata(~tmp);
			spi3w_wcommand(0x26);
			spi3w_wdata(tmp);
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
void dis_img(unsigned char num)
{
	unsigned int row, col;
	unsigned int pcnt;
/***************************************************bw image****************************************************/
	SetWinAddr(0x00, 12, 0x00, 211, 0, 0x00);
	spi3w_wcommand(0x24);

	
	pcnt = 0;											
	for(col=0; col<212; col++)							
	{
		for(row=0; row<13; row++)						
		{
				switch (num)
				{
				case PIC_A:
					
         spi3w_wdata(gImage_mb_bw[pcnt]);                 
					break;	
                                        
                                        
          case PIC_B:
					spi3w_wdata(gImage_HStest_bw[pcnt]);
					break;
                                         
          case PIC_C:
					spi3w_wdata(pic_black[pcnt]);
          break;


					case PIC_R:
							spi3w_wdata(0x00);
							 break;
           case PIC_VLINE:
            if(col>148)
					  spi3w_wdata(0xff);
             else
            spi3w_wdata(0x00);
					break;
                                        
                                       case PIC_HLINE:
                                          if(row>8)
					  spi3w_wdata(0xff);
                                         else if(row==8)
					  spi3w_wdata(0x0f);
                                         else
                                          spi3w_wdata(0x00);
                                       /* if((row%2)<1)
					  SPI4W_WRITEDATA(0xff);
                   
                                          else
                                          SPI4W_WRITEDATA(0x00);*/
					break;	
					
				case PIC_WHITE:
					spi3w_wdata(0xff);
					break;	

				case PIC_BLACK:
					spi3w_wdata(0x00);
					break;	
			   	default:
					break;
				}
			pcnt++;
		}
	}
  /* if(num==PIC_BLACK||num==PIC_WHITE||num==PIC_HLINE||num==PIC_VLINE)
        {
      //  SPI4W_WRITECOM(0x21);
     //   SPI4W_WRITEDATA(0x04);  
        SPI4W_WRITECOM(0x22);
        SPI4W_WRITEDATA(0xc7);
	SPI4W_WRITECOM(0x20);
	DELAY_mS(1);
	READBUSY(); 
        return;
        }
     */
             
/***************************************************bw image****************************************************/

	SetWinAddr(0x00, 12, 0x00, 211, 0, 0x00);
	spi3w_wcommand(0x26);
	
	pcnt = 0;										
	for(col=0; col<212; col++)							
	{
		for(row=0; row<13; row++)					
		{
				switch (num)
				{
				case PIC_A:
				
                                          spi3w_wdata(gImage_mb_red[pcnt]);
					break;	
                                        
                                        
                                  case PIC_B:
					spi3w_wdata(gImage_HStest_red[pcnt]);
                                        break;

							           case PIC_C:
					spi3w_wdata(pic_red[pcnt]);
                                        break;

                                  case PIC_R:
                                         spi3w_wdata(0xFF);
                                       break;
                                  case PIC_VLINE:
                                        spi3w_wdata(0x00);
                                        break;
                                       case PIC_HLINE:
                                     
					  spi3w_wdata(0x00);
                                        
					break;	
					
				case PIC_WHITE:
					spi3w_wdata(0x00);
					break;	

				case PIC_BLACK:
					spi3w_wdata(0x00);
					break;	
			   	default:
					break;
				}
			pcnt++;
		}
	}
        		
        
	UpdateDisplay();
	

}

 void dis_block(unsigned char xs,unsigned char xe,unsigned int ys,unsigned int ye, unsigned char dat, unsigned char mode)

  {	 
	unsigned int row, col;
	unsigned int pcnt;
/***************************************************bw image****************************************************/

	SetWinAddr(xs, xe, ys, ye, xs, ye-ys);
	spi3w_wcommand(0x24);

	
	pcnt = 0;										
	for(col=ys; col<=ye; col++)							
	{
		for(row=xs; row<=xe; row++)					
		{
			
					spi3w_wdata(dat);

		}
			pcnt++;
		
	}


	SetWinAddr(xs, xe, ys, ye, xs, ye-ys);
	spi3w_wcommand(0x26);
	
	pcnt = 0;										
	for(col=ys; col<=ye; col++)						
	{
		for(row=xs; row<=xe; row++)					
		{
					 if(mode==0x26)
					spi3w_wdata(dat);
					else
					spi3w_wdata(0x00);
		}
			pcnt++;
		
	}


   UpdateDisplay();

  }

	
void Demo_Mode()
{
	while(0)
	{


//		dis_img(PIC_C);
//		Delay1ms(3000);
//               
//		spi3w_wcommand(0x21);
//		spi3w_wdata(0x08); 

//    UpdateDisplay();
//		Delay1ms(3000);


//    spi3w_wcommand(0x21);
//    spi3w_wdata(0x00); 

//               
//    dis_img(PIC_WHITE);
//	 	Delay1ms(2000);


//	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)PIC_BLACK\r\n"));
// 	dis_img(PIC_BLACK);
//         Delay1ms(2000);

//		 dis_block(4,9,0+5,40+5, 0xff, 0x24);
//          Delay1ms(2000);

//		 dis_block(4,9,0+5,40+5, 0xff, 0x26);
//          Delay1ms(2000);

//		ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)PIC_WHITE\r\n"));
//    dis_img(PIC_WHITE);
//	 	Delay1ms(2000);


//		dis_block(0,4,0,40, 0x00, 0x24);
//          Delay1ms(2000);

//		 dis_block(0,4,0,40, 0xff, 0x26);
//          Delay1ms(2000);


//		ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)PIC_R\r\n"));
//		dis_img(PIC_R);
//		Delay1ms(2000);

//   		dis_block(0,4,0,40,  0x00, 0x24);
//          Delay1ms(2000);

//		 dis_block(0,4,0,40, 0xff, 0x24);
//         Delay1ms(2000);



//      dis_img(PIC_VLINE);
//		DELAY_S(2);
//		dis_img(PIC_HLINE);
//		DELAY_S(2);

    spi3w_wcommand(0x11);
		spi3w_wdata(0x5);		   
//    ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)PIC_A\r\n"));     
//		dis_img(PIC_A);
//		Delay1ms(3000);
//		
//    ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)PIC_B\r\n"));          
//    dis_img(PIC_B);
//		Delay1ms(2000);
		
		ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)PIC_C\r\n"));    
		Write_BarCode_FULL(EPAPER_COLOR_BLACK_WHITE,EPD_DATA_213_INCH_BW);
		UpdateDisplay();
//    dis_img(PIC_B);
		Delay1ms(2000);
         
                        
	}
}
#endif



void EPaperInit(int nMode, bool fPowerDown)
{
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)EPaperInit_1675A\r\n"));
	Hardware_Reset();		//nRST & BUSY
	Software_Reset();
	/*** initial code ***/
	spi3w_wcommand(0x10);			// Exit Deep Sleep Mode
	spi3w_wdata(0x0);		
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)001\r\n"));
	Software_Reset();
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)002\r\n"));
	READBUSY();
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)003\r\n"));
	/* Panel configration. */
	spi3w_wcommand(0x74);			// 
	spi3w_wdata(0x54);				// Set MUX as 212
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)004\r\n"));
	spi3w_wcommand(0x7E);
	spi3w_wdata(0x3B);				// GD, SM, TB	
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)005\r\n"));

//	//---------------------------//
//	spi3w_wcommand(0x22);
//	spi3w_wdata(0x80);
//	spi3w_wcommand(0x20);
//	READBUSY();
//	//---------------------------//
	/* Panel configration. */
#if 1	
	spi3w_wcommand(0x01);			// 
	spi3w_wdata(0x27);				// Set MUX as 212
	spi3w_wdata(0x01);
	spi3w_wdata(0x00);				// GD, SM, TB
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)006\r\n"));
#endif	
	/* Booster Soft start Control */ 
//	spi3w_wcommand(0x0C);   // Soft Start
//	spi3w_wdata(0xD7);  // 0xCF
//	spi3w_wdata(0xD6);  // 0xCE
//	spi3w_wdata(0x9D);  // 0x8D 

	/* Gate scan start position */
//	spi3w_wcommand(0x0F);//  Gate Scan Start Position
//	spi3w_wdata(0x00);  //
//	spi3w_wdata(0x00);  //
          
	/* Gate driving voltage control */
//	spi3w_wcommand(0x03);
//	switch(nMode)
//	{
//	case EPAPER_MODE_METHOD1:
//		spi3w_wdata(0xAA); //0x75
//		break;
//	case EPAPER_MODE_METHOD2:
//		spi3w_wdata(0x00); //0x75
//		break;
//	case EPAPER_MODE_DEFAULT: 
//		spi3w_wdata(0xEA); //0x75
//		break;
//	}

	//spi3w_wdata(0xAA);				// VGH = 20V, VGL = -20V
	//	

	/* Set dummy line pulse period */
#if 1	
	spi3w_wcommand(0x3A);
	switch(nMode)
	{
	case EPAPER_MODE_METHOD1:
	case EPAPER_MODE_METHOD2:
		spi3w_wdata(0x16);		
		break;
	case EPAPER_MODE_DEFAULT: 
		spi3w_wdata(0x07); //016 //1A   //0x25 //7
		break;
	}
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)008\r\n"));
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
		spi3w_wdata(0x04); //0x06  //4
		break;
	}
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)009\r\n"));
#endif	
//	
//	/* Set Border waveform */
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
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)010\r\n"));
	/** Data operations **/
	/* Data Entry mode */
	spi3w_wcommand(0x11);
	spi3w_wdata(0x5);		
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)011\r\n"));	
//	
	/* X RAM address start/end */
	spi3w_wcommand(0x44);
	spi3w_wdata(0x00);			//XStart = 0x0
	spi3w_wdata(0x0F);
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)012\r\n"));
//	
	/* Y RAM address start/end */
	spi3w_wcommand(0x45);
	spi3w_wdata(0x00);			//YStart = 0x0
	spi3w_wdata(0x00);
	spi3w_wdata(0x27);
	spi3w_wdata(0x01);
//	spi3w_wdata(0x00);
//	/* RAM X address counter */
//	spi3w_wcommand(0x4E);
//	spi3w_wdata(0x00);
//	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)013\r\n"));
	
		/* Source driving voltage control */
#if 1		
	spi3w_wcommand(0x04);
	spi3w_wdata(0x41);				// VSH/VSL = 15V //2D 41 
	spi3w_wdata(0xa8);                         //B2 A8
	spi3w_wdata(0x32);                         //22 32
	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)007\r\n"));
#endif	
//	
	/* RAM X address counter */
	spi3w_wcommand(0x4E);
	spi3w_wdata(0x00);
	/* RAM Y address counter */
	spi3w_wcommand(0x4F);
	spi3w_wdata(0x00);
	spi3w_wdata(0x00);
ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)014\r\n"));
	spi3w_wcommand(0x2C);   // Write VCOM Register
	spi3w_wdata(0x38);//A8 //3C//50 //38
#if 0

ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)015\r\n"));

	if (nMode == EPAPER_MODE_DEFAULT)
	{
		spi3w_wcommand(0x3C);//board
		spi3w_wdata(0xB0);//0x33
	}
#endif	
//	spi3w_wcommand(0x30);
//	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)015\r\n"));
//	spi3w_wcommand(0x22);
//	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)016\r\n"));
//	spi3w_wdata(0x01);
//	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)017\r\n"));
//	spi3w_wcommand(0x20);
//	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)018\r\n"));
	

//	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)017\r\n"));
//	WhiteDM();
//	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)018\r\n"));
//	RedDM();

//	if (nMode == EPAPER_MODE_DEFAULT)
//		UpdateDisplay();
//	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(+)019\r\n"));
		WriteLUT();
//	Demo_Mode();
//	Write_EPD_FULL(EPAPER_COLOR_BLACK_WHITE,EPD_DATA_290_INCH);
//Write_EPD_FULL(EPAPER_COLOR_RED,4736,EPD_DATA_290_INCH_RED);
////	UpdateDisplay();
////	Write_EPD_FULL(EPAPER_COLOR_RED,EPD_DATA_290_INCH_RED);
//	Write_EPD_FULL(EPAPER_COLOR_RED,4736,EPD_DATA_290_INCH_RED);
////	UpdateDisplay();
//	
////	spi3w_wcommand(0x2C);   // Write VCOM Register
////	spi3w_wdata(0x00);//A8 //3C
////	Write_BarCode_FULL(EPAPER_COLOR_BLACK_WHITE,EPD_DATA_213_INCH);
//	UpdateDisplay();
//	Write_BarCode_FULL(EPAPER_COLOR_RED,EPD_DATA_213_INCH_RED);
//	UpdateDisplay();
	
//	TagPowerDownFunction();
//	while(1);
	if (fPowerDown == true)
		TagPowerDownFunction();

	ENTRY_MESSAGE(FLAG_MF_APPLICATION, _T("(-)EPaperInit_1675A\r\n"));
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
//	printf( "EPaperShowTagStatus.\r\n");
	TagPowerUpFunction();
	WhiteDM();
	RedDM();

	LinePrint_Big_Font(1,"%s",szTagStatus);
	LinePrint_Big_Font(2,"%s_%s_%d",szTagMsg1,szTagMsg2, GetTagWakeUpInterval());
//	LinePrint_Big_Font(3,"RED_FW");
	LinePrint_Big_Font(4,"%s",szTagMsg4);
	LinePrint_Big_Font(5,"DMT2901-%s",szTagID);	
	UpdateDisplay();	
	LinePrint_Big_Font(1,"%s",szTagStatus);
	LinePrint_Big_Font(2,"%s_%s_%d",szTagMsg1,szTagMsg2, GetTagWakeUpInterval());
//	LinePrint_Big_Font(3,"RED_FW");
	LinePrint_Big_Font(4,"%s",szTagMsg4);
	LinePrint_Big_Font(5,"DMT2901-%s",szTagID);
	TagPowerDownFunction();

}
