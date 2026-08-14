#pragma once

#include "gd32f30x.h"
#include <stdint.h>
#include <stdlib.h>
#include "bsp_lcd.h"
#include "bsp_ina226.h"
#include "stdio.h"

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

#define I2C_BUS         I2C0
#define I2C_SCL_PORT    GPIOB
#define I2C_SCL_PIN     GPIO_PIN_6
#define I2C_SDA_PORT    GPIOB
#define I2C_SDA_PIN     GPIO_PIN_7

#define FLASH_SPI       SPI1
#define FLASH_PORT      GPIOB
#define FLASH_CS_PIN    GPIO_PIN_12
#define FLASH_SCK_PIN   GPIO_PIN_13
#define FLASH_MISO_PIN  GPIO_PIN_14
#define FLASH_MOSI_PIN  GPIO_PIN_15

void board_init(void);
void board_clock_init(void);
uint32_t board_get_core_clock_hz(void);
void delay_ms(uint32_t ms);
uint32_t Sys_GetTick(void);
uint8_t key_read(uint8_t index);
void dut_power_set(uint8_t on);

uint32_t spi_flash_read_id(void);
uint8_t i2c_probe(uint8_t addr7);
uint32_t i2c_scan_mask_low32(void);
uint32_t i2c_scan_mask_high32(void);
uint16_t i2c_readreg(uint8_t addr7,uint8_t reg_addr);
