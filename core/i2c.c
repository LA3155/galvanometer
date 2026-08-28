#include "i2c.h"
#include "delay.h"

void mx_i2c_init(void)
{
    rcu_periph_clock_enable(RCU_I2C0);
    i2c_clock_config(I2C_BUS, 100000, I2C_DTCY_2);
    i2c_mode_addr_config(I2C_BUS, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x32);
    i2c_enable(I2C_BUS);
    i2c_ack_config(I2C_BUS, I2C_ACK_ENABLE);
}

static uint8_t i2c_wait_flag(uint32_t flag, FlagStatus status, uint32_t timeout)
{
    while (i2c_flag_get(I2C_BUS, flag) != status) {
        if (timeout-- == 0U) {
            return 0;
        }
    }
    return 1;
}

uint8_t i2c_probe(uint8_t addr7)
{
    uint8_t found = 0;

    uint32_t busy_timeout = 50000U;
    while (i2c_flag_get(I2C_BUS, I2C_FLAG_I2CBSY) == SET) {
        if (busy_timeout-- == 0U) {
            return 0;
        }
    }

    i2c_start_on_bus(I2C_BUS);
    if (!i2c_wait_flag(I2C_FLAG_SBSEND, SET, 10000U)) {
        i2c_stop_on_bus(I2C_BUS);
        return 0;
    }

    i2c_master_addressing(I2C_BUS, (uint32_t)(addr7 << 1), I2C_TRANSMITTER);
    for (uint32_t timeout = 0; timeout < 20000U; timeout++) {
        if (i2c_flag_get(I2C_BUS, I2C_FLAG_ADDSEND) == SET) {
            i2c_flag_clear(I2C_BUS, I2C_FLAG_ADDSEND);
            found = 1;
            break;
        }
        if (i2c_flag_get(I2C_BUS, I2C_FLAG_AERR) == SET) {
            i2c_flag_clear(I2C_BUS, I2C_FLAG_AERR);
            break;
        }
    }
    i2c_stop_on_bus(I2C_BUS);
    delay_ms(1);
    return found;
}

uint32_t i2c_scan_mask_low32(void)
{
    uint32_t mask = 0;
    for (uint8_t addr = 0x08; addr < 0x40; addr++) {
        if (i2c_probe(addr) && addr < 32U) {
            mask |= (1UL << addr);
        }
    }
    return mask;
}

uint32_t i2c_scan_mask_high32(void)
{
    uint32_t mask = 0;
    for (uint8_t addr = 0x40; addr < 0x78; addr++) {
        if (i2c_probe(addr)) {
            mask |= (1UL << (addr - 0x40));
        }
    }
    return mask;
}

uint16_t i2c_readreg(uint8_t addr7,uint8_t reg_addr)
{
    uint8_t data_high = 0;
    uint8_t data_low = 0;
    uint32_t timeout = 0;
    uint32_t busy_timeout = 50000U;
    while (i2c_flag_get(I2C_BUS, I2C_FLAG_I2CBSY) == SET) {
        if (busy_timeout-- == 0U) {
            return 0;
        }
    }

    i2c_start_on_bus(I2C_BUS);
    if (!i2c_wait_flag(I2C_FLAG_SBSEND, SET, 10000U)) {
        i2c_stop_on_bus(I2C_BUS);
        return 0;
    }

    i2c_master_addressing(I2C_BUS, (uint32_t)(addr7 << 1), I2C_TRANSMITTER);
    timeout = 20000U;
    while(timeout-- > 0)
    {
        if (i2c_flag_get(I2C_BUS, I2C_FLAG_ADDSEND) == SET) {//收到ACK
            i2c_flag_clear(I2C_BUS, I2C_FLAG_ADDSEND);
            break;
        }
        if (i2c_flag_get(I2C_BUS, I2C_FLAG_AERR) == SET) {//收到NACK
            i2c_flag_clear(I2C_BUS, I2C_FLAG_AERR);
            i2c_stop_on_bus(I2C_BUS);
            return 0;
        }
    }
    if(timeout == 0)
    {
        i2c_stop_on_bus(I2C_BUS);
        return 0;
    }

    while (RESET == i2c_flag_get(I2C_BUS, I2C_FLAG_TBE)) {//I2C_FLAG_TBE为1表示发送缓冲区为空
        if (--timeout == 0) { i2c_stop_on_bus(I2C_BUS); return 0; }
    }

    i2c_data_transmit(I2C_BUS, reg_addr);//寄存器写命令

    while (RESET == i2c_flag_get(I2C_BUS, I2C_FLAG_BTC)) {//I2C_FLAG_BTC为1表示发送字节传输完成
        if (--timeout == 0) { i2c_stop_on_bus(I2C_BUS); return 0; }
    }

    i2c_start_on_bus(I2C_BUS);
    if (!i2c_wait_flag(I2C_FLAG_SBSEND, SET, 10000U)) {
        i2c_stop_on_bus(I2C_BUS);
        return 0;
    }
    
    i2c_master_addressing(I2C_BUS, (uint32_t)(addr7 << 1), I2C_RECEIVER);

    timeout = 20000U;
    while(timeout-- > 0)
    {
        if (i2c_flag_get(I2C_BUS, I2C_FLAG_ADDSEND) == SET) {//收到ACK
            i2c_flag_clear(I2C_BUS, I2C_FLAG_ADDSEND);
            break;
        }
        if (i2c_flag_get(I2C_BUS, I2C_FLAG_AERR) == SET) {//收到NACK
            i2c_flag_clear(I2C_BUS, I2C_FLAG_AERR);
            i2c_stop_on_bus(I2C_BUS);
            return 0;
        }
    }
    if(timeout == 0)
    {
        i2c_stop_on_bus(I2C_BUS);
        return 0;
    }
    i2c_ack_config(I2C_BUS, I2C_ACK_ENABLE);
    while (RESET == i2c_flag_get(I2C_BUS, I2C_FLAG_RBNE)) {//I2C_FLAG_RBNE置位表示接收寄存器非空
        if (--timeout == 0) { i2c_stop_on_bus(I2C_BUS); return 0; }
    }
    data_high = i2c_data_receive(I2C_BUS);

    // 7. 读取低字节（不发送ACK，表示最后一个字节）
    i2c_ack_config(I2C_BUS, I2C_ACK_DISABLE);
    while (RESET == i2c_flag_get(I2C_BUS, I2C_FLAG_RBNE)) {
        if (--timeout == 0) { i2c_stop_on_bus(I2C_BUS); return 0; }
    }
    data_low = i2c_data_receive(I2C_BUS);

    i2c_stop_on_bus(I2C_BUS);
    delay_ms(1);
    return (uint16_t)(data_high<<8) | data_low;
}
