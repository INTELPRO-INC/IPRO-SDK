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

#ifndef _PHYRF_CONFIG_H_
#define _PHYRF_CONFIG_H_

#include "co_int.h"
#include "co_bool.h"

// #define NX_MDM_VER 32
// #define NX_MAC_VER 21
// #define NX_PLF_VER 30
// #define NX_UF_EN 0


#if PHYRF_LOG_EN
#include <stdio.h>
#define logprintf printf
#warning ("PHYRF LOG is enabled")
#else
#define logprintf(...)
#endif

// Prefix used for module filtering
#define D_KE        "\x80"   ///< Prefix for Kernel
#define D_DBG       "\x81"   ///< Prefix for DBG
#define D_IPC       "\x82"   ///< Prefix for IPC
#define D_DMA       "\x83"   ///< Prefix for DMA
#define D_MM        "\x84"   ///< Prefix for LMAC management
#define D_TX        "\x85"   ///< Prefix for Tx path
#define D_RX        "\x86"   ///< Prefix for Rx path
#define D_PHY       "\x87"   ///< Prefix for Modem / RF

// Prefix used for severity filtering
#define D_CRT       "\x9A"   ///< Prefix for critical
#define D_ERR       "\x9B"   ///< Prefix for error
#define D_WRN       "\x9C"   ///< Prefix for warning
#define D_INF       "\x9D"   ///< Prefix for info
#define D_VRB       "\x9E"   ///< Prefix for verbose debug

// Debug prefixes for modules (also defined in wl_config.h, use ifndef to avoid redefinition)
#ifndef DBG_PHY
#define DBG_PHY     "[PHY]"
#endif
#ifndef DBG_RFC
#define DBG_RFC     "[RFC]"
#endif
#ifndef DBG_RF
#define DBG_RF      "[RF] "
#endif
#ifndef DBG_ERR
#define DBG_ERR     "[ERROR]"
#endif

#endif
