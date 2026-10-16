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
 * @file network_lte.h
 * @brief LTE PPP network module — network_ops_t implementation
 */

#ifndef NETWORK_LTE_H
#define NETWORK_LTE_H

#include "app_module.h"

#ifdef __cplusplus
extern "C" {
#endif

/** LTE PPP network ops instance (register with gateway) */
extern const network_ops_t network_lte_ops;

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_LTE_H */
