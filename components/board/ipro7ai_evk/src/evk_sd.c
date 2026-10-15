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
 * @file evk_sd.c
 * @brief micro-SD + FatFS. See evk_sd.h.
 */

#include "evk_sd.h"
#include "board_config.h"

#include <string.h>

#include "hal_gpio.h"
#include "drv_gpio.h"
#include "drv_glb.h"
#include "ff.h"
#include "fatfs_diskio_register.h"
#include "ipro_log.h"

#define LOG_TAG "EVK_SD"

/* Registered by components/fs/fatfs/port/fatfs_sdh_sdcard.c. */
extern void fatfs_sdh_driver_register(void);
extern void hal_sd_bus_4bits_enable(void);

static FATFS s_fatfs;
static bool  s_mounted;
static bool  s_driver_registered;

bool evk_sd_present(void)
{
    uint8_t pin[1] = { BOARD_SD_DET_PIN };

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pin, 1);
    gpio_set_mode(BOARD_SD_DET_PIN, GPIO_INPUT_PP_MODE);
    /* Active low: the switch grounds the pin when a card is seated. */
    return gpio_read(BOARD_SD_DET_PIN) == 0;
}

int evk_sd_mount(void)
{
    FRESULT fr;

    if (s_mounted) {
        return EVK_SD_OK;
    }
    if (!s_driver_registered) {
        /* All four data lines are wired on this board (D0=29 D1=28 D2=33
         * D3=32); SDH_SDCardInit() still falls back to 1-bit on its own if
         * the card does not support it, so this is a request. */
        hal_sd_bus_4bits_enable();
        fatfs_sdh_driver_register();
        s_driver_registered = true;
    }
    /* opt = 1: mount now, so a missing or unformatted card fails here rather
     * than on the first file operation. */
    fr = f_mount(&s_fatfs, "sd:", 1);
    if (fr != FR_OK) {
        IPRO_LOGE(LOG_TAG, "f_mount failed (FRESULT %d)", (int)fr);
        return EVK_SD_ERR_MOUNT;
    }
    s_mounted = true;
    IPRO_LOGI(LOG_TAG, "mounted at sd:/");
    return EVK_SD_OK;
}

int evk_sd_unmount(void)
{
    if (!s_mounted) {
        return EVK_SD_OK;
    }
    f_mount(NULL, "sd:", 0);
    s_mounted = false;
    return EVK_SD_OK;
}

bool evk_sd_mounted(void)
{
    return s_mounted;
}

void evk_sd_power(int on)
{
    uint8_t pin[1] = { BOARD_SD_PWR_EN_PIN };
    int level = on ? BOARD_SD_PWR_ON_LEVEL : !BOARD_SD_PWR_ON_LEVEL;

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pin, 1);
    gpio_set_mode(BOARD_SD_PWR_EN_PIN, GPIO_OUTPUT_MODE);
    gpio_write(BOARD_SD_PWR_EN_PIN, (uint32_t)level);
}

int evk_sd_space_kb(uint32_t *total_kb, uint32_t *free_kb)
{
    FATFS  *fs;
    DWORD   free_clusters;

    if (!s_mounted) {
        return EVK_SD_ERR_STATE;
    }
    if (f_getfree("sd:", &free_clusters, &fs) != FR_OK) {
        return EVK_SD_ERR_IO;
    }
    /* 512 B sectors, so sectors/2 = KiB. */
    if (total_kb) {
        *total_kb = (uint32_t)(((fs->n_fatent - 2) * fs->csize) / 2);
    }
    if (free_kb) {
        *free_kb = (uint32_t)((free_clusters * fs->csize) / 2);
    }
    return EVK_SD_OK;
}

bool evk_sd_exists(const char *path)
{
    FILINFO fno;

    if (!s_mounted || !path) {
        return false;
    }
    return f_stat(path, &fno) == FR_OK;
}
