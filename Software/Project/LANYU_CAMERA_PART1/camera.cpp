#include <stdint.h>
#include <Arduino.h>
#include "camera.h"
#include "ili9341.h"
#include "sd.h"

camera_fb_t * fb = NULL;
esp_err_t res = ESP_OK;

int CONTRAST = 0;
int BRIGHTNESS = 0;
int SATURATION = 0;
int SHARPNESS = 0;

void Camera_Init(void)
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // disable brownout detector

  // 摄像头初始化参数配置，更多可选参数和选项参见esp_camera.h和sensor.h
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;  //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
  config.pixel_format = PIXFORMAT_JPEG;  //YUV422,GRAYSCALE,RGB565,JPEG
  // config.pixel_format = PIXFORMAT_RGB565;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  
  // 是否识别到PSRAM
  if(psramFound()){
    Serial.printf("Camera init with PSRAM!\n");
    // config.frame_size = FRAMESIZE_HVGA; // 480*320
    config.frame_size = FRAMESIZE_QVGA; // 320*240
    config.jpeg_quality = 10;  // JPEG输出的质量。0-63越低，质量越好
    config.fb_count = 2; // 要分配的帧缓冲区的数目。如果大于1，则每帧采集（双倍速度）
    config.grab_mode = CAMERA_GRAB_LATEST;
    config.fb_location = CAMERA_FB_IN_PSRAM;  // 使用PSRAM
  } else {
    Serial.printf("Camera init without PSRAM, use DRAM\n");
    config.frame_size = FRAMESIZE_QVGA; // 320*240
    config.jpeg_quality = 15;
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM; // 使用DRAM
  }
  
  // 摄像头初始化
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // 传感器图像参数配置，更多可选参数和选项参见esp_camera.h和sensor.h
  sensor_t * s = esp_camera_sensor_get();
  s->set_hmirror(s, 0);     // 水平镜像
  s->set_vflip(s, 0);       // 垂直翻转
  s->set_contrast(s, 0);    // 对比度 -2到2
  s->set_brightness(s, 0);  // 亮度   -2到2
  s->set_saturation(s, 0);  // 饱和度 -2到2
  s->set_sharpness(s, 0);   // 清晰度 -2到2

  Serial.println("Camera configuration complete!");

}

// JPEG通过串口发送
void Jpeg2Serial(void)
{
  fb = esp_camera_fb_get();  // 获取帧缓冲区指针
  if (!fb) 
  {
    Serial.println("Camera capture failed");
    res = ESP_FAIL;
  }
  if(res == ESP_OK) 
  {
    Serial.write(fb->buf, fb->len);  // 通过串口发送数据
    Serial.flush();
  }
  if(fb){
    esp_camera_fb_return(fb);  // 返回需要重复使用的帧缓冲区指针
    fb = NULL;
  }
}

