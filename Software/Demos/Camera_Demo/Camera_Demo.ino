#include <stdint.h>
#include <Arduino.h>
#include "camera.h"

void setup() 
{
  // 初始化串口
  Serial.begin(921600);
  Serial.setDebugOutput(false);
  Camera_Init();
}

void loop() 
{
  Jpeg2Serial();
  Camera_Adjuster();
}
