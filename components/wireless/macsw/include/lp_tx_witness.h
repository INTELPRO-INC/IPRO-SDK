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

#ifndef LP_TX_WITNESS_H
#define LP_TX_WITNESS_H

#include <stdint.h>
#include <stddef.h>

/* Carrier-only diagnostic ABI. The LPFW build gets a no-op macro surface. */
#if defined(LP_TXW_HOST_TEST) || (defined(CONFIG_IPRO6) && defined(CONFIG_USE_MACSW) && \
    defined(CONFIG_LWIP_LP) && !IPRO_WIFI_LP_FW)
#define LP_TXW_ROWS 32u

enum lp_txw_site {
    LP_TXW_DISCARD = 0,
    LP_TXW_PUSH,
    LP_TXW_PUSH_INT,
    LP_TXW_UMAC_DISCARD,
    LP_TXW_FRAME_CFM,
    LP_TXW_CFM,
    LP_TXW_TXDESC_GET,
    LP_TXW_TXDESC_RELEASE,
    LP_TXW_RETRY_KEEP,
    LP_TXW_REPUSH = 9,
};

struct lp_txw_row {
    uint32_t sequence;
    uint32_t rtc_lo;
    uint32_t rtc_hi;
    uint32_t site;
    int32_t delta;
    uint32_t ac;
    uint32_t txdesc;
    uint32_t pck_after;
    uint32_t caller;
};

#ifdef LP_TXW_IMPLEMENTATION
#define LP_TXW_WEAK
#else
#define LP_TXW_WEAK __attribute__((weak))
#endif
extern volatile uint32_t lp_txw_enabled LP_TXW_WEAK;
void lp_txw_record(uint32_t site, int32_t delta, uint32_t ac, uintptr_t txdesc,
                   uint32_t pck_after, uintptr_t caller) LP_TXW_WEAK;
void lp_txw_control(const char *operation) LP_TXW_WEAK;
#undef LP_TXW_WEAK

/* Guard at each pck mutation site keeps default-off calls out of the recorder. */
#define LP_TXW_ON ((&lp_txw_enabled != NULL) && lp_txw_enabled && (lp_txw_record != NULL))
#define LP_TXW_RECORD(site, delta, ac, txdesc, pck_after) do { \
    if (LP_TXW_ON) \
        lp_txw_record((site), (delta), (ac), (uintptr_t)(txdesc), (pck_after), \
                      (uintptr_t)__builtin_return_address(0)); \
} while (0)

#else

#define LP_TXW_ON 0
#define LP_TXW_RECORD(site, delta, ac, txdesc, pck_after) do { } while (0)

#endif

#endif
