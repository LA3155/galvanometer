#include "key.h"
#include "gpio.h"

static const uint32_t key_pin[] = {KEY1_PIN, KEY2_PIN, KEY3_PIN, KEY4_PIN};
static const uint32_t key_port[] = {KEY1_PORT, KEY2_PORT, KEY3_PORT, KEY4_PORT};

void key_scanf(uint8_t *level)
{
    for (int i = 0; i < KEY_IDX; i++)
    {
        level[i] = (gpio_input_bit_get(key_port[i], key_pin[i]) == RESET);
    }
}