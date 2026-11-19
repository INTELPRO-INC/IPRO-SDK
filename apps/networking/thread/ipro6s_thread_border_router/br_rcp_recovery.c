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
 * @file br_rcp_recovery.c
 * @brief Counted, backed-off BR reboot after a fatal RCP error; see br_rcp_recovery.h.
 */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "br_rcp_recovery.h"
#include "br_dhcp6_pd.h"
#include "br_dua_proxy.h"
#include "br_infra.h"
#include "br_mdns.h"
#include "br_multicast.h"
#include "br_thread_netif.h"
#include "br_udp.h"
#include "drv_common.h"
#include "hal_hbnram_mgnt.h"
#include "hal_sys.h"
#include "openthread_port.h"
#include "ot_rcp_health.h"
#ifdef CONFIG_THREAD_BR_BLE_MANAGEMENT
#include "br_ble_management.h"
#endif
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

/* reserved[1] layout: upper 16 bits tag, lower 16 bits consecutive stops.
 * The tag guards against stale HBN RAM from an image that never wrote it. */
#define RCP_RECOVERY_SLOT 1
#define RCP_RECOVERY_TAG  0x52430000u /* 'RC' */
#define RCP_RECOVERY_MASK 0xFFFF0000u
#define OT_EXIT_RADIO_SPINEL_INCOMPATIBLE_CODE 3
#define REBOOT_TASK_STACK_WORDS 768u
#define REBOOT_BACKSTOP_MS 3000u /* the BLE shutdown may block; reboot anyway */

/* The reboot task is created only when needed (a static stack would cost RAM
 * for the BR's whole life). A static safety timer is armed first: a failed
 * allocation ends in the malloc-failed hook, which never returns. */
#define REBOOT_TASK_HEAP_BYTES (REBOOT_TASK_STACK_WORDS * sizeof(StackType_t) + sizeof(StaticTask_t) + 256u)
#define REBOOT_SAFETY_EXTRA_MS 1000u /* after the task's own backstop */

static StaticTimer_t s_safety_buf;
static volatile bool s_reboot_pending;
static volatile TickType_t s_reboot_at;
static StaticTimer_t s_backstop_buf;
static StaticTimer_t s_healthy_buf;
static TimerHandle_t s_healthy;
static volatile bool s_hold;

static uint32_t read_count(void)
{
    uint32_t word = hal_hbnram_sysinfo->reserved[RCP_RECOVERY_SLOT];
    return ((word & RCP_RECOVERY_MASK) == RCP_RECOVERY_TAG) ? (word & 0xFFFFu) : 0u;
}

static void write_count(uint32_t count)
{
    hal_hbnram_sysinfo->reserved[RCP_RECOVERY_SLOT] = RCP_RECOVERY_TAG | (count & 0xFFFFu);
}

uint32_t br_rcp_recovery_count(void)
{
    return read_count();
}

uint32_t br_rcp_recovery_delay_s(uint32_t count, bool in_ot_task)
{
    uint32_t shift;
    uint32_t delay;

    /* Another task was parked with its own locks (BLE/LAN management, CHIP):
     * waiting longer only keeps those services wedged. */
    if (!in_ot_task || count <= BR_RCP_FAST_REBOOTS) {
        return BR_RCP_FAST_DELAY_S;
    }
    shift = count - BR_RCP_FAST_REBOOTS - 1u;
    if (shift > 8u) {
        shift = 8u;
    }
    delay = BR_RCP_SLOW_DELAY_S << shift;
    return delay > BR_RCP_MAX_DELAY_S ? BR_RCP_MAX_DELAY_S : delay;
}

static void backstop_cb(TimerHandle_t timer)
{
    (void)timer;
    hal_reboot();
}

/* reboot_task did not run or did not finish: reboot from the timer task,
 * without the BLE shutdown. */
static void safety_cb(TimerHandle_t timer)
{
    (void)timer;
    if (s_hold) {
        s_reboot_pending = false;
        return;
    }
    printf("[BR-RCP] rebooting to restart Thread (no BLE shutdown)\r\n");
    hal_reboot();
}

static void reboot_task(void *arg);

static void schedule_reboot(uint32_t delay_s)
{
    TimerHandle_t safety;
    HeapStats_t heap;

    s_reboot_at = xTaskGetTickCount() + pdMS_TO_TICKS(delay_s * 1000u);
    s_reboot_pending = true;
    safety = xTimerCreateStatic("br_rcp_sf", pdMS_TO_TICKS(delay_s * 1000u + REBOOT_BACKSTOP_MS + REBOOT_SAFETY_EXTRA_MS),
                                pdFALSE, NULL, safety_cb, &s_safety_buf);
    if (safety == NULL || xTimerStart(safety, 0) != pdPASS) {
        hal_reboot();
    }
    /* The task shuts BLE down before the reboot; without it the timer reboots. */
    vPortGetHeapStats(&heap);
    if (heap.xSizeOfLargestFreeBlockInBytes >= REBOOT_TASK_HEAP_BYTES) {
        (void)xTaskCreate(reboot_task, "br_rcp", REBOOT_TASK_STACK_WORDS, NULL, configMAX_PRIORITIES - 2, NULL);
    }
}

/* Nothing will drain what the LAN side queued for OpenThread, and LAN hosts
 * must stop routing to it. Queue work only first (frees heap, takes no lock);
 * the route withdrawal can block on the lwIP core lock. */
