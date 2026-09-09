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

#include <rnm_utils.h>

#include <stdio.h>

#define PRINTF printf
#define NEWLINE "\r\n"

void rnm_hex_dump(const void *data, size_t size)
{
    size_t i;
    unsigned char *d = (unsigned char *)data;
    for (i = 0; i < size; ++i) {
        // last char in a line or of whole data
        if ((i + 1) % 16 == 0 || i + 1 == size) {
            PRINTF("%02X" NEWLINE, d[i]);
        } else {
            PRINTF("%02X ", d[i]);
            if ((i + 1) % 8 == 0) {
                PRINTF(" ");
            }
        }
    }
}
