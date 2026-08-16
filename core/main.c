#include "board.h"
#include "cdc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"

void MX_FREERTOS_Init(void);

int main(void)
{
    board_init();
    cdc_init();
    lcd_init();

    osKernelInitialize();
    MX_FREERTOS_Init();
    osKernelStart();

    while (1) {
    }
}
