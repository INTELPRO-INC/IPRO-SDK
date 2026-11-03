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

#include "fmac_dl_loader.h"

/* Longest wait for the RUN / REBOOT ack upload to complete */
#define RUN_DRAIN_MS        1000u

typedef struct {
    uint32_t entry;
    int run;
    int reboot;
} action_t;

static const fmac_dl_plat_t *s_plat;

static int in_window(uint32_t addr, uint32_t len)
{
    return addr >= FMAC_DL_WIN_BASE && len <= FMAC_DL_WIN_SIZE &&
           addr - FMAC_DL_WIN_BASE <= FMAC_DL_WIN_SIZE - len;
}

static const uint8_t *win_ptr(uint32_t addr)
{
    return (const uint8_t *)(uintptr_t)(addr + FMAC_DL_NOCACHE_OFS);
}

static void handle(const uint8_t *pkt, uint32_t len, fmac_dl_ack_t *ack,
                   action_t *act)
{
    const fmac_dl_req_t *r = (const fmac_dl_req_t *)pkt;
    const uint8_t *payload = pkt + FMAC_DL_HDR_LEN;

    memset(ack, 0, sizeof(*ack));
    ack->magic = FMAC_DL_ACK_MAGIC;
    if (len < FMAC_DL_HDR_LEN || r->magic != FMAC_DL_REQ_MAGIC) {
        ack->status = FMAC_DL_E_HDR;
        return;
    }
    ack->type = r->type;
    ack->seq = r->seq;
    switch (r->type) {
    case FMAC_DL_INFO:
        ack->value = FMAC_STAGE_LOADER;
        ack->info[0] = FMAC_DL_PROTO_VER | (s_plat->loader_ver << 16);
        ack->info[1] = FMAC_DL_WIN_BASE;
        ack->info[2] = FMAC_DL_WIN_SIZE;
        ack->info[3] = FMAC_DL_PAYLOAD_MAX;
        break;
    case FMAC_DL_WRITE:
        if (r->len > FMAC_DL_PAYLOAD_MAX || FMAC_DL_HDR_LEN + r->len > len) {
            ack->status = FMAC_DL_E_HDR;
        } else if ((r->addr | r->len) & 3u) {
            ack->status = FMAC_DL_E_ALIGN;
        } else if (!in_window(r->addr, r->len)) {
            ack->status = FMAC_DL_E_RANGE;
        } else {
            ack->value = fmac_crc32(0, payload, r->len);
            if (ack->value != r->crc32)
                ack->status = FMAC_DL_E_CRC;
            else
                memcpy((void *)win_ptr(r->addr), payload, r->len);
        }
        break;
    case FMAC_DL_VERIFY:
    case FMAC_DL_RUN:
        if (!in_window(r->addr, r->len)) {
            ack->status = FMAC_DL_E_RANGE;
            break;
        }
        ack->value = fmac_crc32(0, win_ptr(r->addr), r->len);
        if (r->type == FMAC_DL_VERIFY)
            break;
        if (ack->value != r->crc32) {
            ack->status = FMAC_DL_E_CRC;
        } else if (!in_window(r->arg, 4) || (r->arg & 1u)) {
            ack->status = FMAC_DL_E_RANGE;
        } else {
            act->entry = r->arg;
            act->run = 1;
        }
        break;
    case FMAC_DL_REBOOT:
        act->reboot = 1;
        break;
    default:
        ack->status = FMAC_DL_E_TYPE;
        break;
    }
}

void fmac_dl_loader_init(const fmac_dl_plat_t *plat)
{
    s_plat = plat;
}

int fmac_dl_loader_serve(const fmac_dl_bus_t *b)
{
    fmac_dl_ack_t ack;
    action_t act = { 0 };
    uint8_t *pkt;
    uint32_t len;

    if (!b->next(&pkt, &len))
        return 0;
    handle(pkt, len, &ack, &act);
    if (!act.run)
        b->release(pkt);
    b->send(&ack, sizeof(ack));
    if (s_plat->log && (ack.type == FMAC_DL_RUN || ack.status != FMAC_DL_OK))
        s_plat->log(b, &ack);
    if (act.run || act.reboot) {
        uint32_t start = s_plat->now_ms();

        while (!b->tx_idle() && s_plat->now_ms() - start < RUN_DRAIN_MS)
            s_plat->poll();
    }
    if (act.reboot)
        s_plat->reboot();
    if (act.run)
        s_plat->jump(act.entry, b->id);
    return 1;
}
