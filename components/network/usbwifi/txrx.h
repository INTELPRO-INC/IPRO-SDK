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
#include <lwip/pbuf.h>
#include <utils_list.h>

struct usbwifi_tx_env_tag {
    struct utils_list free_list;
    struct utils_list ready_list;
    void *curr_attached;
};

struct usbwifi_rx_env_tag {
    struct utils_list pendding_tx_list;
    struct utils_list pendding_free_list;
    const void *curr_attached;
    void *usbwifi;
};

typedef void (*usbwifi_cdc_data_cb_t)(void *arg, size_t len);

struct usbwifi_cdc_env_tag {
    const void *bulk_out_buf_attached;
    usbwifi_cdc_data_cb_t bulk_out_cb;
    void *bulk_out_cb_arg;

    const void *bulk_in_buf_attached;
    usbwifi_cdc_data_cb_t bulk_in_cb;
    void *bulk_in_cb_arg;
};

#define TX_PBUF_FRAME_LEN   1514
#define TX_PBUF_PAYLOAD_LEN (PBUF_LINK_ENCAPSULATION_HLEN + TX_PBUF_FRAME_LEN)

typedef struct {
    struct utils_list_hdr list_hdr;
    struct pbuf_custom pbuf;
    struct usbwifi_tx_env_tag *env;
    uint32_t payload_buf[(TX_PBUF_PAYLOAD_LEN + 3) / 4];
} tx_buf;
#define TX_BUF_GET_USB_BUF(buf) (((struct pbuf *)(&(buf)->pbuf))->payload - sizeof(usb_data_t))

typedef void (*usbwifi_bulk_in_free_t)(void *arg);

int usbwifi_tx_buf_init(struct usbwifi_tx_env_tag *env);
int usbwifi_rx_buf_init(struct usbwifi_rx_env_tag *env);
int usbwifi_tx_buf_mem_register(struct usbwifi_tx_env_tag *env, void *mem, size_t size);
tx_buf *usbwifi_tx_buf_alloc(struct usbwifi_tx_env_tag *env, bool irq);
void usbwifi_tx_buf_reset(tx_buf *buf);
void usbwifi_tx_buf_free(tx_buf *buf);
int usbwifi_bulk_in(struct usbwifi_rx_env_tag *env, void *data, uint16_t len, uint16_t type, usbwifi_bulk_in_free_t free_fn, void *free_arg);
size_t usbwifi_rx_buf_reserve_head(void);

bool usb_try_reattach_bulk_out_buf(struct usbwifi_tx_env_tag *tx_env, tx_buf *buf);
void usb_bulk_in(const void *data, size_t len);
void usb_cdc_bulk_in(struct usbwifi_cdc_env_tag *env, const void *data, size_t len);
void usb_cdc_bulk_out(struct usbwifi_cdc_env_tag *env, void *data, size_t len);
