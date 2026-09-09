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

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>

#include "txrx.h"
#include <rnm_server.h>

#define USBWIFI_DATA_CHANNEL_BUF_LEN 2048

#define USBWIFI_PORT_CLOSED     0
#define USBWIFI_PORT_CONTROLLED 1
#define USBWIFI_PORT_OPEN       2

typedef struct {
    struct usbwifi_cdc_env_tag *cdc_env;

    void *tx_buf, *rx_buf;
    uint32_t txrx_buffer[2][USBWIFI_DATA_CHANNEL_BUF_LEN / 4];
    SemaphoreHandle_t tx_mutex, tx_sem, rx_mutex, rx_sem;
    StaticSemaphore_t tx_mutex_buffer, tx_sem_buffer, rx_mutex_buffer, rx_sem_buffer;
    size_t len_read;
} usbwifi_dc_t;

typedef struct {
    TaskHandle_t usb_worker_task;
    rnms_t *rnm;
    struct usbwifi_tx_env_tag tx_env;
    struct usbwifi_rx_env_tag rx_env;
    struct usbwifi_cdc_env_tag cdc_env;
    uint8_t port_state;

    usbwifi_dc_t data_channel;
} usbwifi_t;

extern usbwifi_t g_usbwifi;

int usbwifi_usb_init();
void usbwifi_set_port_state(usbwifi_t *ctx, uint8_t state);
uint8_t usbwifi_get_port_state(usbwifi_t *ctx);
int usbwifi_start(usbwifi_t *ctx);
int usbwifi_usb_data_process(void);
void usbwifi_configured();
void usbwifi_disconnected();
void usbwifi_register_cdc_cb(struct usbwifi_cdc_env_tag *env, usbwifi_cdc_data_cb_t in_cb, void *in_cb_arg, usbwifi_cdc_data_cb_t out_cb, void *out_cb_arg);

int usbwifi_dc_init(usbwifi_dc_t *dc, struct usbwifi_cdc_env_tag *cdc_env);


/**
 * @brief Send data via data channel.
 *
 * @param dc         Context.
 * @param data       Data to send.
 * @param len        Length of data. Limit: 2048
 * @param timeout_ms Timeout to wait. Note that time actually spent might be longer.
 *
 * @return
 *   0 on success.
 *   -1 on arugment error.
 *   1 on timeout.
 */
int usbwifi_dc_write(usbwifi_dc_t *dc, const void *data, size_t len, unsigned timeout_ms);

/**
 * @brief Receive data via data channel.
 *
 * @param dc         Context.
 * @param data       Buffer to save received data.
 * @param len        Length of buffer data.
 * @param len_read   Length of data read. Limit: 2048
 * @param timeout_ms Timeout to wait. Note that time acutally spent might be longer.
 *
 * @return
 *   0 on success.
 *   -1 on argument error.
 *   1 on timeout.
 *   2 if length received > len
 */
int usbwifi_dc_read(usbwifi_dc_t *dc, void *data, size_t len, size_t *len_read, unsigned timeout_ms);

/**
 * @brief Test/Demo for data channel.
 *
 * The test echoes back what is received.
 * How to use:
 * In Linux, use serial tool to open /dev/ttyACMx, send whatever you want and watch.
 *
 * @param dc Context.
 */
void usbwifi_dc_test(usbwifi_dc_t *dc);
