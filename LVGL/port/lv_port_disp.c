#include "lv_port_disp.h"
#include "lvgl.h"
#include "lcd.h"

static lv_disp_draw_buf_t draw_buf_dsc;
static lv_color_t buf[LCD_WIDTH * 20];

static void disp_flush(lv_disp_drv_t *disp_drv,const lv_area_t *area,lv_color_t *color_p);

void lv_port_disp_init(void)
{
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    lv_disp_draw_buf_init(&draw_buf_dsc,buf,NULL,LCD_WIDTH * 20);
    disp_drv.draw_buf   = &draw_buf_dsc;
    disp_drv.hor_res    = LCD_WIDTH;
    disp_drv.ver_res    = LCD_HEIGHT;
    disp_drv.flush_cb   = disp_flush;

    lv_disp_drv_register(&disp_drv);
}

static void disp_flush(lv_disp_drv_t *disp_drv,const lv_area_t *area,lv_color_t *color_p)
{
    lcd_set_window(area->x1,area->y1,area->x2,area->y2);
    uint32_t len = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2;
    lcd_send_data((uint8_t *)color_p,len);
    lv_disp_flush_ready(disp_drv);
}