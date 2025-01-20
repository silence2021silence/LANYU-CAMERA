#include <stdint.h>
#include <Arduino.h>
#include "ili9341.h"

void setup() {
  LCD_Init();
}

void loop() {
  LCD_Clear(0xF800);
  LCD_Clear(0x07E0);
  LCD_Clear(0x001F);
}
