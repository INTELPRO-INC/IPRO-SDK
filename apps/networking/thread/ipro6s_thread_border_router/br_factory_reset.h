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

/* Owner-requested factory reset: journal now, erase on the next boot.
 * Erased: OpenThread settings ("ot-", including the activation journal), the
 * Wi-Fi profile, the LAN TLS identity/token and BLE Host settings ("bt/",
 * bonds). Kept: the RCP link selection (br-rcp-v1) and any Matter "m/" keys. */
#define BR_FACTORY_RESET_KEY "br-reset-v1"

/* Task-only. Checked write plus read-back of the journal. 0 means committed:
 * br_factory_reset_pending() is then true until reboot, and the caller must
 * reboot even if its reply is lost. Nonzero leaves no request pending. */
int br_factory_reset_request(void);
bool br_factory_reset_pending(void);

enum br_factory_reset_boot {
    BR_FACTORY_RESET_NONE,    /* No journal: nothing touched. */
    BR_FACTORY_RESET_APPLIED, /* Every step and then the journal deleted. */
    BR_FACTORY_RESET_FAILED,  /* Journal kept (retried next boot); logged. */
};

/* Single startup call, before activation recovery, br_wifi_start(), otrStart()
 * and BLE/LAN management: no storage writer may exist. Startup continues on
 * every result. A known journal attempts every step even after a failure, so
 * the credential keys are revoked whenever their own delete works. */
enum br_factory_reset_boot br_factory_reset_boot_apply(void);
/* True after a FAILED boot apply: the next boot retries the erase, so settings
 * written now would be lost. Set before the management transports start and
 * never changed afterwards. */
bool br_factory_reset_incomplete(void);
