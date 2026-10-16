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

/**
 * @file sentino_ota.c
 * @brief OTA firmware upgrade via Sentino MQTT
 */

#include "sentino_ota.h"

/* TODO: Implement in later tasks */

int sentino_ota_report_version(void) { return -1; }

int sentino_ota_on_upgrade(const char *payload) {
    (void)payload;
    return -1;
}
