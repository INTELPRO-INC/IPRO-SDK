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

#include "br_network_startup.h"
#include "openthread_port.h"
#include "br_thread_activation.h"
#include "br_factory_reset.h"
#include "wifi_backhaul.h"
#ifdef IPRO_MATTER_PACKAGE_ACTIVE
#include <ipro_matter_storage.h>
#endif

int br_network_startup(void)
{
#ifdef IPRO_MATTER_PACKAGE_ACTIVE
    /* Must precede br_wifi_start: it loads the saved profile and starts writers.
     * Do not apply the Matter sentinel/migration policy to a normal SDK BR. */
    const int storage_error = ipro_matter_storage_boot_recover();
    if (storage_error != 0) return storage_error;
#endif
    /* Apply an owner-requested factory reset while no writer exists: before
     * activation recovery, Wi-Fi profile loading, OT settings and BLE/LAN
     * management. A failed step keeps its journal for the next boot and logs;
     * the BR still starts so it stays reachable, and management reports
     * storage as uncertain and refuses setting writes until that retry. */
    (void)br_factory_reset_boot_apply();
    /* Also recover on a non-Matter firmware boot after an interrupted Matter
     * activation. An absent journal leaves an ordinary SDK deployment intact. */
    const int activation_error = br_thread_activation_boot_recover();
    if (activation_error != 0) return activation_error;
    const int wifi_error = br_wifi_start();
    if (wifi_error != 0) return wifi_error;
    otrStart();
    return 0;
}
