#include "user_TasksInit.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"

osTimerId_t ina226TimerHandle;
osTimerId_t messageTimerHandle;

osThreadId_t hardwareinitTaskHandle;
const osThreadAttr_t hardwareinitTask_attibutes = {
    .name = "hardware",
    .stack_size = 256*5,
    .priority = (osPriority_t) osPriorityNormal+5,
};

osThreadId_t ina226TaskHandle;
const osThreadAttr_t ina226Task_attibutes = {
    .name = "ina226",
    .stack_size = 256*8,
    .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t keyTaskHandle;
const osThreadAttr_t keyTask_attibutes = {
    .name = "key",
    .stack_size = 256*2,
    .priority = (osPriority_t) osPriorityNormal1,
};

osThreadId_t lcdTaskHandle;
const osThreadAttr_t lcdTask_attibutes = {
    .name = "lcd",
    .stack_size = 256*4,
    .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t messageTaskHandle;
const osThreadAttr_t messageTask_attibutes = {
    .name = "message",
    .stack_size = 256*4,
    .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t lvTaskHandle;
const osThreadAttr_t lvTask_attibutes = {
    .name = "lv",
    .stack_size = 256*12,
    .priority = (osPriority_t) osPriorityNormal,
};

osMessageQueueId_t key_queue;
osMessageQueueId_t ina226_queue;
osMessageQueueId_t lv_queue;
//信号量
osSemaphoreId_t sem;
osSemaphoreId_t dma_done_sem;
//事件组
osEventFlagsId_t message;
osEventFlagsId_t lcd_ready_evt;
//互斥锁
osMutexId_t     spi_mutex;

void User_Tasks_Init(void)
{
    key_queue       = osMessageQueueNew(1,1,NULL);
    ina226_queue    = osMessageQueueNew(1,1,NULL);
    lv_queue        = osMessageQueueNew(1,1,NULL);
    sem             = osSemaphoreNew(1,0,NULL);
    dma_done_sem    = osSemaphoreNew(1,0,NULL);
    message         = osEventFlagsNew(NULL);
    lcd_ready_evt   = osEventFlagsNew(NULL);
    spi_mutex       = osMutexNew(NULL);

    hardwareinitTaskHandle  = osThreadNew(hardwareinit_Task,NULL,&hardwareinitTask_attibutes);
    keyTaskHandle           = osThreadNew(key_Task,NULL,&keyTask_attibutes);
    lcdTaskHandle           = osThreadNew(lcd_Task,NULL,&lcdTask_attibutes);
    ina226TaskHandle        = osThreadNew(ina226_Task,NULL,&ina226Task_attibutes);
    messageTaskHandle       = osThreadNew(message_Task,NULL,&messageTask_attibutes);
    lvTaskHandle            = osThreadNew(lv_Task,NULL,&lvTask_attibutes);

    messageTimerHandle = osTimerNew(messageTimerCallback,osTimerPeriodic,NULL,NULL);
    osTimerStart(messageTimerHandle,1);
}