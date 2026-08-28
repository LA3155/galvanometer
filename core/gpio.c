#include "gpio.h"
#include "i2c.h"

void mx_gpio_init(void)
{
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);

    gpio_init(LCD_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_RST_PIN | LCD_CS_PIN | LCD_DC_PIN);
    gpio_bit_set(LCD_PORT, LCD_CS_PIN | LCD_RST_PIN | LCD_DC_PIN);

    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, LCD_SCK_PIN | LCD_MOSI_PIN);

    gpio_init(KEY1_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEY1_PIN | KEY2_PIN | KEY3_PIN);
    gpio_init(KEY4_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEY4_PIN);

    gpio_init(DUT_EN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, DUT_EN_PIN);
    dut_power_set(1);

    gpio_init(I2C_SCL_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN | I2C_SDA_PIN);

    gpio_init(FLASH_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, FLASH_CS_PIN);
    gpio_bit_set(FLASH_PORT, FLASH_CS_PIN);
    gpio_init(FLASH_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, FLASH_SCK_PIN | FLASH_MOSI_PIN);
    gpio_init(FLASH_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, FLASH_MISO_PIN);
}

void dut_power_set(uint8_t on)
{
    if (on) {
        gpio_bit_set(DUT_EN_PORT, DUT_EN_PIN);
    } else {
        gpio_bit_reset(DUT_EN_PORT, DUT_EN_PIN);
    }
}