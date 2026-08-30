#include "user_lv_Task.h"
#include "page_manager.h"
#include "ui_current.h"
#include "ui_consumption.h"
#include "ina226.h"
#include "lvgl.h"
#include "cmsis_os2.h"

extern osMessageQueueId_t lv_queue;

void lv_Task(void* argument)
{
    uint8_t msg;
    uint8_t count = 0;
    while (1)
    {
        if(osMessageQueueGet(lv_queue, &msg, 0, osWaitForever) == osOK)
        {
            count++;
            if(count >= 20) // 20 * 5ms = 100ms 刷新一次折线和数值
            {
                count = 0;
                page_t *cur = page_get_nowpage();
                if(cur == &page_current)
                {
                    ui_current_update(power.bus_mV, power.current_uA, power.power_uW, power.range);
                }
                if(cur == &page_consumption)
                {
                    ui_consumption_update(g_integral.energy_mAh, g_integral.energy_mWh,g_integral.total_samples);
                }
            }
            
            lv_timer_handler();
        }
        osDelay(1);
    }
}