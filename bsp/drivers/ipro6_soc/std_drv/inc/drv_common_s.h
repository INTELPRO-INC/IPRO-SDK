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

#ifndef __DRV_COMMON_S_H__
#define __DRV_COMMON_S_H__

#include "ipro6.h"
#include "drv_common.h"
#include "platform_common.h"

typedef enum {
    COPY_SUCCESS = 0,
    PARAM_ERROR = 1,
    BUFFER_OVERLAP = 2,
    SIZE_MISMATCHING = 3,
    DST_OVERFLOW = 4,
}errno_t;

errno_t memcpy_s(void *dst, size_t dstSize, const void *src, size_t count);

#endif /* __DRV_COMMON_S_H__ */
