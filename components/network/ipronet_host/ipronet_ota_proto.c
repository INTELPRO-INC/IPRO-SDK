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

/* ipronet_ota_proto.c - OTA transfer and commit, no RTOS. */
#include <string.h>

#include "ipronet_ota_proto.h"

#define OTA_MAGIC  "IPRO_OTA_Ver1.0"
#define OTA_HW     "IPRO6"

int ipronet_ota_check_header(const uint8_t *hdr, uint32_t size)
{
    uint32_t paylen;

    if (size <= IPRONET_OTA_HDR_SIZE) {
        return IPRONET_ERR_OTA_PKG;
    }
    memcpy(&paylen, hdr + 20, sizeof(paylen));
    if (memcmp(hdr, OTA_MAGIC, sizeof(OTA_MAGIC) - 1) != 0 ||
        memcmp(hdr + 16, "XZ  ", 4) != 0 ||
        memcmp(hdr + 32, OTA_HW, sizeof(OTA_HW) - 1) != 0 ||
        paylen != size - IPRONET_OTA_HDR_SIZE) {
        return IPRONET_ERR_OTA_PKG;
    }
    return IPRONET_OK;
}

/* Sends body up to `tries` times and decodes the status reply. */
static int ota_call(ipronet_ota_req_fn req, void *ctx, const uint8_t *body,
                    uint16_t len, uint16_t want, uint32_t *val,
                    uint32_t timeout_ms, int tries,
                    ipronet_ota_report_t *rep)
{
    ipronet_ota_status_t st;
    uint16_t rsp_len = 0;
    uint16_t status = 0;
    int rc = IPRONET_ERR_OTA_TIMEOUT;

    for (int i = 0; i < tries; i++) {
        if (i > 0) {
            rep->retries++;
        }
        rsp_len = sizeof(st);
        status = 0;
        rc = req(ctx, body, len, (uint8_t *)&st, &rsp_len, &status,
                 timeout_ms);
        if (rc != IPRONET_ERR_OTA_TIMEOUT) {
            break;
        }
    }
    if (rc != IPRONET_OK) {
        return rc;
    }
    if (status != 0) {
        rep->dev_err = status;
        return IPRONET_ERR_OTA_DEVICE;
    }
    if (rsp_len < sizeof(st) || st.msg_type != IPRONET_OTA_MSG_STATUS) {
        return IPRONET_ERR_OTA_DEVICE;
    }
    if (st.status_type == IPRONET_OTA_ST_ERROR) {
        rep->dev_err = (uint16_t)st.val;
        return IPRONET_ERR_OTA_DEVICE;
    }
    if (st.status_type != want) {
        return IPRONET_ERR_OTA_DEVICE;
    }
    if (val != NULL) {
        *val = st.val;
    }
    return IPRONET_OK;
}

void ipronet_ota_abort(ipronet_ota_req_fn req, void *ctx)
{
    uint16_t body = IPRONET_OTA_MSG_ABORT;
    ipronet_ota_report_t scratch;

    memset(&scratch, 0, sizeof(scratch));
    (void)ota_call(req, ctx, (const uint8_t *)&body, sizeof(body),
                   IPRONET_OTA_ST_ABORT_ACK, NULL, IPRONET_OTA_ABORT_MS, 1,
                   &scratch);
}

int ipronet_ota_xfer(const ipronet_ota_src_t *src, const uint8_t *hdr,
                     ipronet_ota_req_fn req, void *ctx,
                     const ipronet_ota_opts_t *opts,
                     ipronet_ota_report_t *rep)
{
    uint8_t msg[sizeof(ipronet_ota_part_t) + IPRONET_OTA_CHUNK];
    uint8_t *data = msg + sizeof(ipronet_ota_part_t);
    ipronet_ota_part_t part;
    uint32_t off = 0;
    uint32_t ack = 0;
    int rc;

    part.msg_type = IPRONET_OTA_MSG_START;
    part.offset = 0;
    part.data_len = src->size;
    memcpy(msg, &part, sizeof(part));
    rc = ota_call(req, ctx, msg, sizeof(part), IPRONET_OTA_ST_START_ACK,
                  NULL, IPRONET_OTA_START_MS, IPRONET_OTA_TRIES, rep);
    if (rc != IPRONET_OK) {
        return rc;
    }
    while (off < src->size) {
        uint32_t n = src->size - off;
        uint32_t got = 0;

        if (n > IPRONET_OTA_CHUNK) {
            n = IPRONET_OTA_CHUNK;
        }
        if (off < IPRONET_OTA_HDR_SIZE) {
            got = IPRONET_OTA_HDR_SIZE - off;
            if (got > n) {
                got = n;
            }
            memcpy(data, hdr + off, got);
        }
        while (got < n) {
            int r = src->read(src->ctx, data + got, n - got);

            if (r <= 0) {
                ipronet_ota_abort(req, ctx);
                return IPRONET_ERR_OTA_SOURCE;
            }
            got += (uint32_t)r;
        }
        part.msg_type = IPRONET_OTA_MSG_PROGRAM;
        part.offset = off;
        part.data_len = n;
        memcpy(msg, &part, sizeof(part));
        rc = ota_call(req, ctx, msg, (uint16_t)(sizeof(part) + n),
                      IPRONET_OTA_ST_PROGRAM_ACK, &ack,
                      IPRONET_OTA_PROGRAM_MS, IPRONET_OTA_TRIES, rep);
        if (rc == IPRONET_OK && ack != off + n) {
            rep->dev_err = IPRONET_OTA_E_GAP;
            rc = IPRONET_ERR_OTA_DEVICE;
        }
        if (rc != IPRONET_OK) {
            ipronet_ota_abort(req, ctx);
            return rc;
        }
        off += n;
        rep->bytes = off;
        rep->chunks++;
        if (opts != NULL && opts->on_progress != NULL &&
            (off % 65536U < n || off == src->size)) {
            opts->on_progress(opts->arg, off, src->size);
        }
    }
    return IPRONET_OK;
}

int ipronet_ota_commit(ipronet_ota_req_fn req, ipronet_ota_probe_fn probe,
                       void *ctx, ipronet_ota_report_t *rep)
{
    uint16_t body = IPRONET_OTA_MSG_COMMIT;
    int rc = IPRONET_ERR_OTA_TIMEOUT;

    for (int i = 0; i < IPRONET_OTA_TRIES; i++) {
        rc = ota_call(req, ctx, (const uint8_t *)&body, sizeof(body),
                      IPRONET_OTA_ST_COMMIT_ACK, NULL,
                      IPRONET_OTA_COMMIT_MS, 1, rep);
        if (rc != IPRONET_ERR_OTA_TIMEOUT) {
            return rc;
        }
        if (!probe(ctx)) {
            return IPRONET_OTA_COMMIT_ASSUMED;
        }
        rep->retries++;
    }
    return rc;
}

int ipronet_ota_query_pending(ipronet_ota_req_fn req, void *ctx,
                              uint32_t *pending, ipronet_ota_report_t *rep)
{
    uint16_t body = IPRONET_OTA_MSG_STATUS;

    return ota_call(req, ctx, (const uint8_t *)&body, sizeof(body),
                    IPRONET_OTA_ST_FIN_ACK, pending, IPRONET_OTA_START_MS,
                    IPRONET_OTA_TRIES, rep);
}
