#include "user_lcd_Task.h"
#include "bsp_ina226.h"
#include "cmsis_os2.h"

void lcd_Task(void* argument)
{
    while (1)
    {
        lcd_show_power_sample(&power);
        osDelay(200);
    }
}