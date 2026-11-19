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
 * @file ot_pm.h
 * @brief OpenThread (802.15.4 radio) as an ipro_pm participant.
 *
 * With CONFIG_IPRO_PM_THREAD the OpenThread task registers the radio with the
 * power manager: platform PDS31 is entered between radio activity (a SED's
 * data polls, an SSED's CSL sample windows), sized to the next microsecond
 * alarm, with the MAC kept and the RF warmed up again afterwards.
 */
#ifndef OT_PM_H_
#define OT_PM_H_

#include <stdbool.h>
#include <stdint.h>

#include <openthread/instance.h>

#include "ieee802154.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Register with ipro_pm (OT task, after the instance is up). */
void ot_pm_init(otInstance *instance);
/** OT task, OT lock held, every pass: work a PDS wake handed over (poll alignment). */
void ot_pm_process(void);

/* ---- radio adapter (ot_radio_ieee802154.c) ---- */
#define OT_RADIO_PM_BUSY_TX       (1U << 0) /* a frame handed to the HAL, no TxDone yet */
#define OT_RADIO_PM_BUSY_RX_QUEUE (1U << 1) /* received frames not given to OpenThread */
#define OT_RADIO_PM_BUSY_SCAN     (1U << 2) /* energy scan */
#define OT_RADIO_PM_BUSY_RESTORE  (1U << 3) /* radio or RF restore still to run */

uint32_t ot_radio_pm_busy(void);
bool ot_radio_pm_enabled(void);
/** After a PDS that powered the RF down: warm it up before the next radio use. */
void ot_radio_pm_request_rf_restore(void);
/** 802.15.4 PA setup on the current channel / power (after the RF is up); IRQs may be masked. */
void ot_radio_pm_rf_zb_config(void);
/** The MAC lost its configuration: full reset from the OT task. */
ieee802154_error_t ot_radio_reinit_for_lp_wake(void);
/** The same, without the RF / BZ PHY re-init (BLE is live on the shared RF). */
ieee802154_error_t ot_radio_reinit_for_lp_wake_keep_rf(void);

/* ---- microsecond alarm (alarm.c), IRQs masked ---- */
bool ot_alarm_micro_deadline(uint32_t *deadline);
void ot_alarm_micro_suspend(void);
void ot_alarm_micro_resume(void);

#ifdef __cplusplus
}
#endif

#endif /* OT_PM_H_ */
