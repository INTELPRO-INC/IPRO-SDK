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

#pragma once
#include <stdbool.h>

#define BR_THREAD_ACTIVATION_KEY "ot-br-activation-v1"

/* Before Wi-Fi/OT workers start. Recover only an explicitly pending activation;
 * absence never erases an existing, unmanaged SDK deployment. Nonzero blocks
 * startup. Does not erase Matter fabric, Wi-Fi, BR identity or other OT keys. */
int br_thread_activation_boot_recover(void);

/* Called only with the SDK OT mutex, including from its CLI worker. Once the
 * Matter activation owner opens, deny CLI mutations until reboot; selected
 * argument-free diagnostics remain available. No credential-dump exception. */
bool br_thread_activation_cli_allowed(const char *line);
bool br_thread_activation_pending_allowed(void);
