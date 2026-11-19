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

#include <ipro_matter_wifi_connect.h>
#include "br_wifi_connect_operation.h"
#include "br_wifi_profile.h"
#include <limits.h>
#include <stddef.h>
#include <string.h>

_Static_assert(sizeof(ipro_wifi_connect_request) == IPRO_WIFI_CONNECT_REQUEST_SIZE, "Wi-Fi request ABI size");
_Static_assert(offsetof(ipro_wifi_connect_request, timeout_ms) == 96, "Wi-Fi request ABI layout");
_Static_assert(FHOST_CONNECT_PREPARATION_FAILED == IPRO_WIFI_CONNECT_PREPARATION_FAILED, "result ABI");
_Static_assert(FHOST_CONNECT_PREPARATION_TIMEOUT == IPRO_WIFI_CONNECT_PREPARATION_TIMEOUT, "result ABI");
_Static_assert(FHOST_CONNECT_TASK_FAILED == IPRO_WIFI_CONNECT_TASK_FAILED, "result ABI");
_Static_assert(FHOST_CONNECT_FAILED == IPRO_WIFI_CONNECT_FAILED, "result ABI");
_Static_assert(FHOST_CONNECT_CONFIGURED == IPRO_WIFI_CONNECT_CONFIGURED, "result ABI");
_Static_assert(FHOST_CONNECT_CONNECTED == IPRO_WIFI_CONNECT_CONNECTED, "result ABI");
_Static_assert(FHOST_CONNECT_NOT_CONNECTED == IPRO_WIFI_CONNECT_NOT_CONNECTED, "result ABI");

int ipro_wifi_connect_start(const ipro_wifi_connect_request *request, uint32_t request_size,
                           uint32_t *operation)
{
    if (operation) *operation = 0;
    if (!request || !operation || request_size != sizeof(*request) ||
        request->timeout_ms == 0 || request->timeout_ms > INT_MAX) return -1;
    struct br_wifi_profile profile = {.magic = {'B','W','0','1'},
        .ssid_len = request->ssid_length, .key_len = request->key_length,
        .pmf = request->pmf, .akm = request->security};
    memcpy(profile.ssid, request->ssid, sizeof(profile.ssid));
    memcpy(profile.key, request->key, sizeof(profile.key));
    if (!br_wifi_profile_valid(&profile, sizeof(profile))) {
        br_wifi_profile_wipe(&profile, sizeof(profile));
        return -1;
    }
    wifi_mgmr_sta_connect_params_t config = {0};
    memcpy(config.ssid, profile.ssid, profile.ssid_len);
    memcpy(config.key, profile.key, profile.key_len);
    config.ssid_len = profile.ssid_len;
    config.key_len = profile.key_len;
    config.pmf_cfg = profile.pmf;
    config.use_dhcp = 1;
    config.timeout_ms = (int)request->timeout_ms;
    if (profile.akm) {
        memcpy(config.akm_str, profile.akm == 1 ? "WPA2" : "WPA3", 5);
        config.akm_len = 4;
    }
    br_wifi_profile_wipe(&profile, sizeof(profile));
    uint32_t id = 0;
    const int error = br_wifi_connect_operation_start(&config, &id);
    br_wifi_profile_wipe(&config, sizeof(config));
    if (error == 0) *operation = id;
    return error;
}

int ipro_wifi_connect_take(uint32_t operation, uint32_t *result)
{
    if (!result) return -1;
    *result = 0;
    uint32_t outcome = 0;
    const int error = br_wifi_connect_operation_take(operation, &outcome);
    if (error) return error;
    if (outcome > IPRO_WIFI_CONNECT_NOT_CONNECTED) return -1;
    *result = outcome;
    return 0;
}

int ipro_wifi_connect_close(void) { return br_wifi_connect_operation_close(); }
int ipro_wifi_connect_drain(void) { return br_wifi_connect_operation_drain(); }

int ipro_wifi_disconnect_start(uint32_t *operation)
{
    if (!operation) return -1;
    *operation = 0;
    uint32_t id = 0;
    const int error = br_wifi_disconnect_operation_start(&id);
    if (error == 0) *operation = id;
    return error;
}

int ipro_wifi_disconnect_take(uint32_t operation, uint32_t *result)
{
    if (!result) return -1;
    *result = 0;
    uint32_t outcome = 0;
    const int error = br_wifi_disconnect_operation_take(operation, &outcome);
    if (error) return error;
    if (outcome > IPRO_WIFI_DISCONNECT_FAILED) return -1;
    *result = outcome;
    return 0;
}
