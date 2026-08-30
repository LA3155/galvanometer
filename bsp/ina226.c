#include "ina226.h"
#include "lcd.h"
#include "stdlib.h"
#include "stdio.h"
#include "i2c.h"
#include "cdc.h"
#include "string.h"

power_sample_t power;
power_integral_t g_integral = {0};
log_record_t   data_log;

void ina226_init(void)
{
    power.range = 2;
    power.offset[0] = 43.41f;
    power.offset[1] = 3.622f;
    power.offset[2] = -1.03f;
    memset(&data_log, 0, sizeof(data_log));
}

uint16_t ina226_read(uint8_t reg)
{
    uint16_t data;
    data = i2c_readreg(0X40,reg);
    return data;
}
