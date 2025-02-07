#include <stdint.h>
#include <Arduino.h>
#include "camera.h"
#include "sd.h"

void setup() 
{
  // 初始化串口
  Serial.begin(921600);
  Serial.setDebugOutput(false);
  Camera_Init();
  Key_Init();
  sdmmcInit();
}

void loop() 
{
  Jpeg2Serial();
  Camera_Adjuster();
  Take_Picture_Key();
}
