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

#include "br_cpp_runtime.h"

static enum { NOT_STARTED, RUNNING, COMPLETE } state;

int br_cpp_run_constructors(const br_cpp_constructor *entries, size_t count)
{
    if (state == RUNNING) return -1;
    if (state == COMPLETE) return 0;
    if (!entries && count) return -1;
    for (size_t i = 0; i < count; ++i)
        if (!entries[i]) return -1;
    state = RUNNING;
    for (size_t i = 0; i < count; ++i) entries[i]();
    state = COMPLETE;
    return 0;
}
