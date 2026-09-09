/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

#include "kw307_dev.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_clock.h"
#include "hal_uart.h"

/* A framed detection frame is 34 bytes on the wire, so 2048 buffers ~60 of
 * them - about 3 s at 20 Hz. That is the margin the console printf needs: at
 * 512 bytes a one-second stall in the print path silently ate 19 frames. */
#define KW307_RX_RING_SIZE  2048

/* Single-producer (UART ISR) / single-consumer (task) ring. */
static volatile uint16_t s_head;
static volatile uint16_t s_tail;
static uint8_t           s_ring[KW307_RX_RING_SIZE];

static kw307_parser_t    s_parser;
static kw307_stats_t     s_stats;
static kw307_frame_cb_t  s_frame_cb;
static void             *s_frame_arg;

static int  s_tx_pin = -1;
static int  s_rx_pin = -1;
static int  s_gpo_pin = -1;
static bool s_have_index;
static uint16_t s_last_index;

/* One pending response slot. The module only ever has one command in flight. */
static volatile bool s_resp_valid;
static uint8_t       s_resp_cmd;
static uint8_t       s_resp_len;
static uint8_t       s_resp_data[KW307_MAX_DATA_LEN];
static volatile bool s_ack_valid;
static uint8_t       s_ack_cmd;
static uint8_t       s_ack_status;

/* ------------------------------------------------------------------ */
/* UART plumbing                                                      */
/* ------------------------------------------------------------------ */

static void kw307_rx_isr(void *dev, void *arg, uint32_t size, uint32_t event)
{
    (void)dev; (void)arg; (void)size; (void)event;

    uint8_t tmp[64];
    uint32_t n = uart_read(UART1_INDEX, tmp, sizeof(tmp));

    for (uint32_t i = 0; i < n; i++) {
        uint16_t next = (uint16_t)((s_head + 1) % KW307_RX_RING_SIZE);
        if (next == s_tail) {
            s_stats.rx_overruns++;      /* drop the newest, keep the stream */
            break;
        }
        s_ring[s_head] = tmp[i];
        s_head = next;
    }
}

/* Bind one pad to a UART1 signal. IPRO7 has 12 signal slots; a pad reaches
 * slot (gpio % 12), and the slot is then pointed at the UART1 function. */
static void route_pin(int gpio, bool is_tx)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioPin  = (uint8_t)gpio,
        .gpioFun  = GPIO_FUN_UART,
        .gpioMode = is_tx ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT,
        .pullType = GPIO_PULL_UP,       /* idle-high keeps a floating RX quiet */
        .drive    = 0,
        .smtCtrl  = 1,
    };
    GLB_GPIO_Init(&cfg);

    GLB_UART_Fun_Sel((GLB_UART_SIG_Type)(gpio % 12),
                     is_tx ? GLB_UART_SIG_FUN_UART1_TXD : GLB_UART_SIG_FUN_UART1_RXD);
}

int kw307_dev_init(int tx_pin, int rx_pin, int gpo_pin)
{
    if (tx_pin < 0 || rx_pin < 0 || tx_pin == rx_pin) {
        return -1;
    }
    /* Two pads sharing a signal slot would silently fight over it. */
    if ((tx_pin % 12) == (rx_pin % 12)) {
        return -2;
    }

    s_head = s_tail = 0;
    s_have_index = false;
    kw307_parser_reset(&s_parser);
    memset(&s_stats, 0, sizeof(s_stats));
    s_resp_valid = s_ack_valid = false;

    s_tx_pin = tx_pin;
    s_rx_pin = rx_pin;
    route_pin(tx_pin, true);
    route_pin(rx_pin, false);

    s_gpo_pin = gpo_pin;
    if (gpo_pin >= 0) {
        GLB_GPIO_Cfg_Type cfg = {
            .gpioPin  = (uint8_t)gpo_pin,
            .gpioFun  = GPIO_FUN_GPIO,
            .gpioMode = GPIO_MODE_INPUT,
            .pullType = GPIO_PULL_DOWN,
            .drive    = 0,
            .smtCtrl  = 1,
        };
        GLB_GPIO_Init(&cfg);
    }

    uart_set_baudrate(UART1_INDEX, KW307_DEV_BAUD);
    uart_init(UART1_INDEX, 0);          /* hal_uart enables TX free-run for us */

    uart_ioctl(UART1_INDEX, UART_CTRL_SET_INT,
               (void *)(uintptr_t)(UART_RX_END_IT | UART_RX_FIFO_IT | UART_RTO_IT));
    uart_ioctl(UART1_INDEX, UART_CTRL_SET_CALLBACK, (void *)kw307_rx_isr);

    return 0;
}

