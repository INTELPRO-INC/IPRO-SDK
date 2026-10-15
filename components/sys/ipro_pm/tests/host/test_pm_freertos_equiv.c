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

/* Golden equivalence of the FreeRTOS port with only the BLE radio.
 *
 * Built twice: -DGOLDEN links the port as it was before the participant /
 * arbiter refactor (golden/pm_freertos_v1.c, single pm_backend_* backend),
 * without it links the current port + arbiter with one executor participant.
 * Both drive the port hooks in the order vPortSuppressTicksAndSleep() calls
 * them through the same scenarios and print every call the port makes and
 * every value it returns; the Makefile diffs the two logs. */
#include <stdio.h>
#include <string.h>

#include "ipro_pm.h"
#include "pm_internal.h"

TickType_t host_tick;
uint64_t host_rtc;
uint64_t host_mtime;
int host_mtime_stopped;
uint32_t host_mtime_autostep;

/* ---- scenario ---- */
static ipro_pm_mode_t s_allowed;
static bool s_capable;      /* controller in the connected PDS profile */
static bool s_ctlr_up;      /* controller task exists */
static TickType_t s_kernel_idle;
static TickType_t s_projected;
static uint32_t s_wfi_deadline;
static pm_offer_t s_offer_result;

void host_wfi(void)
{
    printf("  wfi\n");
    host_mtime += 4000U; /* one tick */
}

TickType_t xTaskGetExpectedIdleTimeExcludingTask(TaskHandle_t task)
{
    (void)task;
    return s_kernel_idle;
}

/* ---- core surface used by the port ---- */
int ipro_pm_init(void)
{
    return 0;
}

ipro_pm_mode_t ipro_pm_allowed_mode(void)
{
    return s_allowed;
}

void pm_core_account(ipro_pm_mode_t mode, uint64_t us)
{
    printf("  account mode=%d us=%llu\n", (int)mode, (unsigned long long)us);
}

/* ---- radio ---- */
static TickType_t radio_projected(void)
{
    return s_projected;
}

static uint32_t radio_wfi_deadline(void)
{
    return s_wfi_deadline;
}

static void radio_skipped(void)
{
    printf("  window_skipped\n");
}

#if defined(GOLDEN)
void pm_backend_init(void) {}
TickType_t pm_backend_projected_idle_ticks(void) { return radio_projected(); }
uint32_t pm_backend_wfi_deadline_hus(void) { return radio_wfi_deadline(); }
void pm_backend_window_skipped(void) { radio_skipped(); }
bool pm_backend_pds_capable(void) { return s_capable; }
pm_offer_t pm_backend_offer_pds(uint32_t rtos_hus)
{
    printf("  offer rtos_hus=%u\n", rtos_hus);
    return s_offer_result;
}
static void radio_register(void) {}
#else
static pm_part_state_t radio_state(void)
{
    if (s_capable) {
        return PM_PART_EXECUTOR;
    }
    return s_ctlr_up ? PM_PART_BUSY : PM_PART_ABSENT;
}

static pm_offer_t radio_offer(uint32_t rtos_hus, uint32_t part_hus)
{
    if (part_hus != PM_DEADLINE_NONE) {
        printf("  UNEXPECTED part deadline %u\n", part_hus);
    }
    printf("  offer rtos_hus=%u\n", rtos_hus);
    return s_offer_result;
}

static const pm_participant_t s_radio = {
    .name = "ble",
    .projected_idle_ticks = radio_projected,
    .wfi_deadline_hus = radio_wfi_deadline,
    .pds_state = radio_state,
    .pds_deadline_hus = NULL,
    .offer = radio_offer,
    .window_skipped = radio_skipped,
};

static void radio_register(void)
{
    (void)pm_participant_register(&s_radio);
}
#endif

/* port hooks under test */
TickType_t xPortApplicationProjectIdleTime(TickType_t kernel_expected_ticks);
void vApplicationIdleHook(void);
TickType_t xPortApplicationClampIdleTime(TickType_t expected_ticks);
void vPortApplicationPreSleepRevalidate(TickType_t selected_ticks);
BaseType_t xPortApplicationPreSleepSkipWfi(void);
void vPortApplicationPostSleepMasked(void);
void vPortApplicationTicklessObservation(TickType_t selected_ticks, TickType_t completed_ticks,
                                         BaseType_t wake_by_timer, BaseType_t aborted);

extern volatile uint32_t ipro_pm_port_projected;
extern volatile uint32_t ipro_pm_port_offered;
extern volatile uint32_t ipro_pm_port_offer_rejected;
extern volatile uint32_t ipro_pm_port_short_wfi;
extern volatile uint32_t ipro_pm_port_tickless_wfi;
extern volatile uint32_t ipro_pm_port_aborts;
extern volatile uint32_t ipro_pm_port_retry_wfi;

