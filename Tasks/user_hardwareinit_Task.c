#include "user_hardwareinit_Task.h"
#include "cdc.h"
#include "key.h"
#include "spi.h"
#include "hwdataaccess.h"
#include "lv_port_disp.h"
#include "ui_current.h"
#include "lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"

uint16_t messageTimerCount = 0;
uint8_t msg;
extern osEventFlagsId_t lcd_ready_evt;
extern osMessageQueueId_t ina226_queue;
extern osMessageQueueId_t lv_queue;

void hardwareinit_Task(void* argument)
{
    hw_dataaccess.lcd.Init();
    hw_dataaccess.ina226.Init();
    cdc_init();
    lv_init();
    lv_port_disp_init();
    page_manage_init(&page_current);
    osEventFlagsSet(lcd_ready_evt, 1<<0);
    vTaskDelete(NULL);
}

void messageTimerCallback(void *argument)
{
    lv_tick_inc(1);
    messageTimerCount++;
    if(messageTimerCount %10 == 0)
    {
        osMessageQueuePut(ina226_queue,&msg,0,0);
    }
    if(messageTimerCount %5 == 0)
    {
        osMessageQueuePut(lv_queue,&msg,0,0);
    }
}