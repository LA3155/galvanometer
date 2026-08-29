#ifndef __UI_CURRENT_H__
#define __UI_CURRENT_H__

#include "page_manager.h"
extern page_t page_current;

void ui_current_update(uint32_t bus_mV, int32_t current_uA, uint32_t power_uW, uint8_t range);

#endif