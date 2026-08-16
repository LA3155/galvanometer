#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define CMSIS_device_header "gd32f30x.h"

#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xSemaphoreGetMutexHolder        1
#define INCLUDE_vTaskDelayUntil                 1   
#define INCLUDE_vTaskDelete                     1      
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xTimerPendFunctionCall          1

#define configMAX_PRIORITIES                    56
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUSE_TRACE_FACILITY                1
#define configCPU_CLOCK_HZ          120000000UL
#define configTICK_RATE_HZ          1000
#define configMINIMAL_STACK_SIZE    128
#define configTOTAL_HEAP_SIZE       (20 * 1024)

#define configUSE_PREEMPTION        1
#define configUSE_IDLE_HOOK         0
#define configUSE_TICK_HOOK         0
#define configUSE_16_BIT_TICKS      0
#define configUSE_MUTEXES           1
#define configUSE_COUNTING_SEMAPHORES 1
#define configSUPPORT_DYNAMIC_ALLOCATION 1

#define configUSE_TIMERS            1
#define configTIMER_TASK_PRIORITY   2
#define configTIMER_QUEUE_LENGTH    8
#define configTIMER_TASK_STACK_DEPTH 256
#define configGENERATE_RUN_TIME_STATS        0
#define configUSE_STATS_FORMATTING_FUNCTIONS 0
#define configRUN_TIME_COUNTER_TYPE uint32_t

#define configPRIO_BITS             4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5

#define configKERNEL_INTERRUPT_PRIORITY \
    (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))



#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler

#endif