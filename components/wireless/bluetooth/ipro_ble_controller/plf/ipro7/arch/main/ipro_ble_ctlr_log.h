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
 ****************************************************************************************
 * @file ipro_ble_ctlr_log.h
 *
 * @brief IPRO BLE controller logging wrapper.
 *
 * Controller product builds must not print directly to the console.  Route
 * diagnostics through the SDK log system so applications can control formatting,
 * level and output policy consistently.
 ****************************************************************************************
 */

#ifndef IPRO_BLE_CTLR_LOG_H_
#define IPRO_BLE_CTLR_LOG_H_

#include "ipro_log.h"

#define IPRO_BLE_CTLR_LOG_TAG "BTCTRL"

#define IPRO_BLE_CTLR_LOGE(fmt, ...) IPRO_LOGE(IPRO_BLE_CTLR_LOG_TAG, fmt, ##__VA_ARGS__)
#define IPRO_BLE_CTLR_LOGW(fmt, ...) IPRO_LOGW(IPRO_BLE_CTLR_LOG_TAG, fmt, ##__VA_ARGS__)
#define IPRO_BLE_CTLR_LOGI(fmt, ...) IPRO_LOGI(IPRO_BLE_CTLR_LOG_TAG, fmt, ##__VA_ARGS__)
#define IPRO_BLE_CTLR_LOGD(fmt, ...) IPRO_LOGD(IPRO_BLE_CTLR_LOG_TAG, fmt, ##__VA_ARGS__)
#define IPRO_BLE_CTLR_LOGV(fmt, ...) IPRO_LOGV(IPRO_BLE_CTLR_LOG_TAG, fmt, ##__VA_ARGS__)

#endif /* IPRO_BLE_CTLR_LOG_H_ */
