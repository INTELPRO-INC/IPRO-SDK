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

#ifndef IPRO_BR_DOMAIN_H
#define IPRO_BR_DOMAIN_H
#include <stdbool.h>
#include <openthread/instance.h>

/* Deployment configuration, not a shell writer or Matter commissioning API.
 * Call only from the existing OT owner after BBR readiness handling. */
void br_domain_process(otInstance *instance, bool ready);
#endif
