#include "user_key_Task.h"
#include "key.h"
#include "key_core.h"
#include "page_manager.h"
#include "log_store.h"
#include "ui_consumption.h"
#include "cmsis_os2.h"
#include "power_sample.h"

extern osEventFlagsId_t message;

static void act_range_switch(void)      { range_switch(); }
static void act_ina226_filter(void)     { ina226_filter(&power); }
static void act_page_load(void)         { page_load(&page_consumption); }
static void act_page_back(void)         { page_back(); }
static void act_Snapshot(void)          { Snapshot(&data_log,&power); }
static void act_log_export_csv(void)    { log_export_csv(); }

static void (*const action_tab[KEY_IDX][KEY_EVT_NUM])(void) = {
    [0] = {[KEY_EVT_CLICK] = act_range_switch },
    [1] = {[KEY_EVT_CLICK] = act_ina226_filter },
    [2] = {[KEY_EVT_CLICK] = act_page_load ,[KEY_EVT_LONG] = act_Snapshot },
    [3] = {[KEY_EVT_CLICK] = act_page_back ,[KEY_EVT_LONG] = act_log_export_csv },
};

void key_Task(void* argument)
{
    uint8_t key_level[KEY_IDX] = {0};
    key_core_t key_core[KEY_IDX]= {0};
    key_event_t key_event[KEY_IDX]= {0};
    while (1)
    {
        key_scanf(key_level);
        key_core_tick(key_core,key_level,key_event,KEY_IDX);
        for(int i = 0; i < KEY_IDX; i++){
            if (key_event[i] != KEY_EVT_NONE && action_tab[i][key_event[i]]){
                action_tab[i][key_event[i]]();
            }
        }
        osDelay(10);
    }
}
