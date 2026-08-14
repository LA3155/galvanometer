#include "board.h"
#include "cdc.h"

volatile uint32_t g_flash_id;
volatile uint32_t g_i2c_low_mask;
volatile uint32_t g_i2c_high_mask;
volatile uint8_t  g_key_state;
volatile uint8_t  g_dut_power_state;
extern volatile uint8_t g_key_event;
volatile uint8_t  range = 2;
extern usb_dev usbd_cdc;
uint32_t g_time_ms;
uint8_t g_flag1;
uint8_t g_flag2;

void key_process(void)
{
    if (g_key_event)
    {
        if (g_key_event&0x01)//切换量程 
        {
            g_key_event &= ~0x01;
            range++;
            range%=3;
        }
        if (g_key_event&0x02)//零点校准
        {
            g_key_event &= ~0x02;
            ina226_filter((current_range_t)range);
        }
        if (g_key_event&0x04)//截图，获取时间戳，电流，电压，功耗等数据
        {
            g_key_event &= ~0x04;
            Snapshot(&data_log,(current_range_t)range);
        }
        if (g_key_event&0x08)//导出CSV
        {
            g_key_event &= ~0x08;
        }
    }
}

int main(void)
{
    board_init();
    cdc_init();
    lcd_init();

    g_flash_id = spi_flash_read_id();
    g_i2c_low_mask = i2c_scan_mask_low32();
    g_i2c_high_mask = i2c_scan_mask_high32();
    lcd_fill_color(0x0000);
    lcd_draw_string(0, RANGE_Y, "100mA", WHITE, BLACK);
    lcd_draw_string(0, (WAVE_BOTTOM-(WAVE_BOTTOM-RANGE_Y)/2), "50mA", WHITE, BLACK);
    lcd_draw_string(0, WAVE_BOTTOM-10, "0mA", WHITE, BLACK);
    while (1) {
        if(g_flag1 == 1)
        {
            g_flag1 = 0;
            ina226_read_sample((current_range_t)range,&power);
            waveform_push(&power, &data_log);
            waveform_draw_current(&data_log,Power);
            //按键处理函数
            key_process();
        }
        if(g_flag2 == 1)
        {
            g_flag2 = 0;
            lcd_show_power_sample(&power,(current_range_t)range);
        }
    }
}

void TIMER2_IRQHandler(void)
{
    if (SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP)) {
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
        g_time_ms++;
        if(g_time_ms%10 == 0)
        {
            g_flag1 =1;
        }
        if(g_time_ms%50 == 0)
        {
            g_flag2 =1;
        }
    }
}