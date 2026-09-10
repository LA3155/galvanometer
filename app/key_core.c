#include "key_core.h"

/* 状态机状态 */
typedef enum{
    ST_IDLE = 0,
    ST_PRESSED,
    ST_WAIT_DBL,
    ST_LONG,
    ST_NUM
}key_state_t;

/* 按键输入状态 */
typedef enum{
    KEY_IN_NONE = 0,
    KEY_IN_DOWN,
    KEY_IN_UP,
    KEY_IN_LONG,
    KEY_IN_DBL,
    KEY_IN_NUM
}key_in_t;

typedef struct{
    uint8_t     next;
    key_event_t evt;
}key_trans_t;

#define DEBOUNCE_TICKS  2u    /* 20ms  消抖   */
#define LONG_TICKS      100u  /* 1s    长按   */
#define DBL_TICKS       30u   /* 300ms 双击窗 */

/* 状态转移表
 * 行=当前状态  列=本tick输入  值={次态,输出事件} */
static const key_trans_t trans_tab[ST_NUM][KEY_IN_NUM] = {
    /*                          NONE                        DOWN                            UP                      LONG                      DBL                    */
    /* ST_IDLE     */ {{ST_IDLE,    KEY_EVT_NONE},{ST_PRESSED, KEY_EVT_NONE},{ST_IDLE,     KEY_EVT_NONE},{ST_IDLE,     KEY_EVT_NONE},{ST_IDLE,     KEY_EVT_NONE}},
    /* ST_PRESSED  */ {{ST_PRESSED, KEY_EVT_NONE},{ST_PRESSED, KEY_EVT_NONE},{ST_WAIT_DBL, KEY_EVT_NONE},{ST_LONG, KEY_EVT_LONG},{ST_PRESSED, KEY_EVT_NONE}},
    /* ST_WAIT_DBL */ {{ST_WAIT_DBL,KEY_EVT_NONE},{ST_IDLE,  KEY_EVT_DBL},  {ST_WAIT_DBL, KEY_EVT_NONE},{ST_WAIT_DBL, KEY_EVT_NONE},{ST_IDLE,  KEY_EVT_CLICK}},
    /* ST_LONG     */ {{ST_LONG,    KEY_EVT_NONE},{ST_LONG,    KEY_EVT_NONE},{ST_IDLE,     KEY_EVT_NONE},{ST_LONG,     KEY_EVT_NONE},{ST_LONG,     KEY_EVT_NONE}},
};

void key_core_tick(key_core_t ctx[],const uint8_t level[],key_event_t evt[],uint8_t num)
{
    for(uint8_t i = 0; i < num; i++){
        key_in_t in = KEY_IN_NONE;
        evt[i]      = KEY_EVT_NONE;
        if(level[i] != ctx[i].stable){
            if(++ctx[i].db_cnt >= DEBOUNCE_TICKS){
                ctx[i].db_cnt = 0;
                ctx[i].stable = level[i];
                in = level[i] ? KEY_IN_DOWN : KEY_IN_UP;
            }
        }
        else{
            ctx[i].db_cnt = 0;
        }

        if(in == KEY_IN_NONE){
            if(((ctx[i].state == ST_PRESSED && ++ctx[i].hold_cnt >= LONG_TICKS))){
                in   = KEY_IN_LONG;
                ctx[i].hold_cnt = 0;
            }
            else if(((ctx[i].state == ST_WAIT_DBL && ++ctx[i].idle_cnt >= DBL_TICKS))){
                in = KEY_IN_DBL;
                ctx[i].idle_cnt = 0;
            }
        }

        const key_trans_t t = trans_tab[ctx[i].state][in];
        ctx[i].state = t.next;
        evt[i]       = t.evt;
    }
}