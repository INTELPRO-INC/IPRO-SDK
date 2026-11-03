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
 * @file reef_mode.c
 * @brief The mode sector at mfg + REEF_MODE_OFFSET, and switching by reboot.
 */
#include "reef_mode.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include "hal_boot2.h"
#include "hal_flash.h"
#include "hal_sys.h"
#include "ipro_log.h"

#define LOG_TAG          "MODE"
#define MFG_PARTITION    "mfg"
#ifndef REEF_MODE_OFFSET
#define REEF_MODE_OFFSET 0x1000u
#endif
#define MODE_MAGIC       0x444D5452u            /* "RTMD" */
#define SECTOR           4096u
#define RESTART_MS       300u                   /* the APP OK reply leaves first */

static const char *const s_names[REEF_MODE_COUNT] = { "image", "voice", "sensor" };
static reef_mode_t s_mode = REEF_MODE_IMAGE;

static int sector_addr(uint32_t *addr)
{
    uint32_t base = 0, size = 0;

    if (hal_boot2_partition_addr_active(MFG_PARTITION, &base, &size) != 0 ||
        size < REEF_MODE_OFFSET + SECTOR) {
        return -1;
    }
    *addr = base + REEF_MODE_OFFSET;
    return 0;
}

reef_mode_t reef_mode_load(void)
{
    uint32_t addr, rec[3];

    s_mode = REEF_MODE_IMAGE;
    if (sector_addr(&addr) != 0) {
        IPRO_LOGW(LOG_TAG, "no mfg partition: image mode");
    } else if (flash_read(addr, (uint8_t *)rec, sizeof(rec)) != 0) {
        IPRO_LOGW(LOG_TAG, "mode sector unreadable: image mode");
    } else if (rec[0] == MODE_MAGIC && rec[1] < REEF_MODE_COUNT && rec[2] == ~rec[1]) {
        s_mode = (reef_mode_t)rec[1];
    }
    return s_mode;
}

reef_mode_t reef_mode_current(void)
{
    return s_mode;
}

const char *reef_mode_name(reef_mode_t mode)
{
    return (unsigned)mode < REEF_MODE_COUNT ? s_names[mode] : "?";
}

const char *reef_teach_mode(void)
{
    return s_names[s_mode];
}

const char *reef_teach_modes(void)
{
    return "image voice sensor";
}

static int store(reef_mode_t mode)
{
    uint32_t addr, rec[3] = { MODE_MAGIC, (uint32_t)mode, ~(uint32_t)mode }, back[3];

    if (sector_addr(&addr) != 0 || flash_erase(addr, SECTOR) != 0 ||
        flash_write(addr, (uint8_t *)rec, sizeof(rec)) != 0 ||
        flash_read(addr, (uint8_t *)back, sizeof(back)) != 0 || memcmp(rec, back, sizeof(rec))) {
        return -1;
    }
    return 0;
}

/* A full chip reset, not hal_reboot()'s power-on reset: on the camera Reef about
 * one switch in twelve out of image mode left the next boot hung at a random
 * point when the reset was the power-on one, never when it was the full one. */
static void restart(TimerHandle_t t)
{
    (void)t;
    hal_reboot_full();
}

int reef_teach_switch(const char *mode)
{
    static TimerHandle_t s_restart;
    int m = -1;

    for (int i = 0; i < REEF_MODE_COUNT; i++) {
        if (mode && !strcmp(mode, s_names[i])) {
            m = i;
        }
    }
    if (m < 0) {
        return -2;
    }
    if (store((reef_mode_t)m) != 0) {
        IPRO_LOGE(LOG_TAG, "could not store mode %s", s_names[m]);
        return -3;
    }
    IPRO_LOGI(LOG_TAG, "switching to %s mode: restarting", s_names[m]);
    if (!s_restart) {
        s_restart = xTimerCreate("restart", pdMS_TO_TICKS(RESTART_MS), pdFALSE, NULL, restart);
    }
    if (!s_restart || xTimerStart(s_restart, 0) != pdPASS) {
        hal_reboot_full();              /* no timer: restart now, the reply is lost */
    }
    return 0;
}
