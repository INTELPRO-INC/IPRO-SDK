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
 * @file thread_default_dataset.h
 * @brief Apply a Kconfig-driven default Thread Operational Dataset.
 *
 * Both ipro_thread_ftd_cli and ipro_thread_mtd call
 * thread_apply_default_dataset() once during init, BEFORE
 * otThreadSetEnabled(true). Behaviour:
 *
 *   - If a dataset is already committed (otDatasetIsCommissioned()),
 *     leaves it alone — preserves user-provisioned networks across
 *     reboots / firmware updates.
 *   - Otherwise builds an otOperationalDataset from CONFIG_THREAD_DEFAULT_*
 *     macros and calls otDatasetSetActive().
 *
 * Override defaults via the app's .config:
 *   CONFIG_THREAD_DEFAULT_NETWORK_NAME
 *   CONFIG_THREAD_DEFAULT_CHANNEL
 *   CONFIG_THREAD_DEFAULT_PAN_ID
 *   CONFIG_THREAD_DEFAULT_EXT_PAN_ID         (16 hex chars, 8 bytes)
 *   CONFIG_THREAD_DEFAULT_NETWORK_KEY        (32 hex chars, 16 bytes)
 *   CONFIG_THREAD_DEFAULT_MESH_LOCAL_PREFIX  (16 hex chars, 8 bytes)
 *
 * Reasonable demo defaults are baked in if any are missing — every
 * factory device coming off the line will join the same demo network.
 * For production, set per-device or per-deployment values.
 */

#ifndef __THREAD_DEFAULT_DATASET_H__
#define __THREAD_DEFAULT_DATASET_H__

#include <openthread/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @retval  0  dataset already commissioned, OR successfully applied.
 * @retval -1  hex decode error or otDatasetSetActive failed.
 *
 * Idempotent — safe to call multiple times.
 */
int thread_apply_default_dataset(otInstance *aInstance);

#ifdef __cplusplus
}
#endif

#endif /* __THREAD_DEFAULT_DATASET_H__ */
