#ifndef __UI_CONSUMPTION_H__
#define __UI_CONSUMPTION_H__

#include "page_manager.h"
extern page_t page_consumption;

void ui_consumption_update(double energy_mAh, double energy_mWh, uint32_t total_samples);

#endif