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

#include <generated/autoconf.h>
#include "br_factory_reset.h"
#include "br_lan_management.h"
#include "br_wifi_storage.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ipro_log.h"
#include <easyflash.h>
#include <stddef.h>
#if defined(CONFIG_SHELL) && !defined(CONFIG_MATTER_ENABLE)
#include "shell.h"
#include <stdio.h>
#include <string.h>
#ifdef CONFIG_THREAD_BR_BLE_MANAGEMENT
#include "br_ble_management.h"
#endif
#endif

#define TAG "BR-RESET"

/* Presence is the request; the value is a fixed, secret-free marker.
 * sPending never clears at runtime: the reboot applies the journal.
 * sIncomplete is written once at boot, before any management task exists. */
static const unsigned char sJournal[] = {'F', 'R', 1};
static bool sPending, sIncomplete;

int br_factory_reset_request(void)
{
    /* Staged write with read-back verification: success means committed. */
    const int error = ef_env_set_checked(BR_FACTORY_RESET_KEY, sJournal, sizeof(sJournal));
    if (error != EF_NO_ERR) {
        /* A failed write may still have landed; do not let an unexpected
         * later boot erase what the requester was told survived. */
        const int cleanup = ef_env_del_checked(BR_FACTORY_RESET_KEY);
        IPRO_LOGE(TAG, "Factory reset not saved, error=%d cleanup=%d", error, cleanup);
        return error;
    }
    taskENTER_CRITICAL();
    sPending = true;
    taskEXIT_CRITICAL();
    IPRO_LOGI(TAG, "Factory reset saved; erasing on the next boot");
    return 0;
}

bool br_factory_reset_pending(void)
{
    taskENTER_CRITICAL();
    bool pending = sPending;
    taskEXIT_CRITICAL();
    return pending;
}

struct erase_step {
    const char *key;
    bool prefix;
};
/* Checked deletes verify absence and are idempotent, so a retry after any
 * partial failure is safe. The journal itself is deleted last, and only once
 * every step succeeded. */
static const struct erase_step sSteps[] = {
    {"ot-", true},        /* Datasets, network info, activation journal. */
    {BR_WIFI_KEY, false}, /* Wi-Fi profile, also the Thread autostart flag. */
    {BR_LAN_KEY, false},  /* LAN TLS identity/token; regenerated on use. */
    {"bt/", true},        /* BLE bonds/CCC; the identity is MAC-derived. */
};

static int erase(const char *key, bool prefix)
{
    const int error = prefix ? ef_env_del_prefix_checked(key) : ef_env_del_checked(key);
    return error == EF_ENV_NOT_FOUND ? EF_NO_ERR : error;
}

enum br_factory_reset_boot br_factory_reset_boot_apply(void)
{
    const char *step = "init";
    int error = easyflash_init();
    if (error == EF_NO_ERR) {
        step = "journal";
        error = ef_env_get_checked(BR_FACTORY_RESET_KEY, 0, NULL, 0, NULL, NULL);
        if (error == EF_ENV_NOT_FOUND) return BR_FACTORY_RESET_NONE;
    }
    if (error == EF_NO_ERR) {
        /* Do not stop at a failed namespace: the later credential keys must
         * still be revoked this boot. The first failure is reported. */
        for (size_t i = 0; i < sizeof(sSteps) / sizeof(sSteps[0]); ++i) {
            const int step_error = erase(sSteps[i].key, sSteps[i].prefix);
            if (step_error != EF_NO_ERR && error == EF_NO_ERR) { error = step_error; step = sSteps[i].key; }
        }
        if (error == EF_NO_ERR) { step = BR_FACTORY_RESET_KEY; error = erase(step, false); }
    }
    if (error != EF_NO_ERR) {
        sIncomplete = true;
        IPRO_LOGE(TAG, "Factory reset failed at %s, error=%d; retrying on the next boot", step, error);
        return BR_FACTORY_RESET_FAILED;
    }
    IPRO_LOGI(TAG, "Factory reset applied: Thread, Wi-Fi, LAN identity and BLE bonds erased");
    return BR_FACTORY_RESET_APPLIED;
}

bool br_factory_reset_incomplete(void) { return sIncomplete; }

#if defined(CONFIG_SHELL) && !defined(CONFIG_MATTER_ENABLE)
extern void hal_reboot(void);

/* Local recovery on the private console: same journal as the remote command.
 * A Matter BR owns its own reset flow and does not export this command. */
static void cmd_factory_reset(int argc, char **argv)
{
    if (argc != 2 || strcmp(argv[1], "confirm") != 0) {
        printf("usage: br_factory_reset confirm (erases Thread, Wi-Fi, LAN pairing and BLE bonds; "
               "keeps the RCP selection; reboots)\r\n");
        return;
    }
    if (br_factory_reset_request() != 0) {
        printf("factory reset could not be saved; not rebooting\r\n");
        return;
    }
    printf("factory reset saved; rebooting to erase\r\n");
    vTaskDelay(pdMS_TO_TICKS(100));
#ifdef CONFIG_THREAD_BR_BLE_MANAGEMENT
    br_ble_management_quiesce();
#endif
    hal_reboot();
}
SHELL_CMD_EXPORT_ALIAS(cmd_factory_reset, br_factory_reset, Factory reset the BR: br_factory_reset confirm);
#endif
