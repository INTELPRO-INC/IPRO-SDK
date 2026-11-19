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

#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "freertos_app_hooks.h"
#include "openthread_port.h"
#include <openthread/cli.h>
#include <openthread/thread.h>
#include <openthread/dataset.h>
#include <openthread/ip6.h>
#ifdef CONFIG_SHELL
#include "shell.h"
#endif
#ifdef CONFIG_DEBUG_CONSOLE_USB
#include "usb_console.h"
#endif
#include "br_cli_timing_store.h"
#include "ipro_log.h"
#include "console_guard.h"
#include "br_infra.h"
#include "br_thread_netif.h"
#include "br_udp.h"
#include "br_dua_proxy.h"
#include "br_network_startup.h"
#ifdef CONFIG_THREAD_BR_BLE_MANAGEMENT
#include "br_em_recovery.h"
#endif
#include "br_rcp_recovery.h"
#include "br_cpp_runtime.h"
#include "br_thread_activation.h"
#include "wifi_backhaul.h"
#ifdef CONFIG_THREAD_BR_BLE_MANAGEMENT
#include "br_ble_management.h"
#ifdef CONFIG_THREAD_BR_LAN_MANAGEMENT
#include "br_lan_management.h"
#endif
#endif
#ifdef IPRO_MATTER_PACKAGE_ACTIVE
#include "br_matter_runtime.h"
#endif

static StaticQueue_t sCliQueueControl;
/* As long as OpenThread's CLI takes (a full operational dataset as hex with a
 * MGMT set command is ~300 chars; 384 in the non-Matter defconfigs). */
#ifdef CONFIG_OPENTHREAD_CLI_MAX_LINE_LENGTH
#define BR_CLI_LINE_MAX CONFIG_OPENTHREAD_CLI_MAX_LINE_LENGTH
#else
#define BR_CLI_LINE_MAX 256
#endif
_Static_assert(BR_CLI_LINE_MAX <= SHELL_CMD_SIZE, "an ot line longer than the shell's would be cut there");
struct cli_item {
    char line[BR_CLI_LINE_MAX];
    struct br_cli_timing_event timing;
};
static uint8_t sCliQueueStorage[2 * sizeof(struct cli_item)];
static QueueHandle_t sCliQueue;
/* OT-task-only flush of any synchronous CLI fragment without a final LF. */
extern void otrCliOutputFlush(void);
#ifdef CONFIG_SHELL
static struct br_cli_timing_store sCliTiming, sCliSnapshot;
static uint32_t sCliSequence, sCliRejected;
static int sCliDumpBusy;
#endif

void otrAppProcess(ot_system_event_t events)
{
    struct cli_item item;
    (void)events;
    otInstance *instance = otrGetInstance();
    br_dua_refresh(instance);
    static bool boot_checked;
    if (!boot_checked) {
        boot_checked = true;
        br_rcp_recovery_on_ready();
        /* Only resume an explicitly saved deployment with an existing dataset.
         * Never form or commission a new network as a startup side effect. */
        if (br_wifi_thread_autostart() && otDatasetIsCommissioned(instance)) {
            otError error = otIp6SetEnabled(instance, true);
            if (error == OT_ERROR_NONE) error = otThreadSetEnabled(instance, true);
            IPRO_LOGI("Backhaul", "Thread resume result=%u", (unsigned)error);
        }
    }
    br_infra_process(instance, br_thread_netif_process(instance));
    br_udp_process(instance);
    while (xQueueReceive(sCliQueue, &item, 0) == pdTRUE) {
        item.timing.started = (uint32_t)xTaskGetTickCount();
        if (br_thread_activation_cli_allowed(item.line)) otCliInputLine(item.line);
        else IPRO_LOGW("MatterBR", "Thread configuration is owned by Matter; CLI mutation rejected");
        item.timing.returned = (uint32_t)xTaskGetTickCount();
        otrCliOutputFlush();
#ifdef CONFIG_SHELL
        taskENTER_CRITICAL();
        br_cli_timing_push(&sCliTiming, &item.timing);
        taskEXIT_CRITICAL();
#endif
    }
}

