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
 * @file device_serial.c
 * @brief Per-device unique serial — see device_serial.h.
 */

#include "device_serial.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "easyflash.h"
#include "drv_sec_eng.h"

#define DEVSERIAL_KEY  "dev_serial"

/* RAM cache: the serial is queried several times per boot (USB descriptor, OT
 * EUI-64 during stack init, OT extended address, CoAP id) and MUST be identical
 * every time. Resolve it once, then serve from RAM — also avoids hammering the
 * flash KV store / TRNG on the hot paths.
 *
 * The resolve-once is guarded so callers in DIFFERENT task contexts cannot race
 * the first-boot mint (two concurrent TRNG mints would persist divergent serials
 * and split the device's identity). The neck already primes this single-threaded
 * in main() via usb_console_init(), but the guard makes the function self-safe
 * for any app/order that reuses this shared file (e.g. a USB-less build whose
 * first caller is a task). 'volatile' so the fast-path flag check is reloaded. */
static uint8_t          s_serial[8];
static volatile bool    s_serial_ready = false;
static SemaphoreHandle_t s_lock = NULL;
static StaticSemaphore_t s_lock_buf;

static void device_serial_resolve(uint8_t out[8])
{
    /* easyflash_init() is idempotent — OT's settings layer also calls it. */
    easyflash_init();

    size_t saved = 0;
    if (ef_get_env_blob(DEVSERIAL_KEY, out, 8, &saved) == 8 && saved == 8) {
        return;   /* already minted on a previous boot */
    }

    /* First boot — mint from the hardware TRNG. */
    uint8_t rnd[32];
    Sec_Eng_Trng_Enable();
    if (Sec_Eng_Trng_Get_Random(rnd, sizeof(rnd)) == 0) {
        memcpy(out, rnd, 8);
    } else {
        /* TRNG unavailable — fall back to a fixed-but-noted pattern so the
         * build still works (collisions possible; should not happen on HW). */
        memset(out, 0, 8);
        out[7] = 0x01;
    }
    Sec_Eng_Trng_Disable();

    out[0] = (uint8_t)((out[0] & 0xFC) | 0x02);  /* locally-administered, unicast */

    if (ef_set_env_blob(DEVSERIAL_KEY, out, 8) != EF_NO_ERR) {
        /* Flash write failed → this serial is only in RAM this boot; the next boot
         * mints a different one, changing the device identity (EUI-64 / USB serial /
         * CoAP id / QR). Surface it rather than silently drift. */
        printf("[device_serial] WARN: ef_set_env_blob failed — serial not persisted\r\n");
    }
}

void device_serial_get(uint8_t out[8])
{
    if (out == NULL) return;

    if (!s_serial_ready) {
        /* Lazily create the guard mutex. Creation must be atomic, but it does
         * no I/O, so a brief critical section is safe (and a no-op before the
         * scheduler starts — the neck's first call). */
        taskENTER_CRITICAL();
        if (s_lock == NULL) {
            s_lock = xSemaphoreCreateMutexStatic(&s_lock_buf);
        }
        taskEXIT_CRITICAL();

        /* Serialize the one-time mint+persist. An uncontended take returns
         * immediately even pre-scheduler; the slow flash/TRNG I/O runs under
         * the mutex (NOT a critical section), so interrupts stay enabled. */
        xSemaphoreTake(s_lock, portMAX_DELAY);
        if (!s_serial_ready) {
            device_serial_resolve(s_serial);
            s_serial_ready = true;
        }
        xSemaphoreGive(s_lock);
    }

    /* Once ready, s_serial is immutable — safe to read without the lock. */
    memcpy(out, s_serial, 8);
}
