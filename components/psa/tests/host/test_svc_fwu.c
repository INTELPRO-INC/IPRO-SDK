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

/*
 * Host test for svc_fwu.c (-DSPM_HOST) on a 1 MB NOR flash in RAM: a
 * write can only clear bits, an erase sets a 4 KB sector to 0xFF.
 */
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "fwu_fixture.h"
#include "hal_flash.h"
#include "hal_ptmgr.h"
#include "psa_ipro/fwu_rpc.h"
#include "svc_fwu.h"
#include "utils_crc.h"

#define FLASH_SIZE 0x100000u
#define PT0        0x12000u
#define PT1        0x13000u
#define FWA        0x14000u
#define FWA_SIZE   0x60000u
#define FWB        0x74000u
#define FWB_SIZE   0x50000u
#define PT_LEN     sizeof(fix_pt)
#define FW_AGE_OFF (16u + 36u * FIX_FW_ENTRY + 32u)

static uint8_t g_flash[FLASH_SIZE];
static uint8_t g_snap[FLASH_SIZE];
static uint8_t g_pkg[FWB_SIZE];
static uint32_t g_fail_erase = 0xFFFFFFFFu;
static int g_lose_write;
static int g_erases;
static uint32_t g_svn_floor;
static uint32_t g_ota_res = FWU_OTA_NONE;
static int g_ota_taken;

static int in_flash(uint32_t addr, uint32_t len)
{
    return addr <= FLASH_SIZE && len <= FLASH_SIZE - addr;
}

int flash_read(uint32_t addr, uint8_t *data, uint32_t len)
{
    if (!in_flash(addr, len)) {
        return ERROR;
    }
    memcpy(data, &g_flash[addr], len);
    return SUCCESS;
}

/* every sector [addr, addr + len) touches */
int flash_erase(uint32_t addr, uint32_t len)
{
    uint32_t first = addr & ~(FWU_SECTOR - 1u);

    if (len == 0 || !in_flash(addr, len) || addr == g_fail_erase) {
        return ERROR;
    }
    g_erases++;
    memset(&g_flash[first], 0xFF, addr + len - first);
    for (uint32_t a = addr + len; a % FWU_SECTOR != 0; a++) {
        g_flash[a] = 0xFF;
    }
    return SUCCESS;
}

int flash_write(uint32_t addr, uint8_t *data, uint32_t len)
{
    if (!in_flash(addr, len)) {
        return ERROR;
    }
    for (uint32_t i = 0; i < len; i++) {
        g_flash[addr + i] &= data[i];
    }
    if (g_lose_write && len != 0) {
        g_lose_write = 0;
        g_flash[addr + len - 1u] ^= 0xFF;
    }
    return SUCCESS;
}

uint32_t fwu_hw_svn_floor(void)
{
    return g_svn_floor;
}

uint32_t fwu_hw_take_ota_word(void)
{
    uint32_t v = g_ota_res;

    g_ota_res = 0;
    g_ota_taken++;
    return v;
}

static void put32(uint8_t *p, uint32_t v)
{
    memcpy(p, &v, 4);
}

static uint32_t get32(const uint8_t *p)
{
    uint32_t v;

    memcpy(&v, p, 4);
    return v;
}

/* Blank flash, PT addresses in the bootheader, the table in PT0+PT1 */
static void flash_fresh(const uint8_t *pt, size_t len)
{
    memset(g_flash, 0xFF, sizeof(g_flash));
    put32(&g_flash[164], PT0);
    put32(&g_flash[168], PT1);
    memcpy(&g_flash[PT0], pt, len);
    memcpy(&g_flash[PT1], pt, len);
    g_fail_erase = 0xFFFFFFFFu;
    g_lose_write = 0;
    g_ota_res = FWU_OTA_NONE;
    svc_fwu_init();
    svc_fwu_client_reset();
}

/* Package of len bytes: hdr (32 bytes) or a synthetic one with svn */
static void make_pkg(uint32_t len, const uint8_t *hdr, uint32_t svn)
{
    uint32_t x = 0x12345678u ^ len ^ svn;

    for (uint32_t i = 0; i < len; i++) {
        x = x * 1103515245u + 12345u;
        g_pkg[i] = (uint8_t)(x >> 16);
    }
    if (hdr != NULL) {
        memcpy(g_pkg, hdr, FWU_HDR_CHECK);
        return;
    }
    memcpy(g_pkg, FWU_HDR_MAGIC, sizeof(FWU_HDR_MAGIC));
    memcpy(&g_pkg[FWU_HDR_TYPE_OFF], FWU_HDR_TYPE, 4);
    put32(&g_pkg[FWU_HDR_LEN_OFF], len - FWU_HDR_SIZE);
    put32(&g_pkg[FWU_HDR_SVN_OFF], svn);
    put32(&g_pkg[FWU_HDR_SVN_OFF + 4u], 0);
}

