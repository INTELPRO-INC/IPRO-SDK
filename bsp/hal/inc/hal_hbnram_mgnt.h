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
 * @file hal_hbnram_mgnt.h
 * @brief HBN RAM Memory Management (Chip Level)
 *
 * Central management of HBN RAM (4KB) layout for IPRO SoC.
 * All modules should use these definitions instead of hardcoding addresses.
 *
 * Memory Map (IPRO7: 0x20090000, IPRO6LE: 0x20010000):
 *
 *   +---------------------+ HBN_RAM_BASE + 0x000
 *   | LP Firmware Code    |
 *   | (.hbn_code section) | 1KB
 *   +---------------------+ HBN_RAM_BASE + 0x400
 *   | LP Parameters       |
 *   | (hal_lp_param_t)    | 512B
 *   +---------------------+ HBN_RAM_BASE + 0x600
 *   | Reset Info          |
 *   | (sys_rstinfo, etc)  | 32B
 *   +---------------------+ HBN_RAM_BASE + 0x620
 *   | Reserved + subregs  |
 *   | (WAKE_CTX, BOOT2,   | 2304B
 *   |  future use)        |
 *   +---------------------+ HBN_RAM_BASE + 0xF00
 *   | iot2lp_para         |
 *   | (lp_fw wake params) | 256B
 *   +---------------------+ HBN_RAM_BASE + 0x1000
 *
 * Usage:
 *   #include "hal_hbnram_mgnt.h"
 *   void *lp_param = (void *)HAL_HBNRAM_LP_PARAM_ADDR;
 */

#ifndef __HAL_HBNRAM_MGNT_H__
#define __HAL_HBNRAM_MGNT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <generated/autoconf.h>

/*============================ SoC HBN_RAM_BASE ===============================*/

/* Include SoC header for HBN_RAM_BASE definition.
 * A caller may pre-seed HBN_RAM_BASE before including this header to avoid pulling
 * in the full chip header. This is required by the vendored RF component, which
 * uses Normal-view (0x2xxx) register bases and would otherwise clash with the chip
 * header's Device-view (0x3xxx) RF_BASE etc. HBN_RAM_BASE itself (0x20090000) is the
 * same in both views, so pre-seeding it is safe. */
#ifndef HBN_RAM_BASE
#if defined(CONFIG_IPRO7) || defined(IPRO7)
#include "ipro7.h"
#elif defined(CONFIG_IPRO6) || defined(IPRO6)
#include "ipro6.h"
#elif defined(CONFIG_IPRO6LE) || defined(IPRO6LE)
#include "ipro6le.h"
#else
#error "Unknown IPRO SoC - please define CONFIG_IPRO7, CONFIG_IPRO6LE, or CONFIG_IPRO6"
#endif
#endif

/*============================ HBN RAM Layout =================================*/

/**
 * HBN RAM Total Size: 4KB (0x1000)
 */
#define HAL_HBNRAM_TOTAL_SIZE           0x1000

/*
 * Region: LP Firmware Code
 * - Holds wakeup entry code (.hbn_code section)
 * - Copied from flash before entering low power mode
 * - Bootrom jumps here after PDS wakeup
 */
#define HAL_HBNRAM_LP_FW_OFFSET         0x000
#define HAL_HBNRAM_LP_FW_SIZE           0x400   /* 1KB */
#define HAL_HBNRAM_LP_FW_ADDR           (HBN_RAM_BASE + HAL_HBNRAM_LP_FW_OFFSET)

/*
 * Region: LP Parameters
 * - Stores CPU state, RTC info, flash config for wakeup restore
 * - Survives PDS sleep (HBN RAM is retained)
 */
#define HAL_HBNRAM_LP_PARAM_OFFSET      0x400
#define HAL_HBNRAM_LP_PARAM_SIZE        0x200   /* 512B */
#define HAL_HBNRAM_LP_PARAM_ADDR        (HBN_RAM_BASE + HAL_HBNRAM_LP_PARAM_OFFSET)

/*
 * Region: Reset Info
 * - Stores system reset information (survives PDS sleep)
 * - Structure layout (32 bytes total):
 *   [0-3]   magic: Magic pattern for cold boot detection
 *   [4-7]   reset_reason: System reset reason code (set before reboot)
 *   [8-31]  reserved: For future use
 */
#define HAL_HBNRAM_RSTINFO_OFFSET       0x600
#define HAL_HBNRAM_RSTINFO_SIZE         0x020   /* 32B */
#define HAL_HBNRAM_RSTINFO_ADDR         (HBN_RAM_BASE + HAL_HBNRAM_RSTINFO_OFFSET)

