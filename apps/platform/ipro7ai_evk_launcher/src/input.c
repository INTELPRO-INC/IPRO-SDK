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

/**
 * @file input.c
 * @brief Hand gestures -> UI events. See input.h.
 */

#include "input.h"

#include <string.h>

void input_init(input_t *in, uint32_t hold_ms, uint32_t release_ms, int min_conf)
{
    memset(in, 0, sizeof(*in));
    in->hold_ms    = hold_ms;
    in->release_ms = release_ms;
    in->min_conf   = min_conf;
    in->cand       = INPUT_CLS_NONE;
    /* Not armed until the box has been seen empty once: a hand still in the
     * box from a previous page (or at boot) must not act on the new page. */
    in->armed      = false;
}

input_event_t input_event_for_class(int cls)
{
    switch (cls) {
    case INPUT_CLS_PAPER:    return INPUT_EV_NEXT;
    case INPUT_CLS_ROCK:     return INPUT_EV_SELECT;
    case INPUT_CLS_SCISSORS: return INPUT_EV_ALT;
    default:                 return INPUT_EV_NONE;
    }
}

input_event_t input_update(input_t *in, uint32_t now_ms, int cls, int conf, uint32_t seq)
{
    if (seq == in->last_seq) {
        return INPUT_EV_NONE;            /* nothing new from the classifier */
    }
    in->last_seq = seq;

    /* A low-confidence result is an empty box as far as the hold goes. */
    if (conf < in->min_conf || input_event_for_class(cls) == INPUT_EV_NONE) {
        cls = INPUT_CLS_NONE;
    }

    if (cls == INPUT_CLS_NONE) {
        if (!in->have_none) {
            in->have_none  = true;
            in->none_since = now_ms;
        }
        in->cand     = INPUT_CLS_NONE;
        in->hold_pct = 0;
        if (!in->armed && now_ms - in->none_since >= in->release_ms) {
            in->armed = true;
        }
        return INPUT_EV_NONE;
    }

    in->have_none = false;
    if (cls != in->cand) {
        in->cand       = cls;
        in->cand_since = now_ms;
        in->hold_pct   = 0;
        return INPUT_EV_NONE;
    }

    uint32_t held = now_ms - in->cand_since;
    in->hold_pct = in->hold_ms ? (int)(held >= in->hold_ms ? 100u : held * 100u / in->hold_ms) : 100;

    if (in->armed && held >= in->hold_ms) {
        in->armed = false;               /* one event per visit to the box */
        in->fired++;
        return input_event_for_class(cls);
    }
    return INPUT_EV_NONE;
}

int input_holding(const input_t *in, int *pct)
{
    if (pct) {
        *pct = in->cand == INPUT_CLS_NONE ? 0 : in->hold_pct;
    }
    return in->cand;
}
