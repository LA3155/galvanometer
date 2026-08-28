#pragma once

#include "ina226.h"
#include "lcd.h"

void lcd_show_power_sample(power_sample_t *p);
void waveform_redraw(log_record_t *log,type_t type);