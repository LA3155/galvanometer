#include "gd32f30x_it.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "usbd_lld_int.h"
#include "dma.h"

extern osSemaphoreId_t dma_done_sem;

void USBD_LP_CAN0_RX0_IRQHandler(void)
{
    usbd_isr();
}

void USBD_HP_CAN0_TX_IRQHandler(void)
{
    usbd_int_hpst();
}

void DMA0_Channel2_IRQHandler(void)
{
    if(RESET != dma_interrupt_flag_get(DMA0,DMA_CH2,DMA_INTF_FTFIF))
    {
        dma_interrupt_flag_clear(DMA0,DMA_CH2,DMA_INTF_FTFIF);
        osSemaphoreRelease(dma_done_sem);
    }
}