static psa_status_t w(uint32_t off, const uint8_t *data, uint32_t n)
{
    struct fwu_req r = { off, n };
    psa_invec in[2] = { { &r, sizeof(r) }, { data, n } };

    return svc_fwu_call(FWU_FN_WRITE, in, 2, NULL, 0);
}

static psa_status_t install(uint32_t len)
{
    struct fwu_req r = { 0, len };
    psa_invec in = { &r, sizeof(r) };

    return svc_fwu_call(FWU_FN_INSTALL, &in, 1, NULL, 0);
}

static psa_status_t query(struct fwu_status *q)
{
    psa_outvec out = { q, sizeof(*q) };
    psa_status_t st;

    memset(q, 0xEE, sizeof(*q));
    st = svc_fwu_call(FWU_FN_QUERY, NULL, 0, &out, 1);
    return st == PSA_SUCCESS && out.len != sizeof(*q) ? PSA_ERROR_GENERIC_ERROR
                                                     : st;
}

/* g_pkg[0, len) in FWU_WRITE_MAX chunks from offset 0 */
static psa_status_t stage(uint32_t len)
{
    psa_status_t st = PSA_SUCCESS;

    for (uint32_t off = 0; st == PSA_SUCCESS && off < len;
         off += FWU_WRITE_MAX) {
        uint32_t n = len - off < FWU_WRITE_MAX ? len - off : FWU_WRITE_MAX;

        st = w(off, &g_pkg[off], n);
    }
    return st;
}

/* boot2 after an install: other copy, FW activeindex 0, ages + 1 */
static void boot2_installs(uint32_t from, uint32_t to)
{
    uint8_t t[PT_LEN];
    uint32_t crc;

    memcpy(t, &g_flash[from], PT_LEN);
    t[16u + 36u * FIX_FW_ENTRY + 2u] = 0;
    put32(&t[FW_AGE_OFF], get32(&t[FW_AGE_OFF]) + 1u);
    put32(&t[8], get32(&t[8]) + 1u);
    put32(&t[12], utils_crc32(t, 12));
    crc = utils_crc32(&t[16], PT_LEN - 20u);
    put32(&t[PT_LEN - 4u], crc);
    memset(&g_flash[to], 0xFF, FWU_SECTOR);
    memcpy(&g_flash[to], t, PT_LEN);
}

static int all_ff(uint32_t addr, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        if (g_flash[addr + i] != 0xFF) {
            return 0;
        }
    }
    return 1;
}

static int pts_as_fresh(void)
{
    return memcmp(&g_flash[PT0], fix_pt, PT_LEN) == 0 &&
           memcmp(&g_flash[PT1], fix_pt, PT_LEN) == 0;
}

/* fixture: pt_age1 differs from pt_pending only in the age fields */
static void test_fixture(void)
{
    int only_ages = 1;

    for (size_t i = 0; i < PT_LEN; i++) {
        int age = (i >= 8 && i < 16) ||
                  (i >= FW_AGE_OFF && i < FW_AGE_OFF + 4u) ||
                  i >= PT_LEN - 4u;

        if (!age && fix_pt_age1[i] != fix_pt_pending[i]) {
            only_ages = 0;
        }
    }
    check(only_ages && get32(&fix_pt_age1[8]) == 1u &&
          get32(&fix_pt_age1[FW_AGE_OFF]) == 1u &&
          fix_pt_pending[16u + 36u * FIX_FW_ENTRY + 2u] == 1u,
          "fixture: pt_age1 = pt_pending with table and FW age 1");
}

