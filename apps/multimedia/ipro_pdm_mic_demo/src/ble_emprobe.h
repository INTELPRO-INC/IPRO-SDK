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

/**
 * @file ble_emprobe.h
 * @brief Optional legacy EM ACL TX probe compatibility for ipro_pdm_mic_demo.
 *
 * The original PDM mic demo used the first-generation btblecontroller EM probe
 * while debugging bulk GATT notification corruption.  The new ipro_ble_controller
 * path does not expose that private controller header.  Keep the shell counters
 * buildable through a local weak/stub-compatible definition; all fields remain
 * zero unless a controller implementation provides the probe symbols.
 */

#ifndef IPRO_PDM_MIC_DEMO_BLE_EMPROBE_H_
#define IPRO_PDM_MIC_DEMO_BLE_EMPROBE_H_

#include <stdint.h>

#define BLE_EMPROBE_BUF_MAX   16u
#define BLE_EMPROBE_SNAP_LEN  32u

typedef struct {
    uint32_t alloc_cnt;
    uint32_t free_cnt;
    uint32_t alloc_fail;
    uint32_t double_alloc;
    uint32_t double_free;
    uint32_t clobber;

    uint8_t  cur_inflight;
    uint8_t  max_inflight;

    uint8_t  in_use[BLE_EMPROBE_BUF_MAX];
    uint8_t  has_crc[BLE_EMPROBE_BUF_MAX];
    uint16_t len[BLE_EMPROBE_BUF_MAX];
    uint16_t crc[BLE_EMPROBE_BUF_MAX];

    uint8_t  clob_idx;
    uint16_t clob_len;
    uint16_t clob_crc_want;
    uint16_t clob_crc_got;
    uint8_t  clob_data[BLE_EMPROBE_SNAP_LEN];

    const uint8_t *exp_base;
    uint32_t exp_len;
    uint16_t exp_chunk;
    uint8_t  exp_marker;
    uint8_t  exp_started;
    uint8_t  exp_last_seq;
    uint16_t exp_ext;
    uint32_t exp_pkts;
    uint32_t exp_mismatch;
    uint8_t  exp_seq_at_fail;
    uint32_t exp_off_at_fail;
    uint8_t  exp_got[BLE_EMPROBE_SNAP_LEN];
    uint8_t  exp_want[BLE_EMPROBE_SNAP_LEN];
} ble_emprobe_t;

extern ble_emprobe_t g_emprobe;
void ble_emprobe_expect(const uint8_t *base, uint32_t len, uint16_t chunk,
                        uint8_t marker);

#endif /* IPRO_PDM_MIC_DEMO_BLE_EMPROBE_H_ */
