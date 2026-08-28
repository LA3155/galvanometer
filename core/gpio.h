#pragma once

#include "main.h"

#define LCD_PORT        GPIOA
#define LCD_RST_PIN     GPIO_PIN_1
#define LCD_CS_PIN      GPIO_PIN_4
#define LCD_SCK_PIN     GPIO_PIN_5
#define LCD_DC_PIN      GPIO_PIN_6
#define LCD_MOSI_PIN    GPIO_PIN_7

#define KEY1_PORT       GPIOC
#define KEY1_PIN        GPIO_PIN_13
#define KEY2_PORT       GPIOC
#define KEY2_PIN        GPIO_PIN_14
#define KEY3_PORT       GPIOC
#define KEY3_PIN        GPIO_PIN_15
#define KEY4_PORT       GPIOA
#define KEY4_PIN        GPIO_PIN_0

#define DUT_EN_PORT     GPIOB
#define DUT_EN_PIN      GPIO_PIN_9

#define FLASH_PORT      GPIOB
#define FLASH_CS_PIN    GPIO_PIN_12
#define FLASH_SCK_PIN   GPIO_PIN_13
#define FLASH_MISO_PIN  GPIO_PIN_14
#define FLASH_MOSI_PIN  GPIO_PIN_15

void mx_gpio_init(void);
void dut_power_set(uint8_t on);