void kw307_dev_set_frame_cb(kw307_frame_cb_t cb, void *arg)
{
    s_frame_cb  = cb;
    s_frame_arg = arg;
}

/* ------------------------------------------------------------------ */
/* RX path                                                            */
/* ------------------------------------------------------------------ */

static void dispatch(const kw307_pkt_t *pkt)
{
    if (pkt->cmd == KW307_CMD_OUTPUT) {
        kw307_frame_t frame;
        if (!kw307_parse_output(pkt->data, pkt->len, &frame)) {
            s_stats.short_frames++;
            return;
        }
        if (s_have_index) {
            uint16_t gap = (uint16_t)(frame.index - s_last_index - 1);
            if (gap && gap < 1000) {    /* a larger jump is a reboot, not a gap */
                s_stats.missed += gap;
            }
        }
        s_last_index = frame.index;
        s_have_index = true;
        s_stats.frames++;

        if (s_frame_cb) {
            s_frame_cb(&frame, s_frame_arg);
        }
        return;
    }

    s_stats.other_frames++;

    if (pkt->cmd == KW307_CMD_ACK && pkt->len == 2) {
        s_ack_cmd    = pkt->data[0];
        s_ack_status = pkt->data[1];
        s_ack_valid  = true;
        return;
    }

    s_resp_cmd = pkt->cmd;
    s_resp_len = (uint8_t)(pkt->len > sizeof(s_resp_data) ? sizeof(s_resp_data) : pkt->len);
    memcpy(s_resp_data, pkt->data, s_resp_len);
    s_resp_valid = true;
}

void kw307_dev_poll(void)
{
    uint8_t chunk[64];

    for (;;) {
        uint16_t n = 0;
        while (n < sizeof(chunk) && s_tail != s_head) {
            chunk[n++] = s_ring[s_tail];
            s_tail = (uint16_t)((s_tail + 1) % KW307_RX_RING_SIZE);
        }
        if (!n) {
            break;
        }

        kw307_pkt_t pkt;
        bool more = kw307_parser_feed(&s_parser, chunk, n, &pkt);
        while (more) {
            dispatch(&pkt);
            more = kw307_parser_feed(&s_parser, NULL, 0, &pkt);
        }
    }

    s_stats.crc_errors    = s_parser.crc_errors;
    s_stats.dropped_bytes = s_parser.dropped_bytes;
}

/* ------------------------------------------------------------------ */
/* Command layer                                                      */
/* ------------------------------------------------------------------ */