/* Sysinfo magic pattern for cold boot detection */
#define HAL_HBNRAM_SYSINFO_MAGIC        0x52535449  /* "RSTI" */

/* Boot stage values */
#define BOOT_STAGE_BOOT2        0  /**< In boot2 */
#define BOOT_STAGE_FW_EARLY     1  /**< FW system_init running */
#define BOOT_STAGE_FW_READY     2  /**< FW scheduler started */
#define BOOT_STAGE_FW_OK        3  /**< Boot success confirmed */

/* Boot flags (bitmask) */
#define BOOT_FLAG_WDT_ENABLED   (1 << 0)  /**< AON WDT is active */
#define BOOT_FLAG_OTA_PENDING   (1 << 1)  /**< OTA in progress */
#define BOOT_FLAG_FW_BOOTED     (1 << 2)  /**< FW boot success */
#define BOOT_FLAG_LP_FASTBOOT   (1 << 3)  /**< Woke from LP fastboot (cold boot w/ HBN scratch) */

/* Max consecutive WDT resets before entering download mode */
#define AON_WDT_MAX_RESETS      5

/**
 * @brief System info structure stored in HBN RAM (shared by boot2 + FW)
 *
 * Survives PDS/HBN sleep and warm resets. Cleared on POR (cold boot).
 * Magic field detects cold boot (HBN RAM content undefined after POR).
 *
 * Layout: 32 bytes total, binary compatible with old hal_hbnram_rstinfo_t.
 */
typedef struct {
    /* existing fields (offset 0-7, unchanged) */
    uint32_t magic;             /**< HAL_HBNRAM_SYSINFO_MAGIC for cold boot detection */
    uint32_t reset_reason;      /**< Reset reason code (HAL_RST_REASON_*) */

    /* boot status (offset 8-15) */
    uint8_t  wdt_reset_count;   /**< Consecutive AON WDT reset count */
    uint8_t  boot_stage;        /**< Current boot stage (BOOT_STAGE_*) */
    uint8_t  boot_flags;        /**< Boot flags (BOOT_FLAG_*) */
    uint8_t  _pad0;
    uint32_t boot_uptime_ms;    /**< FW uptime at last checkpoint (debug) */

    /* pure_dcdc bounce stub stats (offset 16-19) — cleared by save_state
     * on cold POR (magic mismatch), survives AON-WDT-fired reset. main()
     * print stub>1 means WDT fired during prior stub run. */
    uint32_t stub_enter_count;

    /* reserved (offset 20-31) */
    uint32_t reserved[3];
} hal_hbnram_sysinfo_t;

/* Pointer to sysinfo structure in HBN RAM */
#define HAL_HBNRAM_SYSINFO_ADDR         HAL_HBNRAM_RSTINFO_ADDR
#define hal_hbnram_sysinfo  ((volatile hal_hbnram_sysinfo_t *)HAL_HBNRAM_SYSINFO_ADDR)

/* Backward compatibility */
#define HAL_HBNRAM_RSTINFO_MAGIC        HAL_HBNRAM_SYSINFO_MAGIC
typedef hal_hbnram_sysinfo_t hal_hbnram_rstinfo_t;
#define hal_hbnram_rstinfo              hal_hbnram_sysinfo

/*
 * Region: LP Wake Context (fastboot mode)
 * - Populated by HBN RAM fastboot stub before jumping to _start
 * - Read by FW after cold boot to branch "LP wakeup" vs "cold boot" paths
 * - Contains wake reason + a small app-owned scratch area
 */
#define HAL_HBNRAM_LP_WAKE_CTX_OFFSET   0x620
#define HAL_HBNRAM_LP_WAKE_CTX_SIZE     0x040   /* 64B */
#define HAL_HBNRAM_LP_WAKE_CTX_ADDR     (HBN_RAM_BASE + HAL_HBNRAM_LP_WAKE_CTX_OFFSET)

/**
 * @brief LP wake context (fastboot path)
 *
 * Written by HBN RAM stub (hal_lp_fastboot_c_entry) on wake.
 * Read by FW via hal_lp_get_wake_context() / hal_lp_is_fastboot_wakeup().
 *
 * Total size: 64 bytes. Keep fields 4-byte aligned for asm access.
 */
