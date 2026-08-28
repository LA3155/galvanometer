#include "log_store.h"
#include "cdc.h"
#include "cmsis_os2.h"
#include "stdio.h"

static log_meta_t meta;
static uint32_t write_addr;

//跨页写
static void flash_write_multi(uint32_t addr,const uint8_t *data,uint16_t len)
{
    while(len > 0)
    {
        uint16_t page_remain = 256 - (addr % 256);
        uint16_t chunk       = (len < page_remain) ? len : page_remain;
        flash_write_page(addr,data,chunk);
        addr += chunk;
        data += chunk;
        len  -= chunk;
    }
}
//跨扇区自动删除
static void check_erase_sector(uint32_t addr)
{
    if((addr % LOG_SECTOR_SIZE) == 0)
    {
        flash_erase_sector(addr);
    }
}

static void meta_save(void)
{
    meta.write_addr = write_addr;
    meta.count++;
    flash_erase_sector(LOG_META_BASE);
    flash_write_page(LOG_META_BASE,(uint8_t *)&meta,sizeof(meta));
}

void log_store_init(void)
{
    flash_read(LOG_META_BASE,(uint8_t *)&meta,sizeof(meta));

    if(meta.magic != META_MAGIC)
    {
        meta.magic      = META_MAGIC;
        meta.write_addr = LOG_DATA_BASE;
        meta.count      = 0;
        meta.crc        = 0;
        flash_erase_sector(LOG_META_BASE);
        flash_write_page(LOG_META_BASE,(uint8_t *)&meta,sizeof(meta));
    }
    write_addr  = meta.write_addr;
}

void log_write_event(event_type_t evt,power_state_t before,power_state_t after,
                    const power_sample_t *trigger,
                    const log_point_t *pre,uint8_t pre_n,
                    const log_point_t *post,uint8_t post_n)
{
    uint16_t total_len = sizeof(event_header_t) + (pre_n + post_n) * sizeof(log_point_t);
    if(write_addr + total_len > LOG_DATA_END)
    {
        write_addr = LOG_DATA_BASE;
    }
    check_erase_sector(write_addr);

    event_header_t hdr = 
    {
        .magic          = 0xEA01,
        .time_ms        = osKernelGetTickCount(),
        .event_type     = (uint8_t)evt,
        .state_before   = (uint8_t)before,
        .state_after    = (uint8_t)after,
        .reserved       = 0,
        .trigger_uA     = trigger->current_uA,
    };
    flash_write_multi(write_addr,(uint8_t *)&hdr,sizeof(hdr));
    write_addr += sizeof(hdr);
    
    flash_write_multi(write_addr,(uint8_t *)pre,pre_n * sizeof(log_point_t));
    write_addr += pre_n * sizeof(log_point_t);

    flash_write_multi(write_addr,(uint8_t *)post,post_n * sizeof(log_point_t));
    write_addr += post_n * sizeof(log_point_t);

    meta_save();
    event_header_t verify;
    flash_read(write_addr - total_len, (uint8_t *)&verify, sizeof(verify));
}

uint32_t log_get_count(void)
{
    return meta.count;
}

void log_erase_all(void)
{
    for(uint32_t addr = LOG_DATA_BASE; addr < LOG_DATA_BASE + 0x10000; addr += LOG_SECTOR_SIZE)
    {
        flash_erase_sector(addr);
    }
    meta.write_addr = LOG_DATA_BASE;
    meta.count      = 0;
    flash_erase_sector(LOG_META_BASE);
    flash_write_page(LOG_META_BASE,(uint8_t *)&meta,sizeof(meta));
    write_addr = LOG_DATA_BASE;
}

void log_export_csv(void)
{
    uint32_t addr = LOG_DATA_BASE;
    uint32_t n = 0;
    uint8_t line[32];
    while(addr < write_addr && n < meta.count)
    {
        event_header_t hdr;
        flash_read(addr,(uint8_t *)&hdr,sizeof(hdr));

        if(hdr.magic != 0xEA01)
            break;
        snprintf(line,sizeof(line),"%lu,%d,%d,%d,%ld\r\n",
                   hdr.time_ms, hdr.event_type,
                   hdr.state_before, hdr.state_after,
                   hdr.trigger_uA / 1000);
        
        cdc_printf("%s",line);
        addr += sizeof(hdr);

        uint16_t total_pts = PRE_COUNT + POST_COUNT;
        for (uint16_t i = 0; i < total_pts; i++)
        {
            log_point_t pt;
            flash_read(addr,(uint8_t *)&pt,sizeof(pt));
            cdc_printf("  %d,%d\r\n", pt.current_mA, pt.bus_mV);
            addr += sizeof(pt);
            osDelay(1);
        }
        n++;
    }
}