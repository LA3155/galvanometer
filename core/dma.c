#include "dma.h"
#include "cmsis_os2.h"

extern osSemaphoreId_t dma_done_sem;
extern osMutexId_t     spi_mutex;

void mx_dma_init(void)
{
    dma_parameter_struct dma_init_struct;

    rcu_periph_clock_enable(RCU_DMA0);

    dma_deinit(DMA_PORT,SPI0_TX_DMA_CHANNEL);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.periph_addr = (uint32_t)&SPI_DATA(SPI0);
    dma_init_struct.periph_inc  = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_addr = 0;
    dma_init_struct.memory_inc  = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_width= DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.memory_width= DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.direction   = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.number      = 0;
    dma_init_struct.priority    = DMA_PRIORITY_HIGH;

    dma_init(DMA_PORT,SPI0_TX_DMA_CHANNEL,&dma_init_struct);
    dma_channel_disable(DMA_PORT,SPI0_TX_DMA_CHANNEL);
    dma_flag_clear(DMA_PORT,SPI0_TX_DMA_CHANNEL,DMA_FLAG_FTF);
    dma_interrupt_enable(DMA_PORT,SPI0_TX_DMA_CHANNEL,DMA_INT_FTF);
    nvic_irq_enable(DMA0_Channel2_IRQn,5,0);

    dma_deinit(DMA_PORT,SPI1_TX_DMA_CHANNEL);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.periph_addr = (uint32_t)&SPI_DATA(SPI1);
    dma_init_struct.periph_inc  = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_addr = 0;
    dma_init_struct.memory_inc  = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_width= DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.memory_width= DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.direction   = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.number      = 0;
    dma_init_struct.priority    = DMA_PRIORITY_HIGH;

    dma_init(DMA_PORT,SPI1_TX_DMA_CHANNEL,&dma_init_struct);
    dma_channel_disable(DMA_PORT,SPI1_TX_DMA_CHANNEL);
    dma_flag_clear(DMA_PORT,SPI1_TX_DMA_CHANNEL,DMA_FLAG_FTF);
}

void spi0_transmit_dma(uint8_t *pData, uint16_t Size)
{
    if(Size ==0 || pData == NULL) return;
    osMutexAcquire(spi_mutex,osWaitForever);

    dma_memory_address_config(DMA_PORT,SPI0_TX_DMA_CHANNEL,(uint32_t)pData);
    dma_transfer_number_config(DMA_PORT,SPI0_TX_DMA_CHANNEL,Size);

    dma_flag_clear(DMA_PORT,SPI0_TX_DMA_CHANNEL,DMA_FLAG_FTF);
    dma_channel_enable(DMA_PORT,SPI0_TX_DMA_CHANNEL);
    spi_dma_enable(SPI0,SPI_DMA_TRANSMIT);

    if (osSemaphoreAcquire(dma_done_sem, 100) != osOK) {   // 阻塞等完成，100 tick 超时
        osSemaphoreAcquire(dma_done_sem, 0);               // 清残留计数
        dma_channel_disable(DMA_PORT, SPI0_TX_DMA_CHANNEL);
        spi_dma_disable(SPI0, SPI_DMA_TRANSMIT);
        spi_disable(SPI0); 
        spi_enable(SPI0);               // 超时兜底复位
        osMutexRelease(spi_mutex);
        return;
    }

    // 等待 SPI 移位寄存器把最后一个字节彻底发完
    while (spi_i2s_flag_get(SPI0, SPI_FLAG_TRANS) == SET) {
    }

    dma_flag_clear(DMA_PORT, SPI0_TX_DMA_CHANNEL, DMA_FLAG_FTF);
    dma_channel_disable(DMA_PORT, SPI0_TX_DMA_CHANNEL);
    spi_dma_disable(SPI0, SPI_DMA_TRANSMIT);
    // 清除全双工发送期间产生的 RX 溢出和残留接收数据
    if(spi_i2s_flag_get(SPI0, SPI_FLAG_RXORERR) != RESET) {
        spi_i2s_data_receive(SPI0);   // 读 STAT 后读 DATA → 清 RXORERR
    }
    while(spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE) != RESET) {
    spi_i2s_data_receive(SPI0);   // 排空 RX 缓冲区
}
    osMutexRelease(spi_mutex);
}

void spi1_transmit_dma(uint8_t *pData, uint16_t Size)
{
    if(Size ==0 || pData == NULL) return;

    dma_memory_address_config(DMA_PORT,SPI1_TX_DMA_CHANNEL,(uint32_t)pData);
    dma_transfer_number_config(DMA_PORT,SPI1_TX_DMA_CHANNEL,Size);

    dma_flag_clear(DMA_PORT,SPI1_TX_DMA_CHANNEL,DMA_FLAG_FTF);
    dma_channel_enable(DMA_PORT,SPI1_TX_DMA_CHANNEL);
    spi_dma_enable(SPI1,SPI_DMA_TRANSMIT);
}