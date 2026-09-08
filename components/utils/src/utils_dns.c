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

#include <utils_dns.h>

int utils_dns_domain_get(uint8_t *records, uint8_t *buf, int *len)
{
    int i, pos_in, pos_out, size_buf;

    size_buf = *len;

    pos_in = 0;
    pos_out = 0;
    i = records[pos_in++];
    while (i > 0 && size_buf > 0) {
        buf[pos_out++] = records[pos_in++];
        i--;
        size_buf--;

        if (0 == i && size_buf > 0) {
            /*current record is empty*/
            buf[pos_out++] = '.';
            i = records[pos_in++];
        }
    }
    *len = pos_out;

    return 0;
}