static void test_pending_table(void)
{
    struct fwu_status q;

    flash_fresh(fix_pt, PT_LEN);
    check(query(&q) == PSA_SUCCESS && q.state == FWU_STATE_IDLE &&
          q.fw_active == 0 && q.pt_active == 0 && q.pt_age == 0 &&
          q.staged_len == 0 && q.slot_size == FWB_SIZE &&
          q.staged_svn == FWU_SVN_NONE && q.last_ota == FWU_OTA_NONE &&
          q.running_svn == FWU_SVN_NONE,
          "fresh table: IDLE, FW active 0, PT0 age 0, FW-B 320 KB");
    make_pkg(FIX_PKG_LEN, fix_pkg_hdr, 0);
    check(stage(FIX_PKG_LEN) == PSA_SUCCESS &&
          memcmp(&g_flash[FWB], g_pkg, FIX_PKG_LEN) == 0,
          "psa_ota_pkg header package staged in FW-B, reads back");
    check(query(&q) == PSA_SUCCESS && q.state == FWU_STATE_WRITING &&
          q.staged_len == FIX_PKG_LEN && q.staged_svn == 1u,
          "after the writes: WRITING, staged_len, header SVN 1");
    memcpy(g_snap, g_flash, sizeof(g_flash));
    check(install(FIX_PKG_LEN) == PSA_SUCCESS, "install: SUCCESS");
    check(memcmp(&g_flash[PT1], fix_pt_age1, PT_LEN) == 0,
          "PT1 = psa_ota_pkg pt_pending with table and FW age 1");
    check(all_ff(PT1 + PT_LEN, FWU_SECTOR - PT_LEN),
          "rest of the PT1 sector erased");
    check(memcmp(g_flash, g_snap, PT1) == 0 &&
          memcmp(&g_flash[PT1 + FWU_SECTOR], &g_snap[PT1 + FWU_SECTOR],
                 FLASH_SIZE - PT1 - FWU_SECTOR) == 0,
          "install writes nothing but the PT1 sector");
    check(query(&q) == PSA_SUCCESS && q.state == FWU_STATE_PENDING &&
          q.fw_active == 1 && q.pt_active == 1 && q.pt_age == 1 &&
          q.staged_svn == 1u,
          "query: PENDING, FW active 1 in PT1 of age 1");
}

/* boot2 installs, the SPE boots: result word, then a second round */
static void test_boot2_cycle(void)
{
    struct fwu_status q;

    boot2_installs(PT1, PT0);
    g_svn_floor = 0;
    g_ota_res = FWU_OTA_RES_MAGIC | FWU_OTA_INSTALLED;
    g_ota_taken = 0;
    svc_fwu_init();
    check(g_ota_taken == 1 && g_ota_res == 0,
          "init takes and clears the boot2 OTA result word");
    check(query(&q) == PSA_SUCCESS && q.state == FWU_STATE_IDLE &&
          q.fw_active == 0 && q.pt_active == 0 && q.pt_age == 2 &&
          q.staged_len == 0 && q.last_ota == 0 && q.running_svn == 1u,
          "after boot2's install: IDLE, PT0 age 2, OTA 0, running SVN 1");
    make_pkg(100000u, NULL, 2);
    check(stage(100000u) == PSA_SUCCESS && install(100000u) ==
          PSA_SUCCESS, "second package staged and installed");
    check(query(&q) == PSA_SUCCESS && q.state == FWU_STATE_PENDING &&
          q.pt_active == 1 && q.pt_age == 3 && q.staged_svn == 2u &&
          g_flash[PT1 + 16u + 36u * FIX_FW_ENTRY + 2u] == 1u &&
          get32(&g_flash[PT1 + FW_AGE_OFF]) == 3u,
          "second round: PT1 age 3, FW age 3, pending");

    g_ota_res = FWU_OTA_RES_MAGIC | 3u;
    svc_fwu_init();
    check(query(&q) == PSA_SUCCESS && q.last_ota == 3u &&
          q.running_svn == FWU_SVN_NONE,
          "boot2 refused (sha): OTA 3, running SVN unknown");
    g_ota_res = 0x12340000u;
    svc_fwu_init();
    check(query(&q) == PSA_SUCCESS && q.last_ota == FWU_OTA_NONE,
          "result word without the OR magic: no OTA result");
    g_svn_floor = 7;
    check(query(&q) == PSA_SUCCESS && q.svn_floor == 7u,
          "svn_floor is the eFuse counter");
    g_svn_floor = 0;
}

