#include "spi.h"

uint8_t spi1_transfer(uint8_t data)
{
    while (RESET == spi_i2s_flag_get(FLASH_SPI, SPI_FLAG_TBE)) {
    }
    spi_i2s_data_transmit(FLASH_SPI, data);
    while (RESET == spi_i2s_flag_get(FLASH_SPI, SPI_FLAG_RBNE)) {
    }
    return (uint8_t)spi_i2s_data_receive(FLASH_SPI);
}

void mx_spi_init(void)
{
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_SPI0);
    spi_struct_para_init(&spi_init_struct);
    spi_init_struct.trans_mode = SPI_TRANSMODE_BDTRANSMIT;//SPI_TRANSMODE_BDTRANSMIT
    spi_init_struct.device_mode = SPI_MASTER;
    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss = SPI_NSS_SOFT;
    spi_init_struct.prescale = SPI_PSC_4;
    spi_init_struct.endian = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);
    spi_nss_internal_high(SPI0);
    spi_enable(SPI0);

    rcu_periph_clock_enable(RCU_SPI1);
    spi_struct_para_init(&spi_init_struct);
    spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode = SPI_MASTER;
    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss = SPI_NSS_SOFT;
    spi_init_struct.prescale = SPI_PSC_8;
    spi_init_struct.endian = SPI_ENDIAN_MSB;
    spi_init(FLASH_SPI, &spi_init_struct);
    spi_enable(FLASH_SPI);
}

