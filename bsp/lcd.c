#include "gpio.h"
#include "lcd.h"
#include "font_8X16.h"
#include "delay.h"
#include "ina226.h"
#include "dma.h"
#include "cmsis_os2.h"

static void lcd_dc_cmd(void) { gpio_bit_reset(LCD_PORT, LCD_DC_PIN); }//引脚置0发送命令
static void lcd_dc_data(void) { gpio_bit_set(LCD_PORT, LCD_DC_PIN); }
static void lcd_cs_low(void) { gpio_bit_reset(LCD_PORT, LCD_CS_PIN); }
static void lcd_cs_high(void) { gpio_bit_set(LCD_PORT, LCD_CS_PIN); }

font_t font = {8,16,ascii_8x16};
extern osMutexId_t     spi_mutex;

// 专用的轮询单字节发送函数
static void spi0_write_byte_polled(uint8_t data)
{
    // 等待发送缓冲区空（只发模式只需要 TBE）
    while (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)) {
    }
    
    // 发送数据
    spi_i2s_data_transmit(SPI0, data);
    
    // 只发模式：等待发送完成（TRA 标志）
    while (SET == spi_i2s_flag_get(SPI0, SPI_FLAG_TRANS)) {
    }
}

static void lcd_send_reg(const uint8_t command)
{
    // uint8_t buf[1];
    // buf[0] = command;
    osMutexAcquire(spi_mutex, osWaitForever);
    lcd_cs_low();
    lcd_dc_cmd();
    spi0_write_byte_polled(command);
    // spi0_transmit_dma(buf,1);
    lcd_dc_data();
    lcd_cs_high();
    osMutexRelease(spi_mutex);
}

void lcd_send_data(uint8_t* buf,uint32_t len)
{
    lcd_cs_low();
    spi0_transmit_dma(buf,len);
    // for (uint32_t i = 0; i < len; i++) spi0_write_byte_polled(buf[i]);
    lcd_cs_high();
}

static void lcd_write_data8(uint8_t data)
{
    lcd_send_data(&data,1);
    // spi0_write_byte_polled(data);
}

static void lcd_write_data16(uint16_t data)
{
    uint8_t b[2] = { (uint8_t)(data >> 8), (uint8_t)data };
    lcd_send_data(b,2);
    // spi0_write_byte_polled((uint8_t)(data >> 8));
    // spi0_write_byte_polled((uint8_t)data);
}

void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    x0 += LCD_X_OFFSET;
    x1 += LCD_X_OFFSET;
    y0 += LCD_Y_OFFSET;
    y1 += LCD_Y_OFFSET;
    uint8_t buf[8] =  { x0 >> 8, x0, x1 >> 8, x1, y0 >> 8, y0, y1 >> 8, y1 };
    lcd_send_reg(0x2A);
    lcd_send_data(buf,4);
    lcd_send_reg(0x2B);
    lcd_send_data(buf+4,4);
    lcd_send_reg(0x2C);
}

void lcd_fill_color(uint16_t color)
{
    uint8_t buf[LCD_WIDTH*2];

    for (uint32_t i = 0; i < LCD_WIDTH; i++) {
        buf[i*2] = ((uint8_t)(color >> 8));
        buf[i*2 +1] = ((uint8_t)color);
    }
    lcd_set_window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    for (int i = 0; i < LCD_HEIGHT; i++)
    {
        lcd_send_data(buf,LCD_WIDTH*2);
    }
}

void lcd_init(void)
{
    gpio_bit_reset(LCD_PORT, LCD_RST_PIN);
    delay_ms(30);
    gpio_bit_set(LCD_PORT, LCD_RST_PIN);
    delay_ms(120);

    lcd_send_reg(0x01); // Software reset
    delay_ms(150);
    lcd_send_reg(0x11); // Sleep out
    delay_ms(150);
    lcd_send_reg(0x36); // MADCTL
    lcd_write_data8(0x60); // 先用 0x00，后面再调方向
    lcd_send_reg(0x3A); // COLMOD
    lcd_write_data8(0x05); // RGB565
    lcd_send_reg(0x21); // Display inversion on，若颜色异常可试 0x20
    lcd_send_reg(0x13); // Normal display mode on
    lcd_send_reg(0x29); // Display on
    delay_ms(50);
    lcd_fill_color(BLACK);
}

