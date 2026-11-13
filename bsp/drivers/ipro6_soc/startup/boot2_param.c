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
 * @brief Boot2 parameter handling implementation for IPRO6
 */

#include "boot2_param.h"
#include "utils_crc.h"
#include <string.h>

_Static_assert(sizeof(Boot2_PtTable_Config) == 16, "PT config is 16 B");
_Static_assert(sizeof(Boot2_PtTable_Entry) == 36, "PT entry is 36 B");
_Static_assert(BOOT2_PARAM_RAW_SIZE >=
               24 + PT_ENTRY_MAX * sizeof(Boot2_PtTable_Entry) + 12 +
               sizeof(SPI_Flash_Cfg_Type), "raw copy covers 16 entries");

/* Offset of PtTable_Config.magicCode in the stream */
#define BOOT2_PARAM_MAGIC_OFFSET        8

/*
 * Raw stream copied by Boot2_Param_Save() before .data/.bss init.
 * NOLOAD and never zeroed; the ld asserts it and the startup stack
 * stay clear of the source bytes.
 */
static uint32_t g_boot2_raw[BOOT2_PARAM_RAW_SIZE / 4]
    __attribute__((section(".nocache_noinit_ram"), aligned(8)));

static Boot2_Param_Type g_boot2_param;
static uint8_t g_boot2_parsed;

/*
 * Runs from startup before .data/.bss init: no libc (memcpy may be a
 * TCM ramfunc that is not copied yet), no globals outside noinit.
 */
void Boot2_Param_Save(void)
{
    volatile uint32_t *src = (volatile uint32_t *)BOOT2_PARAM_SRC_ADDR;
    volatile uint32_t *dst = (volatile uint32_t *)g_boot2_raw;
    uint32_t i;

    for (i = 0; i < BOOT2_PARAM_RAW_SIZE / 4; i++) {
        dst[i] = src[i];
    }
    src[BOOT2_PARAM_MAGIC_OFFSET / 4] = 0;
}

/*
 * Startup calls this entry before .data/.bss init. Only the getters
 * reference it, so --gc-sections drops the save and g_boot2_raw.
 */
static void (*const g_boot2_param_save)(void)
    __attribute__((section(".boot2_param_save"), used)) = Boot2_Param_Save;

static int Boot2_Param_Parse(Boot2_Param_Type *p, const uint8_t *src)
{
    uint32_t offset = 0;
    uint32_t entries_len;
    uint32_t flash_len;

    memset(p, 0, sizeof(*p));

    memcpy(&p->ptTableLen, src + offset, 4);
    offset += 4;
    memcpy(&p->activeID, src + offset, 4);
    offset += 4;
    memcpy(&p->ptTable, src + offset, sizeof(p->ptTable));
    offset += sizeof(p->ptTable);

    if (p->ptTable.magicCode != IPRO_PT_MAGIC_CODE ||
        p->ptTable.entryCnt == 0 || p->ptTable.entryCnt > PT_ENTRY_MAX ||
        p->activeID > 1) {
        return 0;
    }
    entries_len = p->ptTable.entryCnt * sizeof(Boot2_PtTable_Entry);
    if (p->ptTableLen != sizeof(p->ptTable) + entries_len + 4) {
        return 0;
    }
    if (utils_crc32((uint8_t *)&p->ptTable, sizeof(p->ptTable) - 4) !=
        p->ptTable.crc32) {
        return 0;
    }

    memcpy(p->ptEntries, src + offset, entries_len);
    offset += entries_len;
    memcpy(&p->ptEntriesCrc32, src + offset, 4);
    offset += 4;
    if (utils_crc32((uint8_t *)p->ptEntries, entries_len) != p->ptEntriesCrc32) {
        return 0;
    }

    /* The flash cfg is optional: a valid PT stands without it */
    memcpy(&flash_len, src + offset, 4);
    offset += 4;
    memcpy(&p->flashCfgMagic, src + offset, 4);
    offset += 4;
    if (flash_len == sizeof(SPI_Flash_Cfg_Type) &&
        p->flashCfgMagic == IPRO_BOOT2_FLASH_CFG_MAGIC) {
        p->flashCfgLen = flash_len;
        memcpy(&p->flashCfg, src + offset, sizeof(SPI_Flash_Cfg_Type));
    } else {
        p->flashCfgMagic = 0;
    }

    p->valid = 1;
    return 1;
}

static Boot2_Param_Type *Boot2_Param_Parsed(void)
{
    /* Keeps the startup save entry linked while a getter is linked */
    __asm__ volatile("" : : "r"(&g_boot2_param_save));
    if (!g_boot2_parsed) {
        (void)Boot2_Param_Parse(&g_boot2_param, (const uint8_t *)g_boot2_raw);
        g_boot2_parsed = 1;
    }
    return &g_boot2_param;
}

Boot2_Param_Type *Boot2_Param_Get(void)
{
    Boot2_Param_Type *p = Boot2_Param_Parsed();

    return p->valid ? p : NULL;
}

SPI_Flash_Cfg_Type *Boot2_Param_Get_Flash_Cfg(void)
{
    Boot2_Param_Type *p = Boot2_Param_Parsed();

    return (p->valid && p->flashCfgLen != 0) ? &p->flashCfg : NULL;
}

int Boot2_Param_Get_Active_ID(void)
{
    Boot2_Param_Type *p = Boot2_Param_Parsed();

    return p->valid ? (int)p->activeID : -1;
}

int Boot2_Param_Is_Valid(void)
{
    return Boot2_Param_Parsed()->valid ? 1 : 0;
}
