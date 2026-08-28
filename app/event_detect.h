#pragma once

#include "ina226.h"

typedef enum{
    STATE_INIT = 0,       // 初始（刚开机未稳定）
    STATE_SLEEP,          // 睡眠（<1mA 稳态）
    STATE_IDLE,           // 待机（1~10mA 稳态）
    STATE_ACTIVE,         // 活跃（>10mA 稳态）
    STATE_TRANSITION,     // 过渡（电流正在变化，未稳定）
} power_state_t;

// 事件类型（写 log 时用）
typedef enum {
    EVT_NONE = 0,
    EVT_WAKEUP,           // 唤醒（Sleep→Active/Idle）
    EVT_SLEEP_ENTER,      // 进入睡眠（→Sleep）
    EVT_STATE_CHANGE,     // 通用状态转换
    EVT_TX_BURST,         // 蓝牙/WiFi 发射突发
    EVT_OVERCURRENT,      // 过流告警
    EVT_BROWNOUT,         // 电压跌落
} event_type_t;

// 状态机上下文
typedef struct {
    power_state_t state;
    power_state_t prev_state;
    int32_t baseline_uA;       // 慢速滑动平均（约1秒时间常数）
    uint32_t last_change_ms;   // 上次显著变化时刻
    uint32_t state_enter_ms;   // 进入当前稳态的时刻

    // TX 突发检测
    int8_t  in_tx;
    uint32_t tx_start_ms;

    // 过流/跌落 去抖
    uint32_t last_oc_log_ms;
    uint32_t last_bo_log_ms;

    // 统计（每状态累计）
    uint32_t sleep_total_ms;
    uint32_t idle_total_ms;
    uint32_t active_total_ms;
    uint32_t wakeup_count;
    uint32_t tx_burst_count;
} state_machine_t;

// API
void    event_detect_init(state_machine_t *sm, uint32_t now_ms);
// 每次采样调用，返回触发的事件类型（EVT_NONE 表示无事件）
event_type_t event_detect_update(state_machine_t *sm,
                                  const power_sample_t *p,
                                  uint32_t now_ms);
const char* event_state_name(power_state_t s);   // 调试用，返回状态名
void event_pre_push(const power_sample_t *p);