/**
 * @brief  在指定坐标画一个像素点
 * @param  x: X 坐标
 * @param  y: Y 坐标
 * @param  color: 像素点的颜色 (RGB565格式)
 */
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    // 1. 设置画图区域为 (x, y) 到 (x, y)，也就是只选中一个像素大小的窗口
    lcd_set_window(x, y, x, y);
    
    // 2. 把 16 位的颜色数据发给屏幕
    lcd_write_data16(color);
}

void lcd_clear_column(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color)
{
    uint16_t count = WAVE_BOTTOM - WAVE_Y + 1;
    static uint8_t buf[WAVE_H*2];
    lcd_set_window(x, y0, x, y1);
    for (uint16_t i = 0; i < count; i++) {
        buf[i*2] = ((uint8_t)(color >> 8));
        buf[i*2 +1] = ((uint8_t)color);
    }
    lcd_send_data(buf,count*2);
}

void lcd_draw_pixel_update(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_clear_column(x, WAVE_Y, WAVE_BOTTOM, BLACK);
    lcd_draw_pixel(x, y, color);
}

void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint8_t buf[w*2];

    for (uint32_t i = 0; i < w; i++) {
        buf[i*2] = ((uint8_t)(color >> 8));
        buf[i*2 +1] = ((uint8_t)color);
    }
    lcd_set_window(x,y,x+w-1,y+h-1);
    for (int i = 0; i < h; i++)
    {
        lcd_send_data(buf,w*2);
    }
}

/**
 * @brief  在指定位置显示一个字符
 * @param  x: 起始X坐标
 * @param  y: 起始Y坐标
 * @param  ch: 要显示的字符（如 'A'）
 * @param  font: 字体信息
 * @param  fg: 前景色（文字颜色）
 * @param  bg: 背景色
 */
void lcd_draw_char(uint16_t x, uint16_t y, char ch, 
                   const font_t *font, uint16_t fg, uint16_t bg)
{
    uint8_t width = font->width;      // 8
    uint8_t height = font->height;    // 16
    const uint8_t *char_data;
    static uint8_t pixel_buf[256];
    
    if (ch < 0x20 || ch > 0x7E) {
        ch = 0x20;  // 无效字符显示空格
    }
    
    // 'A'的ASCII是0x41，减去0x20得到索引33
    char_data = font->data[ch - 0x20];
    
    // ③ 设置显示窗口（整个字符区域）
    lcd_set_window(x, y, x + width - 1, y + height - 1);
    for (uint8_t row = 0; row < height; row++) {
        uint8_t row_byte = char_data[row];  //16行的字节获取
        
        for (uint8_t col = 0; col < width; col++) {
            // 判断当前列对应的位是0还是1
            uint16_t color = (row_byte & (0x80 >> col)) ? fg : bg;
            pixel_buf[(row*width +col)*2] = ((uint8_t)(color >> 8));
            pixel_buf[(row*width +col)*2 +1] = ((uint8_t)color);
        }
    }
    lcd_send_data(pixel_buf,width * height * 2);
}

void lcd_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg)
{
    while(*str != '\0')
    {
        lcd_draw_char(x, y, *str, &font,fg, bg);
        x += 8; // 因为字体宽 8 像素，画完一个光标向右移动 8
        str++;
    }
}

//数据作图
void lcd_draw_line_into_buf(uint16_t buf[][WAVE_W],int16_t x0,int16_t y0,int16_t x1,int16_t y1,uint16_t color)
{
    int16_t dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
    int16_t dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;

    while (1)
    {
        if (x0 >= 0 && x0 < WAVE_W && y0 >= 0 && y0 < WAVE_H)
            buf[y0][x0] = __builtin_bswap16(color);
        if(x0 == x1 && y0 == y1) break;
        int16_t e2 = 2*err;
        if(e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if(e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
    
}