// 按键调节摄像头参数
void Camera_Adjuster(void)
{
  // 初始化引脚 设置为上拉输入
  pinMode(CONTRAST_KEY, INPUT_PULLUP);
  pinMode(BRIGHTNESS_KEY, INPUT_PULLUP);
  pinMode(SATURATION_KEY, INPUT_PULLUP);
  pinMode(SHARPNESS_KEY, INPUT_PULLUP);

  // 调节对比度
  if(digitalRead(CONTRAST_KEY)==LOW){  // 按键按下
    delay(20);  // 按键消抖
    if(digitalRead(CONTRAST_KEY)==LOW){
      while(digitalRead(CONTRAST_KEY)==LOW);  // 等待按键松开
      CONTRAST++;
      if (CONTRAST > 2) CONTRAST = -2;  // 范围在-2和2之间循环
      sensor_t * s = esp_camera_sensor_get();
      s->set_contrast(s, CONTRAST);  // 设置对比度
    }
  }

  // 调节亮度
  if(digitalRead(BRIGHTNESS_KEY)==LOW){
    delay(20);
    if(digitalRead(BRIGHTNESS_KEY)==LOW){
      while(digitalRead(BRIGHTNESS_KEY)==LOW);
      BRIGHTNESS++;
      if (BRIGHTNESS > 2) BRIGHTNESS = -2;
      sensor_t * s = esp_camera_sensor_get();
      s->set_brightness(s, BRIGHTNESS);
    }
  }

  // 调节饱和度
  if(digitalRead(SATURATION_KEY)==LOW){
    delay(20);
    if(digitalRead(SATURATION_KEY)==LOW){
      while(digitalRead(SATURATION_KEY)==LOW);
      SATURATION++;
      if (SATURATION > 2) SATURATION = -2;
      sensor_t * s = esp_camera_sensor_get();
      s->set_saturation(s, SATURATION);
    }
  }

  // 调节清晰度
  if(digitalRead(SHARPNESS_KEY)==LOW){
    delay(20);
    if(digitalRead(SHARPNESS_KEY)==LOW){
      while(digitalRead(SHARPNESS_KEY)==LOW);
      SHARPNESS++;
      if (SHARPNESS > 2) SHARPNESS = -2;
      sensor_t * s = esp_camera_sensor_get();
      s->set_sharpness(s, 1);
    }
  }
}

// 摄像头RGB565显示到屏幕
// 使用时要改初始化参数config.pixel_format = PIXFORMAT_RGB565;
void Camera2LCD_RGB565_Test(void)
{
  fb = esp_camera_fb_get();
  if (!fb) 
  {
    Serial.println("Camera capture failed");
    res = ESP_FAIL;
  }
  if(res == ESP_OK)
  {
    LCD_ShowPic_16bit(0, 0, fb->width, fb->height, fb->buf);
  }
  if(fb){
    esp_camera_fb_return(fb);
    fb = NULL;
  }
}

// 摄像头JPEG转RGB565后显示到屏幕
void Camera2LCD_Jpeg2RGB565(void)
{
  fb = esp_camera_fb_get();
  if (!fb) 
  {
    Serial.println("Camera capture failed");
    res = ESP_FAIL;
  }
  if(res == ESP_OK)
  {
    size_t out_len = fb->width * fb->height * 2;  // 转换格式后的数据大小 RGB565每个像素占2字节
    uint8_t *out_buf = (uint8_t *)malloc(out_len);  // 创建一个缓存区 动态分配内存空间
    if (!out_buf)
    {
      Serial.println("Failed to allocate memory for output buffer");
      esp_camera_fb_return(fb);
      return;
    }
    bool converter = jpg2rgb565(fb->buf, fb->len, out_buf, JPG_SCALE_NONE);  // JPEG转RGB565输出到out_buf
    if (converter)
    {
      LCD_ShowPic_16bit(0, 0, fb->width, fb->height, out_buf);  // fb为JPEG输出时低位在前
    }
    else
    {
      Serial.println("JPEG to RGB565 conversion failed");
    }
    // 释放内存
    free(out_buf);
  }
  if(fb)
  {
    esp_camera_fb_return(fb);
    fb = NULL;
  }
}

// 拍照按键
void Take_Picture_Key(void)
{
  if(digitalRead(TAKE_PICTURE_KEY)==LOW){
    delay(20);
    if(digitalRead(TAKE_PICTURE_KEY)==LOW){
      while(digitalRead(TAKE_PICTURE_KEY)==LOW);
      fb = esp_camera_fb_get();
      if (!fb) 
      {
        Serial.println("Camera capture failed");
        res = ESP_FAIL;
      }
      if(res == ESP_OK)
      {
        int photo_index = readFileNum(SD_MMC, "/");  // 获取文件名数字索引
        if(photo_index!=-1)
        {
          String path = "/" + String(photo_index) +".jpg";  // 组合路径和文件名
          writejpg(SD_MMC, path.c_str(), fb->buf, fb->len);  // 写入TF卡
        }
      }
      if(fb)
      {
        esp_camera_fb_return(fb);
        fb = NULL;
      }
    }
  }
}



