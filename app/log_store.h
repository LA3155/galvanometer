#pragma once

#include "flash.h"
#include "ina226.h"
#include "event_detect.h"

#define META_MAGIC          0x4C4F4731
#define LOG_META_BASE       0x000000
#define LOG_DATA_BASE       0x001000
#define LOG_DATA_END        0x800000
#define LOG_SECTOR_SIZE     4096

#define PRE_COUNT   50
#define POST_COUNT  50
//数据点
typedef struct __attribute__((packed))
{
    uint16_t bus_mV;
    int16_t current_mA;
}log_point_t;
//事件头
typedef struct __attribute__((packed))
{
    uint16_t magic;
    uint32_t time_ms;
    uint8_t event_type;
    uint8_t state_before;
    uint8_t state_after;
    uint8_t reserved;
    int32_t trigger_uA;
}event_header_t;

typedef struct __attribute__((packed))
{
    uint32_t magic;
    uint32_t write_addr;        //当前写地址，掉电恢复
    uint32_t count;
    uint32_t crc;
}log_meta_t;

uint32_t log_get_count(void);    
void log_erase_all(void);     
void log_export_csv(void);    
void log_store_init(void);
void log_write_event(event_type_t evt, power_state_t before,
                     power_state_t after, const power_sample_t *trigger,
                     const log_point_t *pre, uint8_t pre_n,
                     const log_point_t *post, uint8_t post_n);
void log_export_csv(void);
void Snapshot(log_record_t *data_log,power_sample_t *p);