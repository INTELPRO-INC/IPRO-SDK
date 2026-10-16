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
 * @file network_emac.h
 * @brief EMAC Ethernet network module — network_ops_t implementation
 */

#ifndef NETWORK_EMAC_H
#define NETWORK_EMAC_H

#include "app_module.h"

#ifdef __cplusplus
extern "C" {
#endif

/** EMAC Ethernet network ops instance (register with gateway) */
extern const network_ops_t network_emac_ops;

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_EMAC_H */