/* One idle pass: prvIdleTask's two projections, the idle hook, then the
 * port's tickless sequence when the (projected) idle time reaches 2 ticks. */
static void idle_pass(const char *label)
{
    TickType_t expected;
    TickType_t clamped;

    printf("pass %s\n", label);
    expected = xPortApplicationProjectIdleTime(s_kernel_idle);
    printf("  project1=%u\n", (unsigned)expected);
    vApplicationIdleHook();
    expected = xPortApplicationProjectIdleTime(s_kernel_idle);
    printf("  project2=%u\n", (unsigned)expected);
    if (expected < 2U) {
        return;
    }
    clamped = xPortApplicationClampIdleTime(expected);
    printf("  clamp=%u\n", (unsigned)clamped);
    if (clamped < 2U) {
        return;
    }
    vPortApplicationPreSleepRevalidate(clamped);
    if (xPortApplicationPreSleepSkipWfi() != pdFALSE) {
        printf("  skip\n");
        vPortApplicationTicklessObservation(clamped, 0U, pdFALSE, pdTRUE);
        return;
    }
    host_wfi();
    vPortApplicationPostSleepMasked();
    vPortApplicationTicklessObservation(clamped, clamped - 1U, pdTRUE, pdFALSE);
}

static void scenario(const char *label, ipro_pm_mode_t allowed, bool ctlr_up, bool capable, TickType_t kernel,
                     TickType_t projected, uint32_t wfi_deadline, pm_offer_t offer)
{
    s_allowed = allowed;
    s_ctlr_up = ctlr_up;
    s_capable = capable;
    s_kernel_idle = kernel;
    s_projected = projected;
    s_wfi_deadline = wfi_deadline;
    s_offer_result = offer;
    idle_pass(label);
}

int main(void)
{
    radio_register();
    scenario("active", IPRO_PM_MODE_ACTIVE, true, true, 0U, 50U, UINT32_MAX, PM_OFFER_GRANTED);
    scenario("wfi-short", IPRO_PM_MODE_WFI, true, false, 1U, 0U, UINT32_MAX, PM_OFFER_REJECTED);
    scenario("wfi-tickless", IPRO_PM_MODE_WFI, true, false, 20U, 0U, UINT32_MAX, PM_OFFER_REJECTED);
    scenario("wfi-clamped", IPRO_PM_MODE_WFI, true, false, 20U, 0U, 9000U, PM_OFFER_REJECTED);
    scenario("wfi-clamped-out", IPRO_PM_MODE_WFI, true, false, 20U, 0U, 5000U, PM_OFFER_REJECTED);
    scenario("pds-not-capable", IPRO_PM_MODE_PDS, true, false, 20U, 0U, UINT32_MAX, PM_OFFER_GRANTED);
    scenario("pds-ctlr-down", IPRO_PM_MODE_PDS, false, false, 20U, 0U, UINT32_MAX, PM_OFFER_GRANTED);
    scenario("pds-granted-projected", IPRO_PM_MODE_PDS, true, true, 1U, 400U, 900000U, PM_OFFER_GRANTED);
    scenario("pds-granted-kernel", IPRO_PM_MODE_PDS, true, true, 300U, 0U, UINT32_MAX, PM_OFFER_GRANTED);
    scenario("pds-retry-projected", IPRO_PM_MODE_PDS, true, true, 1U, 400U, 900000U, PM_OFFER_RETRY);
    scenario("pds-after-retry", IPRO_PM_MODE_PDS, true, true, 1U, 400U, 900000U, PM_OFFER_GRANTED);
    scenario("pds-retry-kernel", IPRO_PM_MODE_PDS, true, true, 300U, 0U, UINT32_MAX, PM_OFFER_RETRY);
    scenario("pds-rejected-projected", IPRO_PM_MODE_PDS, true, true, 1U, 400U, 900000U, PM_OFFER_REJECTED);
    scenario("pds-rejected-kernel", IPRO_PM_MODE_PDS, true, true, 300U, 0U, UINT32_MAX, PM_OFFER_REJECTED);
    scenario("pds-huge", IPRO_PM_MODE_PDS, true, true, 1U, 0xFFFFFFF0U, UINT32_MAX, PM_OFFER_GRANTED);
    scenario("native", IPRO_PM_MODE_NATIVE, true, true, 1U, 400U, 900000U, PM_OFFER_GRANTED);
    printf("counters projected=%u offered=%u rejected=%u short_wfi=%u tickless_wfi=%u aborts=%u retry_wfi=%u\n",
           ipro_pm_port_projected, ipro_pm_port_offered, ipro_pm_port_offer_rejected, ipro_pm_port_short_wfi,
           ipro_pm_port_tickless_wfi, ipro_pm_port_aborts, ipro_pm_port_retry_wfi);
    return 0;
}
