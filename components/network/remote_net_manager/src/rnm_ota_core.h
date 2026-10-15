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

#define RNM_OTA_HDR_SIZE   512U
#define RNM_OTA_CHUNK_MAX  1024U
#define RNM_OTA_SECTOR     4096U

/* Wire error codes, equal to rnm_msg.h ipro_ota_error_code_t. */
enum {
    RNM_OTA_E_STATE = 8888,
    RNM_OTA_E_NO_SLOT,
    RNM_OTA_E_HEADER,
    RNM_OTA_E_TOO_BIG,
    RNM_OTA_E_CKSUM,
    RNM_OTA_E_GAP,
    RNM_OTA_E_SIZE,
    RNM_OTA_E_MISC,
};

/* Staging-slot flash access; every op returns 0 on success. */
typedef struct {
    int (*slot_open)(void *ctx, uint32_t *addr, uint32_t *max_len);
    int (*erase)(void *ctx, uint32_t addr, uint32_t len);
    int (*write)(void *ctx, uint32_t addr, const uint8_t *data, uint32_t len);
    int (*read)(void *ctx, uint32_t addr, uint8_t *data, uint32_t len);
    int (*sha256)(void *ctx, uint32_t addr, uint32_t len, uint8_t out[32]);
    int (*commit)(void *ctx);
} rnm_ota_store_t;

typedef struct {
    const rnm_ota_store_t *st;
    void *st_ctx;
    uint8_t active;
    uint8_t hdr_ok;
    uint32_t slot_addr;
    uint32_t slot_max;
    uint32_t total;
    uint32_t next_off;
    uint32_t erased_to;
} rnm_ota_core_t;

/* Each returns 0 or an RNM_OTA_E_* code. Any code except
 * RNM_OTA_E_GAP ends the session. */
uint16_t rnm_ota_core_start(rnm_ota_core_t *c, const rnm_ota_store_t *st,
                            void *st_ctx, uint32_t total);
uint16_t rnm_ota_core_program(rnm_ota_core_t *c, uint32_t off,
                              const uint8_t *data, uint32_t len);
uint16_t rnm_ota_core_commit(rnm_ota_core_t *c);
void rnm_ota_core_abort(rnm_ota_core_t *c);
