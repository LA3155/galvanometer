#pragma once

#include "main.h"

#define FLASH_CMD_WREN    0x06  // 写使能
#define FLASH_CMD_PP       0x02  // 页编程（写）
#define FLASH_CMD_READ     0x03  // 读数据
#define FLASH_CMD_SE       0x20  // 扇区擦除（4KB）
#define FLASH_CMD_RDSR     0x05  // 读状态寄存器
#define FLASH_CMD_RDID    0x9F  // 读ID

uint32_t flash_read_id(void);
void flash_write_enable(void);
void flash_erase_sector(uint32_t addr);
void flash_write_page(uint32_t addr,const uint8_t *data,uint16_t len);
void flash_read(uint32_t addr,uint8_t *data,uint16_t len);