static void test_offsets(void)
{
    struct fwu_status q;
    static const uint8_t d[FWU_WRITE_MAX + 1u];

    flash_fresh(fix_pt, PT_LEN);
    check(w(2048, d, 16) == PSA_ERROR_BAD_STATE,
          "first write not at offset 0: BAD_STATE");
    check(w(0, d, 2048) == PSA_SUCCESS && w(4096, d, 16) ==
          PSA_ERROR_BAD_STATE && query(&q) == PSA_SUCCESS &&
          q.staged_len == 2048,
          "gap after 2048: BAD_STATE, staged_len kept");
    check(w(2048, d, 0) == PSA_ERROR_INVALID_ARGUMENT,
          "empty write: INVALID_ARGUMENT");
    check(w(2048, d, FWU_WRITE_MAX + 1u) == PSA_ERROR_INVALID_ARGUMENT,
          "write over FWU_WRITE_MAX: INVALID_ARGUMENT");
    {
        struct fwu_req r = { 2048, 15 };
        psa_invec in[2] = { { &r, sizeof(r) }, { d, 16 } };

        check(svc_fwu_call(FWU_FN_WRITE, in, 2, NULL, 0) ==
              PSA_ERROR_INVALID_ARGUMENT,
              "req.len != data length: INVALID_ARGUMENT");
    }
    check(w(FWB_SIZE - 8u, d, 16) == PSA_ERROR_INVALID_ARGUMENT,
          "write past the FW-B end: INVALID_ARGUMENT");
    check(w(0xFFFFFFF8u, d, 16) == PSA_ERROR_INVALID_ARGUMENT,
          "offset + len wrapping: INVALID_ARGUMENT");
    check(w(FWB_SIZE, d, 1) == PSA_ERROR_INVALID_ARGUMENT,
          "write at the FW-B end: INVALID_ARGUMENT");

    flash_fresh(fix_pt, PT_LEN);
    memcpy(g_snap, g_flash, sizeof(g_flash));
    make_pkg(FWB_SIZE, NULL, 1);
    check(stage(FWB_SIZE) == PSA_SUCCESS &&
          memcmp(&g_flash[FWB], g_pkg, FWB_SIZE) == 0,
          "whole FW-B staged, last chunk ends at the FW-B end");
    check(memcmp(g_flash, g_snap, FWB) == 0 &&
          memcmp(&g_flash[FWB + FWB_SIZE], &g_snap[FWB + FWB_SIZE],
                 FLASH_SIZE - FWB - FWB_SIZE) == 0,
          "nothing outside FW-B changed by the writes");
    check(install(FWB_SIZE) == PSA_SUCCESS,
          "package of exactly FW-B: installed");
}

static void test_erase_before_write(void)
{
    static uint8_t zero[FWU_WRITE_MAX];
    int erases;

    flash_fresh(fix_pt, PT_LEN);
    memset(zero, 0, sizeof(zero));
    for (uint32_t off = 0; off < 3u * FWU_SECTOR; off += sizeof(zero)) {
        w(off, zero, sizeof(zero));
    }
    make_pkg(3u * FWU_SECTOR, NULL, 4);
    g_erases = 0;
    check(stage(3u * FWU_SECTOR) == PSA_SUCCESS &&
          memcmp(&g_flash[FWB], g_pkg, 3u * FWU_SECTOR) == 0,
          "restart at 0 over written sectors: new bytes read back");
    erases = g_erases;
    check(erases == 3, "one erase per sector");
    g_erases = 0;
    check(w(0, zero, 1000) == PSA_SUCCESS && g_erases == 1 &&
          w(1000, zero, 2048) == PSA_SUCCESS && g_erases == 1 &&
          w(3048, zero, 2048) == PSA_SUCCESS && g_erases == 2 &&
          w(5096, zero, 10) == PSA_SUCCESS && g_erases == 2,
          "a sector is erased when a chunk starts at or crosses into it");
}

/* header patched at off with v (4 bytes) must be refused */
static void refuse_hdr(uint32_t off, uint32_t v, uint32_t len,
                       const char *what)
{
    flash_fresh(fix_pt, PT_LEN);
    make_pkg(len, NULL, 1);
    put32(&g_pkg[off], v);
    check(stage(len) == PSA_SUCCESS &&
          install(len) == PSA_ERROR_DATA_INVALID && pts_as_fresh(), what);
}

static void test_header_refusals(void)
{
    refuse_hdr(0, 0x4f525048u, 8192, "bad magic: DATA_INVALID, PTs kept");
    refuse_hdr(FWU_HDR_TYPE_OFF, 0x20205a4cu, 8192,
               "type not XZ: DATA_INVALID, PTs kept");
    refuse_hdr(FWU_HDR_LEN_OFF, 0, 8192,
               "payload length 0: DATA_INVALID, PTs kept");
    refuse_hdr(FWU_HDR_LEN_OFF, 8192 - FWU_HDR_SIZE + 1u, 8192,
               "payload + 512 > staged: DATA_INVALID, PTs kept");
    refuse_hdr(FWU_HDR_LEN_OFF, 8192 - FWU_HDR_SIZE - 1u, 8192,
               "payload + 512 < staged: DATA_INVALID, PTs kept");
    refuse_hdr(FWU_HDR_LEN_OFF, 0xFFFFFF00u, 256,
               "payload + 512 wrapping to the 256 staged: DATA_INVALID");
}

