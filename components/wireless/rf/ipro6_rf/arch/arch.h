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

#ifndef __ARCH_H__
#define __ARCH_H__

#include "ll.h"
/*
 * CPU WORD SIZE
 ****************************************************************************************
 */
/// ARM is a 32-bit CPU
#define CPU_WORD_SIZE   4

/*
 * CPU Endianness
 ****************************************************************************************
 */
/// ARM is little endian
#define CPU_LE          1

/*
 * ASSERTION CHECK
 ****************************************************************************************
 */
void phy_assert_rec(const char *condition, const char * file, int line);
void phy_assert_err(const char *condition, const char * file, int line);
void phy_assert_warn(const char *condition, const char * file, int line);

#if WL_NIC
#define ASSERT(type, cond)
#else
#define ASSERT(type, cond) phy_assert_##type(cond, "module", __LINE__)
#endif


#if 1
/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR(cond)                              \
    do {                                              \
        if (!(cond)) {                                \
            ASSERT(err, #cond);                       \
        }                                             \
    } while(0)

/// Assertions showing a non-critical problem that has to be fixed by the SW
#define ASSERT_WARN(cond)                             \
    do {                                              \
        if (!(cond)) {                                \
            ASSERT(warn, #cond);                      \
        }                                             \
    } while(0)
#else
#define ASSERT_ERR(cond) \
    if (!(cond)) { \
        while (1) {} \
    }

#define ASSERT_WARN(cond) \
    if (!(cond)) { \
        while (1) {} \
    }
#endif

#if 1
/// Assertions that trigger the automatic recovery mechanism and return void
#define ASSERT_REC(cond)                              \
    {                                                \
        if (!(cond)) {                                \
            ASSERT(rec, #cond);                       \
            return;                                   \
        }                                             \
    }

/// Assertions that trigger the automatic recovery mechanism and return a value
#define ASSERT_REC_VAL(cond, ret)                     \
    {                                                \
        if (!(cond)) {                                \
            ASSERT(rec, #cond);                       \
            return (ret);                             \
        }                                             \
    }

/// Assertions that trigger the automatic recovery mechanism and do not return
#define ASSERT_REC_NO_RET(cond)                       \
    {                                                \
        if (!(cond)) {                                \
            ASSERT(rec, #cond);                       \
        }                                             \
    }

#else
/// Assertions that trigger the automatic recovery mechanism and return void
#define ASSERT_REC(cond)             ASSERT_ERR(cond)

/// Assertions that trigger the automatic recovery mechanism and return a value
#define ASSERT_REC_VAL(cond, ret)    ASSERT_ERR(cond)

/// Assertions that trigger the automatic recovery mechanism and do not return
#define ASSERT_REC_NO_RET(cond)      ASSERT_ERR(cond)
#endif

/// Object allocated in shared memory - check linker script
//#define __SHARED __attribute__ ((section("shram")))

/// Macro checking if a pointer is part of the shared RAM
#define TST_SHRAM_PTR(ptr) ((((uint32_t)(ptr)) < (uint32_t)&_sshram) ||                   \
                            (((uint32_t)(ptr)) >= (uint32_t)&_eshram))

/// Macro checking if a pointer is part of the shared RAM
#define CHK_SHRAM_PTR(ptr) { if (TST_SHRAM_PTR(ptr)) return;}
#endif
