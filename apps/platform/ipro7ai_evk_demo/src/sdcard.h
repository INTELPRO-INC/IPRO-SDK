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
 * @file sdcard.h
 * @brief micro-SD on the SDH controller, with FatFS on top.
 *
 * The board config already does the hard part: BOARD_PINMUX_INIT() muxes
 * GPIO28-33 to SDC (function 27) and drives SD_PWR_EN, BOARD_CLOCK_GATING_INIT()
 * un-gates GLB_AHB_CLOCK_SDH, and BOARD_PERIPHERAL_CLOCK_INIT() programs
 * GLB_Set_SDH_CLK. So mounting only needs the FatFS disk driver registered and
 * f_mount() called - no pinmux code here.
 */
#ifndef SDCARD_H
#define SDCARD_H

#include <stdint.h>
#include <stdbool.h>

#define SD_OK           0
#define SD_ERR_MOUNT   -1
#define SD_ERR_IO      -2
#define SD_ERR_STATE   -3

/** @brief Register the SDH disk driver and mount the card at "sd:". */
int sdcard_mount(void);

/** @brief Unmount. */
int sdcard_unmount(void);

bool sdcard_is_mounted(void);

/** @brief Card-detect pin state. Active low: 0 means a card is seated. */
int sdcard_detect_raw(void);

/** @brief Drive the SD supply gate (BOARD_SD_PWR_EN_PIN). */
void sdcard_power(int on);

/** @brief Total and free space in kibibytes. */
int sdcard_space_kb(uint32_t *total_kb, uint32_t *free_kb);

#endif /* SDCARD_H */
