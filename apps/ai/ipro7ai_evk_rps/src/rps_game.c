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
 * @file rps_game.c
 * @brief Rock-paper-scissors state machine. See rps_game.h.
 */

#include "rps_game.h"

#include <string.h>

/* Timing, ms. */
#define HOLD_TO_START_MS   500     /* hand steady in the box before a round  */
#define COUNT_STEP_MS      700     /* per digit                              */
#define CAPTURE_MS         700     /* voting window after the countdown      */
#define RESULT_MS          2500
#define COOLDOWN_MAX_MS    3000    /* give up waiting for the hand to leave  */
#define HAND_GONE_MS       300
#define WARMUP_MS          3000    /* ignore hands while AE/AWB settle at boot */

static rps_game_view_t s_v;
static int      s_conf_pct = 60;
static uint32_t s_t0;              /* state entry time                       */
static uint32_t s_hand_since;      /* IDLE: first moment a hand was seen     */
static uint32_t s_gone_since;      /* COOLDOWN: first moment no hand seen    */
static uint32_t s_last_seq;
static uint32_t s_votes[3];        /* CAPTURE: confidence-weighted           */
static uint32_t s_nvotes;
static uint32_t s_rng = 0x2545F491u;
static bool     s_force;
static uint32_t s_first_tick;      /* for the boot warm-up                    */

static uint32_t rng_next(void)
{
    /* xorshift32; seeded further with the player's timing on every round,
     * which is the only entropy a board with no RNG peripheral in use has. */
    uint32_t x = s_rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    s_rng = x ? x : 0x9E3779B9u;
    return s_rng;
}

static void enter(rps_state_t st, uint32_t now)
{
    s_v.state = st;
    s_t0 = now;
}

static rps_outcome_t judge(rps_class_t player, rps_class_t board)
{
    if (player == board) {
        return RPS_OUTCOME_DRAW;
    }
    /* rock beats scissors, paper beats rock, scissors beat paper */
    if ((player == RPS_ROCK && board == RPS_SCISSORS) ||
        (player == RPS_PAPER && board == RPS_ROCK) ||
        (player == RPS_SCISSORS && board == RPS_PAPER)) {
        return RPS_OUTCOME_PLAYER;
    }
    return RPS_OUTCOME_BOARD;
}

void rps_game_init(void)
{
    rps_game_reset();
}

void rps_game_reset(void)
{
    memset(&s_v, 0, sizeof(s_v));
    s_v.state    = RPS_ST_IDLE;
    s_v.live_cls = RPS_NONE;
    s_v.player   = RPS_NONE;
    s_v.board    = RPS_NONE;
    s_v.message  = "SHOW YOUR HAND";
    s_hand_since = 0;
    s_gone_since = 0;
    s_force      = false;
    s_first_tick = 0;
}

void rps_game_force_start(void)
{
    s_force = true;
}

int rps_game_set_conf(int pct)
{
    if (pct < 10 || pct > 99) {
        return -1;
    }
    s_conf_pct = pct;
    return 0;
}

int rps_game_get_conf(void) { return s_conf_pct; }

void rps_game_tick(uint32_t now, const rps_result_t *r)
{
    bool fresh = r && r->seq != s_last_seq;
    bool hand  = r && r->cls != RPS_NONE && r->conf >= (uint8_t)s_conf_pct;

    if (r) {
        s_last_seq   = r->seq;
        s_v.live_cls = r->cls;
        s_v.live_conf = r->conf;
        if (fresh) {
            /* Camera noise as entropy: the low bits of the raw logits change
             * every frame even with nothing in the box. Without this the
             * board's first pick after power-on was the same every time. */
            s_rng ^= ((uint32_t)(uint8_t)r->logit[0] << 24) ^ ((uint32_t)(uint8_t)r->logit[1] << 16)
                   ^ ((uint32_t)(uint8_t)r->logit[2] << 8)  ^ (uint32_t)(uint8_t)r->logit[3]
                   ^ (now * 2654435761u);
            (void)rng_next();
        }
    }
    if (!s_first_tick) {
        s_first_tick = now ? now : 1u;
    }
    if (now - s_first_tick < WARMUP_MS) {
        hand = false;              /* exposure still settling: not a real hand */
    }

    switch (s_v.state) {
    case RPS_ST_IDLE:
        s_v.countdown = 0;
        s_v.message = "SHOW YOUR HAND";
        if (hand) {
            if (!s_hand_since) {
                s_hand_since = now;
            }
            s_v.message = "HOLD IT...";
        } else {
            s_hand_since = 0;
        }
        if (s_force || (s_hand_since && now - s_hand_since >= HOLD_TO_START_MS)) {
            s_force = false;
            s_hand_since = 0;
            s_rng ^= now * 2654435761u;         /* human timing as entropy */
            enter(RPS_ST_COUNTDOWN, now);
        }
        break;

    case RPS_ST_COUNTDOWN: {
        uint32_t el = now - s_t0;
        s_v.countdown = el < COUNT_STEP_MS ? 3 : el < 2 * COUNT_STEP_MS ? 2 : 1;
        s_v.message = "READY...";
        if (el >= 3 * COUNT_STEP_MS) {
            memset(s_votes, 0, sizeof(s_votes));
            s_nvotes = 0;
            enter(RPS_ST_CAPTURE, now);
        }
        break;
    }

    case RPS_ST_CAPTURE:
        s_v.countdown = 0;
        s_v.message = "GO!";
        if (fresh && r->cls != RPS_NONE) {
            s_votes[r->cls] += r->conf;
            s_nvotes++;
        }
        if (now - s_t0 >= CAPTURE_MS) {
            if (!s_nvotes) {
                s_v.message = "NO HAND - TRY AGAIN";
                s_v.outcome = RPS_OUTCOME_NONE;
                enter(RPS_ST_COOLDOWN, now);
                break;
            }
            int best = 0;
            for (int i = 1; i < 3; i++) {
                if (s_votes[i] > s_votes[best]) {
                    best = i;
                }
            }
            s_v.player  = (rps_class_t)best;
            s_v.board   = (rps_class_t)(rng_next() % 3u);
            s_v.outcome = judge(s_v.player, s_v.board);
            s_v.round++;
            switch (s_v.outcome) {
            case RPS_OUTCOME_PLAYER: s_v.wins++;   s_v.message = "YOU WIN!";  break;
            case RPS_OUTCOME_BOARD:  s_v.losses++; s_v.message = "I WIN!";    break;
            default:                 s_v.draws++;  s_v.message = "DRAW";      break;
            }
            enter(RPS_ST_RESULT, now);
        }
        break;

    case RPS_ST_RESULT:
        if (now - s_t0 >= RESULT_MS) {
            s_gone_since = 0;
            enter(RPS_ST_COOLDOWN, now);
        }
        break;

    case RPS_ST_COOLDOWN:
        if (s_v.outcome != RPS_OUTCOME_NONE) {
            s_v.message = "REMOVE HAND FOR NEXT ROUND";
        }
        if (!hand) {
            if (!s_gone_since) {
                s_gone_since = now;
            }
        } else {
            s_gone_since = 0;
        }
        if ((s_gone_since && now - s_gone_since >= HAND_GONE_MS) ||
            now - s_t0 >= COOLDOWN_MAX_MS || s_force) {
            s_v.outcome = RPS_OUTCOME_NONE;
            s_v.player  = RPS_NONE;
            s_v.board   = RPS_NONE;
            enter(RPS_ST_IDLE, now);
        }
        break;
    }
}

void rps_game_get_view(rps_game_view_t *out)
{
    if (out) {
        *out = s_v;
    }
}
