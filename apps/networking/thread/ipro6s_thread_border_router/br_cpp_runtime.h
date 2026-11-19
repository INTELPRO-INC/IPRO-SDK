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

#pragma once
#include <stddef.h>

typedef void (*br_cpp_constructor)(void);

/* Internal one-shot runner for the immutable linker table. Single-threaded,
 * after platform heap initialization and before scheduler startup.
 * Returns -1 for reentry or an invalid table before executing any entry.
 * A completed invocation is idempotent; the table cannot be replaced later. */
int br_cpp_run_constructors(const br_cpp_constructor *entries, size_t count);

/* BR main entry point. Only package-enabled builds execute constructors. */
int br_cpp_runtime_init(void);
