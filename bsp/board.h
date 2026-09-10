#pragma once

#include "main.h"
#include <stdint.h>
#include <stdlib.h>
#include "lcd.h"
#include "ina226.h"
#include "stdio.h"
#include "delay.h"



void board_init(void);
void board_clock_init(void);
uint32_t board_get_core_clock_hz(void);
uint32_t Sys_GetTick(void);
uint8_t key_read(uint8_t index);

uint32_t spi_flash_read_id(void);
