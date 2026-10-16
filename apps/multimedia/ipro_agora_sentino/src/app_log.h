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

#ifndef APP_LOG_H
#define APP_LOG_H

#ifdef CONFIG_IPRO_LOG_ENABLE
#include "ipro_log.h"
#else
#include <stdio.h>
#define IPRO_LOGI(tag, fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define IPRO_LOGE(tag, fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define IPRO_LOGW(tag, fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#endif

#endif /* APP_LOG_H */
