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
 * Check and summary helpers shared by the SPM host tests.
 */
#pragma once

#include <stdio.h>

static int g_run;
static int g_pass;

static void check(int cond, const char *what)
{
    g_run++;
    if (cond) {
        g_pass++;
    }
    printf("%s %s\n", cond ? "PASS" : "FAIL", what);
}

/* Prints "PASS n/n" (returns 0) or "FAIL pass/run" (returns 1) */
static int check_summary(void)
{
    int ok = g_pass == g_run;

    printf("%s %d/%d\n", ok ? "PASS" : "FAIL", g_pass, g_run);
    return ok ? 0 : 1;
}
