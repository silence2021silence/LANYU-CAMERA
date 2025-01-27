#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>
#include "ili9341.h"

SPIClass *fspi = nullptr;
uint16_t LCD_WIDTH=240;
uint16_t LCD_HEIGHT=320;

// 初始化SPI总线
void LCD_HardwareSPI_Init(void)
{
  fspi = new SPIClass(FSPI);
  fspi->begin(LCD_SCK, LCD_SDO, LCD_SDI, -1); //SCLK, MISO, MOSI, CS
  fspi->beginTransaction(SPISettings(40000000, MSBFIRST, SPI_MODE0));
}

void LCD_Write_SPI(int8_t data)
{
  LCD_CS_LOW;
  fspi->transfer(data);
  LCD_CS_HIGH;
}

// 写命令
void LCD_Write_Cmd(int8_t data)
{
  LCD_DC_LOW;
  LCD_Write_SPI(data);
}

// 写数据
void LCD_Write_Data(int8_t data)
{
  LCD_DC_HIGH;
  LCD_Write_SPI(data);
}

// 写显存
void LCD_Write_GRAM_16bit(int16_t data)
{
  LCD_Write_Data(data>>8);
  LCD_Write_Data(data);
}

// 初始化屏幕
void LCD_Init(void)
{
  LCD_HardwareSPI_Init();

  pinMode(LCD_CS,OUTPUT);
  pinMode(LCD_RESET,OUTPUT);
  pinMode(LCD_DC,OUTPUT);
  pinMode(LCD_LED,OUTPUT);
  pinMode(LCD_SDO,OUTPUT);

  LCD_CS_HIGH;
  LCD_RESET_HIGH;
  LCD_DC_HIGH;
  LCD_LED_HIGH;
  LCD_SDO_HIGH;

  LCD_RESET_LOW;
  delay(100);
  LCD_RESET_HIGH;
  delay(50);

  LCD_Write_Cmd(0xCF);  
	LCD_Write_Data(0x00); 
	LCD_Write_Data(0xD9); //0xC1 
	LCD_Write_Data(0X30); 
	LCD_Write_Cmd(0xED);  
	LCD_Write_Data(0x64); 
	LCD_Write_Data(0x03); 
	LCD_Write_Data(0X12); 
	LCD_Write_Data(0X81); 
	LCD_Write_Cmd(0xE8);  
	LCD_Write_Data(0x85); 
	LCD_Write_Data(0x10); 
	LCD_Write_Data(0x7A); 
	LCD_Write_Cmd(0xCB);  
	LCD_Write_Data(0x39); 
	LCD_Write_Data(0x2C); 
	LCD_Write_Data(0x00); 
	LCD_Write_Data(0x34); 
	LCD_Write_Data(0x02); 
	LCD_Write_Cmd(0xF7);  
	LCD_Write_Data(0x20); 
	LCD_Write_Cmd(0xEA);  
	LCD_Write_Data(0x00); 
	LCD_Write_Data(0x00); 
	LCD_Write_Cmd(0xC0);    //Power control 
	LCD_Write_Data(0x1B);   //VRH[5:0] 
	LCD_Write_Cmd(0xC1);    //Power control 
	LCD_Write_Data(0x12);   //SAP[2:0];BT[3:0] 0x01
	LCD_Write_Cmd(0xC5);    //VCM control 
	LCD_Write_Data(0x08); 	 //30
	LCD_Write_Data(0x26); 	 //30
	LCD_Write_Cmd(0xC7);    //VCM control2 
	LCD_Write_Data(0XB7); 
	LCD_Write_Cmd(0x36);    // Memory Access Control 
	LCD_Write_Data(0x08); 
	LCD_Write_Cmd(0x3A);   
	LCD_Write_Data(0x55); 
	LCD_Write_Cmd(0xB1);   
	LCD_Write_Data(0x00);   
	LCD_Write_Data(0x1A); 
	LCD_Write_Cmd(0xB6);    // Display Function Control 
	LCD_Write_Data(0x0A); 
	LCD_Write_Data(0xA2); 
	LCD_Write_Cmd(0xF2);    // 3Gamma Function Disable 
	LCD_Write_Data(0x00); 
	LCD_Write_Cmd(0x26);    //Gamma curve selected 
	LCD_Write_Data(0x01); 
	LCD_Write_Cmd(0xE0);    //Set Gamma 
	LCD_Write_Data(0x0F); 
	LCD_Write_Data(0x1D); 
	LCD_Write_Data(0x1A); 
	LCD_Write_Data(0x0A); 
	LCD_Write_Data(0x0D); 
	LCD_Write_Data(0x07); 
	LCD_Write_Data(0x49); 
	LCD_Write_Data(0X66); 
	LCD_Write_Data(0x3B); 
	LCD_Write_Data(0x07); 
	LCD_Write_Data(0x11); 
	LCD_Write_Data(0x01); 
	LCD_Write_Data(0x09); 
	LCD_Write_Data(0x05); 
	LCD_Write_Data(0x04); 		 
	LCD_Write_Cmd(0XE1);    //Set Gamma 
	LCD_Write_Data(0x00); 
	LCD_Write_Data(0x18); 
	LCD_Write_Data(0x1D); 
	LCD_Write_Data(0x02); 
	LCD_Write_Data(0x0F); 
	LCD_Write_Data(0x04); 
	LCD_Write_Data(0x36); 
	LCD_Write_Data(0x13); 
	LCD_Write_Data(0x4C); 
	LCD_Write_Data(0x07); 
	LCD_Write_Data(0x13); 
	LCD_Write_Data(0x0F); 
	LCD_Write_Data(0x2E); 
	LCD_Write_Data(0x2F); 
	LCD_Write_Data(0x05); 
	LCD_Write_Cmd(0x2B); 
	LCD_Write_Data(0x00);
	LCD_Write_Data(0x00);
	LCD_Write_Data(0x01);
	LCD_Write_Data(0x3f);
	LCD_Write_Cmd(0x2A); 
	LCD_Write_Data(0x00);
	LCD_Write_Data(0x00);
	LCD_Write_Data(0x00);
	LCD_Write_Data(0xef);	 
	LCD_Write_Cmd(0x11); //Exit Sleep
	delay(120);
	LCD_Write_Cmd(0x29); //display on
}

