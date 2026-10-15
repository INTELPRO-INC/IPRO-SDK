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

#include "ota_fake_store.h"

#define PAYLOAD   557208U
#define HW_OK     "IPRO6_M_V1.0"

static int g_fail;

#define CHECK(c)                                                    \
    do {                                                            \
        if (!(c)) {                                                 \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c);     \
            g_fail++;                                               \
        }                                                           \
    } while (0)

static uint16_t send_all(rnm_ota_core_t *c, const uint8_t *pkg,
                         uint32_t total, uint32_t chunk)
{
    for (uint32_t off = 0; off < total;) {
        uint32_t n = (total - off < chunk) ? total - off : chunk;
        uint16_t rc = rnm_ota_core_program(c, off, pkg + off, n);

        if (rc != 0) {
            return rc;
        }
        off += n;
    }
    return 0;
}

static uint32_t align4k(uint32_t v)
{
    return (v + 4095U) & ~4095U;
}

static void test_happy_path(void)
{
    fake_store_t *f = fake_store_new();
    rnm_ota_core_t c;
    uint32_t total;
    uint8_t *pkg = make_pkg(PAYLOAD, HW_OK, &total);

    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == 0);
    CHECK(send_all(&c, pkg, total, RNM_OTA_CHUNK_MAX) == 0);
    CHECK(rnm_ota_core_commit(&c) == 0);
    CHECK(f->commits == 1);
    CHECK(f->bad_writes == 0);
    CHECK(f->erase_hi == FAKE_SLOT_ADDR + align4k(total));
    CHECK(memcmp(f->flash + FAKE_SLOT_ADDR, pkg, total) == 0);
    CHECK(rnm_ota_core_commit(&c) == RNM_OTA_E_STATE);
    free(pkg);
    free(f);
}

static void test_odd_size_small_chunks(void)
{
    fake_store_t *f = fake_store_new();
    rnm_ota_core_t c;
    uint32_t total;
    uint8_t *pkg = make_pkg(100001U, HW_OK, &total);

    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == 0);
    CHECK(send_all(&c, pkg, total, 100U) == 0);
    CHECK(rnm_ota_core_commit(&c) == 0);
    CHECK(f->bad_writes == 0);
    CHECK(f->erase_hi == FAKE_SLOT_ADDR + align4k(total));
    CHECK(memcmp(f->flash + FAKE_SLOT_ADDR, pkg, total) == 0);
    free(pkg);
    free(f);
}

static void test_duplicate_chunk(void)
{
    fake_store_t *f = fake_store_new();
    rnm_ota_core_t c;
    uint32_t total;
    uint32_t writes;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == 0);
    CHECK(rnm_ota_core_program(&c, 0, pkg, 1024) == 0);
    writes = f->writes;
    CHECK(rnm_ota_core_program(&c, 0, pkg, 1024) == 0);
    CHECK(f->writes == writes);
    CHECK(c.next_off == 1024);
    CHECK(send_all(&c, pkg, total, 1024) == 0);
    CHECK(rnm_ota_core_commit(&c) == 0);
    CHECK(f->bad_writes == 0);
    free(pkg);
    free(f);
}

static void test_gap_keeps_session(void)
{
    fake_store_t *f = fake_store_new();
    rnm_ota_core_t c;
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == 0);
    CHECK(rnm_ota_core_program(&c, 0, pkg, 1024) == 0);
    CHECK(rnm_ota_core_program(&c, 2048, pkg + 2048, 1024) == RNM_OTA_E_GAP);
    CHECK(c.active == 1);
    CHECK(rnm_ota_core_program(&c, 1024, pkg + 1024, 1024) == 0);
    free(pkg);
    free(f);
}

static void test_size_limits(void)
{
    fake_store_t *f = fake_store_new();
    rnm_ota_core_t c;
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, 512) == RNM_OTA_E_TOO_BIG);
    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, FAKE_SLOT_MAX + 1) ==
          RNM_OTA_E_TOO_BIG);
    CHECK(c.active == 0);
    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == 0);
    CHECK(rnm_ota_core_program(&c, 0, pkg, 0) == RNM_OTA_E_SIZE);
    CHECK(c.active == 0);
    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == 0);
    CHECK(rnm_ota_core_program(&c, 0, pkg, 1025) == RNM_OTA_E_SIZE);
    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == 0);
    CHECK(rnm_ota_core_program(&c, total - 10, pkg, 20) == RNM_OTA_E_SIZE);
    free(pkg);
    free(f);
}

