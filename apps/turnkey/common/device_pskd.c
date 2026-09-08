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
 * @file device_pskd.c
 * @brief Per-device unique Thread Joiner PSKd — see device_pskd.h.
 */

#include "device_pskd.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "easyflash.h"
#include "drv_sec_eng.h"

#define DEVPSKD_KEY "dev_pskd"

/* Base32-Thread alphabet: digits 0-9 + A-Z excluding I, O, Q, Z (32 symbols).
 * OpenThread validates the Joiner PSKd against exactly this set in
 * otJoinerStart, so a minted PSKd MUST use it. */
static const char kCharset[32] = "0123456789ABCDEFGHJKLMNPRSTUVWXY";

/* RAM cache — resolved once (TRNG mint or flash read) then served from RAM, so
 * the join path and the CLI always see the same value and we don't hammer the
 * TRNG/flash. Guarded so two task contexts can't race the first-boot mint. */
static char              s_pskd[DEVICE_PSKD_BUFSZ];
static volatile bool     s_ready = false;
static SemaphoreHandle_t s_lock = NULL;
static StaticSemaphore_t s_lock_buf;

static bool charset_ok(char c)
{
    for (int i = 0; i < 32; i++) {
        if (kCharset[i] == c) {
            return true;
        }
    }
    return false;
}

static void device_pskd_resolve(char out[DEVICE_PSKD_BUFSZ])
{
    /* easyflash_init() is idempotent — OT settings + device_serial also call it. */
    easyflash_init();

    size_t saved = 0;
    size_t got   = ef_get_env_blob(DEVPSKD_KEY, out, DEVICE_PSKD_MAX, &saved);
    if (got == saved && saved >= 6 && saved <= DEVICE_PSKD_MAX) {
        out[saved] = '\0';
        return; /* already provisioned/minted on a previous boot */
    }

    /* First boot — mint a random PSKd from the HW TRNG. */
    uint8_t rnd[32];
    Sec_Eng_Trng_Enable();
    if (Sec_Eng_Trng_Get_Random(rnd, sizeof(rnd)) == 0) {
        for (int i = 0; i < DEVICE_PSKD_GEN_LEN; i++) {
            out[i] = kCharset[rnd[i] & 0x1F]; /* 32-symbol set -> 5-bit index */
        }
    } else {
        /* TRNG unavailable — fixed-but-noted fallback so the build still works
         * (collisions possible; should not happen on real HW). */
        memcpy(out, "GTAGTEST1", DEVICE_PSKD_GEN_LEN);
    }
    Sec_Eng_Trng_Disable();
    out[DEVICE_PSKD_GEN_LEN] = '\0';

    if (ef_set_env_blob(DEVPSKD_KEY, out, DEVICE_PSKD_GEN_LEN) != EF_NO_ERR) {
        /* Flash write failed → the PSKd is only in RAM this boot, so the NEXT boot
         * mints a DIFFERENT one, breaking the printed QR label + OOB join. Surface
         * it rather than silently drift identity. */
        printf("[device_pskd] WARN: ef_set_env_blob failed — PSKd not persisted\r\n");
    }
}

void device_pskd_get(char *out, size_t out_len)
{
    if (out == NULL || out_len < DEVICE_PSKD_BUFSZ) {
        return;
    }

    if (!s_ready) {
        taskENTER_CRITICAL();
        if (s_lock == NULL) {
            s_lock = xSemaphoreCreateMutexStatic(&s_lock_buf);
        }
        taskEXIT_CRITICAL();

        xSemaphoreTake(s_lock, portMAX_DELAY);
        if (!s_ready) {
            device_pskd_resolve(s_pskd);
            s_ready = true;
        }
        xSemaphoreGive(s_lock);
    }

    memcpy(out, s_pskd, DEVICE_PSKD_BUFSZ);
}

int device_pskd_set(const char *pskd)
{
    if (pskd == NULL) {
        return -1;
    }

    size_t n = strlen(pskd);
    if (n < 6 || n > DEVICE_PSKD_MAX) {
        return -1;
    }
    for (size_t i = 0; i < n; i++) {
        if (!charset_ok(pskd[i])) {
            return -1;
        }
    }

    easyflash_init();
    if (ef_set_env_blob(DEVPSKD_KEY, pskd, n) != EF_NO_ERR) {
        return -1;
    }

    /* Refresh the cache so device_pskd_get() returns the new value immediately. */
    taskENTER_CRITICAL();
    if (s_lock == NULL) {
        s_lock = xSemaphoreCreateMutexStatic(&s_lock_buf);
    }
    taskEXIT_CRITICAL();

    xSemaphoreTake(s_lock, portMAX_DELAY);
    memcpy(s_pskd, pskd, n);
    s_pskd[n] = '\0';
    s_ready   = true;
    xSemaphoreGive(s_lock);

    return 0;
}
