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

#include <stdlib.h>
#include <string.h>

#include "drv_glb.h"
#include "drv_romdriver.h"
#include "drv_sf_ctrl.h"
#include "hal_ptmgr.h"
#include "mbedtls/sha256.h"

#include <rnm_ota.h>
#include "rnm_ota_flash.h"

/* BROM handoff in OCRAM: "BPRM", total, fcfg len, "FCFG", cfg. */
#define BROM_PARAM_ADDR    0x2104FE80U
#define BROM_PARAM_MAGIC   0x4D525042U
#define BROM_FCFG_MAGIC    0x47464346U
#define FLASH_PAGE         256U
#define SHA_BUF            1024U

/* boot2 OTA result word in HBN RAM (sysinfo reserved[1]): "OR" in [31:16],
 * reason in [7:0], 0 = installed. Written by boot2, cleared here. */
#define BOOT2_OTA_RES_ADDR  0x20090618U
#define BOOT2_OTA_RES_MAGIC 0x4F520000U

static SPI_Flash_Cfg_Type s_cfg;
static uint8_t s_boot2_res;
static uint8_t s_cfg_ok;
static const char *s_cfg_src = "none";

void rnm_ota_capture_flash_cfg(void)
{
    const uint8_t *p = (const uint8_t *)BROM_PARAM_ADDR;
    uint32_t magic;
    uint32_t fmagic;
    uint32_t flags;

    memcpy(&magic, p, sizeof(magic));
    memcpy(&fmagic, p + 12, sizeof(fmagic));
    if (magic == BROM_PARAM_MAGIC && fmagic == BROM_FCFG_MAGIC) {
        memcpy(&s_cfg, p + 16, sizeof(s_cfg));
        s_cfg_src = "brom";
    } else {
        flags = __irq_save();
        if (RomDriver_SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(0, &s_cfg) == SUCCESS) {
            s_cfg_src = "rom";
        }
        __irq_restore(flags);
    }
    s_cfg_ok = (s_cfg.pageSize == FLASH_PAGE && s_cfg.sectorSize == 4);
    if (!s_cfg_ok) {
        s_cfg_src = "none";
    }
}

const char *rnm_ota_flash_cfg_source(void)
{
    return s_cfg_src;
}

static int rom_read(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint32_t flags = __irq_save();
    int ret = RomDriver_XIP_SFlash_Read_Need_Lock(&s_cfg, addr, data, len, 0,
                                                  SF_CTRL_FLASH_BANK0);

    __irq_restore(flags);
    return ret;
}

/* One ROM call per 4K sector, IRQs off for that sector only. */
static int rom_erase(uint32_t addr, uint32_t len)
{
    uint32_t end = addr + len;
    int ret = SUCCESS;

    for (addr &= ~(RNM_OTA_SECTOR - 1U); addr < end && ret == SUCCESS;
         addr += RNM_OTA_SECTOR) {
        uint32_t flags = __irq_save();

        ret = RomDriver_XIP_SFlash_Erase_Need_Lock(&s_cfg, addr, RNM_OTA_SECTOR,
                                                   0, SF_CTRL_FLASH_BANK0);
        __irq_restore(flags);
    }
    return ret;
}

/* One ROM call per flash page, IRQs off for that page only. */
static int rom_write(uint32_t addr, uint8_t *data, uint32_t len)
{
    int ret = SUCCESS;

    while (len != 0 && ret == SUCCESS) {
        uint32_t n = FLASH_PAGE - (addr % FLASH_PAGE);
        uint32_t flags;

        if (n > len) {
            n = len;
        }
        flags = __irq_save();
        ret = RomDriver_XIP_SFlash_Write_Need_Lock(&s_cfg, addr, data, n, 0,
                                                   SF_CTRL_FLASH_BANK0);
        __irq_restore(flags);
        addr += n;
        data += n;
        len -= n;
    }
    return ret;
}

static const PtTable_Flash_Ops s_pt_ops = {
    rom_read, rom_erase, rom_write,
};

/* FW entry of the active PT copy; 0 on success. */
static int fw_entry_get(PtTable_Entry_Config *e)
{
    PtTable_Stuff_Config *pt;
    PtTable_ID_Type id;
    int ret = -1;

    if (!s_cfg_ok) {
        return -1;
    }
    pt = malloc(2 * sizeof(*pt));
    if (pt == NULL) {
        return -1;
    }
    id = PtTable_Get_Active_Partition_Ops(&s_pt_ops, pt);
    if (id != PT_TABLE_ID_INVALID &&
        PtTable_Get_Active_Entries(&pt[id], PT_ENTRY_FW_CPU0, e) ==
            PT_ERROR_SUCCESS) {
        ret = 0;
    }
    free(pt);
    return ret;
}

