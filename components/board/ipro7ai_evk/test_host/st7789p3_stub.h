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

/* Virtual panel for the host tests. See st7789p3_stub.c. */
#ifndef ST7789P3_STUB_H
#define ST7789P3_STUB_H
#include <stdint.h>
extern int g_lock_depth, g_lock_errors;
void     stub_reset(void);
uint16_t stub_gram(int x, int y);     /* wire order */
#endif
