#include <stdint.h>
#include <Arduino.h>
#include "ili9341.h"
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>

#define minimum(a,b)     (((a) < (b)) ? (a) : (b))


TFT_eSPI tft = TFT_eSPI();

// 初始化屏幕
void LCD_Init(void)
{
  tft.begin();
}

// 设置屏幕方向
// dir:0:0度; 1:90度; 2:180度; 3:270度
void LCD_Set_Display_Dir(uint8_t dir)
{
  tft.setRotation(dir);
}

// 清屏
// color:填充色
void LCD_Clear(uint16_t color)
{
  tft.fillScreen(color);
}

// 显示16位图片
// x:起点x
// y:起点y
// pic_width:图片宽度
// pic_height:图片高度
// pic:图片数据:格式为RGB565, 数组为const uint16类型, 高位优先(MSB First/Big-endian)
// 图片转u16数组工具https://notisrac.github.io/FileToCArray
void LCD_ShowPic_16bit(int16_t x, int16_t y, uint16_t pic_width, uint16_t pic_height, const uint16_t *pic)
{
  tft.pushImage(x, y, pic_width, pic_height, pic);  // 在屏幕绘制RGB565
}

// 回调函数
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* pic)
{
  if (y >= tft.height()) return 0;
  tft.pushImage(x, y, w, h, pic);  // 在屏幕绘制RGB565
  return 1;
}
