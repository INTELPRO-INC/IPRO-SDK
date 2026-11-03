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
 * SPE firmware-update staging service. FW-B and the partition tables
 * are reached with the hal_flash calls; the eFuse counter and the
 * boot2 result word are host-test stand-ins under SPM_HOST.
 */
#include <string.h>

#include "hal_flash.h"
#include "hal_ptmgr.h"
#include "utils_crc.h"
#include "psa_ipro/fwu_rpc.h"
#include "spm_log.h"
#include "svc_fwu.h"

#ifdef SPM_HOST
/* Stand-ins defined by the host test. */
uint32_t fwu_hw_svn_floor(void);
uint32_t fwu_hw_take_ota_word(void);
#else
#include "drv_ef_ctrl.h"

/* HBN RAM sysinfo word 6, written by boot2 when it ran an OTA */
#define FWU_HBN_OTA_RES (*(volatile uint32_t *)(0x20090600u + 24u))
/* boot2_anti_rollback_version, a thermometer code */
#define FWU_EF_SVN_WORD (0x4Cu / 4u)

uint32_t fwu_hw_svn_floor(void)
{
    uint32_t v = 0;

    EF_Ctrl_Read_Direct_R0(FWU_EF_SVN_WORD, &v, 1);
    return v != 0 ? 32u - (uint32_t)__builtin_clz(v) : 0u;
}

/* boot2 OTA result word; reads 0 afterwards */
uint32_t fwu_hw_take_ota_word(void)
{
    uint32_t v = FWU_HBN_OTA_RES;

    FWU_HBN_OTA_RES = 0;
    return v;
}
#endif

/* FW entry of the table boot2 reads, with FW-B */
struct fwu_slot {
    PtTable_ID_Type id;
    PtTable_Entry_Config *fw;
    uint32_t base;
    uint32_t size;
};

static struct {
    uint32_t staged;
    uint32_t last_ota;
    uint32_t running_svn;
} fwu;

/* PT0 and PT1 as read by fwu_slot, with their flash addresses */
static PtTable_Stuff_Config fwu_pt[2];
static uint32_t fwu_pt_addr[2];

/* Table bytes: header, entryCnt entries, entries CRC32 */
static uint32_t fwu_pt_len(const PtTable_Stuff_Config *t)
{
    return (uint32_t)(sizeof(PtTable_Config) + sizeof(uint32_t) +
                      t->ptTable.entryCnt * sizeof(PtTable_Entry_Config));
}

/* 1 when the entries CRC32 after the entries is the stored one */
static int fwu_pt_entries_ok(const PtTable_Stuff_Config *t)
{
    uint32_t n = t->ptTable.entryCnt * sizeof(PtTable_Entry_Config);
    const uint8_t *e = (const uint8_t *)t->ptEntries;
    uint32_t crc;

    memcpy(&crc, e + n, sizeof(crc));
    return utils_crc32(e, n) == crc;
}

/* boot2_validate_pt: magic, 1..PT_ENTRY_MAX entries, both CRC32s */
static int fwu_pt_read(uint32_t addr, PtTable_Stuff_Config *t)
{
    PtTable_Config *h = &t->ptTable;

    memset(t, 0, sizeof(*t));
    return flash_read(addr, (uint8_t *)h, sizeof(*h)) == SUCCESS &&
           h->magicCode == PT_MAGIC_CODE && h->entryCnt != 0 &&
           h->entryCnt <= PT_ENTRY_MAX &&
           utils_crc32(h, sizeof(*h) - sizeof(uint32_t)) == h->crc32 &&
           flash_read(addr + sizeof(*h), (uint8_t *)t->ptEntries,
                      fwu_pt_len(t) - sizeof(*h)) == SUCCESS &&
           fwu_pt_entries_ok(t);
}

/*
 * boot2_get_active_pt: addresses from bootheader words 164 and 168,
 * the valid copy of higher age, PT0 on a tie.
 */
