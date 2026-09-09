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

#include <stdio.h>
#include <string.h>
#include "sigma_dut_priv.h"

#ifndef SIGMA_DUT_VERSION
#define SIGMA_DUT_VERSION "1.0"
#endif

#ifndef SIGMA_DUT_VENDOR
#define SIGMA_DUT_VENDOR "INTELPRO"
#endif

#ifndef SIGMA_DUT_MODEL
#define SIGMA_DUT_MODEL "IPRO6LE"
#endif

#ifndef SIGMA_DUT_FW_VERSION
#define SIGMA_DUT_FW_VERSION "1.0.0"
#endif

static int cmd_ca_get_version(struct sigma_conn *conn,
                              const struct sigma_cmd *cmd)
{
    return sigma_resp_complete(conn, "version,%s", SIGMA_DUT_VERSION);
}

static int cmd_device_get_info(struct sigma_conn *conn,
                               const struct sigma_cmd *cmd)
{
    return sigma_resp_complete(conn,
        "vendor,%s,model,%s,version,%s",
        SIGMA_DUT_VENDOR, SIGMA_DUT_MODEL, SIGMA_DUT_FW_VERSION);
}

static int cmd_device_list_interfaces(struct sigma_conn *conn,
                                      const struct sigma_cmd *cmd)
{
    return sigma_resp_complete(conn,
        "interfaceType,802.11,interfaceID,wlan0");
}

const struct sigma_cmd_entry sigma_device_commands[] = {
    { "ca_get_version",         cmd_ca_get_version },
    { "device_get_info",        cmd_device_get_info },
    { "device_list_interfaces", cmd_device_list_interfaces },
    { NULL, NULL }
};
