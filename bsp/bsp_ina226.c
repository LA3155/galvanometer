#include "bsp_ina226.h"
#include "bsp_lcd.h"
#include "stdlib.h"
#include "stdio.h"
#include "board.h"
#include "cdc.h"

power_sample_t power = {0};
log_record_t   data_log = {0};

float offset_raw[3] = {43.41,3.622,-1.03};

void ina226_filter(current_range_t range)
{
    int32_t sum = 0;
    int32_t i;
    for(i=0;i<300;i++)
    {
        sum += i2c_readreg(0X40,0X01);
    }
    offset_raw[range] = (float)sum/300.0f;
}

void ina226_read_sample(current_range_t range, power_sample_t *out)
{
    out->bus_raw        = i2c_readreg(0X40,0X02);//负载电压大小，进而算出功率
    out->shunt_raw      = i2c_readreg(0X40,0X01);//负载电压大小，进而算出功率
    float corrected_raw = (float)out->shunt_raw - offset_raw[range];//采样电阻电压，进而得出负载电流大小
    out->shunt_uV       = ((int32_t)corrected_raw * 25) / 10;
    out->bus_mV         = ((uint32_t)out->bus_raw * 125) / 100;
    if(range == RANGE_10MR)
    {
        out->current_uA  = out->shunt_uV *100;
    }
    else if(range == RANGE_1R)
    {
        out->current_uA  = out->shunt_uV;
    }
    else if(range == RANGE_10R)
    {
        out->current_uA  = out->shunt_uV /10;
    }
    out->power_uW = (uint32_t)(((int64_t)out->bus_mV * out->current_uA) / 1000);
}

void lcd_show_power_sample(const power_sample_t *p, current_range_t range)
{
    char line[32];
    const char *range_text = "10m";
    uint32_t bus_mV = p->bus_mV;
    int32_t current_mA = p->current_uA / 1000;
    uint32_t power_mW = p->power_uW / 1000;

    if (range == RANGE_10R) {
        range_text = "10R";
    } else if (range == RANGE_1R) {
        range_text = "1R ";
    }

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

    snprintf(line, sizeof(line), "V:%lu.%02luV I:%03ldmA P:%03lumW L:%s",
             bus_mV / 1000,
             (bus_mV % 1000) / 10,
             current_mA,
             power_mW,
             range_text);
    lcd_draw_string(0, 0, line, WHITE, BLACK);
}

void waveform_push(power_sample_t *data,log_record_t *data_log)
{
    data_log->time_ms[data_log->index] = (data_log->index)*10;
    data_log->bus_mV[data_log->index] = data->bus_mV;
    data_log->current_uA[data_log->index] = data->current_uA;
    data_log->power_uW[data_log->index] = data->power_uW;
    if(data_log->index == 0)
    {
        data_log->max_mV = data_log->bus_mV[data_log->index];
        data_log->min_mV = data_log->bus_mV[data_log->index];
        data_log->max_uA = data_log->current_uA[data_log->index];
        data_log->min_uA = data_log->current_uA[data_log->index];
        data_log->max_uW = data_log->power_uW[data_log->index];
        data_log->min_uW = data_log->power_uW[data_log->index];
    }
    else
    {
        if(data_log->bus_mV[data_log->index] > data_log->max_mV)
        {
            data_log->max_mV = data_log->bus_mV[data_log->index];
        }
        if(data_log->bus_mV[data_log->index] < data_log->min_mV)
        {
            data_log->min_mV = data_log->bus_mV[data_log->index];
        }

        if(data_log->current_uA[data_log->index] > data_log->max_uA)
        {
            data_log->max_uA = data_log->current_uA[data_log->index];
        }
        if(data_log->current_uA[data_log->index] < data_log->min_uA)
        {
            data_log->min_uA = data_log->current_uA[data_log->index];
        }

        if(data_log->power_uW[data_log->index] > data_log->max_uW)
        {
            data_log->max_uW = data_log->power_uW[data_log->index];
        }
        if(data_log->power_uW[data_log->index] < data_log->min_uW)
        {
            data_log->min_uW = data_log->power_uW[data_log->index];
        }
    }
}

static uint16_t map_current_to_y(log_record_t *data_log,type_t type)
{
    int32_t max_uA = 100000; // 先固定 100mA 满量程显示
    int32_t max_mV = 3300;
    int32_t max_uW = 330000; 

    switch (type)
    {
        case Voltage:
            if (data_log->bus_mV[data_log->index] <= 0) {
                data_log->bus_mV[data_log->index] = 0;
            }
            if (data_log->bus_mV[data_log->index] > max_mV) {
                data_log->bus_mV[data_log->index] = max_mV;
            }
            return WAVE_BOTTOM - (uint16_t)((data_log->bus_mV[data_log->index] * (WAVE_H - 1)) / max_mV);
            
        break;

        case Current:
            if (data_log->current_uA[data_log->index] <= 0) {
                data_log->current_uA[data_log->index] = 0;
            }
            if (data_log->current_uA[data_log->index] > max_uA) {
                data_log->current_uA[data_log->index] = max_uA;
            }
            return WAVE_BOTTOM - (uint16_t)((data_log->current_uA[data_log->index] * (WAVE_H - 1)) / max_uA);
        break;

        case Power:
            if (data_log->power_uW[data_log->index] <= 0) {
                data_log->power_uW[data_log->index] = 0;
            }
            if (data_log->power_uW[data_log->index] > max_uW) {
                data_log->power_uW[data_log->index] = max_uW;
            }
            return WAVE_BOTTOM - (uint16_t)((data_log->power_uW[data_log->index] * (WAVE_H - 1)) / max_uW);
        break;
    }
    return 0;
}

void waveform_draw_current(log_record_t *data_log,type_t type)
{
    log_record_t *L = data_log;
    int16_t y;
    y = map_current_to_y(L,type);
    lcd_draw_pixel_update(L->index+WAVE_X,y,GREEN);
    data_log->index++;
    if(data_log->index >= SNAPSHOT_SIZE)
    {
        data_log->index  = 0;
        data_log->filled = 1;
    }
}

// void Snapshot(log_record_t *data_log,current_range_t range)
// {
//     int16_t i = 0;
//     cdc_printf("时间,量程,电流,电压,功耗,电流MAX,电流MIN,电压MAX,电压MIN,功耗MAX,功耗MIN\r\n");
//     cdc_printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
//             data_log->time_ms[i],range+1,data_log->current_uA[i],data_log->bus_mV[i],data_log->power_uW[i],
//             data_log->max_uA,data_log->min_uA,data_log->max_mV,data_log->min_mV,data_log->max_uW,data_log->min_uW);
//     for (i = 1; i < 200; i++)
//     {
//         cdc_printf("%d,%d,%d,%d,%d\r\n",
//             data_log->time_ms[i],range+1,data_log->current_uA[i],data_log->bus_mV[i],data_log->power_uW[i]);
//     }
// }

void Snapshot(log_record_t *data_log,current_range_t range)
{
    int16_t i = 0;
    for (i = 1; i < 200; i++)
    {
        cdc_printf("%d\r\n",
            data_log->time_ms[i]);
            delay_ms(2);
    }
}