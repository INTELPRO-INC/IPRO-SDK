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

#ifndef IPRO_FHOST_RX_CONFIG_H
#define IPRO_FHOST_RX_CONFIG_H

/* Shared sizing only: lwIP must not depend on the MAC operation interfaces. */
#if __has_include("export/rwnx_config.h")
#include "export/rwnx_config.h"
#endif

/* Preserve the existing aggregation/default and application override rules. */
#if NX_AMPDU_RX
#ifndef FHOST_RX_BUF_CNT
#define FHOST_RX_BUF_CNT (NX_MAX_BA_RX * NX_AMPDU_RX_BUF_SIZE + 2)
#endif
#else
#define FHOST_RX_BUF_CNT 4
#endif

#endif /* IPRO_FHOST_RX_CONFIG_H */
