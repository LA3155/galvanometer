#include "ui_consumption.h"
#include "stdio.h"

static lv_obj_t *ui_page_root           = NULL;
static lv_obj_t *label_info             = NULL;

static void ui_consumption_init(void)
{
    ui_page_root = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_page_root,240,135);
    lv_obj_set_pos(ui_page_root,0,0);
    lv_obj_set_style_bg_color(ui_page_root,lv_color_black(),0);
    lv_obj_set_style_border_width(ui_page_root,0,0);
    lv_obj_set_style_pad_all(ui_page_root,0,0);

    label_info = lv_label_create(ui_page_root);
    lv_label_set_text(label_info,"energy_mAh:000mAh \nenergy_mWh:000mWh \ntotal_samples:000mW");
    lv_obj_set_style_text_color(label_info,lv_color_white(),0);
    lv_obj_align(label_info,LV_ALIGN_TOP_LEFT,5,2);

}

static void ui_consumption_deinit(void)
{
    if(ui_page_root != NULL)
    {
        lv_obj_del(ui_page_root);
        ui_page_root    = NULL;
        label_info      = NULL;
    }
}

void ui_consumption_update(double energy_mAh, double energy_mWh, uint32_t total_samples)
{
    // 如果页面已经被销毁（处于其他页面），则不操作控件
    if (ui_page_root == NULL) return;
    uint32_t mah_int = (uint32_t)energy_mAh;
    uint32_t mah_dec = (uint32_t)((energy_mAh - mah_int) * 1000);
    uint32_t mwh_int = (uint32_t)energy_mWh;
    uint32_t mwh_dec = (uint32_t)((energy_mWh - mwh_int) * 1000);
    uint32_t run_sec = total_samples / 100;
    char buf[128];
    snprintf(buf, sizeof(buf), 
             "Energy : %lu.%03lu mAh\n"
             "Power  : %lu.%03lu mWh\n"
             "Samples: %lu (%lu s)",
             mah_int, mah_dec,
             mwh_int, mwh_dec,
             total_samples,
             run_sec);
    lv_label_set_text(label_info, buf);
}
page_t page_consumption = 
{
    .init       = ui_consumption_init,
    .deinit       = ui_consumption_deinit,
    .page_obj   = NULL,
};