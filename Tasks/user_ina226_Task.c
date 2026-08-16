#include "user_ina226_Task.h"
#include "bsp_ina226.h"
#include "cmsis_os2.h"

extern osSemaphoreId_t sem;

void ina226TimerCallback(void* argument)
{
    
    osSemaphoreRelease(sem);
}

void ina226_Task(void* argument)
{
    while (1)
    {
        osSemaphoreAcquire(sem,osWaitForever);
        ina226_read_sample(&power);
        waveform_push(&power, &data_log);
        waveform_draw_current(&data_log,Power);
        osDelay(1);
    }
}