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
#include <stdint.h>

#ifdef IPRO_MATTER_PACKAGE_ACTIVE
/* -nostartfiles omits crtbegin. This static firmware never unloads a DSO. */
void *__dso_handle __attribute__((weak)) = &__dso_handle;
extern const br_cpp_constructor __init_array_start[];
extern const br_cpp_constructor __init_array_end[];
#endif

int br_cpp_runtime_init(void)
{
#ifdef IPRO_MATTER_PACKAGE_ACTIVE
    /* The SDK's __libc_init_array is empty. Strong boundaries make missing
     * linker symbols an error; final ELF inspection must still verify that
     * the expected constructor entries were retained inside this range. */
    const uintptr_t start = (uintptr_t)__init_array_start;
    const uintptr_t end = (uintptr_t)__init_array_end;
    if (end < start || (end - start) % sizeof(br_cpp_constructor)) return -1;
    return br_cpp_run_constructors(__init_array_start, (end - start) / sizeof(br_cpp_constructor));
#else
    return 0;
#endif
}
