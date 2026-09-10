#include "power_sample.h"

void ina226_filter(power_sample_t *p)
{
    int32_t sum = 0;
    int32_t i;
    for(i=0;i<300;i++)
    {
        sum += ina226_read(0X01);
    }
    p->offset[p->range] = (float)sum/300.0f;
}

void range_switch(void)
{
    power.range++;
    if(power.range > 3){
        power.range = 0;
    }
}

void power_integrate_update(uint32_t current_uA,int32_t power_uW,uint32_t dt_ms)
{
    if(current_uA > 0)
    {
        // 1 小时 = 3600 秒 = 3,600,000 毫秒
        // uA * ms / 3,600,000 = uAh
        // uAh / 1000 = mAh
        double delta_mAh = (double)(current_uA * dt_ms) / 3600000000.0;

        double delta_mWh = (power_uW * dt_ms) / 3600000000.0;

        g_integral.energy_mAh += delta_mAh;
        g_integral.energy_mWh += delta_mWh;
        g_integral.total_samples++;
    }
}

void ina226_read_sample(power_sample_t *out)
{
    out->bus_raw        = ina226_read(0X02);//负载电压大小，进而算出功率
    out->shunt_raw      = ina226_read(0X01);//负载电压大小，进而算出功率
    float corrected_raw = (float)out->shunt_raw - out->offset[out->range];//采样电阻电压，进而得出负载电流大小
    out->shunt_uV       = ((int32_t)corrected_raw * 25) / 10;
    out->bus_mV         = ((uint32_t)out->bus_raw * 125) / 100;
    if(out->range == RANGE_10MR)
    {
        out->current_uA  = out->shunt_uV *100;
    }
    else if(out->range == RANGE_1R)
    {
        out->current_uA  = out->shunt_uV;
    }
    else if(out->range == RANGE_10R)
    {
        out->current_uA  = out->shunt_uV /10;
    }
    out->power_uW = (uint32_t)(((int64_t)out->bus_mV * out->current_uA) / 1000);
    power_integrate_update(out->current_uA,out->power_uW,10);
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
    data_log->index++;
    if(data_log->index >= SNAPSHOT_SIZE)
    {
        data_log->index  = 0;
        data_log->filled = 1;
    }
}