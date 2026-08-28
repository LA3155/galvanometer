#include "flash.h"

static void flash_cs_low(void)  { gpio_bit_reset(FLASH_PORT, FLASH_CS_PIN); }
static void flash_cs_high(void) { gpio_bit_set(FLASH_PORT, FLASH_CS_PIN); }

static void flash_send_addr(uint32_t addr)
{
    spi1_transfer((addr >> 16) & 0xFF);
    spi1_transfer((addr >> 8) & 0xFF);
    spi1_transfer(addr & 0xFF);
}

uint32_t flash_read_id(void)
{
    uint32_t id;
    flash_cs_low();
    spi1_transfer(FLASH_CMD_RDID);
    id = ((uint32_t)spi1_transfer(0xFF) << 16);
    id |= ((uint32_t)spi1_transfer(0xFF) << 8);
    id |= spi1_transfer(0xFF);
    flash_cs_high();
    return id;
}

static void flash_wait_busy(void)
{
    uint8_t stat;
    do
    {
        flash_cs_low();
        spi1_transfer(FLASH_CMD_RDSR);
        stat = spi1_transfer(0xFF);
        flash_cs_high();
    } while (stat & 0x01);
}

void flash_write_enable(void)
{
    flash_cs_low();
    spi1_transfer(FLASH_CMD_WREN);
    flash_cs_high();
}

void flash_erase_sector(uint32_t addr)
{
    flash_write_enable();
    flash_cs_low();
    spi1_transfer(FLASH_CMD_SE);
    flash_send_addr(addr);
    flash_cs_high();
    flash_wait_busy();
}

void flash_write_page(uint32_t addr,const uint8_t *data,uint16_t len)
{
    flash_write_enable();
    flash_cs_low();
    spi1_transfer(FLASH_CMD_PP);
    flash_send_addr(addr);
    for(uint16_t i = 0; i < len ; i++)
    {
        spi1_transfer(data[i]);
    }
    flash_cs_high();
    flash_wait_busy();
}

void flash_read(uint32_t addr,uint8_t *data,uint16_t len)
{
    flash_cs_low();
    spi1_transfer(FLASH_CMD_READ);
    flash_send_addr(addr);
    for(uint16_t i = 0; i < len ; i++)
    {
        data[i] = spi1_transfer(0XFF);
    }
    flash_cs_high();
}
