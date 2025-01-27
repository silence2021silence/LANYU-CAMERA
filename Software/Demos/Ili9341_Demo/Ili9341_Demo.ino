#include <stdint.h>
#include <Arduino.h>
#include "ili9341.h"
#include "pic1.h"
#include "pic2.h"

void setup() {
  LCD_Init();
}

void loop() {
  LCD_Set_Display_Dir(0);
  LCD_Clear(0xF800);
  LCD_Clear(0x07E0);
  LCD_Clear(0x001F);
  LCD_ShowPic_16bit(0, 0, 240, 320, gImage_pic1);
  delay(1000);
  LCD_Set_Display_Dir(1);
  LCD_Clear(0xF800);
  LCD_Clear(0x07E0);
  LCD_Clear(0x001F);
  LCD_ShowPic_16bit(0, 0, 320, 240, gImage_pic2);
}