static void test_svn_parse(void)
{
    struct fwu_status q;

    flash_fresh(fix_pt, PT_LEN);
    make_pkg(4096, NULL, 5);
    memcpy(&g_pkg[FWU_HDR_SVN_OFF], FWU_HDR_SVN_LEGACY, 8);
    check(stage(4096) == PSA_SUCCESS && query(&q) == PSA_SUCCESS &&
          q.staged_svn == 0, "legacy SVN filler: staged SVN 0");
    make_pkg(4096, NULL, 5);
    check(stage(4096) == PSA_SUCCESS && query(&q) == PSA_SUCCESS &&
          q.staged_svn == 5u, "SVN 5 header: staged SVN 5");
}

static void test_state_refusals(void)
{
    flash_fresh(fix_pt, PT_LEN);
    make_pkg(8192, NULL, 1);
    check(install(8192) == PSA_ERROR_BAD_STATE && pts_as_fresh(),
          "install with nothing staged: BAD_STATE");
    check(stage(8192) == PSA_SUCCESS &&
          install(8191) == PSA_ERROR_INVALID_ARGUMENT && pts_as_fresh(),
          "install len != staged: INVALID_ARGUMENT");
    {
        struct fwu_req r = { 4, 8192 };
        psa_invec in = { &r, sizeof(r) };

        check(svc_fwu_call(FWU_FN_INSTALL, &in, 1, NULL, 0) ==
              PSA_ERROR_INVALID_ARGUMENT, "install offset != 0: refused");
    }
    svc_fwu_client_reset();
    check(install(8192) == PSA_ERROR_BAD_STATE,
          "after the client reset: nothing staged, BAD_STATE");
    check(stage(8192) == PSA_SUCCESS && install(8192) == PSA_SUCCESS,
          "staged again: installed");
    check(w(0, g_pkg, 16) == PSA_ERROR_BAD_STATE &&
          install(8192) == PSA_ERROR_BAD_STATE,
          "pending: write and install BAD_STATE");
}

static void test_flash_failures(void)
{
    struct fwu_status q;

    flash_fresh(fix_pt, PT_LEN);
    make_pkg(8192, NULL, 1);
    g_lose_write = 1;
    check(w(0, g_pkg, 2048) == PSA_ERROR_STORAGE_FAILURE &&
          query(&q) == PSA_SUCCESS && q.staged_len == 0 &&
          w(2048, g_pkg, 16) == PSA_ERROR_BAD_STATE,
          "readback mismatch: STORAGE_FAILURE, staging restarts at 0");
    g_fail_erase = FWB + FWU_SECTOR;
    check(stage(8192) == PSA_ERROR_STORAGE_FAILURE,
          "FW-B erase failure: STORAGE_FAILURE");
    g_fail_erase = 0xFFFFFFFFu;
    check(stage(8192) == PSA_SUCCESS, "staged");
    g_fail_erase = PT1;
    check(install(8192) == PSA_ERROR_STORAGE_FAILURE && pts_as_fresh() &&
          query(&q) == PSA_SUCCESS && q.state == FWU_STATE_WRITING,
          "PT1 erase failure: STORAGE_FAILURE, not pending");
    g_fail_erase = 0xFFFFFFFFu;
    g_lose_write = 1;
    check(install(8192) == PSA_ERROR_STORAGE_FAILURE &&
          memcmp(&g_flash[PT0], fix_pt, PT_LEN) == 0 &&
          query(&q) == PSA_SUCCESS && q.state == FWU_STATE_WRITING &&
          q.pt_active == 0,
          "PT1 readback mismatch: STORAGE_FAILURE, PT0 still active");
}

/* fix_pt with the FW entry's Address[1] / maxLen[1] replaced */
static void table_with_fwb(uint8_t *t, uint32_t addr1, uint32_t size1)
{
    uint8_t *e = &t[16u + 36u * FIX_FW_ENTRY];

    memcpy(t, fix_pt, PT_LEN);
    put32(&e[16], addr1);
    put32(&e[24], size1);
    put32(&t[PT_LEN - 4u], utils_crc32(&t[16], PT_LEN - 20u));
}

