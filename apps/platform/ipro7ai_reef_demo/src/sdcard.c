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
 * @file sdcard.c
 * @brief SD slot helpers. See sdcard.h.
 */

#include "sdcard.h"
#include "board_config.h"

#include <FreeRTOS.h>
#include <task.h>

#include "hal_gpio.h"
#include "drv_glb.h"
#include "ff.h"
#include "fatfs_diskio_register.h"
#include "ipro_log.h"

#define LOG_TAG "SD"

/* Registered by components/fs/fatfs/port/fatfs_sdh_sdcard.c. */
extern void fatfs_sdh_driver_register(void);

static FATFS s_fatfs;
static bool  s_mounted;
static bool  s_driver_registered;
static bool  s_power_on = (BOARD_SD_PWR_ON_LEVEL == 0); /* R55 pull-down = on */

int sdcard_mount(void)
{
    FRESULT fr;

    if (s_mounted) {
        return SD_OK;
    }
    if (!s_driver_registered) {
        /* All four data lines are wired (D0=29 D1=28 D2=33 D3=32); ask for
         * 4-bit before the driver registers or it defaults to 1-bit. The SDH
         * init still falls back to 1-bit if the card cannot do it. */
        hal_sd_bus_4bits_enable();
        fatfs_sdh_driver_register();
        s_driver_registered = true;
    }

    /* Mount immediately (opt = 1) so a missing or unformatted card fails
     * here rather than on the first file operation. */
    fr = f_mount(&s_fatfs, "sd:", 1);
    if (fr != FR_OK) {
        IPRO_LOGE(LOG_TAG, "f_mount failed (FRESULT %d)", (int)fr);
        return SD_ERR_MOUNT;
    }
    s_mounted = true;
    return SD_OK;
}

int sdcard_unmount(void)
{
    if (!s_mounted) {
        return SD_OK;
    }
    f_mount(NULL, "sd:", 0);
    s_mounted = false;
    return SD_OK;
}

bool sdcard_is_mounted(void) { return s_mounted; }

int sdcard_detect_raw(void)
{
    uint8_t pin[1] = { BOARD_SD_DET_PIN };

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pin, 1);
    gpio_set_mode(BOARD_SD_DET_PIN, GPIO_INPUT_PP_MODE);
    return gpio_read(BOARD_SD_DET_PIN);
}

bool sdcard_present(void)
{
    return sdcard_detect_raw() == BOARD_SD_DET_ACTIVE;
}

void sdcard_power(bool on)
{
    uint8_t pin[1] = { BOARD_SD_PWR_EN_PIN };
    int level = on ? BOARD_SD_PWR_ON_LEVEL : !BOARD_SD_PWR_ON_LEVEL;

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pin, 1);
    gpio_set_mode(BOARD_SD_PWR_EN_PIN, GPIO_OUTPUT_MODE);
    gpio_write(BOARD_SD_PWR_EN_PIN, (uint32_t)level);
    s_power_on = on;
}

bool sdcard_power_is_on(void) { return s_power_on; }

void sdcard_power_cycle(uint32_t off_ms)
{
    if (s_mounted) {
        sdcard_unmount();
    }
    sdcard_power(false);
    vTaskDelay(pdMS_TO_TICKS(off_ms ? off_ms : 200));
    sdcard_power(true);
    /* SD spec: 1 ms after VDD stable plus card init; the fin's IPRO6S needs
     * longer to reach its SDIO bridge, so give both a comfortable margin. */
    vTaskDelay(pdMS_TO_TICKS(50));
}

int sdcard_space_kb(uint32_t *total_kb, uint32_t *free_kb)
{
    FATFS   *fs;
    DWORD    free_clusters;
    FRESULT  fr;

    if (!s_mounted) {
        return SD_ERR_STATE;
    }
    fr = f_getfree("sd:", &free_clusters, &fs);
    if (fr != FR_OK) {
        return SD_ERR_IO;
    }
    /* Sectors are 512 B, so sectors/2 = KiB. */
    if (total_kb) {
        *total_kb = (uint32_t)(((fs->n_fatent - 2) * fs->csize) / 2);
    }
    if (free_kb) {
        *free_kb = (uint32_t)((free_clusters * fs->csize) / 2);
    }
    return SD_OK;
}
