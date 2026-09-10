#ifndef __KEY_CORE_H__
#define __KEY_CORE_H__

#include "stdint.h"

typedef enum{
    KEY_EVT_NONE = 0,
    KEY_EVT_CLICK,
    KEY_EVT_DBL,
    KEY_EVT_LONG,
    KEY_EVT_NUM
}key_event_t;

typedef struct {
    uint8_t     state;
    uint8_t     stable;
    uint8_t     db_cnt;
    uint16_t    hold_cnt;
    uint16_t    idle_cnt;
}key_core_t;

void key_core_tick(key_core_t ctx[],const uint8_t level[],key_event_t evt[],uint8_t num);

#endif