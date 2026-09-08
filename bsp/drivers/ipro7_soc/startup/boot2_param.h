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
 * @file boot2_param.h
 * @brief Boot2 parameter handling for IPRO7
 *
 * The bootrom passes parameters to the application via a specific memory region.
 * This module saves these parameters before LMA to VMA copy (which would overwrite them)
 * and provides APIs to access the saved parameters.
 *
 * Parameters passed by bootrom:
 * - Partition table (active ID + entries)
 * - Flash configuration (SPI_Flash_Cfg_Type)
 */

#ifndef __BOOT2_PARAM_H__
#define __BOOT2_PARAM_H__

#include <stdint.h>
#include "drv_sflash.h"

/**
 * @brief Boot2 parameter magic code
 */
#define IPRO_BOOT2_FLASH_CFG_MAGIC      0x47464346  /* "FCFG" */
#define IPRO_PT_MAGIC_CODE              0x54505049  /* "IPPT" */

/**
 * @brief Maximum partition table entries
 */
#define PT_ENTRY_MAX                    16

/**
 * @brief Boot2 parameter buffer size (must match bootrom's allocation)
 */
#define BOOT2_PARAM_BUFFER_SIZE         1024

/**
 * @brief Boot2 parameter source address in OCRAM
 *
 * This is where bootrom stores the parameters.
 * Located at the start of tcm_ocram_ret region (0x11010000).
 * Use the non-cache alias for the early hand-off copy. The cacheable alias
 * can contain stale lines on the boot2 -> FW boundary.
 */
#define BOOT2_PARAM_SRC_ADDR            0x21010000
#define BOOT2_PARAM_SRC_CACHE_ADDR      0x11010000
#define BOOT2_PARAM_LEGACY_ACTIVE_ADDR  0x4202DC00
#define BOOT2_PARAM_LEGACY_TABLE_ADDR   0x4202DC04

/**
 * @brief Partition table config structure
 */
typedef struct {
    uint32_t magicCode;     /*!< Partition table magic code */
    uint16_t version;       /*!< Partition table version */
    uint16_t entryCnt;      /*!< Partition table entry count */
    uint32_t age;           /*!< Partition table age */
    uint32_t crc32;         /*!< Partition table CRC32 value */
} Boot2_PtTable_Config;

/**
 * @brief Partition table entry config structure
 */
typedef struct {
    uint8_t type;           /*!< Partition entry type */
    uint8_t device;         /*!< Partition entry device */
    uint8_t activeIndex;    /*!< Partition entry active index */
    uint8_t name[9];        /*!< Partition entry name */
    uint32_t Address[2];    /*!< Partition entry start address */
    uint32_t maxLen[2];     /*!< Partition entry max length */
    uint32_t len;           /*!< Partition entry length */
    uint32_t age;           /*!< Partition entry age */
} Boot2_PtTable_Entry;

/**
 * @brief Boot2 parameter structure (saved copy)
 */
typedef struct {
    /* Partition table section */
    uint32_t ptTableLen;                        /*!< Length of PT table data */
    uint32_t activeID;                          /*!< Active partition table ID */
    Boot2_PtTable_Config ptTable;               /*!< Partition table config */
    Boot2_PtTable_Entry ptEntries[PT_ENTRY_MAX];/*!< Partition entries */
    uint32_t ptEntriesCrc32;                    /*!< Partition entries CRC32 */

    /* Flash config section */
    uint32_t flashCfgLen;                       /*!< Length of flash config data */
    uint32_t flashCfgMagic;                     /*!< Flash config magic */
    SPI_Flash_Cfg_Type flashCfg;                /*!< Flash configuration */

    /* Status */
    uint8_t valid;                              /*!< Parameters are valid */
    uint8_t reserved[3];                        /*!< Reserved for alignment */
} Boot2_Param_Type;

/**
 * @brief Save boot2 parameters from source address
 *
 * This function should be called early in startup, BEFORE any LMA to VMA copy
 * that might overwrite the boot2 parameter region.
 *
 * @note This function is called from assembly startup code
 */
void Boot2_Param_Save(void);

/**
 * @brief Get pointer to saved boot2 parameters
 *
 * @return Pointer to Boot2_Param_Type, or NULL if not valid
 */
Boot2_Param_Type *Boot2_Param_Get(void);

/**
 * @brief Get flash configuration from boot2 parameters
 *
 * @return Pointer to SPI_Flash_Cfg_Type, or NULL if not valid
 */
SPI_Flash_Cfg_Type *Boot2_Param_Get_Flash_Cfg(void);

/**
 * @brief Get active partition ID from boot2 parameters
 *
 * @return Active partition ID, or -1 if not valid
 */
int Boot2_Param_Get_Active_ID(void);

/**
 * @brief Check if boot2 parameters are valid
 *
 * @return 1 if valid, 0 otherwise
 */
int Boot2_Param_Is_Valid(void);

#if defined(CONFIG_IPRO_BOOT2_PARAM_DIAG)
void Boot2_Param_Debug_Dump(void);
#else
static inline void Boot2_Param_Debug_Dump(void) {}
#endif

#endif /* __BOOT2_PARAM_H__ */
