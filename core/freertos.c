/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"
#include "user_TasksInit.h"
#include "string.h"

// 栈溢出钩子函数
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) 
{
    static char task_name[configMAX_TASK_NAME_LEN];
    if (pcTaskName != NULL) {
        strncpy(task_name, (char *)pcTaskName, sizeof(task_name) - 1);
    }
    while (1) {
    }
}

void MX_FREERTOS_Init(void)
{
    User_Tasks_Init();
}