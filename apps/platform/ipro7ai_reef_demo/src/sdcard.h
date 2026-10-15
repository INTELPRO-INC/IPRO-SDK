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
 * @brief SD / SDIO slot: FatFS mount on the SDH controller, the supply gate
 *        and card detect. The same slot takes the IPRO6S_FIN Wi-Fi fin.
 *
 * Pins come from the board header: D0-3/CLK/CMD on GPIO28-33 (muxed at board
 * init), card detect GPIO15 (LOW = seated), supply gate GPIO14 driving the Q1
 * P-FET when JP3 is on 2-3 (LOW = on, the reset default).
 */
#ifndef SDCARD_H
#define SDCARD_H

#include <stdint.h>
#include <stdbool.h>

enum {
    SD_OK = 0,
    SD_ERR_MOUNT = -1,
    SD_ERR_STATE = -2,
    SD_ERR_IO    = -3,
};

int  sdcard_mount(void);
int  sdcard_unmount(void);
bool sdcard_is_mounted(void);

/** true when the slot's CD contact says a card (or the fin) is seated. */
bool sdcard_present(void);
int  sdcard_detect_raw(void);

/** Drive the SD_VDD gate. Only effective with JP3 on 2-3. */
void sdcard_power(bool on);
bool sdcard_power_is_on(void);

/** Power-cycle the slot: off, wait `off_ms`, on. For a wedged card or fin. */
void sdcard_power_cycle(uint32_t off_ms);

int  sdcard_space_kb(uint32_t *total_kb, uint32_t *free_kb);

#endif /* SDCARD_H */
