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

/* Independent host ceiling; preserve existing behavior when not configured.
 *
 * A disabled level keeps its arguments inside `if (0)` rather than dropping
 * them: the format and its arguments are never evaluated or emitted (the
 * block is dead code and the string is not referenced), but every variable
 * that upstream Zephyr only uses in a log call still counts as used, so
 * -Werror=unused-variable does not fire when a level is compiled out. */
#ifndef CONFIG_IPRO_BT_HOST_LOG_LEVEL
#define CONFIG_IPRO_BT_HOST_LOG_LEVEL LOG_LEVEL_DBG
#endif

#ifndef LOG_TAG
#define LOG_TAG "BT_HOST"
#endif

#ifndef LOG_DBG
#if CONFIG_IPRO_BT_HOST_LOG_LEVEL >= LOG_LEVEL_DBG
#define LOG_DBG(fmt, ...) IPRO_LOGD(LOG_TAG, fmt, ##__VA_ARGS__)
#else
#define LOG_DBG(fmt, ...) do { if (0) { IPRO_LOGD(LOG_TAG, fmt, ##__VA_ARGS__); } } while (0)
#endif
#endif
#ifndef LOG_ERR
#if CONFIG_IPRO_BT_HOST_LOG_LEVEL >= LOG_LEVEL_ERR
#define LOG_ERR(fmt, ...) IPRO_LOGE(LOG_TAG, fmt, ##__VA_ARGS__)
#else
#define LOG_ERR(fmt, ...) do { if (0) { IPRO_LOGE(LOG_TAG, fmt, ##__VA_ARGS__); } } while (0)
#endif
#endif
#ifndef LOG_WRN
#if CONFIG_IPRO_BT_HOST_LOG_LEVEL >= LOG_LEVEL_WRN
#define LOG_WRN(fmt, ...) IPRO_LOGW(LOG_TAG, fmt, ##__VA_ARGS__)
#else
#define LOG_WRN(fmt, ...) do { if (0) { IPRO_LOGW(LOG_TAG, fmt, ##__VA_ARGS__); } } while (0)
#endif
#endif
#ifndef LOG_INF
#if CONFIG_IPRO_BT_HOST_LOG_LEVEL >= LOG_LEVEL_INF
#define LOG_INF(fmt, ...) IPRO_LOGI(LOG_TAG, fmt, ##__VA_ARGS__)
#else
#define LOG_INF(fmt, ...) do { if (0) { IPRO_LOGI(LOG_TAG, fmt, ##__VA_ARGS__); } } while (0)
#endif
#endif

#endif /* ZEPHYR_LOGGING_LOG_H_ */
