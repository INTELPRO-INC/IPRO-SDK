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
 * fatfs_pio_sdh_sdcard.c - FatFS diskio port for the PIO-based SD host
 * (hal_pio_sdh).
 *
 * Registers on the SAME physical drive slot as the hardware-SDH port
 * (DEV_SD): on the A/B bench the card sits in exactly one socket at a time,
 * so whichever host's mount command runs last owns the drive and the
 * /sdcard mount point. The two ports being interchangeable behind the same
 * filesystem IS the A/B criterion.
 */
#include <stddef.h>
#include "ff.h"     /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "hal_pio_sdh.h"

static hal_pio_sdh_cfg_t s_pio_cfg;
static hal_pio_sdh_card_t s_pio_card;
static uint8_t s_pio_inited;

static int PIO_SD_disk_status(void)
{
    return 0;
}

static int PIO_SD_disk_initialize(void)
{
    if (!s_pio_inited) {
        if (hal_pio_sdh_init(&s_pio_cfg, &s_pio_card) != 0) return -1;
        s_pio_inited = 1;
    }
    return 0;
}

static int PIO_SD_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    int rc = hal_pio_sdh_read_blocks(buff, (uint32_t)sector, count);
    if (rc) rc = hal_pio_sdh_read_blocks(buff, (uint32_t)sector, count); /* 1 retry */
    return rc ? -1 : 0;
}

static int PIO_SD_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    int rc = hal_pio_sdh_write_blocks(buff, (uint32_t)sector, count);
    if (rc) rc = hal_pio_sdh_write_blocks(buff, (uint32_t)sector, count); /* 1 retry */
    return rc ? -1 : 0;
}

static int PIO_SD_disk_ioctl(BYTE cmd, void *buff)
{
    switch (cmd) {
        case GET_SECTOR_SIZE:
            *(WORD *)buff = 512;
            break;
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            break;
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = s_pio_card.block_count;
            break;
        case CTRL_SYNC:
            break;
        default:
            break;
    }
    return 0;
}

static DSTATUS PIO_SD_result_code(int result)
{
    return result;
}

void fatfs_pio_sdh_driver_register(const hal_pio_sdh_cfg_t *cfg)
{
    FATFS_DiskioDriverTypeDef drv = { NULL };

    s_pio_cfg = *cfg;
    s_pio_inited = 0;                  /* re-init on next access (card swap) */

    drv.disk_status = PIO_SD_disk_status;
    drv.disk_initialize = PIO_SD_disk_initialize;
    drv.disk_write = PIO_SD_disk_write;
    drv.disk_read = PIO_SD_disk_read;
    drv.disk_ioctl = PIO_SD_disk_ioctl;
    drv.error_code_parsing = PIO_SD_result_code;

    disk_driver_callback_init(DEV_SD, &drv);
}
