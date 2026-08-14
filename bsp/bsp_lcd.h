#pragma once

#define LCD_X_OFFSET  40
#define LCD_Y_OFFSET  53
#define LCD_WIDTH   240//X
#define LCD_HEIGHT  135//Y
#define BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT * 2)  // RGB565每个像素2字节

//Color
#define BLACK   0x0000
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

typedef struct {
    uint8_t width;      // 字符宽度8
    uint8_t height;     // 字符高度16
    const uint8_t (*data)[16];  // 指向字模数组
} font_t;

// 声明字符字模
extern const uint8_t ascii_8x16[95][16];
extern font_t font;

void lcd_init(void);
void lcd_fill_color(uint16_t color);
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void lcd_draw_pixel_update(uint16_t x, uint16_t y, uint16_t color);
void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void lcd_draw_char(uint16_t x, uint16_t y, char ch,const font_t *font, uint16_t fg, uint16_t bg);
void lcd_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg);