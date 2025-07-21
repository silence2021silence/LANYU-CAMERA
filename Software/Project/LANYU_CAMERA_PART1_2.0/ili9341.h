#ifndef __ILI9341_H
#define __ILI9341_H

void LCD_Init(void);
void LCD_Set_Display_Dir(uint8_t dir);
void LCD_Clear(uint16_t color);
void LCD_ShowPic_16bit(int16_t x, int16_t y, uint16_t pic_width, uint16_t pic_height, const uint16_t *pic);
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* pic);

#endif
