#include "esp32.h"

void esp32_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART1);

    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_2);
    gpio_init(GPIOA,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_3);

    usart_deinit(USART1);
    usart_baudrate_set(USART1,115200);
    usart_receive_config(USART1,USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1,USART_TRANSMIT_ENABLE);

    usart_interrupt_enable(USART1,USART_INT_RBNE);
    nvic_irq_enable(USART1_IRQn,5,0);
    usart_enable(USART1);

    
}

void esp32_send_str(const char *str)
{
    while(*str)
    {
        usart_data_transmit(USART1,(uint8_t)(*str++));
        while(RESET == usart_flag_get(USART1,USART_FLAG_TBE));
    }
}