#include <stdint.h>
#include <Arduino.h>
#include "camera.h"
#include "ili9341.h"

void setup() {
  // 初始化串口
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  Camera_Init();
  LCD_Init();
  LCD_Set_Display_Dir(1);
}

void loop() {
  Camera2LCD_Jpeg2RGB565();
  Camera_Adjuster();
  Take_Picture_Key();
}
