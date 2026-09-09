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
 * Transport-agnostic OTA staging core (see ota_common.h).
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hal_flash.h"
#include "hal_ptmgr.h"
#include "hal_boot2.h"
#include "hal_sec_hash.h"
#include "ota_common.h"

#define OTA_MAGIC           "IPRO_OTA_Ver1.0"
#define OTA_TYPE_OFFSET     16
#define OTA_PAYLEN_OFFSET   20
#define OTA_SHA256_OFFSET   64

static uint8_t *s_flash_buf;     /* OTA_FLASH_BUF, malloc'd per command */
static uint8_t *s_hdr;           /* OTA_HEADER_SIZE                     */
/* HW SHA256 (SEC engine). The cacheable handle is fine: hal_sec manages the
 * engine's working-buffer coherency internally (non-cacheable alias + clean),
 * and sec_hash_update() cleans the input dcache itself. */
static sec_hash_handle_t s_sha __attribute__((aligned(32)));
static uint32_t s_fw_b_addr;
static uint32_t s_fw_b_max;
static uint32_t s_flash_off;     /* next flash write offset (rel to FW-B) */
static uint32_t s_buf_len;       /* bytes pending in s_flash_buf          */
static uint32_t s_recv_off;      /* total bytes received                 */
static bool     s_overflow;      /* package larger than FW-B slot         */

int ota_sink_begin(uint32_t fw_b_addr, uint32_t fw_b_max)
{
    s_flash_buf = malloc(OTA_FLASH_BUF);
    s_hdr       = malloc(OTA_HEADER_SIZE);
    if (!s_flash_buf || !s_hdr) {
        ota_sink_end();
        return -1;
    }
    s_fw_b_addr = fw_b_addr;
    s_fw_b_max  = fw_b_max;
    s_flash_off = 0;
    s_buf_len   = 0;
    s_recv_off  = 0;
    s_overflow  = false;
    memset(s_hdr, 0, OTA_HEADER_SIZE);
    sec_hash_init_ex(&s_sha, SEC_HASH_MODE_DIRECT, SEC_HASH_SHA256);
    return 0;
}

void ota_sink_end(void)
{
    free(s_flash_buf); s_flash_buf = NULL;
    free(s_hdr);       s_hdr       = NULL;
}

int ota_sink(const uint8_t *data, uint32_t n)
{
    /* capture OTA header (first 512 B) */
    if (s_recv_off < OTA_HEADER_SIZE) {
        uint32_t hn = OTA_HEADER_SIZE - s_recv_off;
        if (hn > n)
            hn = n;
        memcpy(s_hdr + s_recv_off, data, hn);
    }

    /* hash the payload portion (file offset >= 512) */
    uint32_t pstart = 0;
    if (s_recv_off < OTA_HEADER_SIZE) {
        uint32_t hdr_in_blk = OTA_HEADER_SIZE - s_recv_off;
        pstart = (hdr_in_blk < n) ? hdr_in_blk : n;
    }
    if (pstart < n)
        sec_hash_update(&s_sha, data + pstart, n - pstart);  /* hal cleans input dcache */

    /* stream the whole package (header + payload) to FW-B in 4KB chunks */
    if (s_recv_off + n > s_fw_b_max) {
        s_overflow = true;
        return -1;
    }
    uint32_t off = 0;
    while (off < n) {
        uint32_t space = OTA_FLASH_BUF - s_buf_len;
        uint32_t chunk = (n - off < space) ? (n - off) : space;
        memcpy(s_flash_buf + s_buf_len, data + off, chunk);
        s_buf_len += chunk;
        off       += chunk;
        if (s_buf_len == OTA_FLASH_BUF) {
            if (flash_write(s_fw_b_addr + s_flash_off, s_flash_buf, s_buf_len) != SUCCESS)
                return -1;
            s_flash_off += s_buf_len;
            s_buf_len    = 0;
        }
    }
    s_recv_off += n;
    return 0;
}

int ota_sink_flush(void)
{
    if (s_buf_len) {
        if (flash_write(s_fw_b_addr + s_flash_off, s_flash_buf, s_buf_len) != SUCCESS)
            return -1;
        s_flash_off += s_buf_len;
        s_buf_len    = 0;
    }
    return 0;
}

uint32_t ota_recv_len(void) { return s_recv_off; }

uint32_t ota_total_len(void)
{
    uint32_t payload_len;

    if (s_recv_off < OTA_HEADER_SIZE)
        return 0;
    memcpy(&payload_len, &s_hdr[OTA_PAYLEN_OFFSET], 4);
    return OTA_HEADER_SIZE + payload_len;
}

bool ota_overflowed(void) { return s_overflow; }

int ota_validate(void)
{
    uint8_t  sha[32];
    uint32_t payload_len;

    sec_hash_finish(&s_sha, sha);
    sec_hash_deinit(&s_sha);

    if (s_recv_off < OTA_HEADER_SIZE)
        return -1;
    if (memcmp(s_hdr, OTA_MAGIC, sizeof(OTA_MAGIC) - 1) != 0)
        return -2;
    if (memcmp(&s_hdr[OTA_TYPE_OFFSET], "XZ  ", 4) != 0)
        return -3;
    memcpy(&payload_len, &s_hdr[OTA_PAYLEN_OFFSET], 4);
    if (payload_len == 0 || (s_recv_off - OTA_HEADER_SIZE) != payload_len)
        return -4;
    if (memcmp(sha, &s_hdr[OTA_SHA256_OFFSET], 32) != 0)
        return -5;
    return 0;
}

/* ---- partition table helpers ----
 * Resolve the FW entry through the hal_boot2 runtime partition table
 * (ipro_partition_table), NOT a direct flash_read at the compile-time
 * PT_TABLE0_ADDRESS. On IPRO7 that constant is a 0xE000 legacy fallback — the
 * real PT lives at the address stored in the boot2 bootheader (0x10000 for the
 * u-boot layout), so reading 0xE000 directly returned garbage and OTA failed
 * with "no FW-B slot" even though the A/B table was active. hal_boot2 resolves
 * the correct address on every chip (ipro6 + ipro7). */

int ota_get_fw_b(uint32_t *addr, uint32_t *max_len)
{
    PtTable_Entry_Config e;

    if (hal_boot2_get_active_entries(PT_ENTRY_FW_CPU0, &e) != 0)
        return -1;
    if (e.Address[1] == 0 || e.maxLen[1] == 0)
        return -1;   /* single-slot table — no FW-B staging slot */
    *addr    = e.Address[1];
    *max_len = e.maxLen[1];
    return 0;
}

int ota_mark_fwb_active(void)
{
    PtTable_Entry_Config e;

    if (hal_boot2_get_active_entries(PT_ENTRY_FW_CPU0, &e) != 0)
        return -1;
    /* hal_boot2_update_ptable flips activeIndex (0 -> 1), bumps age and writes
     * the PT back, so boot2 applies FW-B on the next boot. */
    return (hal_boot2_update_ptable(&e) == 0) ? 0 : -1;
}
