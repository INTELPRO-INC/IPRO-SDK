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
 * @file evk_sd.h
 * @brief micro-SD on the SDH controller (GPIO27-34), FatFS mounted at "sd:".
 *
 * The board config does the pin work: BOARD_PINMUX_INIT() muxes GPIO28-33 to
 * SDC and drives SD_PWR_EN, the clock inits un-gate and program the SDH
 * clock. Mounting only needs the FatFS disk driver registered and f_mount()
 * called. 4-bit bus is requested before the driver registers (the port
 * defaults to 1-bit, which is a quarter of the bandwidth and, for sustained
 * reads, out of headroom).
 *
 * Needs CONFIG_EVFS=y, CONFIG_EVFS_USE_FATFS=y, CONFIG_COMPONENTS_FATFS_ENABLE=y
 * (FatFS's port layer includes vfs_inode.h, so EVFS has to come with it).
 *
 * Lifted from apps/platform/ipro7ai_evk_demo/src/sdcard.c.
 */
#ifndef EVK_SD_H
#define EVK_SD_H

#include <stdint.h>
#include <stdbool.h>

#define EVK_SD_OK           0
#define EVK_SD_ERR_MOUNT   -1
#define EVK_SD_ERR_IO      -2
#define EVK_SD_ERR_STATE   -3

#define EVK_SD_ROOT         "sd:/"

/** @brief True when the card-detect switch says a card is seated. */
bool evk_sd_present(void);

/** @brief Register the SDH disk driver and mount the card at "sd:".
 *         Idempotent; fails fast on a missing or unformatted card. */
int  evk_sd_mount(void);
int  evk_sd_unmount(void);
bool evk_sd_mounted(void);

/** @brief Drive the SD supply gate (BOARD_SD_PWR_EN_PIN). */
void evk_sd_power(int on);

/** @brief Total and free space in KiB. */
int  evk_sd_space_kb(uint32_t *total_kb, uint32_t *free_kb);

/** @brief True if @p path exists on the mounted card ("sd:/x/y"). */
bool evk_sd_exists(const char *path);

#endif /* EVK_SD_H */
