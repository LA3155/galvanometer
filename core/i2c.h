#pragma once

#include "main.h"

#define I2C_BUS         I2C0
#define I2C_SCL_PORT    GPIOB
#define I2C_SCL_PIN     GPIO_PIN_6
#define I2C_SDA_PORT    GPIOB
#define I2C_SDA_PIN     GPIO_PIN_7

void mx_i2c_init(void);
uint8_t i2c_probe(uint8_t addr7);
uint32_t i2c_scan_mask_low32(void);
uint32_t i2c_scan_mask_high32(void);
uint16_t i2c_readreg(uint8_t addr7,uint8_t reg_addr);