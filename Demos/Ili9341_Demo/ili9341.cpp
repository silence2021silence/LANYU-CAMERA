#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>
#include "ili9341.h"

SPIClass *fspi = nullptr;

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

  LCD_RESET_HIGH;
  delay(5); 
  LCD_RESET_LOW;
  delay(15);
  LCD_RESET_HIGH;
  delay(15);

  LCD_Write_Cmd(0xCB);  
  LCD_Write_Data(0x39); 
  LCD_Write_Data(0x2C); 
  LCD_Write_Data(0x00); 
  LCD_Write_Data(0x34); 
  LCD_Write_Data(0x02); 

  LCD_Write_Cmd(0xCF);  
  LCD_Write_Data(0x00); 
  LCD_Write_Data(0XC1); 
  LCD_Write_Data(0X30); 

  LCD_Write_Cmd(0xE8);  
  LCD_Write_Data(0x85); 
  LCD_Write_Data(0x00); 
  LCD_Write_Data(0x78); 

  LCD_Write_Cmd(0xEA);  
  LCD_Write_Data(0x00); 
  LCD_Write_Data(0x00); 

  LCD_Write_Cmd(0xED);  
  LCD_Write_Data(0x64); 
  LCD_Write_Data(0x03); 
  LCD_Write_Data(0X12); 
  LCD_Write_Data(0X81); 

  LCD_Write_Cmd(0xF7);  
  LCD_Write_Data(0x20); 

  LCD_Write_Cmd(0xC0);    //Power control 
  LCD_Write_Data(0x23);   //VRH[5:0] 

  LCD_Write_Cmd(0xC1);    //Power control 
  LCD_Write_Data(0x10);   //SAP[2:0];BT[3:0] 

  LCD_Write_Cmd(0xC5);    //VCM control 
  LCD_Write_Data(0x3E);   //Contrast
  LCD_Write_Data(0x28); 

  LCD_Write_Cmd(0xC7);    //VCM control2 
  LCD_Write_Data(0x86);   //--

  LCD_Write_Cmd(0x36);    // Memory Access Control 
  LCD_Write_Data(0x48);   

  LCD_Write_Cmd(0x3A);    
  LCD_Write_Data(0x55); 

  LCD_Write_Cmd(0xB1);    
  LCD_Write_Data(0x00);  
  LCD_Write_Data(0x18); 

  LCD_Write_Cmd(0xB6);    // Display Function Control 
  LCD_Write_Data(0x08); 
  LCD_Write_Data(0x82);
  LCD_Write_Data(0x27);  

  LCD_Write_Cmd(0x11);    //Exit Sleep 
  delay(120); 
      
  LCD_Write_Cmd(0x29);    //Display on 
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
	LCD_Set_Window(0,0,240-1,320-1);
  LCD_Write_Cmd(0x2C);
	for(i=0;i<240;i++)
  {
    for(j=0;j<320;j++)
    {
      LCD_Write_GRAM_16bit(color);
    }
  }
}

