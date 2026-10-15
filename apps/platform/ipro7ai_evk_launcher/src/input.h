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
 * @file input.h
 * @brief Hand gestures -> UI events. Pure logic, no I/O (host-tested).
 *
 * The classifier reports a class and a confidence ~15 times a second. A
 * gesture becomes an event when the same class has been reported, above the
 * confidence floor, for at least `hold_ms`; the event fires once, and the
 * input then stays armed-off until the box has shown NONE (or a low
 * confidence) for `release_ms`. So a hand that stays in the box does not
 * auto-repeat, and the open-hand / fist transition on the way to a gesture
 * does not fire the wrong one as long as it takes less than `hold_ms`.
 *
 *   PAPER    -> INPUT_EV_NEXT    (next page)
 *   ROCK     -> INPUT_EV_SELECT  (the page's action)
 *   SCISSORS -> INPUT_EV_ALT     (the page's secondary action)
 */
#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

/* Class ids match the classifier's output order. */
#define INPUT_CLS_ROCK      0
#define INPUT_CLS_PAPER     1
#define INPUT_CLS_SCISSORS  2
#define INPUT_CLS_NONE      3

typedef enum {
    INPUT_EV_NONE = 0,
    INPUT_EV_NEXT,
    INPUT_EV_SELECT,
    INPUT_EV_ALT,
} input_event_t;

typedef struct {
    uint32_t hold_ms;        /* stable time before a gesture fires           */
    uint32_t release_ms;     /* NONE time before the next gesture may fire   */
    int      min_conf;       /* confidence floor, percent                    */

    int      cand;           /* class currently being held                   */
    uint32_t cand_since;     /* when it started                              */
    uint32_t none_since;     /* when the box last went empty                 */
    bool     armed;          /* a gesture may fire                           */
    bool     have_none;      /* none_since is valid                          */
    uint32_t last_seq;       /* de-duplicate classifier results              */
    int      hold_pct;       /* progress toward firing, 0..100 (for the UI)  */
    uint32_t fired;          /* events emitted, for status                   */
} input_t;

void input_init(input_t *in, uint32_t hold_ms, uint32_t release_ms, int min_conf);

/**
 * @brief Feed one classifier result.
 * @param now_ms  Wall clock.
 * @param cls     INPUT_CLS_*.
 * @param conf    Confidence, percent.
 * @param seq     Classifier result sequence number; a repeated seq is ignored
 *                (the UI polls faster than the NPU produces).
 * @return The event that fired on this call, or INPUT_EV_NONE.
 */
input_event_t input_update(input_t *in, uint32_t now_ms, int cls, int conf, uint32_t seq);

/** @brief Class being held (INPUT_CLS_NONE when nothing), and how far along
 *         the hold is in percent - for drawing a progress cue. */
int input_holding(const input_t *in, int *pct);

/** @brief The event a class maps to (INPUT_EV_NONE for NONE / unknown). */
input_event_t input_event_for_class(int cls);

#endif /* INPUT_H */
