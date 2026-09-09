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

#include <stdint.h>

#define USBWIFI_DATA_TYPE_CMD 0xA55A
#define USBWIFI_DATA_TYPE_PKT 0x6996

#define USB_DATA_FLAG_AP_PKT (1u << 0)

typedef struct {
    uint16_t type;
    uint16_t length;
    uint16_t flags;
    uint16_t payload_offset;
    uint32_t rsvd[8];
    uint8_t payload[];
} __attribute__((aligned(4))) usb_data_t;

typedef struct {
    uint16_t type;
    uint16_t length;
    uint16_t flags;
    uint16_t payload_offset;
    uint32_t list_hdr;
    uint32_t free_fn;
    uint32_t free_arg;
    uint32_t tx_len;
    /* Phase 2A.10 Counter J: bisect 11ms RX buffer hold time.
     * submit_us stamped in usbwifi_bulk_in (txrx.c) just before USB EP IN
     * submission; complete_us stamped in wifi_bulk_in ISR (usb.c) when
     * the bulk-IN transfer completes. Read in usbwifi_usb_data_process
     * before free_fn() releases the underlying RX buffer. Lives in
     * uncached MAWOC ram_wifi region so no fence/flush needed for the
     * CPU-only write/read path. */
    uint32_t submit_us;
    uint32_t complete_us;
    uint32_t rsvd[2];
    uint8_t payload[];
} __attribute__((aligned(4))) usb_data_emb_t;

#define USBWIFI_BULK_IN_ALIGN 8