static PtTable_ID_Type fwu_pt_active(void)
{
    int v[2];

    if (flash_read(164, (uint8_t *)&fwu_pt_addr[0], 4) != SUCCESS ||
        flash_read(168, (uint8_t *)&fwu_pt_addr[1], 4) != SUCCESS) {
        return PT_TABLE_ID_INVALID;
    }
    v[0] = fwu_pt_read(fwu_pt_addr[0], &fwu_pt[0]);
    v[1] = fwu_pt_read(fwu_pt_addr[1], &fwu_pt[1]);
    if (v[0] && v[1]) {
        return fwu_pt[0].ptTable.age >= fwu_pt[1].ptTable.age
                   ? PT_TABLE_ID_0 : PT_TABLE_ID_1;
    }
    return v[0] ? PT_TABLE_ID_0 : v[1] ? PT_TABLE_ID_1 : PT_TABLE_ID_INVALID;
}

/* 1 when [a, a + an) and [b, b + bn) share a byte */
static int fwu_overlap(uint32_t a, uint32_t an, uint32_t b, uint32_t bn)
{
    return a < b + bn && b < a + an;
}

/*
 * Active table and FW-B. STORAGE_FAILURE without a valid table or FW
 * entry; NOT_SUPPORTED when FW-B is missing, or FW-B or the other
 * table is unaligned or overlaps FW-A or each other.
 */
static psa_status_t fwu_slot(struct fwu_slot *s)
{
    const PtTable_Entry_Config *fw;
    uint32_t other;

    s->id = fwu_pt_active();
    s->fw = NULL;
    for (uint32_t i = 0; s->id != PT_TABLE_ID_INVALID &&
                         i < fwu_pt[s->id].ptTable.entryCnt; i++) {
        if (fwu_pt[s->id].ptEntries[i].type == PT_ENTRY_FW_CPU0) {
            s->fw = &fwu_pt[s->id].ptEntries[i];
            break;
        }
    }
    if (s->fw == NULL) {
        return PSA_ERROR_STORAGE_FAILURE;
    }
    fw = s->fw;
    s->base = fw->Address[1];
    s->size = fw->maxLen[1];
    other = fwu_pt_addr[s->id == PT_TABLE_ID_0 ? 1 : 0];
    if (s->base == 0 || s->size < FWU_HDR_SIZE ||
        s->base % FWU_SECTOR != 0 || s->size % FWU_SECTOR != 0 ||
        s->size > UINT32_MAX - s->base || other % FWU_SECTOR != 0 ||
        fw->maxLen[0] > UINT32_MAX - fw->Address[0] ||
        fwu_overlap(s->base, s->size, fw->Address[0], fw->maxLen[0]) ||
        fwu_overlap(other, FWU_SECTOR, fw->Address[0], fw->maxLen[0]) ||
        fwu_overlap(other, FWU_SECTOR, s->base, s->size)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    return PSA_SUCCESS;
}

static uint32_t fwu_le32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 |
           (uint32_t)p[3] << 24;
}

/* Magic and type of an OTA package header */
static int fwu_hdr_kind_ok(const uint8_t h[FWU_HDR_CHECK])
{
    return memcmp(h, FWU_HDR_MAGIC, sizeof(FWU_HDR_MAGIC) - 1u) == 0 &&
           memcmp(&h[FWU_HDR_TYPE_OFF], FWU_HDR_TYPE,
                  sizeof(FWU_HDR_TYPE) - 1u) == 0;
}

/* As boot2 reads it: the legacy filler is SVN 0 */
static uint32_t fwu_hdr_svn(const uint8_t h[FWU_HDR_CHECK])
{
    if (memcmp(&h[FWU_HDR_SVN_OFF], FWU_HDR_SVN_LEGACY,
               sizeof(FWU_HDR_SVN_LEGACY) - 1u) == 0) {
        return 0;
    }
    return fwu_le32(&h[FWU_HDR_SVN_OFF]);
}

/* SVN of the package header in FW-B, FWU_SVN_NONE without one */
static uint32_t fwu_slot_svn(const struct fwu_slot *s)
{
    uint8_t h[FWU_HDR_CHECK];

    if (flash_read(s->base, h, sizeof(h)) != SUCCESS ||
        !fwu_hdr_kind_ok(h)) {
        return FWU_SVN_NONE;
    }
    return fwu_hdr_svn(h);
}

