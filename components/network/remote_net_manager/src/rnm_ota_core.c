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

#include <string.h>

#include "rnm_ota_core.h"

#define OTA_MAGIC       "IPRO_OTA_Ver1.0"
#define OTA_TYPE_OFF    16U
#define OTA_PAYLEN_OFF  20U
#define OTA_HWVER_OFF   32U
#define OTA_SHA_OFF     64U
#define OTA_HWVER_CHIP  "IPRO6"

static uint16_t fail(rnm_ota_core_t *c, uint16_t code)
{
    c->active = 0;
    return code;
}

uint16_t rnm_ota_core_start(rnm_ota_core_t *c, const rnm_ota_store_t *st,
                            void *st_ctx, uint32_t total)
{
    memset(c, 0, sizeof(*c));
    c->st = st;
    c->st_ctx = st_ctx;
    if (st->slot_open(st_ctx, &c->slot_addr, &c->slot_max) != 0 ||
        c->slot_max == 0 ||
        (c->slot_addr % RNM_OTA_SECTOR) != 0 ||
        (c->slot_max % RNM_OTA_SECTOR) != 0) {
        return RNM_OTA_E_NO_SLOT;
    }
    if (total <= RNM_OTA_HDR_SIZE || total > c->slot_max) {
        return RNM_OTA_E_TOO_BIG;
    }
    c->total = total;
    c->active = 1;
    return 0;
}

/* Magic, XZ type, payload length and target chip, read back. */
static uint16_t check_header(rnm_ota_core_t *c)
{
    uint8_t h[OTA_HWVER_OFF + sizeof(OTA_HWVER_CHIP) - 1];
    uint32_t paylen;

    if (c->st->read(c->st_ctx, c->slot_addr, h, sizeof(h)) != 0) {
        return RNM_OTA_E_MISC;
    }
    memcpy(&paylen, &h[OTA_PAYLEN_OFF], sizeof(paylen));
    if (memcmp(h, OTA_MAGIC, sizeof(OTA_MAGIC) - 1) != 0 ||
        memcmp(&h[OTA_TYPE_OFF], "XZ  ", 4) != 0 ||
        paylen != c->total - RNM_OTA_HDR_SIZE ||
        memcmp(&h[OTA_HWVER_OFF], OTA_HWVER_CHIP,
               sizeof(OTA_HWVER_CHIP) - 1) != 0) {
        return RNM_OTA_E_HEADER;
    }
    c->hdr_ok = 1;
    return 0;
}

uint16_t rnm_ota_core_program(rnm_ota_core_t *c, uint32_t off,
                              const uint8_t *data, uint32_t len)
{
    uint16_t rc;

    if (!c->active) {
        return RNM_OTA_E_STATE;
    }
    if (len == 0 || len > RNM_OTA_CHUNK_MAX || off > c->total ||
        len > c->total - off) {
        return fail(c, RNM_OTA_E_SIZE);
    }
    if (off + len <= c->next_off) {
        return 0;
    }
    if (off != c->next_off) {
        return RNM_OTA_E_GAP;
    }
    while (c->erased_to < off + len) {
        if (c->st->erase(c->st_ctx, c->slot_addr + c->erased_to,
                         RNM_OTA_SECTOR) != 0) {
            return fail(c, RNM_OTA_E_MISC);
        }
        c->erased_to += RNM_OTA_SECTOR;
    }
    if (c->st->write(c->st_ctx, c->slot_addr + off, data, len) != 0) {
        return fail(c, RNM_OTA_E_MISC);
    }
    c->next_off = off + len;
    if (!c->hdr_ok && c->next_off >= RNM_OTA_HDR_SIZE) {
        rc = check_header(c);
        if (rc != 0) {
            return fail(c, rc);
        }
    }
    return 0;
}

uint16_t rnm_ota_core_commit(rnm_ota_core_t *c)
{
    uint8_t want[32];
    uint8_t got[32];

    if (!c->active) {
        return RNM_OTA_E_STATE;
    }
    if (c->next_off != c->total) {
        return fail(c, RNM_OTA_E_SIZE);
    }
    if (c->st->read(c->st_ctx, c->slot_addr + OTA_SHA_OFF, want,
                    sizeof(want)) != 0 ||
        c->st->sha256(c->st_ctx, c->slot_addr + RNM_OTA_HDR_SIZE,
                      c->total - RNM_OTA_HDR_SIZE, got) != 0) {
        return fail(c, RNM_OTA_E_MISC);
    }
    if (memcmp(want, got, sizeof(want)) != 0) {
        return fail(c, RNM_OTA_E_CKSUM);
    }
    if (c->st->commit(c->st_ctx) != 0) {
        return fail(c, RNM_OTA_E_MISC);
    }
    c->active = 0;
    return 0;
}

void rnm_ota_core_abort(rnm_ota_core_t *c)
{
    c->active = 0;
}