static void start_task(void *arg)
{
    (void)arg;
#ifdef CONFIG_DEBUG_CONSOLE_USB
    /* Mirror log output and shell input on a USB CDC-ACM port. It needs the
     * scheduler, so it comes up here rather than beside the UART shell. */
    if (usb_console_init() != 0) IPRO_LOGE("BR-USB", "USB console unavailable");
#endif
    sCliQueue = xQueueCreateStatic(2, sizeof(struct cli_item), sCliQueueStorage, &sCliQueueControl);
    br_rcp_recovery_boot();
    if (br_infra_prepare() != 0) {
        puts("Border infrastructure initialization failed");
        vTaskDelete(NULL);
        return;
    }
    const int startup_error = br_network_startup();
    if (startup_error != 0) {
        IPRO_LOGE("Backhaul", "Network startup stopped, error=%d", startup_error);
        vTaskDelete(NULL);
        return;
    }
#ifdef CONFIG_THREAD_BR_BLE_MANAGEMENT
    if (br_ble_management_start() != 0) IPRO_LOGE("BR-BLE", "Management task unavailable");
#endif
#ifdef CONFIG_THREAD_BR_LAN_MANAGEMENT
    /* WPA and BLE initialization have large temporary heaps. Create the TLS
     * worker only after boot connect cleanup, not merely manager-ready. */
    struct br_wifi_boot_status boot;
    do {
        vTaskDelay(pdMS_TO_TICKS(200));
        br_wifi_boot_status_get(&boot);
    } while (boot.phase != BR_WIFI_BOOT_COMPLETE && boot.phase != BR_WIFI_BOOT_FAILED);
    for (unsigned wait = 0; wait < 100 && !br_ble_management_ready(); ++wait)
        vTaskDelay(pdMS_TO_TICKS(100));
    if (!br_ble_management_ready() || br_lan_start() != 0)
        IPRO_LOGE("BR-LAN", "Management task unavailable");
#endif
    otInstance *instance = otrGetInstance();
    if (instance) {
        printf("IPRO6S RCP host ready, Thread version %u\r\n", otThreadGetVersion());
    } else if (otrIsStopped()) {
        printf("IPRO6S BR up without Thread: RCP failure (rcp_status)\r\n");
    }
#ifdef IPRO_MATTER_PACKAGE_ACTIVE
    IPRO_LOGI("MatterBR", "Waiting for SDK network; local bootstrap command: br_wifi_provision");
    uint32_t matter_error;
    enum br_matter_runtime_status state;
    while ((state = br_matter_runtime_step(&matter_error)) == BR_MATTER_RUNTIME_WAITING)
        vTaskDelay(pdMS_TO_TICKS(100));
    IPRO_LOGI("MatterBR", "Runtime state=%u error=0x%08lx heap=%u stack_free_words=%u",
              (unsigned)state, (unsigned long)matter_error, (unsigned)xPortGetFreeHeapSize(),
              (unsigned)uxTaskGetStackHighWaterMark(NULL));
#endif
    vTaskDelete(NULL);
}

#ifdef CONFIG_SHELL
static void cmd_ot(int argc, char **argv)
{
    struct cli_item item = {0};
    item.timing.entered = (uint32_t)xTaskGetTickCount();
    char *line = item.line;
    size_t used = 0;
    if (!otrGetInstance()) {
        if (otrIsStopped()) puts("OpenThread stopped after an RCP failure (rcp_status)");
        return;
    }
    for (int i = 1; i < argc; ++i) {
        size_t len = strlen(argv[i]);
        if (used + len + 2 > sizeof(item.line)) {
            printf("OpenThread CLI line too long (max %u)\r\n", (unsigned)sizeof(item.line) - 1);
            return;
        }
        if (used) line[used++] = ' ';
        memcpy(line + used, argv[i], len);
        used += len;
    }
    line[used] = 0;
    taskENTER_CRITICAL();
    item.timing.sequence = ++sCliSequence;
    taskEXIT_CRITICAL();
    /* Includes any preemption before the nonblocking queue send accepts it. */
    item.timing.submitted = (uint32_t)xTaskGetTickCount();
    if (xQueueSend(sCliQueue, &item, 0) != pdTRUE) {
        taskENTER_CRITICAL();
        sCliRejected++;
        taskEXIT_CRITICAL();
        puts("OpenThread CLI busy");
        return;
    }
    otSysEventSignalPending();
}
SHELL_CMD_EXPORT_ALIAS(cmd_ot, ot, OpenThread CLI);

