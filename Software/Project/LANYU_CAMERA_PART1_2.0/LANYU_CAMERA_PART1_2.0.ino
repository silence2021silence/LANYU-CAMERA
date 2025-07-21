#include <stdint.h>
#include <Arduino.h>
#include "camera.h"
#include "ili9341.h"
#include "sd.h"

void setup() {
  Serial.begin(115200);  // 初始化串口
  Serial.setDebugOutput(false);
  Camera_Init();  // 初始化摄像头
  Key_Init();  // 初始化按键
  sdmmcInit();  // 初始化SD卡
  LCD_Init();  // 初始化屏幕
  LCD_Set_Display_Dir(1);  // 旋转到横屏
}

void loop() {
  uint32_t drawTime = millis();   // 开始计时
  Camera2LCD_Jpeg();  // 将摄像头画面显示到屏幕
  drawTime = millis() - drawTime;  // 结束计时
  Serial.print(drawTime); Serial.println(F(" ms"));  // 打印出显示一帧所需的时间
  Camera_Adjuster();  // 调节摄像头画面参数的按键
  Take_Picture_Key();  // 拍照按键
  // Camera2LCD_RGB565_Test();  // 测试RGB565直接显示到屏幕
}
