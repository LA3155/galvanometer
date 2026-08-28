#include "waveform_draw.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

static uint16_t fram_buf[WAVE_H][WAVE_W];
uint8_t y_buf[WAVE_W];

void lcd_show_power_sample(power_sample_t *p)
{
    char line[32];
    uint32_t bus_mV = p->bus_mV;
    int32_t current_mA = p->current_uA / 1000;
    uint32_t power_mW = p->power_uW / 1000;

    if (bus_mV > 9999U) {
        bus_mV = 9999U;
    }
    if (current_mA > 999) {
        current_mA = 999;
    } else if (current_mA < -99) {
        current_mA = -99;
    }
    if (power_mW > 999U) {
        power_mW = 999U;
    }

    snprintf(line, sizeof(line), "V:%lu.%02luV I:%03ldmA P:%03lumW L:%d",
             p->bus_mV / 1000,
             (p->bus_mV % 1000) / 10,
             p->current_uA/1000,
             p->power_uW/1000,
             p->range);
    lcd_draw_string(0, 0, line, WHITE, BLACK);
}

static uint16_t map_current_to_y_at(log_record_t *data_log,type_t type,uint16_t index)
{
    int32_t max_uA = 100000; // 先固定 100mA 满量程显示
    int32_t max_mV = 3300;
    int32_t max_uW = 330000; 

    switch (type)
    {
        case Voltage:
            if (data_log->bus_mV[index] <= 0) {
                data_log->bus_mV[index] = 0;
            }
            if (data_log->bus_mV[index] > max_mV) {
                data_log->bus_mV[index] = max_mV;
            }
            return WAVE_BOTTOM - (uint16_t)((data_log->bus_mV[index] * (WAVE_H - 1)) / max_mV);
            
        break;

        case Current:
            if (data_log->current_uA[index] <= 0) {
                data_log->current_uA[index] = 0;
            }
            if (data_log->current_uA[index] > max_uA) {
                data_log->current_uA[index] = max_uA;
            }
            return WAVE_BOTTOM - (uint16_t)((data_log->current_uA[index] * (WAVE_H - 1)) / max_uA);
        break;

        case Power:
            if (data_log->power_uW[index] <= 0) {
                data_log->power_uW[index] = 0;
            }
            if (data_log->power_uW[index] > max_uW) {
                data_log->power_uW[index] = max_uW;
            }
            return WAVE_BOTTOM - (uint16_t)((data_log->power_uW[index] * (WAVE_H - 1)) / max_uW);
        break;
    }
    return 0;
}

static void update_y_buf(log_record_t *log, type_t type)
{
    for (int i = 0; i < SNAPSHOT_SIZE; i++) 
    {
        y_buf[i] = map_current_to_y_at(log, type, i);
    }
}

void waveform_redraw(log_record_t *log,type_t type)
{
    update_y_buf(log,type);
    memset(fram_buf,0,sizeof(fram_buf));

    for (int i = 0; i < SNAPSHOT_SIZE; i++)
    {
        lcd_draw_line_into_buf(fram_buf,i-1,y_buf[i-1]-WAVE_Y,
                                        i,y_buf[i]-WAVE_Y,GREEN);
    }
    
    lcd_set_window(WAVE_X,WAVE_Y,WAVE_X+WAVE_W - 1,WAVE_BOTTOM);
    lcd_send_data((uint8_t *)fram_buf,sizeof(fram_buf));
}