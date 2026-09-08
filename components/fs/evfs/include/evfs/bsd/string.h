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

#ifndef BSD_STRING_H
#define BSD_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

// Newlib has strlcpy() and strlcat() so we inhibit a duplicate declaration
#ifdef linux
size_t strlcpy(char *dst, const char *src, size_t size);
size_t strlcat(char *dst, const char *src, size_t size);
#endif

size_t strxcpy(char *dst, const char *src, size_t size);

#define strlcpy_check(d, s, sz)  (strlcpy((d), (s), (sz)) < (sz) ? true : false)


#ifdef __cplusplus
}
#endif


#endif // BSD_STRING_H