static void stop_lan_side(void)
{
    br_infra_drop_queued();
    br_mdns_drop_queued();
    br_udp_drop_queued();
    br_dua_drop_queued();
    br_multicast_drop_queued();
    br_thread_netif_drop_queued();
}

static void reboot_task(void *arg)
{
    TimerHandle_t backstop;

    (void)arg;
    for (TickType_t now = xTaskGetTickCount(); (int32_t)(s_reboot_at - now) > 0; now = xTaskGetTickCount()) {
        vTaskDelay(s_reboot_at - now);
    }
    if (s_hold) {
        printf("[BR-RCP] reboot held\r\n");
        s_reboot_pending = false;
        vTaskDelete(NULL);
    }
    printf("[BR-RCP] rebooting to restart Thread\r\n");
    backstop = xTimerCreateStatic("br_rcp_bs", pdMS_TO_TICKS(REBOOT_BACKSTOP_MS), pdFALSE, NULL, backstop_cb,
                                  &s_backstop_buf);
    if (backstop) {
        xTimerStart(backstop, 0);
    }
#ifdef CONFIG_THREAD_BR_BLE_MANAGEMENT
    /* As the other reboot paths: no BLE scheduler owning EM across a warm reset. */
    br_ble_management_quiesce();
#endif
    arch_delay_ms(100); /* let the UART drain */
    hal_reboot();
}

static void healthy_cb(TimerHandle_t timer)
{
    uint32_t count = read_count();

    (void)timer;
    if (otrIsStopped()) {
        return;
    }
    if (count) {
        printf("[BR-RCP] Thread up for %u s; cleared %lu earlier stop(s)\r\n", (unsigned)BR_RCP_HEALTHY_CLEAR_S,
               (unsigned long)count);
    }
    write_count(0);
}

/* Runs in the failing task, OpenThread mutex held: no OpenThread APIs. */
void ot_rcp_on_fatal(const ot_rcp_fault_t *fault)
{
    uint32_t count;
    uint32_t delay;

    if (s_healthy) {
        xTimerStop(s_healthy, 0);
    }
    stop_lan_side();
    if (s_hold) {
        printf("[BR-RCP] Thread stopped (exit=%d cause=%s); recovery held (rcpcon), reboot to restore\r\n",
               fault->exit_code, fault->cause);
    } else if (fault->exit_code == OT_EXIT_RADIO_SPINEL_INCOMPATIBLE_CODE) {
        /* A wrong RCP image does not fix itself: no reboot loop. */
        printf("[BR-RCP] Thread stopped: the RCP is incompatible (%s); flash the RCP, then reboot\r\n",
               fault->cause);
    } else {
        count = read_count() + 1u;
        delay = br_rcp_recovery_delay_s(count, fault->in_ot_task);
        write_count(count);
        printf("[BR-RCP] Thread stopped (exit=%d cause=%s task=%s); reboot %lu in %lu s\r\n", fault->exit_code,
               fault->cause, fault->task, (unsigned long)count, (unsigned long)delay);
        schedule_reboot(delay);
    }
    /* After any reboot is scheduled: these can block on the lwIP core lock. */
    br_dhcp6_pd_stop();
    printf("[BR-RCP] withdrew %u Thread route(s) from the LAN\r\n", br_infra_withdraw_routes());
}

void br_rcp_recovery_hold(void)
{
    s_hold = true;
}

void br_rcp_recovery_boot(void)
{
    uint32_t count = read_count();

    if (count) {
        printf("[BR-RCP] boot after %lu consecutive Thread stop(s)\r\n", (unsigned long)count);
    }
}

void br_rcp_recovery_on_ready(void)
{
    if (otrIsStopped() || s_healthy) {
        return;
    }
    s_healthy = xTimerCreateStatic("br_rcp_ok", pdMS_TO_TICKS(BR_RCP_HEALTHY_CLEAR_S * 1000u), pdFALSE, NULL,
                                   healthy_cb, &s_healthy_buf);
    if (s_healthy) {
        xTimerStart(s_healthy, 0);
    }
}

/* OpenThread's DieNow() calls exit(), and newlib's exit() runs the atexit
 * handlers (C++ destructors of the Matter build) before _exit(): take the
 * fatal over first (linked with -Wl,--wrap=exit). */
extern void __real_exit(int status) __attribute__((noreturn));
extern void newlibc_fatal_hook(const char *cause, int status, const char *file, int line);

void __attribute__((noreturn)) __wrap_exit(int status)
{
    newlibc_fatal_hook("exit", status, NULL, 0);
    __real_exit(status);
}

#ifdef CONFIG_SHELL
static void cmd_rcp_status(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    ot_rcp_print_status();
    printf("BR: consecutive Thread stops=%lu%s", (unsigned long)read_count(), s_hold ? ", recovery held" : "");
    if (s_reboot_pending) {
        TickType_t now = xTaskGetTickCount();
        TickType_t left = ((int32_t)(s_reboot_at - now) > 0) ? (s_reboot_at - now) : 0;
        printf(", reboot in %lu s", (unsigned long)(left * portTICK_PERIOD_MS / 1000u));
    }
    printf("\r\n");
}
SHELL_CMD_EXPORT_ALIAS(cmd_rcp_status, rcp_status, RCP link state and recovery counters);
#endif