typedef struct {
    uint32_t magic;             /**< HAL_HBNRAM_LP_WAKE_CTX_MAGIC when valid */
    uint32_t wake_reason;       /**< HAL_LP_WAKEUP_* bitmask (see hal_lp.h) */
    uint32_t wake_gpio_bits;    /**< GPIO bits that triggered wakeup */
    uint32_t rtc_cnt_low;       /**< RTC counter low at wake time (32.768kHz) */
    uint32_t rtc_cnt_high;      /**< RTC counter high at wake time */
    uint32_t wake_counter;      /**< Incremented by stub on every fastboot wake */
    uint32_t reserved[10];      /**< Padding to 64B; app may reuse if needed */
} hal_hbnram_lp_wake_ctx_t;

#define HAL_HBNRAM_LP_WAKE_CTX_MAGIC    0x4657414B  /* "KAWF" - wakeup fastboot */

#define hal_hbnram_lp_wake_ctx \
    ((volatile hal_hbnram_lp_wake_ctx_t *)HAL_HBNRAM_LP_WAKE_CTX_ADDR)

/*
 * Region: Boot2 Params Snapshot (for fastboot HBN_0 wake)
 * - Raw bytes from BOOT2_PARAM_SRC_ADDR (0x11010000) copied here before HBN sleep
 * - Boot2_Param_Save() checks 0x11010000 magic; if invalid (fastboot wake)
 *   it falls back to this snapshot so PT table / flash cfg survive no-OCRAM wake
 * - Layout: 4B magic + 640B raw boot2 data (covers max PT with 16 entries)
 *
 * Raw boot2 bytes max size (BOOT2_PARAM_SRC format):
 *   4 (ptTableLen) + 4 (activeID) + 16 (PtTableConfig)
 *   + 16 * 32 (PtEntries) + 4 (crc32) + 4 (flashCfgLen) + 4 (magic)
 *   + ~84 (SPI_Flash_Cfg_Type, packed) = ~632B → round up to 640
 */
#define HAL_HBNRAM_BOOT2_OFFSET         0x660
#define HAL_HBNRAM_BOOT2_SIZE           0x284   /* 4B magic + 640B raw */
#define HAL_HBNRAM_BOOT2_ADDR           (HBN_RAM_BASE + HAL_HBNRAM_BOOT2_OFFSET)
#define HAL_HBNRAM_BOOT2_DATA_SIZE      0x280   /* 640B payload */
#define HAL_HBNRAM_BOOT2_MAGIC          0x32544F42  /* "BOT2" */

/*
 * Region: Reserved
 * - Available for future use
 * - Can be subdivided as needed
 * - IPRO6: the tail 0x400 is carved out for the WL/RF cal RMEM (see WLCAL below),
 *   so RESERVED shrinks from 0x61C to 0x21C. RESERVED_OFFSET (0x8E4) is unchanged
 *   because RESERVED_ADDR is used elsewhere as a boot magic.
 */
#define HAL_HBNRAM_RESERVED_OFFSET      0x8E4
#if defined(CONFIG_IPRO6) || defined(IPRO6)
#define HAL_HBNRAM_RESERVED_SIZE        0x21C   /* 540B (IPRO6: tail carved for WLCAL) */
#else
#define HAL_HBNRAM_RESERVED_SIZE        0x61C   /* 1564B */
#endif
#define HAL_HBNRAM_RESERVED_ADDR        (HBN_RAM_BASE + HAL_HBNRAM_RESERVED_OFFSET)

/*
 * Region: iot2lp_para (lp_fw <-> full firmware wake parameters)
 * - Shared params for the standalone lp_fw Stage-1 image (DTIM wake path).
 * - SEPARATE from LP_PARAM (the resume-in-place CPU snapshot): both must
 *   survive PDS, so they cannot overlap.
 * - Carved from the tail of HBN RAM so RESERVED_ADDR (used elsewhere as a
 *   boot magic) keeps its address. The standalone lp_fw image cannot include
 *   this header, so it hardcodes IOT2LP_PARA_ADDR in lp_fw.h; a _Static_assert
 *   in hal_lp.c pins that literal to HAL_HBNRAM_IOT2LP_ADDR.
 */
#define HAL_HBNRAM_IOT2LP_OFFSET        0xF00
#define HAL_HBNRAM_IOT2LP_SIZE          0x100   /* 256B */
#define HAL_HBNRAM_IOT2LP_ADDR          (HBN_RAM_BASE + HAL_HBNRAM_IOT2LP_OFFSET)

/*
 * Region: WL / RF calibration RMEM (IPRO6 only)
 * - Backing store for the WiFi/RF driver's wl_rmem_t (wl_cfg + rf_calib_data + wl_env,
 *   ~992B). The RF driver writes per-channel LO cal (lo[].fcal/acal) etc. here, so it
 *   MUST be writable retained RAM.
 * - Before this region existed the RF CLI hardcoded 0x20010600, which on IPRO6 maps to
 *   the (broken) ipro6le HBN base and is NOT backed by writable SRAM: cal results were
 *   dropped, config_channel read fcal=0, the LO never locked and txcal hung.
 * - Carved from the tail of the RESERVED region (ends exactly at IOT2LP). A
 *   _Static_assert in wl_api.c pins sizeof(struct wl_rmem_t) <= HAL_HBNRAM_WLCAL_SIZE.
 */
