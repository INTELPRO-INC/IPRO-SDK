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

#ifndef __HOSTROUTER_LOG_H__
#define __HOSTROUTER_LOG_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef __FILENAME__
#define __FILENAME__ \
            (strchr(__FILE__, '\\') \
            ? ((strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)) \
            : ((strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)))
#endif

extern void hr_debug(const char *tag, const char *filename, const char *funcname, const long line, const char *format, ...);
#define HR_LOGD(...) //hr_debug("d", __FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define HR_LOGI(...) hr_debug("i", __FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define HR_LOGW(...) hr_debug("w", __FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define HR_LOGE(...) hr_debug("e", __FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define HR_LOGA(...) hr_debug("a", __FILENAME__, __func__, __LINE__, __VA_ARGS__)

int hr_binary_regshow(uint32_t *reg);
int hr_binary_valshow(uint32_t value);

#endif
