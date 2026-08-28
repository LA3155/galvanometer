#pragma once

#include "ina226.h"
#include "lcd.h"

void ina226_filter(power_sample_t *p);
void ina226_read_sample(power_sample_t *out);
void waveform_push(power_sample_t *data,log_record_t *data_log);