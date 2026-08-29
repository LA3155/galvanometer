#ifndef __PAGE_MANAGE_H__
#define __PAGE_MANAGE_H__

#include "lvgl.h"
#define MAX_DEPTH 3

typedef struct 
{
    void (*init)(void);
    void (*deinit)(void);
    lv_obj_t *page_obj;
}page_t;

typedef struct 
{
    page_t *pages[MAX_DEPTH];
    uint8_t top;
}pagestack_t;


void page_manage_init(page_t *homepage);
void page_load(page_t *new_page);
void page_back(void);
page_t *page_get_nowpage(void);

#endif