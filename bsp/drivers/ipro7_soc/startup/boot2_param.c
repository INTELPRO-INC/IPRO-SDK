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

/**
 * @file boot2_param.c
 * @brief Boot2 parameter handling implementation for IPRO7
 */

#include "boot2_param.h"
#include "hal_hbnram_mgnt.h"
#include "utils_crc.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Saved boot2 parameters (in .noinit section to survive warm reset)
 */
static Boot2_Param_Type g_boot2_param __attribute__((section(".noinit_data")));

/**
 * @brief Choose boot2 param source for this boot
 *
 * Normal cold boot: bootrom deposited boot2 params at BOOT2_PARAM_SRC_ADDR
 * (0x11010000) before jumping to FW. Validate magic inline.
 *
 * LP fastboot (HBN_0) wake: OCRAM was power-cycled, 0x11010000 now holds FW
 * .data garbage. Before entering HBN we snapshot the raw bytes to HBN RAM
 * (HAL_HBNRAM_BOOT2_ADDR). If we detect that snapshot's magic here, use it.
 *
 * @return Pointer to raw boot2 source bytes (length guaranteed ≥ HAL_HBNRAM_BOOT2_DATA_SIZE)
 */
static uint8_t *Boot2_Param_Choose_Source(void)
{
    /* Primary: OCRAM non-cache alias at BOOT2_PARAM_SRC_ADDR — peek PtTable magic at offset 8
     * (ptTableLen 4B + activeID 4B + PtTableConfig.magicCode 4B). */
    uint8_t *primary = (uint8_t *)BOOT2_PARAM_SRC_ADDR;
    uint32_t primary_magic;
    memcpy(&primary_magic, primary + 8, 4);
    if (primary_magic == IPRO_PT_MAGIC_CODE) {
        return primary;
    }

    /* Secondary: cacheable alias kept only for legacy/bootloader paths that
     * intentionally reconstruct params there before caches are enabled. */
    uint8_t *cache = (uint8_t *)BOOT2_PARAM_SRC_CACHE_ADDR;
    uint32_t cache_magic;
    memcpy(&cache_magic, cache + 8, 4);
    if (cache_magic == IPRO_PT_MAGIC_CODE) {
        return cache;
    }

    /* Fallback: HBN RAM snapshot written by hal_lp_enter_fastboot().
     * Layout: [0..3] HAL_HBNRAM_BOOT2_MAGIC ("BOT2") then raw payload. */
    uint32_t *hbn_hdr = (uint32_t *)HAL_HBNRAM_BOOT2_ADDR;
    if (hbn_hdr[0] == HAL_HBNRAM_BOOT2_MAGIC) {
        return (uint8_t *)(HAL_HBNRAM_BOOT2_ADDR + 4);
    }

    /* No valid source — let caller fail validation */
    return primary;
}

/**
 * @brief Save boot2 parameters from source address
 */
