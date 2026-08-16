#include "bsp_delay.h"
#include "gd32f30x.h"
#include "FreeRTOS.h"
#include "task.h"

void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;
    
    // 计算需要的时钟周期数
    ticks = nus * SYS_CLK;  // nus * 120
    
    told = SysTick->VAL;
    
    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
            } else {
                // 发生了溢出（计数器从 0 跳到 reload）
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) break;
        }
    }
}

/**
 * @brief 毫秒延时（基于 delay_us 实现）
 * @param ms 延时毫秒数
 * @note  在 FreeRTOS 任务中建议用 vTaskDelay，
 *        在初始化阶段或中断中可用此函数
 */
void delay_ms(uint32_t ms)
{
    // 如果 FreeRTOS 调度器已启动，使用 vTaskDelay（不占CPU）
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        vTaskDelay(pdMS_TO_TICKS(ms));
        return;
    }
    
    // 否则用 delay_us 实现（忙等待）
    delay_us(ms * 1000UL);
}

uint32_t Sys_GetTick(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
    }

    return 0;
}

