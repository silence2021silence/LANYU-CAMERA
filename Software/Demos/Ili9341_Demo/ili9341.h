#ifndef __ILI9341_H
#define __ILI9341_H

#define LCD_CS    14
#define LCD_RESET 21
#define LCD_DC    41
#define LCD_SDI   42
#define LCD_SCK   2
#define LCD_LED   1
#define LCD_SDO   -1

#define LCD_CS_HIGH     digitalWrite(LCD_CS, HIGH)
#define LCD_RESET_HIGH  digitalWrite(LCD_RESET, HIGH)
#define LCD_DC_HIGH     digitalWrite(LCD_DC, HIGH)
#define LCD_SDI_HIGH    digitalWrite(LCD_SDI, HIGH)
#define LCD_SCK_HIGH    digitalWrite(LCD_SCK, HIGH)
#define LCD_LED_HIGH    digitalWrite(LCD_LED, HIGH)
#define LCD_SDO_HIGH    digitalWrite(LCD_SDO, HIGH)
#define LCD_CS_LOW      digitalWrite(LCD_CS, LOW)
#define LCD_RESET_LOW   digitalWrite(LCD_RESET, LOW)
#define LCD_DC_LOW      digitalWrite(LCD_DC, LOW)
#define LCD_SDI_LOW     digitalWrite(LCD_SDI, LOW)
#define LCD_SCK_LOW     digitalWrite(LCD_SCK, LOW)
#define LCD_LED_LOW     digitalWrite(LCD_LED, LOW)
#define LCD_SDO_LOW     digitalWrite(LCD_SDO, LOW)

void LCD_HardwareSPI_Init(void);
void LCD_Write_SPI(uint8_t data);
void LCD_Write_Cmd(uint8_t data);
void LCD_Write_Data(uint8_t data);
void LCD_Write_GRAM_16bit(uint16_t data);
void LCD_Init(void);
void LCD_Set_Display_Dir(uint8_t dir);
void LCD_Set_Window(int16_t start_x,int16_t start_y,int16_t end_x,int16_t end_y);
void LCD_Clear(uint16_t color);
void LCD_ShowPic_16bit(int16_t x, int16_t y, uint16_t pic_width, uint16_t pic_height, const uint8_t *pic);

#endif
