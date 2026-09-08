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
 * Stub utils_log.h - provides logging macros used by utils_base64.c
 * Maps to ipro_log.h logging or no-ops if not available.
 */
#ifndef UTILS_LOG_H
#define UTILS_LOG_H

#include <stdio.h>

#ifndef log_error
#define log_error(fmt, ...) ((void)0)
#endif

#ifndef log_info
#define log_info(fmt, ...) ((void)0)
#endif

#ifndef log_warning
#define log_warning(fmt, ...) ((void)0)
#endif

#ifndef log_debug
#define log_debug(fmt, ...) ((void)0)
#endif

#endif /* UTILS_LOG_H */
