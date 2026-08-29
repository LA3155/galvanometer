#include "ui_current.h"
#include "stdio.h"

static lv_obj_t *ui_page_root           = NULL;
static lv_obj_t *label_info             = NULL;
static lv_obj_t *chart_waveform         = NULL;
static lv_chart_series_t *ser_current   = NULL;

static void ui_current_init(void)
{
    ui_page_root = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_page_root,240,135);
    lv_obj_set_pos(ui_page_root,0,0);
    lv_obj_set_style_bg_color(ui_page_root,lv_color_black(),0);
    lv_obj_set_style_border_width(ui_page_root,0,0);
    lv_obj_set_style_pad_all(ui_page_root,0,0);

    label_info = lv_label_create(ui_page_root);
    lv_label_set_text(label_info,"V:0.00V I:000mA P:000mW L:1");
    lv_obj_set_style_text_color(label_info,lv_color_white(),0);
    lv_obj_align(label_info,LV_ALIGN_TOP_LEFT,5,2);

    chart_waveform = lv_chart_create(ui_page_root);
    lv_obj_set_size(chart_waveform,230,105);
    lv_obj_align(chart_waveform,LV_ALIGN_BOTTOM_MID,0,-2);
    lv_obj_set_style_bg_color(chart_waveform,lv_color_black(),0);
    lv_obj_set_style_border_color(chart_waveform,lv_palette_main(LV_PALETTE_GREY),0);

    lv_chart_set_type(chart_waveform,LV_CHART_TYPE_LINE);
    lv_chart_set_update_mode(chart_waveform,LV_CHART_UPDATE_MODE_SHIFT);
    lv_chart_set_point_count(chart_waveform,100);
    lv_chart_set_range(chart_waveform,LV_CHART_AXIS_PRIMARY_Y,0,100);

    ser_current = lv_chart_add_series(chart_waveform,lv_palette_main(LV_PALETTE_GREEN),LV_CHART_AXIS_PRIMARY_Y);
}

static void ui_current_deinit(void)
{
    if(ui_page_root != NULL)
    {
        lv_obj_del(ui_page_root);
        ui_page_root    = NULL;
        label_info      = NULL;
        chart_waveform  = NULL;
        ser_current     = NULL;
    }
}

void ui_current_update(uint32_t bus_mV, int32_t current_uA, uint32_t power_uW, uint8_t range)
{
    // 如果页面已经被销毁（处于其他页面），则不操作控件
    if (ui_page_root == NULL) return;
    // 1. 更新顶部数值
    char buf[48];
    snprintf(buf, sizeof(buf), "V:%lu.%02luV I:%02ldmA P:%02ldmW L:%d",
             bus_mV / 1000, (bus_mV % 1000) / 10,
             current_uA / 1000,
             power_uW / 1000,
             range);

    lv_label_set_text(label_info,buf);
    lv_chart_set_next_value(chart_waveform,ser_current,(lv_coord_t)(current_uA / 1000));
}

page_t page_current = 
{
    .init       = ui_current_init,
    .deinit       = ui_current_deinit,
    .page_obj   = NULL,
};