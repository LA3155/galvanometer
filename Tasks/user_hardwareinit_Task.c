#include "user_hardwareinit_Task.h"
#include "key.h"
#include "lcd.h"
#include "cdc.h"
#include "spi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"

extern osEventFlagsId_t lcd_ready_evt;

void hardwareinit_Task(void* argument)
{
    lcd_init();
    key_exti_init();
    cdc_init();
    osEventFlagsSet(lcd_ready_evt, 1<<0);
    vTaskDelete(NULL);
}