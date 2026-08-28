#include "user_messagesend_Task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"
#include "ina226.h"
#include "cdc.h"
#include "log_store.h"
#include "user_key_Task.h"

extern osEventFlagsId_t message;

void Snapshot(log_record_t *data_log,power_sample_t *p)
{
    int16_t i = 0;
    cdc_printf("时间,量程,电流,电压,功耗,电流MAX,电流MIN,");
    osDelay(2);
    cdc_printf("电压MAX,电压MIN,功耗MAX,功耗MIN\r\n");
    osDelay(2);
    cdc_printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
            data_log->time_ms[i],p->range+1,data_log->current_uA[i],data_log->bus_mV[i],data_log->power_uW[i],
            data_log->max_uA,data_log->min_uA,data_log->max_mV,data_log->min_mV,data_log->max_uW,data_log->min_uW);
    osDelay(2);
    for (i = 1; i < 200; i++)
    {
        cdc_printf("%d,%d,%d,%d,%d\r\n",
            data_log->time_ms[i],p->range+1,data_log->current_uA[i],data_log->bus_mV[i],data_log->power_uW[i]);
        osDelay(2);
    }
}

void message_Task(void* argument)
{
    uint8_t flags;
    while (1)
    {
        flags = osEventFlagsWait(message,EVENT_KEY3|EVENT_KEY4,osFlagsWaitAny,osWaitForever);
        if (flags & EVENT_KEY3)
        {
            Snapshot(&data_log,&power);
        }
        if (flags & EVENT_KEY4)
        {
            log_export_csv();
        }
        osDelay(1);
    }
}