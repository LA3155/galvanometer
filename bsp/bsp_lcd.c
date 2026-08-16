#include "board.h"
#include "bsp_lcd.h"
#include "font_8X16.h"
#include "cmsis_os2.h"

static void lcd_cs_low(void) { gpio_bit_reset(LCD_PORT, LCD_CS_PIN); }
static void lcd_cs_high(void) { gpio_bit_set(LCD_PORT, LCD_CS_PIN); }
static void lcd_dc_cmd(void) { gpio_bit_reset(LCD_PORT, LCD_DC_PIN); }//引脚置0发送命令
static void lcd_dc_data(void) { gpio_bit_set(LCD_PORT, LCD_DC_PIN); }
font_t font = {8,16,ascii_8x16};

osMutexId_t spi_mutex;

static uint8_t spi0_xfer(uint8_t data)
{
    uint8_t result;
    while (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));// 等待发送缓冲区为空（TBE = Transmit Buffer Empty）
    spi_i2s_data_transmit(SPI0, data);
    while (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));
    result = (uint8_t)spi_i2s_data_receive(SPI0);
    
    return result;
}

static void lcd_write_cmd(uint8_t cmd)
{
    lcd_cs_low();
    lcd_dc_cmd();
    spi0_xfer(cmd);
    lcd_cs_high();
}

static void lcd_write_data8(uint8_t data)
{
    lcd_cs_low();
    lcd_dc_data();
    spi0_xfer(data);
    lcd_cs_high();
}

static void lcd_write_data16(uint16_t data)
{
    lcd_cs_low();
    lcd_dc_data();
    spi0_xfer((uint8_t)(data >> 8));
    spi0_xfer((uint8_t)data);
    lcd_cs_high();
}

static void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    x0 += LCD_X_OFFSET;
    x1 += LCD_X_OFFSET;
    y0 += LCD_Y_OFFSET;
    y1 += LCD_Y_OFFSET;
    lcd_write_cmd(0x2A);
    lcd_write_data16(x0);
    lcd_write_data16(x1);
    lcd_write_cmd(0x2B);
    lcd_write_data16(y0);
    lcd_write_data16(y1);
    lcd_write_cmd(0x2C);
}

void lcd_fill_color(uint16_t color)
{
    lcd_set_window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    lcd_cs_low();
    lcd_dc_data();
    for (uint32_t i = 0; i < (LCD_WIDTH * LCD_HEIGHT); i++) {
        spi0_xfer((uint8_t)(color >> 8));
        spi0_xfer((uint8_t)color);
    }
    lcd_cs_high();
}

void lcd_init(void)
{
    gpio_bit_reset(LCD_PORT, LCD_RST_PIN);
    delay_ms(30);
    gpio_bit_set(LCD_PORT, LCD_RST_PIN);
    delay_ms(120);

    lcd_write_cmd(0x01); // Software reset
    delay_ms(150);
    lcd_write_cmd(0x11); // Sleep out
    delay_ms(150);
    lcd_write_cmd(0x36); // MADCTL
    lcd_write_data8(0x60); // 先用 0x00，后面再调方向
    lcd_write_cmd(0x3A); // COLMOD
    lcd_write_data8(0x05); // RGB565
    lcd_write_cmd(0x21); // Display inversion on，若颜色异常可试 0x20
    lcd_write_cmd(0x13); // Normal display mode on
    lcd_write_cmd(0x29); // Display on
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
    uint16_t count = y1 - y0 + 1;

    lcd_set_window(x, y0, x, y1);

    lcd_cs_low();
    lcd_dc_data();

    for (uint16_t i = 0; i < count; i++) {
        spi0_xfer((uint8_t)(color >> 8));
        spi0_xfer((uint8_t)color);
    }

    lcd_cs_high();
}

void lcd_draw_pixel_update(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_clear_column(x, WAVE_Y, WAVE_BOTTOM, BLACK);
    lcd_draw_pixel(x, y, color);
}

void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint16_t total_point,i;
    total_point = w*h;
    lcd_set_window(x,y,x+w-1,y+h-1);
    for ( i = 0; i < total_point; i++)
    {
        lcd_write_data16(color);
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
    
    if (ch < 0x20 || ch > 0x7E) {
        ch = 0x20;  // 无效字符显示空格
    }
    
    // 'A'的ASCII是0x41，减去0x20得到索引33
    char_data = font->data[ch - 0x20];
    
    // ③ 设置显示窗口（整个字符区域）
    lcd_set_window(x, y, x + width - 1, y + height - 1);
    lcd_cs_low();
    lcd_dc_data();
    for (uint8_t row = 0; row < height; row++) {
        uint8_t row_byte = char_data[row];  //16行的字节获取
        
        for (uint8_t col = 0; col < width; col++) {
            // 判断当前列对应的位是0还是1
            uint16_t color = (row_byte & (0x80 >> col)) ? fg : bg;
            spi0_xfer((uint8_t)(color >> 8));
            spi0_xfer((uint8_t)color);
        }
    }
    lcd_cs_high();
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
