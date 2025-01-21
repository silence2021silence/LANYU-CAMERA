#include <stdint.h>
#include <Arduino.h>
#include "camera.h"

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
  config.xclk_freq_hz = 20000000;  //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
  config.pixel_format = PIXFORMAT_JPEG;  //YUV422,GRAYSCALE,RGB565,JPEG
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  
  // 是否识别到PSRAM
  if(psramFound()){
    Serial.printf("Camera init with PSRAM!\n");
    config.frame_size = FRAMESIZE_HVGA; // 480*320
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

void Camera_Adjuster(void)
{
  // 初始化引脚 设置为上拉输入
  pinMode(SET_CONTRAST_KEY, INPUT_PULLUP);
  pinMode(SET_BRIGHTNESS_KEY, INPUT_PULLUP);
  pinMode(SET_SATURATION_KEY, INPUT_PULLUP);
  pinMode(SET_SHARPNESS_KEY, INPUT_PULLUP);

  // 调节对比度
  if(digitalRead(SET_CONTRAST_KEY)==LOW){
    delay(20);  // 按键消抖
    if(digitalRead(SET_CONTRAST_KEY)==LOW){
      while(digitalRead(SET_CONTRAST_KEY)==LOW);
      CONTRAST++;
      if (CONTRAST > 2) CONTRAST = -2;  // 范围在-2和2之间循环
      sensor_t * s = esp_camera_sensor_get();
      s->set_contrast(s, CONTRAST);  // 设置对比度
    }
  }

  // 调节亮度
  if(digitalRead(SET_BRIGHTNESS_KEY)==LOW){
    delay(20);
    if(digitalRead(SET_BRIGHTNESS_KEY)==LOW){
      while(digitalRead(SET_BRIGHTNESS_KEY)==LOW);
      BRIGHTNESS++;
      if (BRIGHTNESS > 2) BRIGHTNESS = -2;
      sensor_t * s = esp_camera_sensor_get();
      s->set_brightness(s, BRIGHTNESS);
    }
  }

  // 调节饱和度
  if(digitalRead(SET_SATURATION_KEY)==LOW){
    delay(20);
    if(digitalRead(SET_SATURATION_KEY)==LOW){
      while(digitalRead(SET_SATURATION_KEY)==LOW);
      SATURATION++;
      if (SATURATION > 2) SATURATION = -2;
      sensor_t * s = esp_camera_sensor_get();
      s->set_saturation(s, SATURATION);
    }
  }

  // 调节清晰度
  if(digitalRead(SET_SHARPNESS_KEY)==LOW){
    delay(20);
    if(digitalRead(SET_SHARPNESS_KEY)==LOW){
      while(digitalRead(SET_SHARPNESS_KEY)==LOW);
      SHARPNESS++;
      if (SHARPNESS > 2) SHARPNESS = -2;
      sensor_t * s = esp_camera_sensor_get();
      s->set_sharpness(s, 1);
    }
  }
}
