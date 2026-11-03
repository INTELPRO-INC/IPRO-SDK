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

#ifndef FMAC_SMID_H
#define FMAC_SMID_H

#include <stdint.h>

/* FBR1: std-IF 0xF, extended code 0xA6 -> host class 0xA6 */
#define FMAC_SMID_FBR1_IDENT  0xA6Fu

/*
 * takeover = 0: pinmux, clock, controller reset, FBR1 (cold loader).
 * takeover = 1: keep the enumerated session the loader left (no reset,
 * no pinmux, FBR1 untouched).
 */
void fmac_smid_init(int takeover);
/* Next received request, or 0 when none */
int fmac_smid_next(uint8_t **pkt, uint32_t *len);
/* fn runs in IRQ context when a download lands or an upload completes */
void fmac_smid_set_notify(void (*fn)(void));
/* No upload queued */
int fmac_smid_tx_done(void);
/* A free upload buffer, *max bytes (NULL if none): fmac_smid_send() of it
 * copies nothing */
uint8_t *fmac_smid_upld_buf(uint32_t *max);
/* Upload time totals since the last call, us: queue head readable to its
 * completion, completion to the next push into an empty queue; n uploads */
void fmac_smid_timing(uint32_t *read_us, uint32_t *turn_us, uint32_t *n);
/* Download buffers the host may fill at once */
uint32_t fmac_smid_dnld_slots(void);
/* Re-posts a buffer returned by fmac_smid_next() */
void fmac_smid_release(uint8_t *pkt);
/* Queues one upload of len bytes (<= FMAC_SMID_UPLD_MAX / NQ), zero-padded
 * to whole blocks; waits for a free upload buffer */
int fmac_smid_send(const void *msg, uint32_t len);
/* 1 when an upload buffer is free */
int fmac_smid_tx_idle(void);
/* Keeps the card ready and re-writes a lost upload doorbell */
void fmac_smid_poll(void);
/* Doorbell re-writes so far */
uint32_t fmac_smid_rearms(void);
/* Masks the SMID interrupt; the session stays enumerated */
void fmac_smid_quiesce(void);
/* Prints transfer counts and controller state */
void fmac_smid_diag(const char *why);
/* Block size of the last CMD53 */
uint32_t fmac_smid_blk(void);

#endif
