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

#ifndef SENTINO_OTA_H
#define SENTINO_OTA_H

/**
 * @brief Report current firmware version for OTA
 * @return 0 on success, negative on failure
 */
int sentino_ota_report_version(void);

/**
 * @brief Handle OTA upgrade notification from cloud
 * @param payload Raw JSON payload from MQTT
 * @return 0 on success, negative on failure
 */
int sentino_ota_on_upgrade(const char *payload);

#endif /* SENTINO_OTA_H */
