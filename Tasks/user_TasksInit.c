#include "user_TasksInit.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"

osTimerId_t ina226TimerHandle;
osTimerId_t lcdTimerHandle;

osThreadId_t ina226TaskHandle;
const osThreadAttr_t ina226Task_attibutes = {
    .name = "ina226",
    .stack_size = 256*8,
    .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t keyTaskHandle;
const osThreadAttr_t keyTask_attibutes = {
    .name = "key",
    .stack_size = 256*1,
    .priority = (osPriority_t) osPriorityNormal1,
};

osThreadId_t lcdTaskHandle;
const osThreadAttr_t lcdTask_attibutes = {
    .name = "lcd",
    .stack_size = 256*2,
    .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t messageTaskHandle;
const osThreadAttr_t messageTask_attibutes = {
    .name = "message",
    .stack_size = 256*4,
    .priority = (osPriority_t) osPriorityNormal,
};

osMessageQueueId_t key_queue;
//信号量
osSemaphoreId_t sem;
//事件组
osEventFlagsId_t message;

void User_Tasks_Init(void)
{
    key_queue = osMessageQueueNew(1,1,NULL);
    sem       = osSemaphoreNew(1,0,NULL);
    message   = osEventFlagsNew(NULL);


    keyTaskHandle       = osThreadNew(key_Task,NULL,&keyTask_attibutes);
    lcdTaskHandle       = osThreadNew(lcd_Task,NULL,&lcdTask_attibutes);
    ina226TaskHandle    = osThreadNew(ina226_Task,NULL,&ina226Task_attibutes);
    messageTaskHandle   = osThreadNew(message_Task,NULL,&messageTask_attibutes);

    ina226TimerHandle = osTimerNew(ina226TimerCallback,osTimerPeriodic,NULL,NULL);
    osTimerStart(ina226TimerHandle,10);
}