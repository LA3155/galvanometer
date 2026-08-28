#pragma once

#include "main.h"

#define SPI0_TX_DMA_CHANNEL    DMA_CH2
#define SPI1_TX_DMA_CHANNEL    DMA_CH4
#define DMA_PORT               DMA0

void mx_dma_init(void);
void spi0_transmit_dma(uint8_t *pData, uint16_t Size);
void spi1_transmit_dma(uint8_t *pData, uint16_t Size);