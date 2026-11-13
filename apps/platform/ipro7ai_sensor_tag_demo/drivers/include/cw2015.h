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
 * @file cw2015.h
 * @brief CellWise CW2015 I2C fuel gauge (bring-up subset).
 *
 * The Sensor Tag board carries a CW2015CHBD across the Li-Po cell. It does
 * coulomb-counting + voltage-based SOC estimation and exposes both the
 * state-of-charge (%) and the true cell voltage over I2C — so it serves as
 * BOTH the primary battery-percent source and the voltage cross-check.
 *
 * I2C: SDA=GPIO29 / SCL=GPIO30 (I2C0). The datasheet 8-bit address is 0xC4
 * (write) / 0xC5 (read) → 7-bit address 0x62, which is what the SDK I2C HAL
 * expects (it appends the R/W bit itself).
 */

#ifndef __CW2015_H__
#define __CW2015_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Mux the I2C pins, bring up the I2C peripheral, wake the gauge from sleep
 * and confirm it responds (VERSION register). Idempotent.
 * Returns 0 on success, negative on failure (gauge absent / I2C error).
 */
int cw2015_init(void);

/** True once cw2015_init() has confirmed the gauge is present. */
bool cw2015_present(void);

/**
 * Read state-of-charge in whole percent (0..100).
 * Returns 0 on success, negative on I2C error / gauge absent.
 */
int cw2015_read_soc(uint8_t *pct);

/**
 * Read the cell voltage in millivolts (VCELL, 305 µV/LSB).
 * Returns 0 on success, negative on I2C error / gauge absent.
 */
int cw2015_read_vcell_mv(uint16_t *mv);

/**
 * Re-seed the gauge's SOC estimate from the current cell voltage
 * (datasheet quick-start). Use when SOC is implausible vs VCELL —
 * typical for a profile-less gauge that hasn't converged.
 */
int cw2015_quick_start(void);

#ifdef __cplusplus
}
#endif

#endif /* __CW2015_H__ */
