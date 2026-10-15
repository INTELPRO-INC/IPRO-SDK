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

#include <stdio.h>
#include <string.h>

#include "ipronet_ota_proto.h"
#include "ota_fake_store.h"

_Static_assert((int)IPRONET_OTA_E_STATE == (int)RNM_OTA_E_STATE, "ota code");
_Static_assert((int)IPRONET_OTA_E_MISC == (int)RNM_OTA_E_MISC, "ota code");
_Static_assert(IPRONET_OTA_CHUNK == RNM_OTA_CHUNK_MAX, "ota chunk");
_Static_assert(sizeof(ipronet_ota_part_t) == 10, "part size");
_Static_assert(sizeof(ipronet_ota_status_t) == 8, "status size");

#define HW_OK "IPRO6_M_V1.0"

static int g_fail;

#define CHECK(c)                                                    \
    do {                                                            \
        if (!(c)) {                                                 \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c);     \
            g_fail++;                                               \
        }                                                           \
    } while (0)

/* Device model: the core behind the rnm_ota.c message mapping. */
typedef struct {
    rnm_ota_core_t core;
    fake_store_t *fs;
    int calls;
    uint64_t drop_req;
    uint64_t drop_rsp;
    int alive;
    uint16_t no_handler;
    int aborts;
    int commits;
    uint32_t pending;
} fake_dev_t;

static int fake_req(void *ctx, const uint8_t *body, uint16_t len,
                    uint8_t *rsp, uint16_t *rsp_len, uint16_t *status,
                    uint32_t timeout_ms)
{
    fake_dev_t *d = ctx;
    int i = d->calls++;
    ipronet_ota_status_t st;
    ipronet_ota_part_t p;
    uint16_t type;
    uint16_t code = 0;
    uint32_t val;

    (void)timeout_ms;
    if (i < 64 && ((d->drop_req >> i) & 1U)) {
        return IPRONET_ERR_OTA_TIMEOUT;
    }
    *status = d->no_handler;
    if (d->no_handler != 0) {
        *rsp_len = 0;
        return IPRONET_OK;
    }
    memcpy(&type, body, sizeof(type));
    memset(&st, 0, sizeof(st));
    st.msg_type = IPRONET_OTA_MSG_STATUS;
    switch (type) {
    case IPRONET_OTA_MSG_START:
        memcpy(&p, body, sizeof(p));
        st.status_type = IPRONET_OTA_ST_START_ACK;
        code = rnm_ota_core_start(&d->core, &fake_store_ops, d->fs, p.data_len);
        break;
    case IPRONET_OTA_MSG_PROGRAM:
        memcpy(&p, body, sizeof(p));
        st.status_type = IPRONET_OTA_ST_PROGRAM_ACK;
        if (p.data_len != (uint32_t)(len - sizeof(p))) {
            code = RNM_OTA_E_SIZE;
            break;
        }
        code = rnm_ota_core_program(&d->core, p.offset, body + sizeof(p),
                                    p.data_len);
        break;
    case IPRONET_OTA_MSG_COMMIT:
        d->commits++;
        st.status_type = IPRONET_OTA_ST_COMMIT_ACK;
        code = rnm_ota_core_commit(&d->core);
        break;
    case IPRONET_OTA_MSG_ABORT:
        d->aborts++;
        st.status_type = IPRONET_OTA_ST_ABORT_ACK;
        rnm_ota_core_abort(&d->core);
        break;
    case IPRONET_OTA_MSG_STATUS:
        st.status_type = IPRONET_OTA_ST_FIN_ACK;
        break;
    default:
        code = RNM_OTA_E_STATE;
        break;
    }
    val = (type == IPRONET_OTA_MSG_STATUS) ? d->pending : d->core.next_off;
    if (code != 0) {
        st.status_type = IPRONET_OTA_ST_ERROR;
        st.val = code;
    } else {
        st.val = val;
    }
    if (i < 64 && ((d->drop_rsp >> i) & 1U)) {
        return IPRONET_ERR_OTA_TIMEOUT;
    }
    memcpy(rsp, &st, sizeof(st));
    *rsp_len = sizeof(st);
    return IPRONET_OK;
}

static int fake_probe(void *ctx)
{
    return ((fake_dev_t *)ctx)->alive;
}

typedef struct {
    const uint8_t *buf;
    uint32_t len;
    uint32_t off;
    uint32_t fail_at;
} mem_src_t;

static int mem_read(void *ctx, uint8_t *dst, uint32_t n)
{
    mem_src_t *m = ctx;

    if (m->fail_at != 0 && m->off + n > m->fail_at) {
        return -1;
    }
    if (n > m->len - m->off) {
        n = m->len - m->off;
    }
    memcpy(dst, m->buf + m->off, n);
    m->off += n;
    return (int)n;
}

