#include "gd32f30x_it.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "board.h"
#include "usbd_lld_int.h"

extern osMessageQueueId_t key_queue;

void USBD_LP_CAN0_RX0_IRQHandler(void)
{
    usbd_isr();
}

void USBD_HP_CAN0_TX_IRQHandler(void)
{
    usbd_int_hpst();
}

void EXTI10_15_IRQHandler(void)
{
    uint8_t key_code = 0;
    if (RESET != exti_interrupt_flag_get(EXTI_13)) {
        exti_interrupt_flag_clear(EXTI_13);
        key_code = 1;
        osMessageQueuePut(key_queue,&key_code,0,0);
    }
    if (RESET != exti_interrupt_flag_get(EXTI_14)) {
        exti_interrupt_flag_clear(EXTI_14);
        key_code = 2;
        osMessageQueuePut(key_queue,&key_code,0,0);
    }
    if (RESET != exti_interrupt_flag_get(EXTI_15)) {
        exti_interrupt_flag_clear(EXTI_15);
        key_code = 3;
        osMessageQueuePut(key_queue,&key_code,0,0);
    }
}

void EXTI0_IRQHandler(void)
{
    uint8_t key_code = 0;
    if (RESET != exti_interrupt_flag_get(EXTI_0)) {
        exti_interrupt_flag_clear(EXTI_0);
        key_code = 4;
        osMessageQueuePut(key_queue,&key_code,0,0);
    }
}