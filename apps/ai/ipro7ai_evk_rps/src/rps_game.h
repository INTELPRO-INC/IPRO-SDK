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
 * @file rps_game.h
 * @brief Rock-paper-scissors against the board: state machine only, no I/O.
 *
 *   IDLE ──hand held in the box──► COUNTDOWN (3,2,1) ──► CAPTURE (vote over
 *   the classifier results for a moment) ──► RESULT (board picks at random,
 *   scores) ──► COOLDOWN (until the hand leaves) ──► IDLE
 *
 * The UI task calls rps_game_tick() at its frame rate with the newest
 * classifier result; everything the screen needs is in rps_game_view_t.
 */
#ifndef RPS_GAME_H
#define RPS_GAME_H

#include <stdint.h>
#include <stdbool.h>
#include "rps_model.h"

typedef enum {
    RPS_ST_IDLE = 0,
    RPS_ST_COUNTDOWN,
    RPS_ST_CAPTURE,
    RPS_ST_RESULT,
    RPS_ST_COOLDOWN,
} rps_state_t;

typedef enum {
    RPS_OUTCOME_NONE = 0,
    RPS_OUTCOME_PLAYER,
    RPS_OUTCOME_BOARD,
    RPS_OUTCOME_DRAW,
} rps_outcome_t;

typedef struct {
    rps_state_t   state;
    int           countdown;         /* 3,2,1 while COUNTDOWN, else 0    */
    rps_class_t   live_cls;          /* what the classifier sees now      */
    uint8_t       live_conf;
    rps_class_t   player;            /* the captured move (RESULT/COOLDOWN)*/
    rps_class_t   board;
    rps_outcome_t outcome;
    uint16_t      wins, losses, draws;
    uint16_t      round;
    const char   *message;           /* bottom-bar text                   */
} rps_game_view_t;

void rps_game_init(void);
void rps_game_reset(void);

/** @brief Advance the game. @p now_ms wall clock, @p r newest classifier
 *         result (r->seq lets the game count distinct inferences). */
void rps_game_tick(uint32_t now_ms, const rps_result_t *r);

void rps_game_get_view(rps_game_view_t *out);

/** @brief Confidence needed to count a hand (default 60 %). */
int  rps_game_set_conf(int pct);
int  rps_game_get_conf(void);

/** @brief Start a round now regardless of what is in the box. */
void rps_game_force_start(void);

#endif /* RPS_GAME_H */
