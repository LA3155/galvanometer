#include "user_ina226_Task.h"
#include "power_sample.h"
#include "event_detect.h"
#include "cdc.h"
#include "log_store.h"
#include "cmsis_os2.h"

extern osSemaphoreId_t sem;
extern osEventFlagsId_t lcd_ready_evt;
static state_machine_t sm;

void ina226TimerCallback(void* argument)
{
    osSemaphoreRelease(sem);
}

void ina226_Task(void* argument)
{
    uint32_t now;
    event_type_t evt;
    osEventFlagsWait(lcd_ready_evt, 1<<0, osFlagsWaitAny | osFlagsNoClear, osWaitForever);
    event_detect_init(&sm,osKernelGetTickCount());
    log_store_init();
    while (1)
    {
        osSemaphoreAcquire(sem,osWaitForever);
        ina226_read_sample(&power);
        waveform_push(&power, &data_log);
        event_pre_push(&power);
        now = osKernelGetTickCount();
        evt = event_detect_update(&sm,&power,now);

        if (evt != EVT_NONE)
        {
            const char *name = "UNKNOWN";
            switch (evt) 
            {
                case EVT_WAKEUP:        name = "WAKEUP";        break;
                case EVT_SLEEP_ENTER:   name = "SLEEP_ENTER";   break;
                case EVT_STATE_CHANGE:  name = "STATE_CHANGE";  break;
                case EVT_TX_BURST:      name = "TX_BURST";      break;
                case EVT_OVERCURRENT:   name = "OVERCURRENT";   break;
                case EVT_BROWNOUT:      name = "BROWNOUT";      break;
                default: break;
            }
            cdc_printf("[%lu] %s I=%lduA V=%lumV state=%d\r\n",
                       now, name, power.current_uA, power.bus_mV, sm.state);
        }
        osDelay(1);
    }
}