uint8_t rnm_ota_capture_boot2_result(void)
{
    volatile uint32_t *w = (volatile uint32_t *)BOOT2_OTA_RES_ADDR;
    uint32_t v = *w;

    s_boot2_res = 0;
    if ((v & 0xFFFF0000U) == BOOT2_OTA_RES_MAGIC) {
        s_boot2_res = (uint8_t)(v & 0xFFU);
        *w = 0;
    }
    return s_boot2_res;
}

uint8_t rnm_ota_boot2_result(void)
{
    return s_boot2_res;
}

int rnm_ota_flash_pending(void)
{
    PtTable_Entry_Config e;

    return (fw_entry_get(&e) == 0) ? (int)e.activeIndex : -1;
}

static int st_slot_open(void *ctx, uint32_t *addr, uint32_t *max_len)
{
    PtTable_Entry_Config e;

    (void)ctx;
    if (fw_entry_get(&e) != 0 || e.Address[1] == 0 || e.maxLen[1] == 0) {
        return -1;
    }
    *addr = e.Address[1];
    *max_len = e.maxLen[1];
    return 0;
}

static int st_erase(void *ctx, uint32_t addr, uint32_t len)
{
    (void)ctx;
    return (rom_erase(addr, len) == SUCCESS) ? 0 : -1;
}

static int st_write(void *ctx, uint32_t addr, const uint8_t *data, uint32_t len)
{
    (void)ctx;
    return (rom_write(addr, (uint8_t *)data, len) == SUCCESS) ? 0 : -1;
}

static int st_read(void *ctx, uint32_t addr, uint8_t *data, uint32_t len)
{
    (void)ctx;
    return (rom_read(addr, data, len) == SUCCESS) ? 0 : -1;
}

static int st_sha256(void *ctx, uint32_t addr, uint32_t len, uint8_t out[32])
{
    mbedtls_sha256_context sha;
    uint8_t *buf;
    int ret = 0;

    (void)ctx;
    buf = malloc(SHA_BUF);
    if (buf == NULL) {
        return -1;
    }
    mbedtls_sha256_init(&sha);
    mbedtls_sha256_starts_ret(&sha, 0);
    while (len != 0) {
        uint32_t n = (len < SHA_BUF) ? len : SHA_BUF;

        if (rom_read(addr, buf, n) != SUCCESS) {
            ret = -1;
            break;
        }
        mbedtls_sha256_update_ret(&sha, buf, n);
        addr += n;
        len -= n;
    }
    if (ret == 0) {
        mbedtls_sha256_finish_ret(&sha, out);
    }
    mbedtls_sha256_free(&sha);
    free(buf);
    return ret;
}

/* FW activeIndex 1 into the inactive PT copy, then read back. */
static int st_commit(void *ctx)
{
    PtTable_Stuff_Config *pt;
    PtTable_Entry_Config e;
    PtTable_ID_Type id;
    PtTable_ID_Type target;
    int ret = -1;

    (void)ctx;
    pt = malloc(2 * sizeof(*pt));
    if (pt == NULL) {
        return -1;
    }
    id = PtTable_Get_Active_Partition_Ops(&s_pt_ops, pt);
    if (id == PT_TABLE_ID_INVALID ||
        PtTable_Get_Active_Entries(&pt[id], PT_ENTRY_FW_CPU0, &e) !=
            PT_ERROR_SUCCESS) {
        goto out;
    }
    target = (id == PT_TABLE_ID_0) ? PT_TABLE_ID_1 : PT_TABLE_ID_0;
    e.activeIndex = 1;
    e.age++;
    if (PtTable_Update_Entry_Ops(&s_pt_ops, target, &pt[id], &e) !=
        PT_ERROR_SUCCESS) {
        goto out;
    }
    if (PtTable_Get_Active_Partition_Ops(&s_pt_ops, pt) == target &&
        PtTable_Get_Active_Entries(&pt[target], PT_ENTRY_FW_CPU0, &e) ==
            PT_ERROR_SUCCESS &&
        e.activeIndex == 1) {
        ret = 0;
    }
out:
    free(pt);
    return ret;
}

static const rnm_ota_store_t s_store = {
    st_slot_open, st_erase, st_write, st_read, st_sha256, st_commit,
};

const rnm_ota_store_t *rnm_ota_flash_store(void)
{
    return &s_store;
}
