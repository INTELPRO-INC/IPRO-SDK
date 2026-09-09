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
#include <string.h>

/**
 * @brief Saved boot2 parameters (in .noinit section to survive warm reset)
 */
static Boot2_Param_Type g_boot2_param __attribute__((section(".noinit_data")));

/**
 * @brief Save boot2 parameters from source address
 */
void Boot2_Param_Save(void)
{
    uint8_t *pSrc = (uint8_t *)BOOT2_PARAM_SRC_ADDR;
    uint32_t ptLen, flashLen;
    uint32_t offset = 0;
    uint32_t entryCnt;

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

    /* Read PT entries */
    entryCnt = g_boot2_param.ptTable.entryCnt;
    if (entryCnt > PT_ENTRY_MAX) {
        entryCnt = PT_ENTRY_MAX;
    }

    if (entryCnt > 0) {
        memcpy(g_boot2_param.ptEntries, pSrc + offset,
               entryCnt * sizeof(Boot2_PtTable_Entry));
        offset += entryCnt * sizeof(Boot2_PtTable_Entry);
    }

    /* Read PT entries CRC32 */
    memcpy(&g_boot2_param.ptEntriesCrc32, pSrc + offset, 4);
    offset += 4;

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
    if (g_boot2_param.valid) {
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