void Boot2_Param_Save(void)
{
    uint8_t *pSrc = Boot2_Param_Choose_Source();
    uint32_t ptLen, flashLen;
    uint32_t offset = 0;
    uint32_t entryCnt;
    uint32_t table_crc;
    uint32_t entries_crc;

    /* Clear the structure */
    memset(&g_boot2_param, 0, sizeof(g_boot2_param));

    /* Read PT table length */
    memcpy(&ptLen, pSrc + offset, 4);
    offset += 4;
    g_boot2_param.ptTableLen = ptLen;

    /* Sanity check */
    if (ptLen == 0 || ptLen > 512) {
        g_boot2_param.valid = 0;
        return;
    }

    /* Read active ID */
    memcpy(&g_boot2_param.activeID, pSrc + offset, 4);
    offset += 4;

    /* Read PT table config */
    memcpy(&g_boot2_param.ptTable, pSrc + offset, sizeof(Boot2_PtTable_Config));
    offset += sizeof(Boot2_PtTable_Config);

    /* Validate magic code */
    if (g_boot2_param.ptTable.magicCode != IPRO_PT_MAGIC_CODE) {
        g_boot2_param.valid = 0;
        return;
    }
    table_crc = utils_crc32((uint8_t *)&g_boot2_param.ptTable,
                            sizeof(g_boot2_param.ptTable) - sizeof(g_boot2_param.ptTable.crc32));
    if (table_crc != g_boot2_param.ptTable.crc32) {
        g_boot2_param.valid = 0;
        return;
    }

    /* Read PT entries */
    entryCnt = g_boot2_param.ptTable.entryCnt;
    if ((entryCnt == 0) || (entryCnt > PT_ENTRY_MAX)) {
        g_boot2_param.valid = 0;
        return;
    }

    if (entryCnt > 0) {
        memcpy(g_boot2_param.ptEntries, pSrc + offset,
               entryCnt * sizeof(Boot2_PtTable_Entry));
        offset += entryCnt * sizeof(Boot2_PtTable_Entry);
    }

    /* Read PT entries CRC32 */
    memcpy(&g_boot2_param.ptEntriesCrc32, pSrc + offset, 4);
    offset += 4;
    entries_crc = utils_crc32((uint8_t *)g_boot2_param.ptEntries,
                              entryCnt * sizeof(Boot2_PtTable_Entry));
    if (entries_crc != g_boot2_param.ptEntriesCrc32) {
        g_boot2_param.valid = 0;
        return;
    }

    /* Read flash config length */
    memcpy(&flashLen, pSrc + offset, 4);
    offset += 4;
    g_boot2_param.flashCfgLen = flashLen;

    /* Sanity check */
    if (flashLen == 0 || flashLen > 256) {
        /* Flash config not provided or invalid, but PT table is valid */
        g_boot2_param.valid = 1;
        return;
    }

    /* Read flash config magic */
    memcpy(&g_boot2_param.flashCfgMagic, pSrc + offset, 4);
    offset += 4;

    /* Validate flash config magic */
    if (g_boot2_param.flashCfgMagic != IPRO_BOOT2_FLASH_CFG_MAGIC) {
        /* Flash config magic invalid, but PT table is still valid */
        g_boot2_param.flashCfgLen = 0;
        g_boot2_param.valid = 1;
        return;
    }

    /* Read flash config */
    memcpy(&g_boot2_param.flashCfg, pSrc + offset, sizeof(SPI_Flash_Cfg_Type));

    /* Mark as valid */
    g_boot2_param.valid = 1;
}

/**
 * @brief Get pointer to saved boot2 parameters
 */
Boot2_Param_Type *Boot2_Param_Get(void)
{
    uint32_t table_crc;
    uint32_t entries_crc;

    if (g_boot2_param.valid &&
        g_boot2_param.ptTableLen > 0 &&
        g_boot2_param.ptTableLen <= 512 &&
        g_boot2_param.ptTable.magicCode == IPRO_PT_MAGIC_CODE &&
        g_boot2_param.ptTable.entryCnt > 0 &&
        g_boot2_param.ptTable.entryCnt <= PT_ENTRY_MAX) {
        table_crc = utils_crc32((uint8_t *)&g_boot2_param.ptTable,
                                sizeof(g_boot2_param.ptTable) - sizeof(g_boot2_param.ptTable.crc32));
        entries_crc = utils_crc32((uint8_t *)g_boot2_param.ptEntries,
                                  g_boot2_param.ptTable.entryCnt * sizeof(Boot2_PtTable_Entry));
        if ((table_crc != g_boot2_param.ptTable.crc32) ||
            (entries_crc != g_boot2_param.ptEntriesCrc32)) {
            return NULL;
        }
        return &g_boot2_param;
    }
    return NULL;
}

/**
 * @brief Get flash configuration from boot2 parameters
 */
SPI_Flash_Cfg_Type *Boot2_Param_Get_Flash_Cfg(void)
{
    if (g_boot2_param.valid && g_boot2_param.flashCfgLen > 0 &&
        g_boot2_param.flashCfgMagic == IPRO_BOOT2_FLASH_CFG_MAGIC) {
        return &g_boot2_param.flashCfg;
    }
    return NULL;
}

/**
 * @brief Get active partition ID from boot2 parameters
 */
int Boot2_Param_Get_Active_ID(void)
{
    if (g_boot2_param.valid) {
        return (int)g_boot2_param.activeID;
    }
    return -1;
}

/**
 * @brief Check if boot2 parameters are valid
 */
int Boot2_Param_Is_Valid(void)
{
    return g_boot2_param.valid ? 1 : 0;
}