/* 1 when the n bytes at flash addr equal p */
static int fwu_verify(uint32_t addr, const uint8_t *p, uint32_t n)
{
    uint8_t buf[256];

    for (uint32_t off = 0; off < n; off += sizeof(buf)) {
        uint32_t k = n - off < sizeof(buf) ? n - off : sizeof(buf);

        if (flash_read(addr + off, buf, k) != SUCCESS ||
            memcmp(buf, p + off, k) != 0) {
            return 0;
        }
    }
    return 1;
}

/*
 * Sectors that start inside the chunk are erased first; with writes
 * in order from offset 0 that covers every byte written.
 */
static psa_status_t fwu_write(const psa_invec *in, psa_outvec *out)
{
    const struct fwu_req *r = in[0].base;
    const uint8_t *data = in[1].base;
    uint32_t n = (uint32_t)in[1].len;
    struct fwu_slot s;
    psa_status_t st = fwu_slot(&s);

    (void)out;
    if (st != PSA_SUCCESS) {
        return st;
    }
    if (n == 0 || n > FWU_WRITE_MAX || r->len != n ||
        r->offset > s.size || n > s.size - r->offset) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (s.fw->activeIndex != 0 ||
        (r->offset != 0 && r->offset != fwu.staged)) {
        return PSA_ERROR_BAD_STATE;
    }
    fwu.staged = 0;
    for (uint32_t sec = (r->offset + FWU_SECTOR - 1u) & ~(FWU_SECTOR - 1u);
         sec < r->offset + n; sec += FWU_SECTOR) {
        if (flash_erase(s.base + sec, FWU_SECTOR) != SUCCESS) {
            return PSA_ERROR_STORAGE_FAILURE;
        }
    }
    if (flash_write(s.base + r->offset, (uint8_t *)(uintptr_t)data, n) !=
            SUCCESS ||
        !fwu_verify(s.base + r->offset, data, n)) {
        return PSA_ERROR_STORAGE_FAILURE;
    }
    fwu.staged = r->offset + n;
    return PSA_SUCCESS;
}

/* Format checks of boot2 before it authenticates the package */
static int fwu_hdr_ok(const uint8_t h[FWU_HDR_CHECK], uint32_t len,
                      uint32_t slot_size)
{
    uint32_t payload = fwu_le32(&h[FWU_HDR_LEN_OFF]);

    return fwu_hdr_kind_ok(h) && payload != 0 &&
           payload <= slot_size - FWU_HDR_SIZE &&
           payload + FWU_HDR_SIZE == len;
}

/*
 * Erases the sector at addr and writes the table bytes of t with
 * both CRC32s computed; 1 when they read back.
 */
static int fwu_pt_write(uint32_t addr, PtTable_Stuff_Config *t)
{
    uint32_t n = t->ptTable.entryCnt * sizeof(PtTable_Entry_Config);
    uint8_t *e = (uint8_t *)t->ptEntries;
    uint32_t crc = utils_crc32(e, n);

    t->ptTable.crc32 = utils_crc32(&t->ptTable,
                                   sizeof(PtTable_Config) - sizeof(uint32_t));
    memcpy(e + n, &crc, sizeof(crc));
    return flash_erase(addr, FWU_SECTOR) == SUCCESS &&
           flash_write(addr, (uint8_t *)t, fwu_pt_len(t)) == SUCCESS &&
           fwu_verify(addr, (const uint8_t *)t, fwu_pt_len(t));
}

/*
 * The active table with FW activeindex 1, FW age + 1 and table age
 * + 1 goes to the other copy; boot2 must then read it, pending.
 */
