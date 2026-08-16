#pragma once

#include "stdint.h"
#define KEY1 1
#define KEY2 2
#define KEY3 3
#define KEY4 4

#define EVENT_KEY3  (1 << 2)  // 0x04
#define EVENT_KEY4  (1 << 3)  // 0x08

void key_Task(void* argument);
