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
 * @file coex_bz.h
 * @brief BLE and 802.15.4 on the IPRO7's one radio: the PTA arbiter.
 *
 * The BLE controller and the 802.15.4 MAC share the RF front end and the
 * baseband ("BZ").  A digital arbiter (PTA) grants the radio per request by
 * priority: the BLE core raises its own, the MAC's come from its
 * COEX_PRIORITY register (TX, a unicast being received, a frame just
 * synchronised, idle listening).  This module is the only owner of the
 * arbiter enable (GLB BZ_COEX_CTRL) and of those priorities.
 *
 * The RF stays in the 802.15.4 mode the OpenThread port sets
 * (IPRO7_RF_MODE_ZB_ONLY): switching it to BZ_COEX breaks the 802.15.4 PA
 * setup (every unicast NoAck, Matter bench); the arbiter is independent of
 * the analog mode.
 */
#ifndef COEX_BZ_H_
#define COEX_BZ_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 802.15.4 request priorities, 0..15 (the BLE core's connection events are
 *  ~7 on this scale, advertising ~2-3). */
typedef struct {
    uint8_t tx;                /**< transmitting (incl. CSMA) */
    uint8_t rx_unicast;        /**< receiving a frame addressed to us */
    uint8_t rx_sfd;            /**< a frame just synchronised */
    uint8_t rx_idle;           /**< listening */
    bool rx_on_after_abort;    /**< back to receive after the arbiter took the radio */
} coex_bz_pri_t;

/**
 * Enable the arbiter with these 802.15.4 priorities (NULL: the Kconfig
 * defaults).  After the 802.15.4 driver is initialised; idempotent.
 */
int coex_bz_enable(const coex_bz_pri_t *pri);

/** Arbiter off (802.15.4 alone on the radio again). */
void coex_bz_disable(void);

/** The arbiter is enabled (by coex_bz_enable()). */
bool coex_bz_enabled(void);

/**
 * After a platform PDS or a MAC reset (IRQs masked): write the arbiter
 * enable (GLB, outside the MAC restore) and the MAC priorities back if they
 * were reset.  A TX priority the MAC escalated after arbiter aborts is kept.
 * Returns true when something had to be written.
 */
bool coex_bz_resume(void);

#ifdef __cplusplus
}
#endif

#endif /* COEX_BZ_H_ */