static void expect_header_reject(uint8_t *pkg, uint32_t total, uint32_t claim)
{
    fake_store_t *f = fake_store_new();
    rnm_ota_core_t c;

    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, claim) == 0);
    CHECK(rnm_ota_core_program(&c, 0, pkg, total < 1024 ? total : 1024) ==
          RNM_OTA_E_HEADER);
    CHECK(c.active == 0);
    CHECK(f->commits == 0);
    free(f);
}

static void test_bad_header(void)
{
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);
    uint8_t *ipro7 = make_pkg(20000U, "IPRO7_M_V1.0", &total);

    pkg[0] ^= 0x01;
    expect_header_reject(pkg, total, total);
    pkg[0] ^= 0x01;
    memcpy(pkg + 16, "RAW ", 4);
    expect_header_reject(pkg, total, total);
    memcpy(pkg + 16, "XZ  ", 4);
    expect_header_reject(pkg, total, total + 4096U);
    expect_header_reject(ipro7, total, total);
    free(pkg);
    free(ipro7);
}

static void test_corrupt_payload(void)
{
    fake_store_t *f = fake_store_new();
    rnm_ota_core_t c;
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    pkg[512 + 4096] ^= 0x01;
    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == 0);
    CHECK(send_all(&c, pkg, total, 1024) == 0);
    CHECK(rnm_ota_core_commit(&c) == RNM_OTA_E_CKSUM);
    CHECK(f->commits == 0);
    CHECK(c.active == 0);
    free(pkg);
    free(f);
}

static void test_commit_early_and_no_session(void)
{
    fake_store_t *f = fake_store_new();
    rnm_ota_core_t c;
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == 0);
    CHECK(rnm_ota_core_program(&c, 0, pkg, 1024) == 0);
    CHECK(rnm_ota_core_commit(&c) == RNM_OTA_E_SIZE);
    CHECK(f->commits == 0);
    CHECK(rnm_ota_core_program(&c, 1024, pkg + 1024, 1024) == RNM_OTA_E_STATE);
    CHECK(rnm_ota_core_commit(&c) == RNM_OTA_E_STATE);
    free(pkg);
    free(f);
}

static void test_store_errors(void)
{
    fake_store_t *f = fake_store_new();
    rnm_ota_core_t c;
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    f->fail_write = 3;
    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == 0);
    CHECK(send_all(&c, pkg, total, 1024) == RNM_OTA_E_MISC);
    CHECK(c.active == 0);
    f->slot_max = 0;
    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == RNM_OTA_E_NO_SLOT);
    f->slot_max = 0x10001U;
    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == RNM_OTA_E_NO_SLOT);
    free(pkg);
    free(f);
}

static void test_restart_mid_transfer(void)
{
    fake_store_t *f = fake_store_new();
    rnm_ota_core_t c;
    uint32_t total;
    uint8_t *pkg = make_pkg(20000U, HW_OK, &total);

    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == 0);
    CHECK(rnm_ota_core_program(&c, 0, pkg, 1024) == 0);
    CHECK(rnm_ota_core_program(&c, 1024, pkg + 1024, 1024) == 0);
    CHECK(rnm_ota_core_start(&c, &fake_store_ops, f, total) == 0);
    CHECK(c.next_off == 0);
    CHECK(send_all(&c, pkg, total, 1024) == 0);
    CHECK(rnm_ota_core_commit(&c) == 0);
    CHECK(f->bad_writes == 0);
    free(pkg);
    free(f);
}

int main(void)
{
    test_happy_path();
    test_odd_size_small_chunks();
    test_duplicate_chunk();
    test_gap_keeps_session();
    test_size_limits();
    test_bad_header();
    test_corrupt_payload();
    test_commit_early_and_no_session();
    test_store_errors();
    test_restart_mid_transfer();
    printf("%s (%d failures)\n", g_fail ? "FAILED" : "PASSED", g_fail);
    return g_fail != 0;
}
