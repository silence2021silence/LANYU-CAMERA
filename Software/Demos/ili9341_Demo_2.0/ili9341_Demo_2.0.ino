#include <stdint.h>
#include <Arduino.h>
#include "ili9341.h"
#include "pic1.h"
#include "pic2.h"
#include "pic22.h"
#include <TJpg_Decoder.h>


void setup(void) {
  LCD_Init();
}

void loop() {
  LCD_Set_Display_Dir(0);
  // 清屏填充颜色
  LCD_Clear(0xF800);
  LCD_Clear(0x07E0);
  LCD_Clear(0x001F);
  // 显示16位RGB565图片
  LCD_ShowPic_16bit(0, 0, 240, 320, pic1);
  delay(2000);

  LCD_Set_Display_Dir(1);
  LCD_Clear(0xF800);
  LCD_Clear(0x07E0);
  LCD_Clear(0x001F);
  LCD_ShowPic_16bit(0, 0, 320, 240, pic2);
  delay(2000);

  // 显示24位JPEG图片, 文件转数据工具https://tomeko.net/online_tools/file_to_hex.php?lang=en
  LCD_Clear(0x0000);
  TJpgDec.setJpgScale(1);  // 放大倍数
  TJpgDec.setSwapBytes(true);  // 切换高/低位优先
  TJpgDec.setCallback(tft_output);  // 设置回调函数
  TJpgDec.drawJpg(0, 0, pic22_0, sizeof(pic22_0));  // 绘制JPEG, 参数:起点坐标x,y,数据(const uint8类型),数据大小
  delay(2000);
}
