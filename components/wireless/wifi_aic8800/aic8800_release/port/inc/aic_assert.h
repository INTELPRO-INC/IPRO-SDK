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
 * Shim header replacing platform/inc/aic_assert.h
 */

#ifndef _AIC_ASSERT_H_
#define _AIC_ASSERT_H_

#include "aic_log.h"

#ifndef AIC_ASSERT_ERR
#define AIC_ASSERT_ERR(cond)        do { if (!(cond)) { \
                                            DBG_CMN_CRT("Assertion failed: %s, file %s, line %d\n", #cond, __FILE__, __LINE__); \
                                            while (1); \
                                        } } while (0)
#define ASSERT(cond)
#endif

#endif /* _AIC_ASSERT_H_ */