#if defined(CONFIG_IPRO6) || defined(IPRO6)
#define HAL_HBNRAM_WLCAL_SIZE           0x400   /* 1024B (wl_rmem_t ~992B + headroom) */
#define HAL_HBNRAM_WLCAL_OFFSET         (HAL_HBNRAM_IOT2LP_OFFSET - HAL_HBNRAM_WLCAL_SIZE)  /* 0xB00 */
#define HAL_HBNRAM_WLCAL_ADDR           (HBN_RAM_BASE + HAL_HBNRAM_WLCAL_OFFSET)
#endif

/*============================ Validation Macros ==============================*/

/* Compile-time check: ensure regions don't overlap */
#define HAL_HBNRAM_LP_FW_END        (HAL_HBNRAM_LP_FW_OFFSET + HAL_HBNRAM_LP_FW_SIZE)
#define HAL_HBNRAM_LP_PARAM_END     (HAL_HBNRAM_LP_PARAM_OFFSET + HAL_HBNRAM_LP_PARAM_SIZE)
#define HAL_HBNRAM_RSTINFO_END      (HAL_HBNRAM_RSTINFO_OFFSET + HAL_HBNRAM_RSTINFO_SIZE)
#define HAL_HBNRAM_LP_WAKE_CTX_END  (HAL_HBNRAM_LP_WAKE_CTX_OFFSET + HAL_HBNRAM_LP_WAKE_CTX_SIZE)
#define HAL_HBNRAM_BOOT2_END        (HAL_HBNRAM_BOOT2_OFFSET + HAL_HBNRAM_BOOT2_SIZE)
#define HAL_HBNRAM_RESERVED_END     (HAL_HBNRAM_RESERVED_OFFSET + HAL_HBNRAM_RESERVED_SIZE)
#define HAL_HBNRAM_IOT2LP_END       (HAL_HBNRAM_IOT2LP_OFFSET + HAL_HBNRAM_IOT2LP_SIZE)

#if (HAL_HBNRAM_LP_FW_END > HAL_HBNRAM_LP_PARAM_OFFSET)
#error "HBN RAM layout error: LP_FW overlaps with LP_PARAM"
#endif

#if (HAL_HBNRAM_LP_PARAM_END > HAL_HBNRAM_RSTINFO_OFFSET)
#error "HBN RAM layout error: LP_PARAM overlaps with RSTINFO"
#endif

#if (HAL_HBNRAM_RSTINFO_END > HAL_HBNRAM_LP_WAKE_CTX_OFFSET)
#error "HBN RAM layout error: RSTINFO overlaps with LP_WAKE_CTX"
#endif

#if (HAL_HBNRAM_LP_WAKE_CTX_END > HAL_HBNRAM_BOOT2_OFFSET)
#error "HBN RAM layout error: LP_WAKE_CTX overlaps with BOOT2"
#endif

#if (HAL_HBNRAM_BOOT2_END > HAL_HBNRAM_RESERVED_OFFSET)
#error "HBN RAM layout error: BOOT2 overlaps with RESERVED"
#endif

#if (HAL_HBNRAM_RESERVED_END > HAL_HBNRAM_IOT2LP_OFFSET)
#error "HBN RAM layout error: RESERVED overlaps with IOT2LP"
#endif

#if defined(HAL_HBNRAM_WLCAL_OFFSET)
#define HAL_HBNRAM_WLCAL_END        (HAL_HBNRAM_WLCAL_OFFSET + HAL_HBNRAM_WLCAL_SIZE)
#if (HAL_HBNRAM_RESERVED_END > HAL_HBNRAM_WLCAL_OFFSET)
#error "HBN RAM layout error: RESERVED overlaps with WLCAL"
#endif
#if (HAL_HBNRAM_WLCAL_END > HAL_HBNRAM_IOT2LP_OFFSET)
#error "HBN RAM layout error: WLCAL overlaps with IOT2LP"
#endif
#endif

#if (HAL_HBNRAM_IOT2LP_END > HAL_HBNRAM_TOTAL_SIZE)
#error "HBN RAM layout error: IOT2LP exceeds total HBN RAM size"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __HAL_HBNRAM_MGNT_H__ */
