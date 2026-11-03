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
 * SPE console output: polled UART printf, one "[SPE] " line each.
 */
#pragma once

#include <stdio.h>

#define SPM_LOG(fmt, ...) printf("[SPE] " fmt "\r\n", ##__VA_ARGS__)

/* Print a halt line and spin with interrupts off. */
void spm_fatal(void) __attribute__((noreturn));