static int g_progress_calls;
static uint32_t g_progress_last;

static void on_progress(void *arg, uint32_t done, uint32_t total)
{
    (void)arg;
    (void)total;
    g_progress_calls++;
    g_progress_last = done;
}

/* Precheck, transfer and commit, as ipronet_ota_update() does. */
static int run(fake_dev_t *d, const uint8_t *pkg, uint32_t total,
               uint32_t fail_at, ipronet_ota_report_t *rep)
{
    mem_src_t m = { pkg, total, 0, fail_at };
    ipronet_ota_src_t src = { total, mem_read, &m };
    ipronet_ota_opts_t opts = { 60000U, 30000U, on_progress, NULL };
    uint8_t hdr[IPRONET_OTA_HDR_SIZE];
    int rc;

    memset(rep, 0, sizeof(*rep));
    if (mem_read(&m, hdr, sizeof(hdr)) != (int)sizeof(hdr)) {
        return IPRONET_ERR_OTA_SOURCE;
    }
    rc = ipronet_ota_check_header(hdr, total);
    if (rc == IPRONET_OK) {
        rc = ipronet_ota_xfer(&src, hdr, fake_req, d, &opts, rep);
    }
    if (rc == IPRONET_OK) {
        rc = ipronet_ota_commit(fake_req, fake_probe, d, rep);
    }
    return rc;
}

static fake_dev_t *dev_new(void)
{
    fake_dev_t *d = calloc(1, sizeof(*d));

    d->fs = fake_store_new();
    d->alive = 1;
    return d;
}

static void dev_free(fake_dev_t *d)
{
    free(d->fs);
    free(d);
}

static uint32_t nchunks(uint32_t total)
{
    return (total + IPRONET_OTA_CHUNK - 1U) / IPRONET_OTA_CHUNK;
}

static void test_check_header(void)
{
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    CHECK(ipronet_ota_check_header(pkg, total) == IPRONET_OK);
    CHECK(ipronet_ota_check_header(pkg, total + 1) == IPRONET_ERR_OTA_PKG);
    CHECK(ipronet_ota_check_header(pkg, 512) == IPRONET_ERR_OTA_PKG);
    pkg[3] ^= 0x01;
    CHECK(ipronet_ota_check_header(pkg, total) == IPRONET_ERR_OTA_PKG);
    pkg[3] ^= 0x01;
    memcpy(pkg + 16, "RAW ", 4);
    CHECK(ipronet_ota_check_header(pkg, total) == IPRONET_ERR_OTA_PKG);
    free(pkg);

    pkg = make_pkg(20000U, "IPRO7_M_V1.0", &total);
    CHECK(ipronet_ota_check_header(pkg, total) == IPRONET_ERR_OTA_PKG);
    free(pkg);
}

static void test_happy_path(void)
{
    fake_dev_t *d = dev_new();
    ipronet_ota_report_t rep;
    uint32_t total;
    uint8_t *pkg = make_pkg(557208U, HW_OK, &total);

    g_progress_calls = 0;
    CHECK(run(d, pkg, total, 0, &rep) == IPRONET_OK);
    CHECK(rep.bytes == total);
    CHECK(rep.chunks == nchunks(total));
    CHECK(rep.retries == 0);
    CHECK(d->fs->commits == 1);
    CHECK(memcmp(d->fs->flash + FAKE_SLOT_ADDR, pkg, total) == 0);
    CHECK(g_progress_calls >= 8);
    CHECK(g_progress_last == total);
    free(pkg);
    dev_free(d);
}

static void test_odd_size(void)
{
    fake_dev_t *d = dev_new();
    ipronet_ota_report_t rep;
    uint32_t total;
    uint8_t *pkg = make_pkg(100001U, HW_OK, &total);

    CHECK(run(d, pkg, total, 0, &rep) == IPRONET_OK);
    CHECK(memcmp(d->fs->flash + FAKE_SLOT_ADDR, pkg, total) == 0);
    free(pkg);
    dev_free(d);
}

static void test_lost_request_and_reply(void)
{
    fake_dev_t *d = dev_new();
    ipronet_ota_report_t rep;
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    d->drop_req = 1ULL << 6;
    d->drop_rsp = 1ULL << 9;
    CHECK(run(d, pkg, total, 0, &rep) == IPRONET_OK);
    CHECK(rep.retries == 2);
    CHECK(d->fs->bad_writes == 0);
    CHECK(memcmp(d->fs->flash + FAKE_SLOT_ADDR, pkg, total) == 0);
    free(pkg);
    dev_free(d);
}

