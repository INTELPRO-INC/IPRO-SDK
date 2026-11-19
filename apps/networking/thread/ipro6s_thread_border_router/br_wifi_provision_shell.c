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

#include <generated/autoconf.h>
#if defined(CONFIG_SHELL) && defined(IPRO_MATTER_PACKAGE_ACTIVE)
#include "br_wifi_provision.h"
#include "shell.h"
#include "ipro_log.h"

int shell_input_intercept(uint8_t data)
{
    return br_wifi_provision_input(data);
}

static void cmd_wifi_provision(int argc, char **argv)
{
    (void)argv;
    if (argc != 1 || br_wifi_provision_arm() != 0) {
        IPRO_LOGE("MatterBR", "BR_WIFI_INPUT_UNAVAILABLE");
        return;
    }
    /* Print only after RX interception is active. Host MUST wait for this
     * marker before sending its record; no credentials belong in argv. */
    IPRO_LOGI("MatterBR", "BR_WIFI_INPUT_READY");
}
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_provision, br_wifi_provision, Local Wi-Fi bootstrap before Matter startup);
#endif
