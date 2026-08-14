#pragma once

#include <stdint.h>
#define SNAPSHOT_SIZE 200
#define WAVE_X      40
#define WAVE_Y      20
#define WAVE_W      200
#define WAVE_H      115
#define WAVE_BOTTOM (WAVE_Y + WAVE_H - 1)
#define RANGE_X     40
#define RANGE_Y     20

typedef struct {
    int16_t shunt_raw;
    uint16_t bus_raw;
    int32_t shunt_uV;
    uint32_t bus_mV;
    int32_t current_uA;
    uint32_t power_uW;
} power_sample_t;

extern power_sample_t power;

typedef enum {
    RANGE_10R,
    RANGE_1R,
    RANGE_10MR,
} current_range_t;

typedef enum {
    Voltage,
    Current,
    Power,
} type_t;

typedef struct {
    uint32_t time_ms[200];
    uint16_t bus_mV[200];
    int32_t current_uA[200];
    int32_t power_uW[200];
    int32_t max_mV;
    int32_t min_mV;
    int32_t max_uA;
    int32_t min_uA;
    int32_t max_uW;
    int32_t min_uW;
    int16_t index;
    uint8_t filled;
    uint8_t flags;
    uint16_t crc;
} log_record_t;

extern log_record_t data_log;

void ina226_filter(current_range_t range);
void ina226_read_sample(current_range_t range, power_sample_t *out);
void lcd_show_power_sample(const power_sample_t *p, current_range_t range);
void waveform_push(power_sample_t *data,log_record_t *log);
void waveform_draw_current(log_record_t *log,type_t type);
void Snapshot(log_record_t *data_log,current_range_t range);