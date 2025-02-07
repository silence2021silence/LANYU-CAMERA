#ifndef CAMERA_H
#define CAMERA_H

#include "esp_camera.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "soc/rtc_cntl_reg.h"
#include "esp32-hal-psram.h"

// ESP32-S3-CAM
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 15
#define SIOD_GPIO_NUM 4
#define SIOC_GPIO_NUM 5
#define Y2_GPIO_NUM 11
#define Y3_GPIO_NUM 9
#define Y4_GPIO_NUM 8
#define Y5_GPIO_NUM 10
#define Y6_GPIO_NUM 12
#define Y7_GPIO_NUM 18
#define Y8_GPIO_NUM 17
#define Y9_GPIO_NUM 16
#define VSYNC_GPIO_NUM 6
#define HREF_GPIO_NUM 7
#define PCLK_GPIO_NUM 13

// ESP32-CAM
// #define PWDN_GPIO_NUM     32
// #define RESET_GPIO_NUM    -1
// #define XCLK_GPIO_NUM      0
// #define SIOD_GPIO_NUM     26
// #define SIOC_GPIO_NUM     27

// #define Y9_GPIO_NUM       35
// #define Y8_GPIO_NUM       34
// #define Y7_GPIO_NUM       39
// #define Y6_GPIO_NUM       36
// #define Y5_GPIO_NUM       21
// #define Y4_GPIO_NUM       19
// #define Y3_GPIO_NUM       18
// #define Y2_GPIO_NUM        5
// #define VSYNC_GPIO_NUM    25
// #define HREF_GPIO_NUM     23
// #define PCLK_GPIO_NUM     22

#define CONTRAST_KEY     0
#define BRIGHTNESS_KEY   3
#define SATURATION_KEY   45
#define SHARPNESS_KEY    46

#define TAKE_PICTURE_KEY 47

void Camera_Init(void);
void Jpeg2Serial(void);
void Key_Init(void);
void Camera_Adjuster(void);
void Take_Picture_Key(void);

#endif
