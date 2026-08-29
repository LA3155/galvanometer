#include "user_lv_Task.h"
#include "page_manager.h"
#include "ui_current.h"
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
                if(page_get_nowpage() == &page_current)
                {
                    ui_current_update(power.bus_mV, power.current_uA, power.power_uW, power.range);
                }
            }
            
            lv_timer_handler();
        }
        osDelay(1);
    }
}