static psa_status_t fwu_install(const psa_invec *in, psa_outvec *out)
{
    const struct fwu_req *r = in[0].base;
    uint8_t h[FWU_HDR_CHECK];
    struct fwu_slot s;
    PtTable_ID_Type to;
    psa_status_t st = fwu_slot(&s);

    (void)out;
    if (st != PSA_SUCCESS) {
        return st;
    }
    if (s.fw->activeIndex != 0 || fwu.staged == 0) {
        return PSA_ERROR_BAD_STATE;
    }
    if (r->offset != 0 || r->len != fwu.staged) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (flash_read(s.base, h, sizeof(h)) != SUCCESS) {
        return PSA_ERROR_STORAGE_FAILURE;
    }
    if (!fwu_hdr_ok(h, r->len, s.size)) {
        SPM_LOG("FWU package header refused");
        return PSA_ERROR_DATA_INVALID;
    }
    to = s.id == PT_TABLE_ID_0 ? PT_TABLE_ID_1 : PT_TABLE_ID_0;
    s.fw->activeIndex = 1;
    s.fw->age++;
    fwu_pt[s.id].ptTable.age++;
    if (!fwu_pt_write(fwu_pt_addr[to], &fwu_pt[s.id]) ||
        fwu_slot(&s) != PSA_SUCCESS || s.id != to ||
        s.fw->activeIndex != 1) {
        return PSA_ERROR_STORAGE_FAILURE;
    }
    SPM_LOG("FWU %lu B svn %lu pending, PT%u age %lu",
            (unsigned long)r->len, (unsigned long)fwu_hdr_svn(h),
            (unsigned)s.id, (unsigned long)fwu_pt[s.id].ptTable.age);
    return PSA_SUCCESS;
}

static psa_status_t fwu_query(const psa_invec *in, psa_outvec *out)
{
    struct fwu_status q;
    struct fwu_slot s;
    psa_status_t st = fwu_slot(&s);

    (void)in;
    if (st != PSA_SUCCESS) {
        return st;
    }
    memset(&q, 0, sizeof(q));
    q.state = s.fw->activeIndex != 0 ? FWU_STATE_PENDING
              : fwu.staged != 0     ? FWU_STATE_WRITING
                                    : FWU_STATE_IDLE;
    q.fw_active = s.fw->activeIndex;
    q.pt_active = (uint32_t)s.id;
    q.pt_age = fwu_pt[s.id].ptTable.age;
    q.staged_len = fwu.staged;
    q.slot_size = s.size;
    q.staged_svn = fwu_slot_svn(&s);
    q.svn_floor = fwu_hw_svn_floor();
    q.running_svn = fwu.running_svn;
    q.last_ota = fwu.last_ota;
    memcpy(out[0].base, &q, sizeof(q));
    out[0].len = sizeof(q);
    return PSA_SUCCESS;
}

/* Vector shape of each fn: counts, then exact in[0] / out[0] size */
static const struct {
    uint8_t in_len;
    uint8_t out_len;
    uint8_t in0;
    uint8_t out0;
    psa_status_t (*run)(const psa_invec *in, psa_outvec *out);
} fwu_fns[FWU_FN_QUERY + 1] = {
    [FWU_FN_WRITE] = { 2, 0, sizeof(struct fwu_req), 0, fwu_write },
    [FWU_FN_INSTALL] = { 1, 0, sizeof(struct fwu_req), 0, fwu_install },
    [FWU_FN_QUERY] = { 0, 1, 0, sizeof(struct fwu_status), fwu_query },
};

psa_status_t svc_fwu_call(int32_t type, const psa_invec *in,
                          size_t in_len, psa_outvec *out, size_t out_len)
{
    psa_status_t st = PSA_ERROR_NOT_SUPPORTED;

    if (type >= FWU_FN_WRITE && type <= FWU_FN_QUERY) {
        st = in_len == fwu_fns[type].in_len &&
             out_len == fwu_fns[type].out_len &&
             (in_len == 0 || in[0].len == fwu_fns[type].in0) &&
             (out_len == 0 || out[0].len == fwu_fns[type].out0)
                 ? PSA_SUCCESS : PSA_ERROR_INVALID_ARGUMENT;
    }
    for (size_t i = 0; i < out_len; i++) {
        out[i].len = 0;
    }
    if (st == PSA_SUCCESS) {
        st = fwu_fns[type].run(in, out);
    }
    return st;
}

void svc_fwu_init(void)
{
    struct fwu_slot s;
    uint32_t v = fwu_hw_take_ota_word();

    memset(&fwu, 0, sizeof(fwu));
    fwu.last_ota = (v & 0xFFFF0000u) == FWU_OTA_RES_MAGIC ? v & 0xFFu
                                                          : FWU_OTA_NONE;
    fwu.running_svn = FWU_SVN_NONE;
    if (fwu.last_ota == FWU_OTA_INSTALLED && fwu_slot(&s) == PSA_SUCCESS) {
        fwu.running_svn = fwu_slot_svn(&s);
    }
}

void svc_fwu_client_reset(void)
{
    fwu.staged = 0;
}
