#include "user_key_Task.h"
#include "page_manager.h"
#include "ui_consumption.h"
#include "cmsis_os2.h"
#include "power_sample.h"

extern osMessageQueueId_t key_queue;
extern osEventFlagsId_t message;


void key_Task(void* argument)
{
    uint8_t keycode;
    while (1)
    {
        osMessageQueueGet(key_queue,&keycode,NULL,osWaitForever);
        switch (keycode)
        {
            case KEY1://切换量程
                power.range++;
                power.range%=3;
                break;
            
            case KEY2://校准
                ina226_filter(&power);
                break;

            case KEY3:
                osEventFlagsSet(message,EVENT_KEY3);
                if(page_get_nowpage() != &page_consumption)
                {
                    page_load(&page_consumption);
                }
                break;

            case KEY4:
                // osEventFlagsSet(message,EVENT_KEY4);
                page_back();
                break;
            default:
                break;
        }
        osDelay(10);
    }
}