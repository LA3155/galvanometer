#include "user_lcd_Task.h"
#include "waveform_draw.h"
#include "cmsis_os2.h"

extern osEventFlagsId_t lcd_ready_evt;

void lcd_Task(void* argument)
{
    osEventFlagsWait(lcd_ready_evt, 1<<0, osFlagsWaitAny | osFlagsNoClear, osWaitForever);
    while (1)
    {
        lcd_show_power_sample(&power);
        waveform_redraw(&data_log,Power);
        osDelay(20);
    }
}