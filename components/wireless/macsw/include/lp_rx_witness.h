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

#ifndef LP_RX_WITNESS_H
#define LP_RX_WITNESS_H

#include <stdint.h>
#include <stddef.h>

/* Carrier-only diagnostic ABI. Never include it in the RX-only LPFW build. */
#define LP_RXW_WORDS 11u
#define LP_RXW_RX_ROWS 16u
#define LP_RXW_TCP_ROWS 16u
#define LP_RXW_RESUME_ROWS 8u

enum lp_rxw_stream { LP_RXW_RX, LP_RXW_TCP, LP_RXW_RESUME };
enum lp_rxw_event {
    LP_RXW_RX_RESULT = 1,
    LP_RXW_PDS_ENTER,
    LP_RXW_RESUME_BEFORE,
    LP_RXW_RESUME_AFTER,
    LP_RXW_TCP_INPUT,
    LP_RXW_TIMER_START,
    LP_RXW_TIMER_ADD,
    LP_RXW_TIMER_CANCEL,
    LP_RXW_TIMER_FIRE,
    LP_RXW_KA_RESULT,
    LP_RXW_SERVICE_EXIT
};

/* RX_RESULT data[8] bit 0 is the handler return, not final delivery.
 * With bit 3 (BA) set, classify as BA-handled/unknown: frames may be
 * queued or already uploaded. There is no RX-to-pbuf identity token.
 * TCP_INPUT data[6] is payload bytes after TCP header/options removal. */
struct lp_rxw_row {
    uint32_t sequence, pds, rtc_lo, rtc_hi, event;
    uint32_t data[LP_RXW_WORDS];
};

#ifdef LP_RXW_IMPLEMENTATION
#define LP_RXW_WEAK
#else
#define LP_RXW_WEAK __attribute__((weak))
#endif
extern volatile uint32_t lp_rxw_enabled LP_RXW_WEAK;
void lp_rxw_emit(uint32_t stream, uint32_t event, const uint32_t *data,
                 uint32_t count) LP_RXW_WEAK;
void lp_rxw_control(const char *operation) LP_RXW_WEAK;
#undef LP_RXW_WEAK

#define LP_RXW_ON ((&lp_rxw_enabled != NULL) && lp_rxw_enabled && (lp_rxw_emit != NULL))
/* Implemented by RXU; snapshots only retained software state. */
void rxu_lp_witness_resume(uint32_t event);
#endif