static void test_persistent_loss(void)
{
    fake_dev_t *d = dev_new();
    ipronet_ota_report_t rep;
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    d->drop_req = (1ULL << 3) | (1ULL << 4) | (1ULL << 5);
    CHECK(run(d, pkg, total, 0, &rep) == IPRONET_ERR_OTA_TIMEOUT);
    CHECK(d->aborts == 1);
    CHECK(d->fs->commits == 0);
    free(pkg);
    dev_free(d);
}

static void test_device_errors(void)
{
    fake_dev_t *d = dev_new();
    ipronet_ota_report_t rep;
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    pkg[512 + 4096] ^= 0x01;
    CHECK(run(d, pkg, total, 0, &rep) == IPRONET_ERR_OTA_DEVICE);
    CHECK(rep.dev_err == IPRONET_OTA_E_CKSUM);
    CHECK(d->fs->commits == 0);
    pkg[512 + 4096] ^= 0x01;
    dev_free(d);

    d = dev_new();
    d->fs->slot_max = 0x1000U;
    CHECK(run(d, pkg, total, 0, &rep) == IPRONET_ERR_OTA_DEVICE);
    CHECK(rep.dev_err == IPRONET_OTA_E_TOO_BIG);
    dev_free(d);

    d = dev_new();
    d->no_handler = IPRONET_STATUS_NOT_IMPLEMENTED;
    CHECK(run(d, pkg, total, 0, &rep) == IPRONET_ERR_OTA_DEVICE);
    CHECK(rep.dev_err == IPRONET_STATUS_NOT_IMPLEMENTED);
    dev_free(d);
    free(pkg);
}

static void test_wrong_chip(void)
{
    fake_dev_t *d = dev_new();
    ipronet_ota_report_t rep;
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, "IPRO7_M_V1.0", &total);

    CHECK(run(d, pkg, total, 0, &rep) == IPRONET_ERR_OTA_PKG);
    CHECK(d->calls == 0);
    CHECK(d->fs->commits == 0);
    free(pkg);
    dev_free(d);
}

static void test_reader_failure(void)
{
    fake_dev_t *d = dev_new();
    ipronet_ota_report_t rep;
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    CHECK(run(d, pkg, total, 5000U, &rep) == IPRONET_ERR_OTA_SOURCE);
    CHECK(d->aborts == 1);
    CHECK(d->fs->commits == 0);
    free(pkg);
    dev_free(d);
}

static void test_commit_request_lost(void)
{
    fake_dev_t *d = dev_new();
    ipronet_ota_report_t rep;
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    d->drop_req = 1ULL << (1 + nchunks(total));
    CHECK(run(d, pkg, total, 0, &rep) == IPRONET_OK);
    CHECK(d->commits == 1);
    CHECK(d->fs->commits == 1);
    CHECK(rep.retries == 1);
    free(pkg);
    dev_free(d);
}

static void test_commit_reply_lost(void)
{
    fake_dev_t *d = dev_new();
    ipronet_ota_report_t rep;
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    d->drop_rsp = 1ULL << (1 + nchunks(total));
    d->alive = 0;
    CHECK(run(d, pkg, total, 0, &rep) == IPRONET_OTA_COMMIT_ASSUMED);
    CHECK(d->commits == 1);
    CHECK(d->fs->commits == 1);
    free(pkg);
    dev_free(d);
}

static void test_query_pending(void)
{
    fake_dev_t *d = dev_new();
    ipronet_ota_report_t rep;
    uint32_t pending = 7;

    memset(&rep, 0, sizeof(rep));
    CHECK(ipronet_ota_query_pending(fake_req, d, &pending, &rep) ==
          IPRONET_OK);
    CHECK(pending == 0);
    d->pending = 1;
    CHECK(ipronet_ota_query_pending(fake_req, d, &pending, &rep) ==
          IPRONET_OK);
    CHECK(pending == 1);
    d->no_handler = IPRONET_STATUS_NOT_IMPLEMENTED;
    CHECK(ipronet_ota_query_pending(fake_req, d, &pending, &rep) ==
          IPRONET_ERR_OTA_DEVICE);
    CHECK(rep.dev_err == IPRONET_STATUS_NOT_IMPLEMENTED);
    dev_free(d);
}

int main(void)
{
    test_check_header();
    test_happy_path();
    test_odd_size();
    test_lost_request_and_reply();
    test_persistent_loss();
    test_device_errors();
    test_wrong_chip();
    test_reader_failure();
    test_commit_request_lost();
    test_commit_reply_lost();
    test_query_pending();
    printf("%s (%d failures)\n", g_fail ? "FAILED" : "PASSED", g_fail);
    return g_fail != 0;
}
