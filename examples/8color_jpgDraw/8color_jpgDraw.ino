#include <Arduino.h>
#include <lvgl.h>
#include <LittleFS.h>
#include "ST7306_LCD.h"
#include "ST7306_LVGL.h"
#include "ST7306_DrawUtils.h"

#include "esp_sleep.h"


// --- 1. 硬件配置 (请根据您的实际接线调整 PIN) ---
/*
//8 color
#define PIN_TE 9
#define PIN_CLK 4
#define PIN_MOSI 3
#define PIN_CS 2
#define PIN_DC 1
#define PIN_RST 0
#define WAKEUP_PIN 5
*/

/*
//mono
*/
#define PIN_TE 12
#define PIN_CLK 21
#define PIN_MOSI 20
#define PIN_CS 19
#define PIN_DC 18
#define PIN_RST 1

#define WAKEUP_PIN 8



void setupGpioHold() {
  Serial.println("设置 GPIO0 保持功能...");

  // 配置 GPIO0 为输出模式
  pinMode(PIN_RST, OUTPUT);

  // 设置 GPIO0 为高电平
  digitalWrite(PIN_RST, HIGH);
  Serial.println("GPIO0 设置为高电平");

  // 启用 GPIO0 的保持功能
  gpio_hold_en((gpio_num_t)PIN_RST);
  Serial.println("GPIO0 保持功能已启用");
}
// 位掩码计算：1ULL << WAKEUP_PIN_NUMBER
// 1ULL << 4 等于 0x10 (十进制 16)
uint64_t wakeup_pin_mask = 1ULL << WAKEUP_PIN;
// RTC_DATA_ATTR 变量保存在 RTC 内存中，在深度睡眠重启后不会丢失
RTC_DATA_ATTR int bootCount = 0;
// 实例化 LCD 驱动 (全局对象)
ST7306_LCD lcd(PIN_MOSI, PIN_CLK, PIN_CS, PIN_DC, PIN_RST, PIN_TE);

void setup() {
  Serial.begin(115200);
  pinMode(WAKEUP_PIN, INPUT_PULLUP);

  bootCount++;
  delay(300);
  lcd.begin();

  Serial.printf("\n--- Boot Count: %d ---\n", bootCount);

  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed!");
    while (1) delay(1);  // 停止程序
  }


  const char *image_path1 = "/bg.jpg";
  const char *image_path2 = "/bg1.jpg";
  drawJpgFileFromFS(image_path1, 0, 0, ROTATION_0);


  lcd.refreshReal();
  delay(2000);
  drawJpgFileFromFS(image_path2, 0, 0, ROTATION_0);

  delay(2000);

  lcd.refreshReal();


   esp_deep_sleep_enable_gpio_wakeup(wakeup_pin_mask, ESP_GPIO_WAKEUP_GPIO_LOW);

  Serial.println("Configured GPIO wakeup and going to sleep...");

  Serial.println("禁用 GPIO0 保持功能...");

  setupGpioHold();
  // 3. 进入深度睡眠
 // esp_deep_sleep_start();
  //LCD_RST_HIGH;
}


void loop() {

  //lcd.refreshReal();

  // 休息一下，避免占用全部 CPU 资源
  //delay(5);
}