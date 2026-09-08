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
 * @file vbus.h
 * @brief Is the USB-C connector powered? Read from the USB transceiver.
 *
 * The charger's PGOOD and CHG outputs drive LEDs D14/D15 and reach no GPIO
 * (BOARD_HAS_CHARGE_STATUS is 0), so the obvious way to answer this does not
 * exist on this board. The USB transceiver does have the answer: VBUS_DET,
 * bit 31 of the transceiver interface status word, is a comparator on the
 * SoC's own VBUS pin. It reports ANY powered source - an enumerated host or a
 * dumb wall charger - because it is watching the rail, not the data lines.
 *
 * >>> WHAT THIS IS NOT <<<
 *
 * Not charge state, and not a battery gauge. It says "the connector has 5 V on
 * it" and nothing more. Whether the cell is charging, full, or absent is still
 * unreadable - see the power-tree notes in the board config.
 *
 * >>> WHEN THE ANSWER IS INTERESTING <<<
 *
 * Follow the power tree before using this to gate anything:
 *
 *     CN1 -> OVP -> VBUS -> charger -> VBAT -> buck-boost -> 3V3_SOC -> SoC
 *
 * The SoC is downstream of VBAT, not of VBUS. So:
 *
 *   - No cell fitted: VBUS is the only way the board is powered at all, so it
 *     is true by the time any code runs. Gating on it is a no-op - plugging
 *     the cable IS the power-on event.
 *   - Cell fitted: the board runs off the battery whether or not the cable is
 *     in, and the question becomes real. This is the case worth gating on: it
 *     distinguishes "running on the cell" from "running on the wall".
 *
 * The transceiver is powered up for the comparator and left there. The D+
 * pull-up (_GLB_USB_ENUM) is deliberately NOT asserted: it would announce a
 * device to the host, which would then fail to enumerate one that has no
 * endpoints, and the host reports that as a broken device. Sensing VBUS does
 * not need it.
 */
#ifndef VBUS_H
#define VBUS_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Power the USB transceiver so VBUS_DET reads true state.
 *
 * Idempotent, cheap, and safe to call before the scheduler starts. Called
 * automatically by vbus_present(); exposed for callers that want the settling
 * to have happened earlier.
 */
void vbus_init(void);

/** @brief True while the USB-C connector has power on it. */
bool vbus_present(void);

/**
 * @brief Block until the connector is powered, or until the wait runs out.
 *
 * Returns immediately if VBUS is already there. Requires the scheduler to be
 * running - it sleeps between polls rather than spinning, so the console and
 * any other task stay responsive for the whole wait.
 *
 * @param timeout_ms  How long to wait. 0 waits forever.
 * @return true if VBUS is present on return, false if the wait expired first.
 */
bool vbus_wait(uint32_t timeout_ms);

#endif /* VBUS_H */
