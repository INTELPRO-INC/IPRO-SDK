/* SPDX-License-Identifier: Apache-2.0 */
#ifndef BT_PAWR_REASSEMBLY_H
#define BT_PAWR_REASSEMBLY_H
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
/* Bounded interleaving support per advertising set; only present with PAwR. */
#define BT_PAWR_REASSEMBLY_CONTEXTS 4
#define BT_PAWR_RESPONSE_MAX 251
struct bt_pawr_fragment_context {
    uint8_t data[BT_PAWR_RESPONSE_MAX];
    uint32_t metadata;
    uint16_t length;
    uint8_t subevent, slot;
    bool active;
};
struct bt_pawr_reassembly {
    struct bt_pawr_fragment_context ctx[BT_PAWR_REASSEMBLY_CONTEXTS];
    bool poisoned;
};
static inline void bt_pawr_reassembly_reset(struct bt_pawr_reassembly *s)
{
    /* Do not overwrite callback-visible payload bytes during lifecycle cleanup. */
    for (unsigned i = 0; i < BT_PAWR_REASSEMBLY_CONTEXTS; i++) s->ctx[i].active = false;
    s->poisoned = false;
}
/* 0: pending, 1: complete (output valid until next call), 2: RX failed,
 * -1: stream cannot safely be reconstructed. HCI has no response sequence ID;
 * after overflow/malformed metadata, fail closed until advertising restarts.
 * Never deliver a tail fragment as an independent successful response. */
static inline int bt_pawr_fragment(struct bt_pawr_reassembly *s,
                                  uint8_t subevent, uint8_t slot, uint32_t metadata,
                                  uint8_t status, const uint8_t *data, uint16_t len,
                                  uint8_t **out, uint16_t *out_len)
{
    struct bt_pawr_fragment_context *ctx = NULL, *free_ctx = NULL;
    *out = NULL;
    *out_len = 0;
    if (s->poisoned) return -1;
    for (unsigned i = 0; i < BT_PAWR_REASSEMBLY_CONTEXTS; i++) {
        if (!s->ctx[i].active) free_ctx = &s->ctx[i];
        else if (s->ctx[i].subevent == subevent && s->ctx[i].slot == slot)
            ctx = &s->ctx[i];
    }
    if (status == 255) {
        if (ctx) ctx->active = false;
        return 2;
    }
    if (status > 1 || len > BT_PAWR_RESPONSE_MAX || (!data && len)) goto invalid;
    if (!ctx) {
        if (!free_ctx) goto invalid;
        ctx = free_ctx;
        ctx->length = 0;
        ctx->subevent = subevent;
        ctx->slot = slot;
        ctx->metadata = metadata;
    } else if (ctx->metadata != metadata) goto invalid;
    if (len > BT_PAWR_RESPONSE_MAX - ctx->length) goto invalid;
    if (len) memcpy(ctx->data + ctx->length, data, len);
    ctx->length += len;
    ctx->active = status == 1;
    if (ctx->active) return 0;
    *out = ctx->data;
    *out_len = ctx->length;
    return 1;
invalid:
    s->poisoned = true;
    return -1;
}
#endif