static void cmd_cli_timing(int argc, char **argv)
{
    uint32_t sequence, rejected;
    (void)argc; (void)argv;
    taskENTER_CRITICAL();
    if (sCliDumpBusy) {
        taskEXIT_CRITICAL();
        return;
    }
    sCliDumpBusy = 1;
    sCliSnapshot = sCliTiming;
    sequence = sCliSequence;
    rejected = sCliRejected;
    taskEXIT_CRITICAL();
    IPRO_LOGI("BR-CLI", "count=%u overwritten=%lu attempts=%lu rejected=%lu tick_hz=%lu console_drops=%lu",
              sCliSnapshot.count, (unsigned long)sCliSnapshot.overwritten,
              (unsigned long)sequence, (unsigned long)rejected, (unsigned long)configTICK_RATE_HZ,
              (unsigned long)newlibc_console_drops());
    unsigned first = (sCliSnapshot.next + BR_CLI_TIMING_CAPACITY - sCliSnapshot.count) % BR_CLI_TIMING_CAPACITY;
    for (unsigned i = 0; i < sCliSnapshot.count; ++i) {
        const struct br_cli_timing_event *e = &sCliSnapshot.events[(first + i) % BR_CLI_TIMING_CAPACITY];
        IPRO_LOGI("BR-CLI", "seq=%lu entry=%lu submit=%lu start=%lu return=%lu",
                  (unsigned long)e->sequence, (unsigned long)e->entered,
                  (unsigned long)e->submitted, (unsigned long)e->started, (unsigned long)e->returned);
    }
    /* Query after the full dump so inline profiles expose the shell's peak
     * stack usage, without allocating a task-list buffer or another worker. */
    IPRO_LOGI("BR-CLI", "stack_free_words=%lu",
              (unsigned long)uxTaskGetStackHighWaterMark(NULL));
    taskENTER_CRITICAL();
    sCliDumpBusy = 0;
    taskEXIT_CRITICAL();
}
SHELL_CMD_EXPORT_ALIAS(cmd_cli_timing, br_cli_timing, Dump bounded CLI timing metadata);
#endif

int main(void)
{
    ipro_platform_init();
    if (br_cpp_runtime_init() != 0) {
        IPRO_LOGE("Backhaul", "C++ constructor initialization failed");
        return -1;
    }
    if (!newlibc_console_init()) return -1;
#ifdef CONFIG_THREAD_BR_BLE_MANAGEMENT
    {
        int ble_rc = br_ble_controller_prepare();
        if (ble_rc != 0) {
            IPRO_LOGE("BR-BLE", "Controller initialization failed (%d)", ble_rc);
            /* EM self-test failures clear on the next reset far more often
             * than not: reboot (bounded) instead of halting before the scheduler. */
            br_em_recovery_on_failure(ble_rc);
            return -1;
        }
        br_em_recovery_on_success();
    }
#endif
    /* Matter startup includes C++ server/crypto initialization. Commissioning
     * then runs on its own event task; this temporary task is deleted above. */
#ifdef IPRO_MATTER_PACKAGE_ACTIVE
    const unsigned startup_stack_words = 2048;
#else
    const unsigned startup_stack_words = 1024;
#endif
    if (xTaskCreate(start_task, "br_start", startup_stack_words, NULL, 5, NULL) != pdPASS) return -1;
    vTaskStartScheduler();
    return -1;
}
