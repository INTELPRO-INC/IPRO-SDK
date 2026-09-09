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

#ifndef _PHY_BZ_H_
#define _PHY_BZ_H_

#include "co_int.h"

struct bz_phy_cfg_t {
    int8_t txpwr_max;
    int8_t txpwr_min;
    uint8_t time_txpwrup_us;
    uint8_t time_txpwrdn_us;
    uint8_t time_rxpwrup_us;
    uint8_t time_rx_ant_delay;
    uint8_t time_tx_ant_delay;

    void (*reset)(void);
    void (*helper_rssi_convert)(void);
    void (*helper_freqoff_convert)(uint8_t);
};

extern struct bz_phy_cfg_t bz_phy;

void bz_phy_init();

#endif
