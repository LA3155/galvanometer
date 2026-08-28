#pragma once

#include "main.h"

#define FLASH_SPI       SPI1

void mx_spi_init(void);
uint8_t spi1_transfer(uint8_t data);