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
 * @file ipro_coex.h
 * @brief WiFi/BT coexistence (PTA arbiter) control.
 *
 * Ported from bl_iot_sdk components/network/wifi_bt_coex, renamed to
 * ipro_coex. The structure is the reference's: a thin public API here, a
 * per-chip register backend behind ipro_coex_impl.h, a shared timing context,
 * and a console command.
 *
 * Deviations from the reference, all deliberate:
 *
 *  - struct wifi_bt_coex_ctx (here struct ipro_coex_ctx) moved out of the
 *    public header into the private ipro_coex_ctx.h. The reference exposes it
 *    here, which drags FreeRTOS.h and timers.h into every consumer -- macsw
 *    among them, which has no business depending on the OS headers. Nothing
 *    outside this module reads the struct.
 *
 *  - ipro_coex_init_default() is new. The reference gives no way to say "put
 *    the arbiter back the way hal_machw_init() leaves it", which is why that
 *    function had to poke the three registers raw. This is the call that
 *    replaces those pokes.
 */
#ifndef __IPRO_COEX_H__
#define __IPRO_COEX_H__

/// How force_wlan()/force_bt() are meant to win the medium.
enum IPRO_COEX_FORCE_MODE {
    /// Arbiter off; the loser's tx/rx abort bits are asserted outright.
    IPRO_COEX_FORCE_MODE_PTA_FORCE,
    /// Arbiter on, PTI disabled; the winner takes the fixed default priority.
    IPRO_COEX_FORCE_MODE_PTI_DEFAULT_FORCE,
    /// Arbiter on with PTI, plus a MAC CoexControl/CoexPTI programming pass.
    IPRO_COEX_FORCE_MODE_PTI_PRIORITY_FORCE,
    /// As above with the alternate CoexControl encoding (forced WLAN PTI).
    IPRO_COEX_FORCE_MODE_PTI_PRIORITY_FORCE2,
};

enum IPRO_COEX_EVENT {
    IPRO_COEX_EVENT_BT_A2DP_UNDERRUN,
};

/**
 * Kept for source compatibility with the reference, where calling it is what
 * stops the linker from dropping the CLI translation unit. Here the component
 * is linked whole-archive, so this is a no-op.
 */
int ipro_coex_cli_init(void);

/// Decode and print the PTA block and the MAC coex block.
int ipro_coex_dump_all(void);

/// Restore what a MAC reset leaves behind: arbitrated, PTI priority, no
/// simultaneous tx/rx. Called from hal_machw_init().
int ipro_coex_init_default(void);

/// Hand the medium to WiFi, using the mode set by ipro_coex_force_mode().
int ipro_coex_force_wlan(void);

/// Hand the medium to BT. WiFi will stop transmitting in the PTA_FORCE mode.
int ipro_coex_force_bt(void);

/// Select how the next force_wlan()/force_bt() arbitrates.
int ipro_coex_force_mode(enum IPRO_COEX_FORCE_MODE mode);

int ipro_coex_event_notify(enum IPRO_COEX_EVENT event, void *event_arg);

#endif /* __IPRO_COEX_H__ */
