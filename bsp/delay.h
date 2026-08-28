#pragma once

#include "stdint.h"
#include "freertosconfig.h"
#define SYS_CLK  (configCPU_CLOCK_HZ / 1000000UL)  // 120MHz / 1e6 = 120

void delay_ms(uint32_t ms);
uint32_t Sys_GetTick(void);
