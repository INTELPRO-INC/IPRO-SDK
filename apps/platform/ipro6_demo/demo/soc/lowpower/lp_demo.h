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
 * @file lp_demo.h
 * @brief IPRO6 Low Power demo — PDS Level 31 + DTIM wake (No BLE, No PSRAM)
 *
 * Ported into ipro6_demo from the standalone ipro_lp_test app. Gated by
 * CONFIG_DEMO_LOWPOWER; provides the `lp` shell command and the PDS/HBN app
 * flow (staged lp_fw Stage-1 image + DTIM re-sleep simulation).
 */

#ifndef __LP_DEMO_H__
#define __LP_DEMO_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Bring up the LP demo app flow.
 *
 * Initializes the hal_lp framework, stages the embedded lp_fw Stage-1 image to
 * its retained-OCRAM run address, registers the UART/SysTick wakeup hook, and
 * prints the boot-time HBN-RAM retention probe. Call once from main() after the
 * heap is up.
 */
void lp_demo_app_init(void);

/**
 * @brief Idle-hook entry — enters PDS when auto sleep is enabled.
 *
 * Call from vApplicationIdleHook(). No-op unless `lp auto on` was issued.
 */
void lp_demo_idle(void);

/* Replay tickless oversleep into the kernel clock (xTaskCatchUpTicks is only
 * legal with the scheduler live). Call from vApplicationIdleHook. */
void lp_tickless_tick_catchup(void);

/**
 * @brief Enter PDS Level 31 sleep (DTIM simulation)
 * @param sleep_ms Sleep duration per DTIM cycle, in milliseconds
 * @param cnt      DTIM re-sleep cycles before waking the full firmware
 *                 (0 => LP_FW_SIM_DTIM_CYCLES default)
 */
void lp_demo_sleep(uint32_t sleep_ms, uint32_t cnt);

/**
 * @brief Seed the LP (DTIM) parameter block from the connected STA.
 *
 * Reads the associated-AP context (BSSID / our MAC / channel / AID / beacon
 * interval) from the WiFi stack and writes it into iot2lp_para, plus the fixed
 * fields the lp_fw blob needs (beacon rate, WRAM RX-ring base). Called
 * automatically before every PDS entry; also exposed via `lp populate` for
 * bench verification. Returns 0 if associated, -1 otherwise (manual `lp mw`
 * seeds are left untouched). No-op returning -1 on No-WiFi builds.
 */
int lpfw_populate_from_sta(void);

/**
 * @brief Enable/disable automatic PDS in the idle hook
 */
void lp_demo_auto_enable(bool enable);

/**
 * @brief DTIM standby test over a live TCP connection (lp_tcpdtim.c).
 *
 * Connects to the tcpdtim host server, then loops: request a delayed wake
 * message, PDS-sleep, re-sleep on dataless wakes, count message-bearing wakes
 * until max_wakes. Traffic-free while sleeping (unlike ping/iperf, which set
 * TIM on the first beacon). Returns 0 if every round got its message.
 * Stub printing an error on No-WiFi/No-lwIP builds.
 */
int lp_tcpdtim_run(const char *host_ip, uint16_t port, uint32_t period_s,
                   uint32_t max_wakes, uint32_t dtim_ms, uint32_t cycles);

/**
 * @brief PDS sleep entry for the tickless idle hook (lp_tickless.c).
 *
 * IDLE-task/scheduler-suspended variant of lp_demo_sleep(): no PS handshake
 * (done once at tickless arm), no blocking waits, no ps_exit on wake — only
 * the non-blocking pre-sleep gates, the PDS trampoline (tick-compensating)
 * and the register-level MAC resume. ps_active mirrors lp_demo_sleep's
 * ps_announced gate: pass true only when the STA announced PS at arm time
 * (the MAC snapshot/resume dance faults on a never-PS-entered MAC).
 * Returns the HAL_LP_WAKEUP_* reason, or -1 if the sleep was aborted
 * (retry on the next idle pass); *slept_ms gets the measured sleep time —
 * the caller must vTaskStepTick it (this path cannot xTaskCatchUpTicks:
 * it asserts under a suspended scheduler). Only built on CONFIG_WIFI6 +
 * CONFIG_IPRO_WIFI_PS.
 */
int lp_demo_sleep_from_idle(uint32_t dtim_ms, uint32_t cycles, bool ps_active,
                            uint32_t *slept_ms);

/**
 * @brief FreeRTOS tickless DTIM standby (lp_tickless.c).
 *
 * start: connect to the tcpdtim host server, request a delayed wake command,
 * announce static PS and arm the vApplicationSleep hook — every sufficiently
 * long idle period then enters DTIM PDS. The hook disarms when the host's
 * TCP wake command arrives (or via stop). See lp_tickless.c for the flow.
 *
 * repeat: number of standby cycles to run back-to-back on one socket; 0 keeps
 * the original one-shot behaviour (end the run on the first wake command).
 * awake_s: full-power window between repeat cycles (0 -> 60 s default).
 */
int lp_tickless_start(const char *host, uint16_t port, uint32_t delay_s,
                      uint32_t dtim_ms, uint32_t repeat, uint32_t awake_s);
int lp_tickless_stop(void);
/* WiFi-disconnect hook: kicks the service task so an armed standby disarms
 * instead of DTIM-cycling deaf forever on a dead association. */
void lp_tickless_on_disconnect(void);

/**
 * @brief Check if auto PDS is enabled
 */
bool lp_demo_auto_is_enabled(void);

/**
 * @brief Get sleep statistics
 */
void lp_demo_get_stats(uint32_t *total_sleep_ms,
                       uint32_t *sleep_count,
                       uint32_t *last_sleep_ms,
                       int32_t *last_wakeup_reason);

/**
 * @brief Reset statistics
 */
void lp_demo_reset_stats(void);

#endif /* __LP_DEMO_H__ */