// 设置屏幕方向
// dir:0:0度; 1:90度; 2:180度; 3:270度
void LCD_Set_Display_Dir(uint8_t dir)
{
  switch (dir) {
  case 0:
    LCD_WIDTH=240;
    LCD_HEIGHT=320;
    LCD_Write_Cmd(0x36);
    LCD_Write_Data(0x08);  // MY->0 MX->0 MV->0 BGR->1  0000 1000
    break;
  case 1:
    LCD_WIDTH=320;
    LCD_HEIGHT=240;
    LCD_Write_Cmd(0x36);
    LCD_Write_Data(0x68);  // MY->0 MX->1 MV->1 BGR->1  0110 1000
    break;
  case 2:
    LCD_WIDTH=240;
    LCD_HEIGHT=320;
    LCD_Write_Cmd(0x36);
    LCD_Write_Data(0xC8);  // MY->1 MX->1 MV->0 BGR->1  1100 1000
    break;
  case 3:
    LCD_WIDTH=320;
    LCD_HEIGHT=240;
    LCD_Write_Cmd(0x36);
    LCD_Write_Data(0xA8);  // MY->1 MX->0 MV->1 BGR->1  1010 1000
    break;
  }
}

// 设置窗口范围
// start_x:起点x
// start_y:起点y
// end_x:终点x
// end_y:终点y
void LCD_Set_Window(int16_t start_x,int16_t start_y,int16_t end_x,int16_t end_y)
{
  LCD_Write_Cmd(0x2A); 
  LCD_Write_Data(start_x>>8); 
  LCD_Write_Data(start_x&0XFF);	 
  LCD_Write_Data(end_x>>8); 
  LCD_Write_Data(end_x&0XFF);  

  LCD_Write_Cmd(0x2B); 
  LCD_Write_Data(start_y>>8); 
  LCD_Write_Data(start_y&0XFF); 
  LCD_Write_Data(end_y>>8); 
  LCD_Write_Data(end_y&0XFF); 
}

// 清屏
// color:填充色
void LCD_Clear(uint16_t color)
{
  uint16_t i,j;
	LCD_Set_Window(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
  LCD_Write_Cmd(0x2C);
	for(i=0;i<LCD_WIDTH;i++)
  {
    for(j=0;j<LCD_HEIGHT;j++)
    {
      LCD_Write_GRAM_16bit(color);
    }
  }
}

// 显示16位图片
// x:起点x
// y:起点y
// pic_width:图片宽度
// pic_height:图片高度
// pic:图片数据
void LCD_ShowPic_16bit(uint16_t x, uint16_t y, uint16_t pic_width, uint16_t pic_height, const uint8_t *pic)
{
  int i;
  uint8_t picH, picL;
  LCD_Set_Window(x, y, x+pic_width-1, y+pic_height-1);
  LCD_Write_Cmd(0x2C);
  for(i=0; i<pic_width*pic_height; i++)
  {
    picH = *(pic+i*2);
    picL = *(pic+i*2+1);
    LCD_Write_GRAM_16bit(picH<<8|picL);
  }
}
