#include "event_detect.h"
#include "log_store.h"

// 阈值参数（可调）
#define BASELINE_ALPHA       5        // 基线滑动系数：baseline = baseline*95/100 + cur*5/100
#define CHANGE_RATIO         3        // 3倍变化算显著
#define STABLE_MS            300      // 过渡态持续300ms才算稳定
#define TX_THRESHOLD_UA      50000    // >50mA 视为可能TX
#define TX_MAX_DURATION_MS   50       // TX突发持续<50ms
#define OC_THRESHOLD_UA      200000   // 过流阈值 200mA
#define OC_LOG_INTERVAL_MS   1000     // 过流日志去抖 1秒一条
#define BO_THRESHOLD_MV      3000     // 电压跌落阈值 3.0V
#define BO_LOG_INTERVAL_MS   1000     // 跌落日志去抖 1秒一条

static log_point_t pre_buf[PRE_COUNT];
static log_point_t post_buf[POST_COUNT];
static log_point_t pre_linear[PRE_COUNT];//处理排序问题
static uint8_t pre_index;
static power_state_t pending_before,pending_after;
static power_sample_t pending_trigger;
static event_type_t pending_evt;

//记录事件触发时数据到FLASH
static int16_t post_trigger_count = 0;

void event_detect_init(state_machine_t *sm,uint32_t now_ms)
{
    sm->state           = STATE_INIT;
    sm->baseline_uA      = 0;
    sm->last_change_ms   = now_ms;
    sm->state_enter_ms   = now_ms;
    sm->in_tx            = 0;
    sm->tx_start_ms      = 0;
    sm->last_oc_log_ms   = 0;
    sm->last_bo_log_ms   = 0;
    sm->sleep_total_ms   = 0;
    sm->idle_total_ms    = 0;
    sm->active_total_ms  = 0;
    sm->wakeup_count     = 0;
    sm->tx_burst_count   = 0;
}

static power_state_t classify_steady(int32_t current_uA)
{
    if (current_uA < 20000)  return STATE_SLEEP;    // <10mA
    if (current_uA < 60000) return STATE_IDLE;     // 10~60mA
    return STATE_ACTIVE;                            // >10mA
}

void event_pre_push(const power_sample_t *p)
{
    pre_buf[pre_index].bus_mV = p->bus_mV;
    pre_buf[pre_index].current_mA = (uint16_t)(p->current_uA / 1000);
    pre_index = (pre_index+1) % PRE_COUNT;
}

static void linearize_pre(void)
{
    for (uint8_t i = 0; i < PRE_COUNT; i++) {
        pre_linear[i] = pre_buf[(pre_index + i) % PRE_COUNT];
    }
}

static void event_trigger_start(event_type_t evt, power_state_t before,
                                power_state_t after, const power_sample_t *p)
{
    pending_evt     = evt;
    pending_before  = before;
    pending_after   = after;
    pending_trigger = *p;
    post_trigger_count  = POST_COUNT;
}

event_type_t event_detect_update(state_machine_t *sm,
                            const power_sample_t *p,uint32_t now_ms)
{
    int32_t cur = p->current_uA;
    event_type_t evt = EVT_NONE;
    if (sm->baseline_uA == 0)
    {
        sm->baseline_uA = cur;
    }
    else
    {
        sm->baseline_uA = (sm->baseline_uA *(100 - BASELINE_ALPHA) + 
                            (cur * BASELINE_ALPHA) / 100);
    }

    if (sm->state == STATE_INIT) 
    {
        if (now_ms > 1000) {                          // 开机1秒后
            sm->state = classify_steady(cur);
            sm->prev_state = sm->state;
            sm->state_enter_ms = now_ms;              
        }
        return EVT_NONE;                             
    }

    int8_t significant = 0;
    if(sm->baseline_uA >100 && cur >100)
    {
        if(cur > sm->baseline_uA * CHANGE_RATIO)   significant = 1;
        if(cur * CHANGE_RATIO < sm->baseline_uA)   significant = 1;
    }

    if(significant && sm->state != STATE_TRANSITION)
    {
        sm->prev_state = sm->state;
        sm->state = STATE_TRANSITION;
        sm->last_change_ms = now_ms;
    }

    if(sm->state == STATE_TRANSITION)
    {
        if(now_ms - sm->last_change_ms > STABLE_MS)
        {
            power_state_t new_state = classify_steady(cur);

            if(new_state == STATE_SLEEP && sm->prev_state != STATE_SLEEP)
            {
                evt = EVT_SLEEP_ENTER;
                event_trigger_start(evt,sm->prev_state,new_state,p);
            }
            else if(sm->prev_state == STATE_SLEEP &&
                    (new_state == STATE_ACTIVE || new_state == STATE_IDLE))
            {
                evt = EVT_WAKEUP;
                sm->wakeup_count++;
                event_trigger_start(evt,sm->prev_state,new_state,p);
            }
            else if((sm->prev_state == STATE_IDLE && new_state == STATE_ACTIVE) ||
                    (sm->prev_state == STATE_ACTIVE && new_state == STATE_IDLE))
            {
                evt = EVT_STATE_CHANGE;
                event_trigger_start(evt,sm->prev_state,new_state,p);
            }

            uint32_t dur = now_ms - sm->state_enter_ms;
            if(sm->prev_state == STATE_SLEEP)
            {
                sm->sleep_total_ms += dur;
            }
            else if(sm->prev_state == STATE_IDLE)
            {
                sm->idle_total_ms += dur;
            }
            else if(sm->prev_state == STATE_ACTIVE)
            {
                sm->active_total_ms += dur;
            }
            sm->state = new_state;
            sm->state_enter_ms = now_ms;
        }
    }
    //事件触发记录
    if(post_trigger_count > 0)
    {
        post_buf[POST_COUNT - post_trigger_count].bus_mV = p->bus_mV;
        post_buf[POST_COUNT - post_trigger_count].current_mA = (uint16_t)(p->current_uA / 1000);
        post_trigger_count--;
        if(post_trigger_count ==0)
        {
            linearize_pre();
            log_write_event(pending_evt,pending_before,pending_after,
                            &pending_trigger,pre_linear,PRE_COUNT,post_buf,POST_COUNT);
        }
    }

    return evt;
}