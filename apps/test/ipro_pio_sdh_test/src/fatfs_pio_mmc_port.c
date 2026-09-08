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

/*
 * fatfs_pio_mmc_port.c - FatFS diskio port binding the PIO SD host to the
 * DEV_MMC drive slot ("mmc:" volume), so it can be mounted SIMULTANEOUSLY
 * with the hardware SDH on DEV_SD ("sd:" / "/sdcard").
 *
 * Dual-mount layout of this app:
 *   HW SDH  (GPIO28-33) -> DEV_SD  -> EVFS "sdcard" -> /sdcard
 *   PIO SDH (GPIO22-27) -> DEV_MMC -> EVFS "piosd"  -> /piosd
 *
 * (The shared components/fs/fatfs/port/fatfs_pio_sdh_sdcard.c registers the
 * PIO host on DEV_SD for the one-socket A/B flow; this app deliberately does
 * NOT call it - both hosts get their own drive here.)
 */
#include <stddef.h>
#include "ff.h"
#include "diskio.h"
#include "hal_pio_sdh.h"

static hal_pio_sdh_cfg_t s_cfg;
static hal_pio_sdh_card_t s_card;
static uint8_t s_inited;

static int PIO_MMC_disk_status(void)
{
    return 0;
}

static int PIO_MMC_disk_initialize(void)
{
    if (!s_inited) {
        if (hal_pio_sdh_init(&s_cfg, &s_card) != 0) return -1;
        s_inited = 1;
    }
    return 0;
}

static int PIO_MMC_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    int rc = hal_pio_sdh_read_blocks(buff, (uint32_t)sector, count);
    if (rc) rc = hal_pio_sdh_read_blocks(buff, (uint32_t)sector, count); /* 1 retry */
    return rc ? -1 : 0;
}

static int PIO_MMC_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    int rc = hal_pio_sdh_write_blocks(buff, (uint32_t)sector, count);
    if (rc) rc = hal_pio_sdh_write_blocks(buff, (uint32_t)sector, count); /* 1 retry */
    return rc ? -1 : 0;
}

static int PIO_MMC_disk_ioctl(BYTE cmd, void *buff)
{
    switch (cmd) {
        case GET_SECTOR_SIZE:
            *(WORD *)buff = 512;
            break;
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            break;
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = s_card.block_count;
            break;
        case CTRL_SYNC:
            break;
        default:
            break;
    }
    return 0;
}

static DSTATUS PIO_MMC_result_code(int result)
{
    return result;
}

void fatfs_pio_mmc_driver_register(const hal_pio_sdh_cfg_t *cfg)
{
    FATFS_DiskioDriverTypeDef drv = { NULL };

    s_cfg = *cfg;
    s_inited = 0;                      /* re-init on next access (card swap) */

    drv.disk_status = PIO_MMC_disk_status;
    drv.disk_initialize = PIO_MMC_disk_initialize;
    drv.disk_write = PIO_MMC_disk_write;
    drv.disk_read = PIO_MMC_disk_read;
    drv.disk_ioctl = PIO_MMC_disk_ioctl;
    drv.error_code_parsing = PIO_MMC_result_code;

    disk_driver_callback_init(DEV_MMC, &drv);
}
