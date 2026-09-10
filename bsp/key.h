#pragma once

#include "main.h"

#define KEY_IDX 4

typedef enum{
    KEY_NONE = 0,
    KEY_PRESS,
    KEY_RELEASE,
    KEY_TIMEOUT,
}key_enum;

typedef struct{
    uint8_t     level[KEY_IDX];
    uint32_t    time;
}key_t;

void key_scanf(uint8_t *level);