#if defined(CONFIG_IPRO_BOOT2_PARAM_DIAG)
static uint32_t Boot2_Param_Read32(const void *addr)
{
    uint32_t value;

    memcpy(&value, addr, sizeof(value));
    return value;
}

void Boot2_Param_Debug_Dump(void)
{
    const uint8_t *primary = (const uint8_t *)BOOT2_PARAM_SRC_ADDR;
    const uint8_t *cache = (const uint8_t *)BOOT2_PARAM_SRC_CACHE_ADDR;
    const uint8_t *legacy_table = (const uint8_t *)BOOT2_PARAM_LEGACY_TABLE_ADDR;
    const uint32_t *hbn_hdr = (const uint32_t *)HAL_HBNRAM_BOOT2_ADDR;

    printf("[boot2-param] primary@0x%08lx len=%lu active=%lu magic=0x%08lx ent=%lu crc=0x%08lx\r\n",
           (unsigned long)BOOT2_PARAM_SRC_ADDR,
           (unsigned long)Boot2_Param_Read32(primary + 0),
           (unsigned long)Boot2_Param_Read32(primary + 4),
           (unsigned long)Boot2_Param_Read32(primary + 8),
           (unsigned long)((Boot2_Param_Read32(primary + 12) >> 16) & 0xffffu),
           (unsigned long)Boot2_Param_Read32(primary + 20));
    printf("[boot2-param] cache@0x%08lx len=%lu active=%lu magic=0x%08lx ent=%lu crc=0x%08lx\r\n",
           (unsigned long)BOOT2_PARAM_SRC_CACHE_ADDR,
           (unsigned long)Boot2_Param_Read32(cache + 0),
           (unsigned long)Boot2_Param_Read32(cache + 4),
           (unsigned long)Boot2_Param_Read32(cache + 8),
           (unsigned long)((Boot2_Param_Read32(cache + 12) >> 16) & 0xffffu),
           (unsigned long)Boot2_Param_Read32(cache + 20));
    printf("[boot2-param] legacy active@0x%08lx=%lu table@0x%08lx magic=0x%08lx ent=%lu crc=0x%08lx entries_crc=0x%08lx\r\n",
           (unsigned long)BOOT2_PARAM_LEGACY_ACTIVE_ADDR,
           (unsigned long)Boot2_Param_Read32((const void *)BOOT2_PARAM_LEGACY_ACTIVE_ADDR),
           (unsigned long)BOOT2_PARAM_LEGACY_TABLE_ADDR,
           (unsigned long)Boot2_Param_Read32(legacy_table + 0),
           (unsigned long)((Boot2_Param_Read32(legacy_table + 4) >> 16) & 0xffffu),
           (unsigned long)Boot2_Param_Read32(legacy_table + 12),
           (unsigned long)Boot2_Param_Read32(legacy_table + 16 + (16u * sizeof(Boot2_PtTable_Entry))));
    printf("[boot2-param] hbn@%p hdr=0x%08lx len=%lu active=%lu magic=0x%08lx\r\n",
           (const void *)HAL_HBNRAM_BOOT2_ADDR,
           (unsigned long)hbn_hdr[0],
           (unsigned long)Boot2_Param_Read32((const uint8_t *)HAL_HBNRAM_BOOT2_ADDR + 4),
           (unsigned long)Boot2_Param_Read32((const uint8_t *)HAL_HBNRAM_BOOT2_ADDR + 8),
           (unsigned long)Boot2_Param_Read32((const uint8_t *)HAL_HBNRAM_BOOT2_ADDR + 12));
    printf("[boot2-param] saved@%p valid=%u len=%lu active=%lu magic=0x%08lx ent=%u crc=0x%08lx flashLen=%lu flashMagic=0x%08lx\r\n",
           (void *)&g_boot2_param,
           (unsigned int)g_boot2_param.valid,
           (unsigned long)g_boot2_param.ptTableLen,
           (unsigned long)g_boot2_param.activeID,
           (unsigned long)g_boot2_param.ptTable.magicCode,
           (unsigned int)g_boot2_param.ptTable.entryCnt,
           (unsigned long)g_boot2_param.ptTable.crc32,
           (unsigned long)g_boot2_param.flashCfgLen,
           (unsigned long)g_boot2_param.flashCfgMagic);
}
#endif
