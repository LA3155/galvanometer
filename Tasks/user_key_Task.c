#include "user_key_Task.h"
#include "cmsis_os2.h"
#include "bsp_ina226.h"

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
                break;

            case KEY4:
                osEventFlagsSet(message,EVENT_KEY4);
                break;
            default:
                break;
        }
        osDelay(10);
    }
}