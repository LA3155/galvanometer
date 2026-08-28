#include "main.h"
#include "cdc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"

void MX_FREERTOS_Init(void);

int main(void)
{
    SystemCoreClockUpdate();
    mx_gpio_init();
    mx_spi_init();
    mx_i2c_init();
    mx_dma_init();

    osKernelInitialize();
    MX_FREERTOS_Init();
    osKernelStart();

    while (1) {
    }
}