static int wait_ack(uint8_t cmd, uint32_t timeout_ms)
{
    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeout_ms);

    while (xTaskGetTickCount() < deadline) {
        kw307_dev_poll();
        if (s_ack_valid && s_ack_cmd == cmd) {
            s_ack_valid = false;
            return s_ack_status;        /* 0 = OK, otherwise the reject code */
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    return -1;
}

int kw307_dev_write_cmd(uint8_t cmd, const uint8_t *data, uint8_t len)
{
    uint8_t tx[KW307_MAX_TX_LEN];
    size_t n = kw307_build_write(tx, cmd, data, len);
    if (!n) {
        return -2;
    }

    s_ack_valid = false;
    uart_write(UART1_INDEX, tx, n);
    return wait_ack(cmd, KW307_DEV_ACK_TIMEOUT);
}

int kw307_dev_read_cmd(uint8_t cmd, uint8_t *out, uint8_t out_max, uint8_t *out_len)
{
    uint8_t tx[KW307_MAX_TX_LEN];
    size_t n = kw307_build_read(tx, cmd);

    s_ack_valid = s_resp_valid = false;
    uart_write(UART1_INDEX, tx, n);

    /* Guide 1.4.2: the ACK comes first; only then does the data frame follow. */
    int status = wait_ack(cmd, KW307_DEV_ACK_TIMEOUT);
    if (status != 0) {
        return status;
    }

    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(KW307_DEV_ACK_TIMEOUT);
    while (xTaskGetTickCount() < deadline) {
        kw307_dev_poll();
        if (s_resp_valid && s_resp_cmd == cmd) {
            s_resp_valid = false;
            uint8_t len = s_resp_len < out_max ? s_resp_len : out_max;
            memcpy(out, s_resp_data, len);
            if (out_len) {
                *out_len = len;
            }
            return 0;
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    return -1;
}

int kw307_dev_set_param(uint8_t pid, uint16_t value)
{
    const uint8_t data[3] = { pid, (uint8_t)(value >> 8), (uint8_t)(value & 0xFF) };
    return kw307_dev_write_cmd(KW307_CMD_PARAM, data, sizeof(data));
}

int kw307_dev_set_power_mode(bool normal)
{
    uint8_t v = normal ? 1 : 0;
    return kw307_dev_write_cmd(KW307_CMD_POWER_MODE, &v, 1);
}

int kw307_dev_set_gain(uint8_t index)
{
    if (index > 8) {
        return -2;
    }
    return kw307_dev_write_cmd(KW307_CMD_GAIN, &index, 1);
}

int kw307_dev_get_gain(uint8_t *index)
{
    uint8_t buf[4], len = 0;
    int rc = kw307_dev_read_cmd(KW307_CMD_GAIN, buf, sizeof(buf), &len);
    if (rc == 0 && len >= 1 && index) {
        *index = buf[0];
    }
    return rc;
}

int kw307_dev_set_detect_range(uint16_t min_cm, uint16_t max_cm)
{
    if (min_cm >= max_cm || max_cm > 6553) {
        return -2;                      /* cm x10 must still fit in 16 bits */
    }
    int rc = kw307_dev_set_param(KW307_PID_RANGE_MIN, (uint16_t)(min_cm * 10));
    if (rc != 0) {
        return rc;
    }
    return kw307_dev_set_param(KW307_PID_RANGE_MAX, (uint16_t)(max_cm * 10));
}

int kw307_dev_set_detect_fov(uint16_t fov_deg)
{
    if (fov_deg < 1 || fov_deg > 180) {
        return -2;
    }
    return kw307_dev_set_param(KW307_PID_FOV, (uint16_t)(fov_deg * 10));
}

int kw307_dev_set_output_mode(uint8_t mode)
{
    return kw307_dev_write_cmd(KW307_CMD_OUTPUT_MODE, &mode, 1);
}

int kw307_dev_gpo_level(void)
{
    if (s_gpo_pin < 0) {
        return -1;
    }
    return (int)GLB_GPIO_Read(s_gpo_pin);
}

const kw307_stats_t *kw307_dev_stats(void) { return &s_stats; }
int kw307_dev_tx_pin(void) { return s_tx_pin; }
int kw307_dev_rx_pin(void) { return s_rx_pin; }

void kw307_dev_stats_reset(void)
{
    memset(&s_stats, 0, sizeof(s_stats));
    s_parser.crc_errors = s_parser.dropped_bytes = 0;
    s_have_index = false;
}
