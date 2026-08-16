#include "board.h"
#include "usbd_lld_int.h"

void board_clock_init(void)
{
    SystemCoreClockUpdate();
}

uint32_t board_get_core_clock_hz(void)
{
    return SystemCoreClock;
}

static uint8_t spi1_xfer(uint8_t data)
{
    while (RESET == spi_i2s_flag_get(FLASH_SPI, SPI_FLAG_TBE)) {
    }
    spi_i2s_data_transmit(FLASH_SPI, data);
    while (RESET == spi_i2s_flag_get(FLASH_SPI, SPI_FLAG_RBNE)) {
    }
    return (uint8_t)spi_i2s_data_receive(FLASH_SPI);
}

static void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);

    gpio_init(LCD_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_RST_PIN | LCD_CS_PIN | LCD_DC_PIN);
    gpio_bit_set(LCD_PORT, LCD_CS_PIN | LCD_RST_PIN | LCD_DC_PIN);

    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, LCD_SCK_PIN | LCD_MOSI_PIN);

    gpio_init(KEY1_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEY1_PIN | KEY2_PIN | KEY3_PIN);
    gpio_init(KEY4_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEY4_PIN);

    gpio_init(DUT_EN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, DUT_EN_PIN);
    dut_power_set(1);

    gpio_init(I2C_SCL_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN | I2C_SDA_PIN);

    gpio_init(FLASH_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, FLASH_CS_PIN);
    gpio_bit_set(FLASH_PORT, FLASH_CS_PIN);
    gpio_init(FLASH_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, FLASH_SCK_PIN | FLASH_MOSI_PIN);
    gpio_init(FLASH_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, FLASH_MISO_PIN);
}

static void spi_config(void)
{
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_SPI0);
    spi_struct_para_init(&spi_init_struct);
    spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
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

static void i2c_config(void)
{
    rcu_periph_clock_enable(RCU_I2C0);
    i2c_clock_config(I2C_BUS, 100000, I2C_DTCY_2);
    i2c_mode_addr_config(I2C_BUS, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x32);
    i2c_enable(I2C_BUS);
    i2c_ack_config(I2C_BUS, I2C_ACK_ENABLE);
}

void key_exti_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);

    gpio_init(GPIOC, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_13);
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_14);
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_15);

    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_0);

    exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_13);

    exti_init(EXTI_14, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_14);

    exti_init(EXTI_15, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_15);

    exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_0);

    nvic_irq_enable(EXTI10_15_IRQn, 5, 0);

    nvic_irq_enable(EXTI0_IRQn, 5, 0);
}

volatile uint8_t g_key_event = 0;

void board_init(void)
{
    board_clock_init();
    SysTick_Config(SystemCoreClock / 1000U);
    gpio_config();
    spi_config();
    i2c_config();
    key_exti_init();
}

void dut_power_set(uint8_t on)
{
    if (on) {
        gpio_bit_set(DUT_EN_PORT, DUT_EN_PIN);
    } else {
        gpio_bit_reset(DUT_EN_PORT, DUT_EN_PIN);
    }
}

uint32_t spi_flash_read_id(void)
{
    uint32_t id;
    gpio_bit_reset(FLASH_PORT, FLASH_CS_PIN);
    spi1_xfer(0x9F);
    id = ((uint32_t)spi1_xfer(0xFF) << 16);
    id |= ((uint32_t)spi1_xfer(0xFF) << 8);
    id |= spi1_xfer(0xFF);
    gpio_bit_set(FLASH_PORT, FLASH_CS_PIN);
    return id;
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
