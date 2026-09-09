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

#ifndef _PHY_CONFIG_H_
#define _PHY_CONFIG_H_

#ifndef WL_PROFILE_ON
#define WL_PROFILE_ON  (0)
#endif

// #define NX_MDM_VER 32
// #define NX_MAC_VER 21
// #define NX_PLF_VER 30
// #define NX_UF_EN 0

#ifndef WL_BB_CE
#define WL_BB_CE (1)
#endif

// IPRO6 PHY/RF configuration
// WL_IPRO6 is the primary chip identifier for this driver
#if !defined(WL_IPRO6)
#define WL_IPRO6  (1)
#endif

#ifndef WL_BB_LOG_EN
#define WL_BB_LOG_EN (1)
#endif

#ifndef WL_RF_LOG_EN
#define WL_RF_LOG_EN (0)
#endif

#if WL_BB_LOG_EN
#include <stdio.h>
#define wl_logprintf printf
#elif CFG_LPFW
/* LP blob (no console on the wake path): compile RF/PHY debug logging OUT so it
 * costs zero UART time in the warm-restore hot path. NOTE: the WL_BB_LOG_EN=0 #else
 * path was buggy (it also mapped wl_logprintf->printf, so logging could never be
 * disabled); we only fix that for CFG_LPFW to avoid removing normal-fw CLI/at_rx
 * output which also routes through wl_logprintf (cli_app.c at_rx JSON). */
#define wl_logprintf(...)   ((void)0)
#else
#include <stdio.h>
#define wl_logprintf printf
#endif

// TODO
#define PHY_GLOBAL_INT_DISABLE()
#define PHY_GLOBAL_INT_RESTORE()

#define PHY_MAX(a,b)                ((a) > (b) ? (a) : (b))
#define PHY_MIN(a,b)                ((a) < (b) ? (a) : (b))
#define PHY_ABS(a)                  ((a) < (0) ? (-a) : (a))

#define DBG_PHY                     "[PHY]"
#define DBG_RFC                     "[RFC]"
#define DBG_RF                      "[RF] "
#define DBG_ERR                     "[ERROR]"
#endif
