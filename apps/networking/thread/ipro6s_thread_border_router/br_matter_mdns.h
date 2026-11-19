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

#ifndef BR_MATTER_MDNS_H
#define BR_MATTER_MDNS_H

#include <stdint.h>
#include <openthread/error.h>
#include <openthread/instance.h>

/* Call with the SDK OT lock held, before first infra initialization. Never
 * restarts mDNS or changes an already advertised hostname. MAC is STA identity. */
otError br_matter_mdns_prepare(otInstance *instance, const uint8_t mac[6]);

#endif
