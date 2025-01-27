#include <stdint.h>
#include <Arduino.h>
#include "sd.h"
#include "img_converters.h"
#include "espHPD482.h"

size_t RGB565ToMonochrome(uint8_t *rgb565, size_t width, size_t height, uint8_t *monochrome) {
  size_t byte_index = 0;
  for (size_t i = 0; i < width * height; i++) {
      uint16_t pixel = (rgb565[i * 2 + 1] << 8) | rgb565[i * 2]; // 获取16位像素点数据
      // 提取RGB565的各个颜色分量
      uint8_t r = (pixel >> 11) & 0x1F;  // 5位红色分量
      uint8_t g = (pixel >> 5) & 0x3F;   // 6位绿色分量
      uint8_t b = pixel & 0x1F;          // 5位蓝色分量
      // 用简单加权法将RGB转换为灰度值
      // 灰度值 = 0.299 * R + 0.587 * G + 0.114 * B
      uint8_t gray = (r * 30 + g * 59 + b * 11) / 100; // 将加权值转换为灰度值
      // 使用阈值将灰度图像转换为单色
      // 灰度值大于阈值归为白色 反之归为黑色
      uint8_t pixel_monochrome = (gray > 30) ? 0x00 : 0xFF; // 白色为0x00 黑色为0xFF
      // 将每8个像素存储在一个字节中
      if (i % 8 == 0) {
          monochrome[byte_index] = 0;
      }
      // 设置对应的像素位
      if (pixel_monochrome == 0xFF) {
          monochrome[byte_index] |= (1 << (7 - (i % 8)));  // 1为黑色 0为白色
      }
      // 每8个像素后递增字节索引
      if (i % 8 == 7) {
          byte_index++;
      }
  }
  return byte_index;
}

void setup() {
  // 初始化串口
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  // 初始化SD卡
  sdmmcInit();

  // 创建1MB数组储存读取到的数据
  size_t bufferSize = 1024*1024;
  uint8_t *buffer = (uint8_t *)malloc(bufferSize);  // 动态分配内存
  // 读取文件并将内容存入字节数组
  size_t jpeg_bytes = readFile2Bytes(SD_MMC, "/6.jpg", buffer, bufferSize);
  // 如果读取成功
  if (jpeg_bytes > 0) {
      Serial.println("JPEG data read successfully");
      for (int i = 0; i < jpeg_bytes && i < 16; i++) {  // 打印前16个字节作为示例
          Serial.printf("0x%02X ", buffer[i]);
      }
      Serial.println();
  }

  // 创建储存RGB565图片数据的数组
  uint8_t *out_buf = (uint8_t *)malloc(320*240*2);  // 1像素点占2字节大小
  // JPEG转RGB565
  bool converter = jpg2rgb565(buffer, jpeg_bytes, out_buf, JPG_SCALE_NONE);
  if (converter) {
      Serial.println("JPEG to RGB565 conversion successful");
  } else {
      Serial.println("JPEG to RGB565 conversion failed");
  }

  // 彩色图像二值化
  uint8_t *monochrome = (uint8_t *)malloc(320*240/8);
  size_t monochrome_bytes = RGB565ToMonochrome(out_buf, 320, 240, monochrome);
  // 如果转换成功
  if (monochrome_bytes > 0) {
      Serial.println("JPEG data read successfully");
      for (int i = 0; i < monochrome_bytes && i < 16; i++) {  // 打印前16个字节作为示例
          Serial.printf("0x%02X ", monochrome[i]);
      }
      Serial.println();
  }

  HPD482 printer = HPD482();  // 初始化打印头
  printer.Print_SetDeep(15);  // 设置打印颜色深度（用于修改打印颜色深度，不需要每次都设置，初始化里已有，此句可不要）
  printer.Motor_Run(40,0);  // 步进电机旋转进纸40/8=5mm
  printer.Print_Img2Lcd(0, monochrome);
  printer.Motor_Run(40,0);
}

void loop() {
  // put your main code here, to run repeatedly:

}









