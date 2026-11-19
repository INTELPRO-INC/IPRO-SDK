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
#include <stdint.h>

enum br_matter_runtime_status {
    BR_MATTER_RUNTIME_WAITING,
    BR_MATTER_RUNTIME_REBOOT_REQUIRED,
    BR_MATTER_RUNTIME_STARTED,
    BR_MATTER_RUNTIME_FAILED,
};
/* One startup task only, after br_network_startup succeeds; no network/CHIP
 * locks held. Poll while WAITING. Does not block waiting for CONNECT cleanup.
 * Local UART provisioning is sealed before the one Matter startup attempt.
 * A profile write (including uncertain/failed writes) requires reboot; never
 * start Matter over a live connection with newly replaced persistent state.
 * error is opaque diagnostic status, not otError. STARTED is not fabric join.
 */
enum br_matter_runtime_status br_matter_runtime_step(uint32_t *error);
