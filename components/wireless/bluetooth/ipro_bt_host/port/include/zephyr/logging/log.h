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

/*
 * Zephyr logging mapped onto the SDK logger.  Keep this shim self-contained:
 * the legacy blestack logging headers are not part of the unified host.
 */
#ifndef ZEPHYR_LOGGING_LOG_H_
#define ZEPHYR_LOGGING_LOG_H_

#include "ipro_log.h"

#ifndef LOG_MODULE_REGISTER
#define LOG_MODULE_REGISTER(module_name, level)
#endif

#ifndef LOG_MODULE_DECLARE
#define LOG_MODULE_DECLARE(module_name, level)
#endif

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERR  1
#define LOG_LEVEL_WRN  2
#define LOG_LEVEL_INF  3
#define LOG_LEVEL_DBG  4

#ifndef LOG_TAG
#define LOG_TAG "BT_HOST"
#endif

#ifndef LOG_DBG
#define LOG_DBG(fmt, ...) IPRO_LOGD(LOG_TAG, fmt, ##__VA_ARGS__)
#endif
#ifndef LOG_ERR
#define LOG_ERR(fmt, ...) IPRO_LOGE(LOG_TAG, fmt, ##__VA_ARGS__)
#endif
#ifndef LOG_WRN
#define LOG_WRN(fmt, ...) IPRO_LOGW(LOG_TAG, fmt, ##__VA_ARGS__)
#endif
#ifndef LOG_INF
#define LOG_INF(fmt, ...) IPRO_LOGI(LOG_TAG, fmt, ##__VA_ARGS__)
#endif

#endif /* ZEPHYR_LOGGING_LOG_H_ */
