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

#ifndef LP_WAKE_MATCHER_H
#define LP_WAKE_MATCHER_H
#include <stddef.h>
#include <stdint.h>
/* Legacy case-sensitive substring tokens, carried across TCP receive chunks. */
struct lp_wake_matcher { uint8_t upper, lower, pending; };
static inline void lp_wake_reset(struct lp_wake_matcher *s)
{
    s->upper = s->lower = s->pending = 0;
}
static inline void lp_wake_feed(struct lp_wake_matcher *s, const void *bytes, size_t len)
{
    const unsigned char *p = bytes;
    static const char upper[] = "WAKE", lower[] = "wakeup";
    for (size_t i = 0; i < len; i++) {
        unsigned char c = p[i];
        s->upper = c == upper[s->upper] ? s->upper + 1 : (c == upper[0]);
        s->lower = c == lower[s->lower] ? s->lower + 1 : (c == lower[0]);
        if (s->upper == sizeof(upper) - 1) {
            s->pending = 1;
            s->upper = 0;
        }
        if (s->lower == sizeof(lower) - 1) {
            s->pending = 1;
            s->lower = 0;
        }
    }
}
static inline int lp_wake_take(struct lp_wake_matcher *s)
{
    int matched = s->pending;
    s->pending = 0;
    return matched;
}
#endif
