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
#include <stdint.h>

enum br_wifi_boot_phase {
    BR_WIFI_BOOT_NOT_STARTED,
    BR_WIFI_BOOT_WAIT_MANAGER,
    BR_WIFI_BOOT_RESTORING,
    BR_WIFI_BOOT_COMPLETE,
    BR_WIFI_BOOT_FAILED,
};

struct br_wifi_boot_status {
    enum br_wifi_boot_phase phase;
    bool manager_ready;
    bool profile_present;
    bool result_valid;
    uint32_t result; /* fhost_connect_result, only when result_valid. */
    int submit_error;
};

/* Call once before network tasks start. Failure may leave partial startup;
 * reboot instead of retrying. Thread autostart policy is immutable thereafter. */
int br_wifi_start(void);
bool br_wifi_thread_autostart(void);

/* Task-only, atomic observation with no secrets. COMPLETE means no boot profile
 * or submission returned and its callback arrived, NOT successful association,
 * IP readiness, worker cleanup, or live-network ownership transfer to Matter.
 * A completed failed join has result_valid=true with the actual worker outcome.
 * No profile has result_valid=false. FAILED reports startup/admission failure.
 * This SDK-native struct must not cross the package ABI. NULL returns -1. */
int br_wifi_boot_status_get(struct br_wifi_boot_status *status);
