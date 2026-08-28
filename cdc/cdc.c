#include "cdc.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "cdc_acm_core.h"
#include "usbd_hw.h"
#include "usbd_core.h"
#include "delay.h"

static usb_dev usbd_cdc = {0};
static char cli_line[CLI_LINE_MAX];
static uint16_t cli_index = 0;

void cdc_init(void)
{
    rcu_config();
    gpio_config();

    usbd_init(&usbd_cdc, &cdc_desc, &cdc_class);

    nvic_config();
    usbd_connect(&usbd_cdc);
}

uint8_t cdc_is_ready(void)
{
    return (USBD_CONFIGURED == usbd_cdc.cur_status) &&
           (0U == cdc_acm_check_ready(&usbd_cdc));
}

uint32_t cdc_write(const void *data, uint32_t len)
{
    usb_cdc_handler *cdc;
    const uint8_t *p = (const uint8_t *)data;
    uint32_t sent = 0;
    uint32_t chunk;
    uint32_t start;

    if ((data == NULL) || (len == 0U)) {
        return 0U;
    }

    if (USBD_CONFIGURED != usbd_cdc.cur_status) {
        return 0U;
    }

    cdc = (usb_cdc_handler *)usbd_cdc.class_data[CDC_COM_INTERFACE];
    if (cdc == NULL) {
        return 0U;
    }

    while (sent < len) {
        start = Sys_GetTick();

        while (cdc_acm_check_ready(&usbd_cdc) != 0U) {
            if ((Sys_GetTick() - start) > 100U) {
                return sent;
            }
        }

        chunk = len - sent;
        if (chunk > USB_CDC_RX_LEN) {
            chunk = USB_CDC_RX_LEN;
        }

        memcpy(cdc->data, &p[sent], chunk);
        cdc->receive_length = chunk;
        cdc_acm_data_send(&usbd_cdc);

        sent += chunk;
    }

    return sent;
}

uint32_t cdc_read(uint8_t *buf, uint32_t max_len)
{
    usb_cdc_handler *cdc;
    uint32_t len;

    if ((NULL == buf) || (0U == max_len)) {
        return 0U;
    }
    if (USBD_CONFIGURED != usbd_cdc.cur_status) {
        return 0U;
    }
    cdc = (usb_cdc_handler *)usbd_cdc.class_data[CDC_COM_INTERFACE];
    if (NULL == cdc) {
        return 0U;
    }
    if (0U == cdc->packet_receive) {
        return 0U;
    }
    len = cdc->receive_length;
    if (len > max_len) {
        len = max_len;
    }

    memcpy(buf, cdc->data, len);
    cdc_acm_data_receive(&usbd_cdc);

    return len;
}

static void cli_handle_command(const char *cmd,power_sample_t *power,uint8_t current_range)
{
    if (0 == strcmp(cmd, "help")) {
        cdc_printf("cmd:\r\n");
        cdc_printf("  help\r\n");
        cdc_printf("  status\r\n");
    } else if (0 == strcmp(cmd, "status")) {
        cdc_printf("V=%lu mV, I=%ld uA, P=%ld uW, range=%d\r\n",
                   power->bus_mV,
                   power->current_uA,
                   power->power_uW,
                   current_range);
    } else {
        cdc_printf("ERR: unknown cmd: %s\r\n", cmd);
    }
}

void cli_poll(power_sample_t *power,uint8_t current_range)
{
    uint8_t rx[64];
    uint32_t len = cdc_read(rx, sizeof(rx));

    for (uint32_t i = 0; i < len; i++) {
        char ch = (char)rx[i];

        if ((ch == '\r') || (ch == '\n')) {
            if (cli_index > 0) {
                cli_line[cli_index] = '\0';
                cli_handle_command(cli_line,power,current_range);
                cli_index = 0;
            }
        } else {
            if (cli_index < (CLI_LINE_MAX - 1)) {
                cli_line[cli_index++] = ch;
            } else {
                cli_index = 0;
                cdc_printf("ERR: line too long\r\n");
            }
        }
    }
}

int cdc_printf(const char *fmt, ...)
{
    char buf[256];
    va_list args;
    int len;

    va_start(args, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (len <= 0) {
        return len;
    }

    if ((uint32_t)len >= sizeof(buf)) {
        len = (int)sizeof(buf) - 1;
    }

    uint32_t sent = cdc_write(buf, (uint32_t)len);

    if (sent != (uint32_t)len) {
        return -1;
    }
    return len;
}

