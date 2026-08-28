#pragma once

#include "usbd_core.h"
#include <stddef.h>
#include <stdint.h>
#include "ina226.h"
#define CLI_LINE_MAX 64


void cdc_init(void);
uint8_t cdc_is_ready(void);
uint32_t cdc_write(const void *data, uint32_t len);
int cdc_printf(const char *fmt, ...);
uint8_t cdc_is_configured(void);
uint32_t cdc_read(uint8_t *buf, uint32_t max_len);
void cli_poll(power_sample_t *power,uint8_t current_range);