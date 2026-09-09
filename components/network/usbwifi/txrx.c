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

#include "usbwifi.h"
#include "utils.h"
#include "usb.h"

#include <stddef.h>
#include <stdio.h>
#include <utils_list.h>
#include <lwip/pbuf.h>

#if defined(IPRO6LE) || defined(IPRO6)
#include "drv_l1c.h"
#endif

extern void dsd_usb_direct_inc(void);
extern void dsd_usb_queued_inc(uint32_t depth);
extern uint32_t inline_hal_machw_time(void);

static void sanity_check(void)
{
    usb_data_emb_t ud;
    _Static_assert(sizeof(usb_data_t) == sizeof(usb_data_emb_t));
    _Static_assert(sizeof(ud.list_hdr) == sizeof(struct utils_list_hdr));
    _Static_assert(sizeof(ud.free_fn) >= sizeof(int *));
    _Static_assert(sizeof(ud.free_arg) >= sizeof(int *));
}

int usbwifi_tx_buf_init(struct usbwifi_tx_env_tag *env)
{
    if (!env) {
        return -1;
    }

    utils_list_init(&env->free_list);
    utils_list_init(&env->ready_list);

    return 0;
}

int usbwifi_rx_buf_init(struct usbwifi_rx_env_tag *env)
{
    if (!env) {
        return -1;
    }

    sanity_check();

    utils_list_init(&env->pendding_tx_list);
    utils_list_init(&env->pendding_free_list);

    return 0;
}

size_t usbwifi_rx_buf_reserve_head(void)
{
    return sizeof(usb_data_emb_t) + USBWIFI_BULK_IN_ALIGN;
}

int usbwifi_bulk_in(struct usbwifi_rx_env_tag *env, void *data, uint16_t len, uint16_t type, usbwifi_bulk_in_free_t free_fn, void *free_arg)
{
    usb_data_emb_t *ud = (usb_data_emb_t *)ALIGN_LOW(USBWIFI_BULK_IN_ALIGN, data - sizeof(usb_data_emb_t));
    uint32_t payload_offset = data - (void *)ud;
    uint32_t tx_len = payload_offset + len;
    uint8_t port_state = usbwifi_get_port_state(env->usbwifi);

    if (port_state == USBWIFI_PORT_CLOSED ||
            (port_state == USBWIFI_PORT_CONTROLLED &&
             (type != USBWIFI_DATA_TYPE_CMD || !rnms_msg_is_get_mac(data)))) {
        return -1;
    }

    memset(ud, 0, payload_offset);
    ud->type = type;
    ud->length = len;
    ud->payload_offset = payload_offset;
    ud->free_fn = (uint32_t)free_fn;
    ud->free_arg = (uint32_t)free_arg;
    ud->tx_len = tx_len;
    /* Counter J: timestamp at submit. complete_us stays at 0 from the
     * memset above; will be filled by wifi_bulk_in ISR on completion. */
    {
        uint32_t now = inline_hal_machw_time();
        ud->submit_us = now ? now : 1;
    }

#if defined(IPRO6LE) || defined(IPRO6)
    /* Phase 2A.8 path delta.fix: flush header + payload to physical RAM
     * before USB DMA reads. Allows callers to write through the cached
     * alias (much faster than uncached MAWOC writes for 1408B payload).
     * Cheap no-op for callers that already wrote via uncached alias since
     * no cache lines exist for those addresses. */
    L1C_DCache_Clean_By_Addr((uintptr_t)ud, tx_len);
#endif

    ENTER_CRITICAL();
    {
        if (env->curr_attached) {
            struct utils_list_hdr *hdr = (struct utils_list_hdr *)&ud->list_hdr;
            utils_list_push_back(&env->pendding_tx_list, hdr);
            dsd_usb_queued_inc(utils_list_cnt(&env->pendding_tx_list));
        } else {
            usb_bulk_in(ud, tx_len);
            dsd_usb_direct_inc();
        }
    }
    EXIT_CRITICAL();

    return 0;
}

int usbwifi_tx_buf_mem_register(struct usbwifi_tx_env_tag *env, void *mem, size_t size)
{
    if (!env) {
        return -1;
    }

    if (!mem) {
        return 0;
    }

    long s = size;
    tx_buf *p = (tx_buf *)ALIGN_PTR_HIGH(mem);
    s -= ALIGN_PTR_PADD(mem);
    ENTER_CRITICAL();
    {
        while (s >= sizeof(tx_buf)) {
            p->env = env;
            utils_list_push_back(&env->free_list, (struct utils_list_hdr *)p);
            ++p;
            s -= sizeof(tx_buf);
        }
    }
    EXIT_CRITICAL();

    printf("[USBWIFI] tx buf added to %u\r\n", utils_list_cnt(&env->free_list));

    return 0;
}

void usbwifi_tx_buf_free(tx_buf *buf)
{
    if (!buf) {
        return;
    }
    struct usbwifi_tx_env_tag *env = buf->env;

    ENTER_CRITICAL();
    {
        if (!usb_try_reattach_bulk_out_buf(env, buf)) {
            utils_list_push_back(&env->free_list, &buf->list_hdr);
        }
    }
    EXIT_CRITICAL();
}

static void usbwifi_tx_buf_free_pbuf(struct pbuf *p)
{
    tx_buf *buf = container_of((struct pbuf_custom *)p, tx_buf, pbuf);
    usbwifi_tx_buf_free(buf);
}

void usbwifi_tx_buf_reset(tx_buf *buf)
{
    buf->pbuf.custom_free_function = usbwifi_tx_buf_free_pbuf;
    pbuf_alloced_custom(PBUF_RAW_TX, TX_PBUF_FRAME_LEN, PBUF_RAM, &buf->pbuf, buf->payload_buf, TX_PBUF_PAYLOAD_LEN);
}

tx_buf *usbwifi_tx_buf_alloc(struct usbwifi_tx_env_tag *env, bool irq)
{
    tx_buf *tp;

    if (!irq) {
        ENTER_CRITICAL();
        tp = (tx_buf *)utils_list_pop_front(&env->free_list);
        EXIT_CRITICAL();
    } else {
        tp = (tx_buf *)utils_list_pop_front(&env->free_list);
    }

    if (tp) {
        usbwifi_tx_buf_reset(tp);
    }

    return tp;
}