static void test_tables(void)
{
    struct fwu_status q;
    uint8_t t[PT_LEN];
    uint8_t d[16] = { 0 };

    table_with_fwb(t, 0, 0);
    flash_fresh(t, PT_LEN);
    check(query(&q) == PSA_ERROR_NOT_SUPPORTED &&
          w(0, d, 16) == PSA_ERROR_NOT_SUPPORTED,
          "table without FW-B: NOT_SUPPORTED");
    table_with_fwb(t, FWA + 0x10000u, FWB_SIZE);
    flash_fresh(t, PT_LEN);
    check(w(0, d, 16) == PSA_ERROR_NOT_SUPPORTED,
          "FW-B overlapping FW-A: NOT_SUPPORTED");
    table_with_fwb(t, FWB + 0x100u, FWB_SIZE);
    flash_fresh(t, PT_LEN);
    check(w(0, d, 16) == PSA_ERROR_NOT_SUPPORTED,
          "FW-B not sector aligned: NOT_SUPPORTED");
    table_with_fwb(t, FWB, FWB_SIZE);
    check(memcmp(t, fix_pt, PT_LEN) == 0, "table rebuild = fixture");

    flash_fresh(fix_pt, PT_LEN);
    put32(&g_flash[168], FWB);
    check(w(0, d, 16) == PSA_ERROR_NOT_SUPPORTED,
          "other table address inside FW-B: NOT_SUPPORTED");
    flash_fresh(fix_pt, PT_LEN);
    g_flash[PT0 + 20] ^= 1;
    g_flash[PT1 + 12] ^= 1;
    check(query(&q) == PSA_ERROR_STORAGE_FAILURE,
          "no valid table: STORAGE_FAILURE");
    flash_fresh(fix_pt, PT_LEN);
    g_flash[PT0 + 20] ^= 1;
    check(query(&q) == PSA_SUCCESS && q.pt_active == 1,
          "PT0 entries CRC bad: PT1 is the table");
}

static void test_shapes(void)
{
    struct fwu_req r = { 0, 16 };
    uint8_t d[16] = { 0 };
    uint8_t big[sizeof(struct fwu_status) + 1u];
    psa_invec in[2] = { { &r, sizeof(r) }, { d, sizeof(d) } };
    psa_invec in_short[1] = { { &r, 4 } };
    psa_outvec out = { big, sizeof(struct fwu_status) - 1u };
    psa_outvec out_big = { big, sizeof(big) };

    flash_fresh(fix_pt, PT_LEN);
    check(svc_fwu_call(FWU_FN_WRITE, in, 1, NULL, 0) ==
          PSA_ERROR_INVALID_ARGUMENT, "write with one in vector: refused");
    check(svc_fwu_call(FWU_FN_WRITE, in, 2, &out_big, 1) ==
          PSA_ERROR_INVALID_ARGUMENT && out_big.len == 0,
          "write with an out vector: refused, out len 0");
    check(svc_fwu_call(FWU_FN_INSTALL, in_short, 1, NULL, 0) ==
          PSA_ERROR_INVALID_ARGUMENT, "install req of 4 bytes: refused");
    check(svc_fwu_call(FWU_FN_QUERY, NULL, 0, &out, 1) ==
          PSA_ERROR_INVALID_ARGUMENT && out.len == 0,
          "query out of 39 bytes: refused, len 0");
    out_big.len = sizeof(big);
    check(svc_fwu_call(FWU_FN_QUERY, NULL, 0, &out_big, 1) ==
          PSA_ERROR_INVALID_ARGUMENT, "query out of 41 bytes: refused");
    out.len = sizeof(struct fwu_status);
    check(svc_fwu_call(FWU_FN_QUERY, in, 1, &out, 1) ==
          PSA_ERROR_INVALID_ARGUMENT, "query with an in vector: refused");
    check(svc_fwu_call(0, NULL, 0, NULL, 0) == PSA_ERROR_NOT_SUPPORTED &&
          svc_fwu_call(FWU_FN_QUERY + 1, NULL, 0, NULL, 0) ==
              PSA_ERROR_NOT_SUPPORTED,
          "unknown fn: NOT_SUPPORTED");
}

int main(void)
{
    test_fixture();
    test_pending_table();
    test_boot2_cycle();
    test_offsets();
    test_erase_before_write();
    test_header_refusals();
    test_svn_parse();
    test_state_refusals();
    test_flash_failures();
    test_tables();
    test_shapes();
    return check_summary();
}
